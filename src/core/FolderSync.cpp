// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "FolderSync.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QSysInfo>

#include <algorithm>

#include "Crypto.h"
#include "Database.h"

using namespace Qt::StringLiterals;

namespace Qupil {

// ---------------------------------------------------------------------------- format

namespace SyncFormat {

QByteArray encode(const QList<Change> &changes)
{
    QJsonArray list;
    for (const Change &c : changes) {
        list.append(QJsonArray { c.origin, double(c.seq), c.hlc, c.entity, c.rowId, c.field,
                                 Store::encodeValue(c.value) });
    }
    const QJsonObject root { { u"v"_s, 1 }, { u"changes"_s, list } };
    return qCompress(QJsonDocument(root).toJson(QJsonDocument::Compact), 6);
}

std::optional<QList<Change>> decode(const QByteArray &data)
{
    const QByteArray json = qUncompress(data);
    if (json.isEmpty())
        return std::nullopt;
    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject() || doc.object().value(u"v"_s).toInt() != 1)
        return std::nullopt;
    QList<Change> out;
    for (const QJsonValue &v : doc.object().value(u"changes"_s).toArray()) {
        const QJsonArray a = v.toArray();
        if (a.size() != 7)
            return std::nullopt;
        out.append({ a.at(0).toString(), qint64(a.at(1).toDouble()), a.at(2).toString(), a.at(3).toString(),
                     a.at(4).toString(), a.at(5).toString(), Store::decodeValue(a.at(6).toString()) });
    }
    return out;
}

} // namespace SyncFormat

// ---------------------------------------------------------------------------- folder sync

namespace {

const QByteArray kCheckText = "qupil-sync-key";
const QByteArray kCheckAd = "qupil-key-check";
const QByteArray kDeviceAd = "qupil-device";

struct Batch
{
    QString name;
    qint64 first = 0;
    qint64 last = 0;
};

//! "000000000001-000000002000.bin"
std::optional<Batch> parseBatch(const QString &name)
{
    static const QRegularExpression pattern(u"^(\\d{12})-(\\d{12})\\.bin$"_s);
    const auto m = pattern.match(name);
    if (!m.hasMatch())
        return std::nullopt;
    return Batch { name, m.captured(1).toLongLong(), m.captured(2).toLongLong() };
}

QString batchName(qint64 first, qint64 last)
{
    return u"%1-%2.bin"_s.arg(first, 12, 10, QLatin1Char('0')).arg(last, 12, 10, QLatin1Char('0'));
}

} // namespace

FolderSync::FolderSync(Store *store, RemoteFolder *remote, const QString &root)
    : m_store(store)
    , m_remote(remote)
    , m_root(root)
{
}

bool FolderSync::fail(const QString &message)
{
    m_error = message;
    return false;
}

QString FolderSync::path(const QString &relative) const
{
    return relative.isEmpty() ? m_root : m_root + u'/' + relative;
}

std::optional<bool> FolderSync::isSetUp()
{
    const auto files = m_remote->list(m_root);
    if (!files) {
        m_error = m_remote->lastError();
        return std::nullopt;
    }
    return files->contains(u"key.json"_s);
}

QByteArray FolderSync::unlock(const QString &password, Status *status)
{
    *status = Status::Failed;
    if (!m_remote->makeFolder(m_root)) {
        fail(m_remote->lastError());
        return {};
    }
    const auto existing = m_remote->read(path(u"key.json"_s));
    if (existing) {
        const QJsonObject info = QJsonDocument::fromJson(*existing).object();
        const QByteArray salt = QByteArray::fromBase64(info.value(u"salt"_s).toString().toLatin1());
        Crypto::KdfParams params;
        params.memoryKiB = info.value(u"memoryKiB"_s).toInt(params.memoryKiB);
        params.passes = info.value(u"passes"_s).toInt(params.passes);
        QByteArray key = Crypto::deriveKey(password, salt, params);
        const auto check = Crypto::open(key, QByteArray::fromBase64(info.value(u"check"_s).toString().toLatin1()), kCheckAd);
        if (!check || *check != kCheckText) {
            Crypto::wipe(key);
            *status = Status::WrongPassword;
            return {};
        }
        *status = Status::Ok;
        return key;
    }
    if (!m_remote->lastErrorWasNotFound()) {
        fail(m_remote->lastError());
        return {};
    }

    // First device: set up the folder for this password
    const QByteArray salt = Crypto::randomBytes(Crypto::SaltSize);
    const Crypto::KdfParams params;
    QByteArray key = Crypto::deriveKey(password, salt, params);
    const QJsonObject info {
        { u"v"_s, 1 }, { u"kdf"_s, u"argon2id"_s }, { u"memoryKiB"_s, params.memoryKiB }, { u"passes"_s, params.passes },
        { u"salt"_s, QString::fromLatin1(salt.toBase64()) },
        { u"check"_s, QString::fromLatin1(Crypto::seal(key, kCheckText, kCheckAd).toBase64()) },
        { u"created"_s, QDateTime::currentDateTimeUtc().toString(Qt::ISODate) },
    };
    if (!m_remote->write(path(u"key.json"_s), QJsonDocument(info).toJson())) {
        Crypto::wipe(key);
        fail(m_remote->lastError());
        return {};
    }
    *status = Status::Ok;
    return key;
}

FolderSync::Status FolderSync::run(const QByteArray &key, const QString &deviceName, Stats *stats)
{
    Stats local;
    Stats &s = stats ? *stats : local;
    Database *db = m_store->database();
    const QString me = db->deviceId();

    for (const QString &folder : { QString(), u"devices"_s, u"changes"_s }) {
        if (!m_remote->makeFolder(path(folder)))
            return fail(m_remote->lastError()), Status::Failed;
    }

    // What is in the folder: per device (origin) the covered sequence range
    const auto origins = m_remote->list(path(u"changes"_s));
    if (!origins)
        return fail(m_remote->lastError()), Status::Failed;
    QHash<QString, QList<Batch>> remote;
    for (const QString &entry : *origins) {
        if (!entry.endsWith(u'/'))
            continue;
        const QString origin = entry.chopped(1);
        const auto files = m_remote->list(path(u"changes/"_s + origin));
        if (!files)
            return fail(m_remote->lastError()), Status::Failed;
        QList<Batch> batches;
        for (const QString &f : *files)
            if (const auto b = parseBatch(f))
                batches << *b;
        std::sort(batches.begin(), batches.end(), [](const Batch &a, const Batch &b) { return a.first < b.first; });
        remote.insert(origin, batches);
    }
    auto remoteMax = [&remote](const QString &origin) {
        qint64 max = 0;
        for (const Batch &b : remote.value(origin))
            max = std::max(max, b.last);
        return max;
    };

    // 1. Download what is new for us (before uploading, so relayed changes are complete)
    const VersionVector known = m_store->versionVector();
    for (auto it = remote.cbegin(); it != remote.cend(); ++it) {
        const qint64 have = known.value(it.key(), 0);
        for (const Batch &b : it.value()) {
            if (b.last <= have)
                continue;
            const QString file = u"changes/"_s + it.key() + u'/' + b.name;
            const auto sealed = m_remote->read(path(file));
            if (!sealed)
                return fail(m_remote->lastError()), Status::Failed;
            const auto plain = Crypto::open(key, *sealed, (it.key() + u'/' + b.name).toUtf8());
            if (!plain)
                return fail(u"Could not decrypt %1"_s.arg(file)), Status::WrongPassword;
            const auto changes = SyncFormat::decode(*plain);
            if (!changes)
                return fail(u"Unreadable sync file %1"_s.arg(file)), Status::Failed;
            const int applied = m_store->applyRemote(*changes);
            if (applied < 0)
                return fail(m_store->lastError()), Status::Failed;
            s.downloaded += int(changes->size());
            s.applied += applied;
            for (const Change &c : *changes)
                if (c.hlc > s.maxHlc)
                    s.maxHlc = c.hlc;
        }
    }

    // 2. Upload every change the folder does not have yet – ours and those we got
    //    from other devices by Wi-Fi, so devices that only use the folder get them too
    const VersionVector mine = m_store->versionVector();
    for (auto it = mine.cbegin(); it != mine.cend(); ++it) {
        const QString origin = it.key();
        qint64 covered = remoteMax(origin);
        if (it.value() <= covered)
            continue;
        if (!m_remote->makeFolder(path(u"changes/"_s + origin)))
            return fail(m_remote->lastError()), Status::Failed;
        while (true) {
            const QList<Change> batch = m_store->changesOf(origin, covered, BatchSize);
            if (batch.isEmpty())
                break;
            const QString name = batchName(batch.first().seq, batch.last().seq);
            const QByteArray sealed = Crypto::seal(key, SyncFormat::encode(batch), (origin + u'/' + name).toUtf8());
            if (!m_remote->write(path(u"changes/"_s + origin + u'/' + name), sealed))
                return fail(m_remote->lastError()), Status::Failed;
            s.uploaded += int(batch.size());
            covered = batch.last().seq;
        }
    }

    // 3. Who else syncs here
    const QJsonObject device {
        { u"id"_s, me }, { u"name"_s, deviceName }, { u"platform"_s, QSysInfo::prettyProductName() },
        { u"lastSync"_s, QDateTime::currentDateTimeUtc().toString(Qt::ISODate) },
    };
    if (!m_remote->write(path(u"devices/"_s + me + u".bin"_s),
                         Crypto::seal(key, QJsonDocument(device).toJson(QJsonDocument::Compact), kDeviceAd)))
        return fail(m_remote->lastError()), Status::Failed;
    if (const auto devices = m_remote->list(path(u"devices"_s))) {
        for (const QString &f : *devices) {
            if (!f.endsWith(u".bin"_s) || f == me + u".bin"_s)
                continue;
            if (const auto sealed = m_remote->read(path(u"devices/"_s + f))) {
                if (const auto plain = Crypto::open(key, *sealed, kDeviceAd))
                    s.devices << QJsonDocument::fromJson(*plain).object().value(u"name"_s).toString();
            }
        }
    }
    return Status::Ok;
}

} // namespace Qupil
