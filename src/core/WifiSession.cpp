// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "WifiSession.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QtEndian>

#include "Crypto.h"
#include "FolderSync.h"

using namespace Qt::StringLiterals;

namespace Qupil::WifiSync {

namespace {

// First byte of every frame
constexpr char kClear = 0;       // JSON greeting or error
constexpr char kSealed = 1;      // encrypted
// First byte inside an encrypted frame
constexpr char kJson = 'J';
constexpr char kChanges = 'C';
constexpr char kEnd = 'E';

constexpr int kBatch = 2000;

QString b64(const QByteArray &data)
{
    return QString::fromLatin1(data.toBase64());
}

QByteArray unb64(const QJsonValue &v)
{
    return QByteArray::fromBase64(v.toString().toLatin1());
}

QByteArray clearFrame(const QJsonObject &o)
{
    return kClear + QJsonDocument(o).toJson(QJsonDocument::Compact);
}

std::optional<QJsonObject> parseClear(const std::optional<QByteArray> &frame)
{
    if (!frame || frame->isEmpty() || frame->at(0) != kClear)
        return std::nullopt;
    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(frame->mid(1), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject())
        return std::nullopt;
    return doc.object();
}

Status statusOf(const QString &error)
{
    if (error == u"wrong_code"_s)
        return Status::WrongCode;
    if (error == u"not_pairing"_s)
        return Status::NotPairing;
    if (error == u"unknown"_s)
        return Status::UnknownDevice;
    if (error == u"paused"_s)
        return Status::Refused;
    return Status::Failed;
}

/*!
    Both directions of an encrypted session. Each frame carries a counter in
    its authenticated data: a repeated, dropped or reordered frame fails.
*/
class Secure
{
public:
    Secure(FrameChannel *channel, const QByteArray &master, bool client)
        : m_channel(channel)
    {
        const QByteArray c2s = Crypto::hash("c2s", Crypto::KeySize, master);
        const QByteArray s2c = Crypto::hash("s2c", Crypto::KeySize, master);
        m_sendKey = client ? c2s : s2c;
        m_receiveKey = client ? s2c : c2s;
        m_sendLabel = client ? "c2s" : "s2c";
        m_receiveLabel = client ? "s2c" : "c2s";
    }
    ~Secure()
    {
        Crypto::wipe(m_sendKey);
        Crypto::wipe(m_receiveKey);
    }

    bool send(char type, const QByteArray &payload)
    {
        const QByteArray sealed = Crypto::seal(m_sendKey, type + payload, ad(m_sendLabel, m_sent++));
        return m_channel->send(kSealed + sealed);
    }
    bool sendJson(const QJsonObject &o) { return send(kJson, QJsonDocument(o).toJson(QJsonDocument::Compact)); }

    //! Type byte + payload; nullopt if missing or not authentic. *clear gets a clear error frame.
    std::optional<QByteArray> receive(QJsonObject *clear = nullptr)
    {
        const auto frame = m_channel->receive();
        if (!frame || frame->isEmpty())
            return std::nullopt;
        if (frame->at(0) == kClear) {
            if (clear)
                *clear = parseClear(frame).value_or(QJsonObject());
            return std::nullopt;
        }
        const auto plain = Crypto::open(m_receiveKey, frame->mid(1), ad(m_receiveLabel, m_received));
        if (!plain || plain->isEmpty())
            return std::nullopt;
        ++m_received;
        return plain;
    }
    std::optional<QJsonObject> receiveJson(QJsonObject *clear = nullptr)
    {
        const auto plain = receive(clear);
        if (!plain || plain->at(0) != kJson)
            return std::nullopt;
        const QJsonDocument doc = QJsonDocument::fromJson(plain->mid(1));
        if (!doc.isObject())
            return std::nullopt;
        return doc.object();
    }
    bool everReceived() const { return m_received > 0; }

private:
    static QByteArray ad(const char *label, quint64 counter)
    {
        QByteArray a(label);
        char n[8];
        qToBigEndian(counter, n);
        return a + QByteArray(n, 8);
    }

    FrameChannel *m_channel;
    QByteArray m_sendKey;
    QByteArray m_receiveKey;
    const char *m_sendLabel;
    const char *m_receiveLabel;
    quint64 m_sent = 0;
    quint64 m_received = 0;
};

QByteArray transcript(const QByteArray &a, const QByteArray &b)
{
    return Crypto::hash(a + b, 32);
}

// CPace generator: a point from the code and this session, nobody knows its logarithm
QByteArray generator(const QString &code, const QByteArray &sessionId, const QString &clientId)
{
    const QByteArray c = code.toUtf8();
    return Crypto::hashToPoint("qupil-cpace-v1" + QByteArray::number(c.size()) + ':' + c + sessionId
                               + clientId.toUtf8());
}

QJsonObject toJson(const VersionVector &vv)
{
    QJsonObject o;
    for (auto it = vv.cbegin(); it != vv.cend(); ++it)
        o.insert(it.key(), double(it.value()));
    return o;
}

VersionVector fromJson(const QJsonObject &o)
{
    VersionVector vv;
    for (auto it = o.constBegin(); it != o.constEnd(); ++it)
        vv.insert(it.key(), qint64(it.value().toDouble()));
    return vv;
}

//! Sends every change the other side has not seen, then the end marker
bool sendChanges(Secure &secure, Store *store, const VersionVector &theirs, Stats *stats)
{
    const VersionVector mine = store->versionVector();
    for (auto it = mine.cbegin(); it != mine.cend(); ++it) {
        qint64 after = theirs.value(it.key(), 0);
        while (after < it.value()) {
            const QList<Change> batch = store->changesOf(it.key(), after, kBatch);
            if (batch.isEmpty())
                break;
            if (!secure.send(kChanges, SyncFormat::encode(batch)))
                return false;
            stats->sent += int(batch.size());
            after = batch.last().seq;
        }
    }
    return secure.send(kEnd, {});
}

//! Applies changes until the end marker
bool receiveChanges(Secure &secure, Store *store, Stats *stats, QString *error)
{
    while (true) {
        const auto plain = secure.receive();
        if (!plain) {
            *error = u"connection lost"_s;
            return false;
        }
        if (plain->at(0) == kEnd)
            return true;
        if (plain->at(0) != kChanges) {
            *error = u"unexpected message"_s;
            return false;
        }
        const auto changes = SyncFormat::decode(plain->mid(1));
        if (!changes) {
            *error = u"unreadable changes"_s;
            return false;
        }
        const int applied = store->applyRemote(*changes);
        if (applied < 0) {
            *error = store->lastError();
            return false;
        }
        stats->received += int(changes->size());
        stats->applied += applied;
        for (const Change &c : *changes)
            if (c.hlc > stats->maxHlc)
                stats->maxHlc = c.hlc;
    }
}

QByteArray syncMaster(const QByteArray &ee, const QByteArray &es, const QByteArray &se, const QByteArray &ss,
                      const QByteArray &transcriptHash)
{
    return Crypto::hash(ee + es + se + ss + transcriptHash, Crypto::KeySize, "qupil-wifi-sync-v1");
}

void setError(QString *error, const QString &text)
{
    if (error)
        *error = text;
}

} // namespace

QString normalizeCode(const QString &code)
{
    QString digits;
    for (const QChar c : code)
        if (c.isDigit())
            digits += QChar(u'0' + c.digitValue());
    return digits;
}

QString newCode()
{
    return u"%1"_s.arg(QRandomGenerator::system()->bounded(1000000), 6, 10, QLatin1Char('0'));
}

// ---------------------------------------------------------------------------- client

Status pair(FrameChannel *channel, const Identity &me, const QString &code, bool hasData, Peer *peer, QString *error)
{
    const QString digits = normalizeCode(code);
    const QByteArray sessionId = Crypto::randomBytes(16);
    QByteArray scalar = Crypto::randomBytes(32);
    const QByteArray y = Crypto::x25519(scalar, generator(digits, sessionId, me.id));
    if (y.isEmpty())
        return setError(error, u"key generation failed"_s), Status::Failed;

    const QByteArray hello = clearFrame({ { u"v"_s, ProtocolVersion }, { u"kind"_s, u"pair"_s }, { u"id"_s, me.id },
                                          { u"name"_s, me.name }, { u"sid"_s, b64(sessionId) }, { u"y"_s, b64(y) } });
    if (!channel->send(hello))
        return setError(error, u"connection lost"_s), Status::Failed;
    const auto answerFrame = channel->receive();
    const auto answer = parseClear(answerFrame);
    if (!answer)
        return setError(error, u"no answer"_s), Status::Failed;
    if (answer->contains(u"error"_s))
        return statusOf(answer->value(u"error"_s).toString());

    QByteArray k = Crypto::x25519(scalar, unb64(answer->value(u"y"_s)));
    Crypto::wipe(scalar);
    if (k.isEmpty())
        return setError(error, u"bad key"_s), Status::Failed;
    QByteArray master = Crypto::hash(k + transcript(hello, *answerFrame), Crypto::KeySize, "qupil-wifi-pair-v1");
    Crypto::wipe(k);
    Secure secure(channel, master, true);
    Crypto::wipe(master);

    if (!secure.sendJson({ { u"pub"_s, b64(me.publicKey) }, { u"name"_s, me.name }, { u"hasData"_s, hasData } }))
        return setError(error, u"connection lost"_s), Status::Failed;
    QJsonObject clear;
    const auto theirs = secure.receiveJson(&clear);
    if (!theirs) {
        if (clear.contains(u"error"_s))
            return statusOf(clear.value(u"error"_s).toString());
        return Status::WrongCode;   // nothing readable: the keys differ
    }
    const QByteArray pub = unb64(theirs->value(u"pub"_s));
    if (pub.size() != 32)
        return setError(error, u"bad key"_s), Status::Failed;
    peer->id = answer->value(u"id"_s).toString();
    peer->name = theirs->value(u"name"_s).toString();
    peer->publicKey = pub;
    peer->hasData = theirs->value(u"hasData"_s).toBool();
    return Status::Ok;
}

Status sync(FrameChannel *channel, Store *store, const Identity &me, const Peer &peer, Stats *stats, QString *error)
{
    Stats local;
    Stats &s = stats ? *stats : local;
    Crypto::KeyPair ephemeral = Crypto::x25519KeyPair();
    const QByteArray hello = clearFrame({ { u"v"_s, ProtocolVersion }, { u"kind"_s, u"sync"_s }, { u"id"_s, me.id },
                                          { u"eph"_s, b64(ephemeral.publicKey) } });
    if (!channel->send(hello))
        return setError(error, u"connection lost"_s), Status::Failed;
    const auto answerFrame = channel->receive();
    const auto answer = parseClear(answerFrame);
    if (!answer)
        return setError(error, u"no answer"_s), Status::Failed;
    if (answer->contains(u"error"_s))
        return statusOf(answer->value(u"error"_s).toString());
    if (answer->value(u"id"_s).toString() != peer.id)
        return setError(error, u"a different device answered"_s), Status::AuthFailed;

    const QByteArray theirEphemeral = unb64(answer->value(u"eph"_s));
    QByteArray ee = Crypto::x25519(ephemeral.secretKey, theirEphemeral);
    QByteArray es = Crypto::x25519(ephemeral.secretKey, peer.publicKey);
    QByteArray se = Crypto::x25519(me.secretKey, theirEphemeral);
    QByteArray ss = Crypto::x25519(me.secretKey, peer.publicKey);
    Crypto::wipe(ephemeral.secretKey);
    if (ee.isEmpty() || es.isEmpty() || se.isEmpty() || ss.isEmpty())
        return setError(error, u"bad key"_s), Status::AuthFailed;
    QByteArray master = syncMaster(ee, es, se, ss, transcript(hello, *answerFrame));
    for (QByteArray *secret : { &ee, &es, &se, &ss })
        Crypto::wipe(*secret);
    Secure secure(channel, master, true);
    Crypto::wipe(master);

    if (!secure.sendJson({ { u"vv"_s, toJson(store->versionVector()) } }))
        return setError(error, u"connection lost"_s), Status::Failed;
    const auto theirs = secure.receiveJson();
    if (!theirs)
        return setError(error, u"not authentic"_s), Status::AuthFailed;
    QString problem;
    if (!sendChanges(secure, store, fromJson(theirs->value(u"vv"_s).toObject()), &s))
        return setError(error, u"connection lost"_s), Status::Failed;
    if (!receiveChanges(secure, store, &s, &problem))
        return setError(error, problem), Status::Failed;
    return Status::Ok;
}

// ---------------------------------------------------------------------------- server

ServeResult serve(FrameChannel *channel, Store *store, const Identity &me, const ServerState &state)
{
    ServeResult r;
    const auto helloFrame = channel->receive();
    const auto hello = parseClear(helloFrame);
    if (!hello) {
        r.error = u"no greeting"_s;
        return r;
    }
    const QString kind = hello->value(u"kind"_s).toString();
    const QString clientId = hello->value(u"id"_s).toString();
    auto refuse = [&](const char *why, Status status) {
        channel->send(clearFrame({ { u"error"_s, QString::fromLatin1(why) } }));
        r.status = status;
        r.error = QString::fromLatin1(why);
        return r;
    };
    if (hello->value(u"v"_s).toInt() != ProtocolVersion)
        return refuse("version", Status::Failed);

    if (kind == u"pair"_s) {
        r.kind = ServeResult::Paired;
        if (state.pairingCode.isEmpty())
            return refuse("not_pairing", Status::NotPairing);
        QByteArray scalar = Crypto::randomBytes(32);
        const QByteArray g = generator(normalizeCode(state.pairingCode), unb64(hello->value(u"sid"_s)), clientId);
        const QByteArray y = Crypto::x25519(scalar, g);
        QByteArray k = Crypto::x25519(scalar, unb64(hello->value(u"y"_s)));
        Crypto::wipe(scalar);
        if (y.isEmpty() || k.isEmpty())
            return refuse("bad_key", Status::Failed);
        const QByteArray answer = clearFrame({ { u"v"_s, ProtocolVersion }, { u"kind"_s, u"pair"_s },
                                               { u"id"_s, me.id }, { u"name"_s, me.name }, { u"y"_s, b64(y) } });
        if (!channel->send(answer)) {
            r.error = u"connection lost"_s;
            return r;
        }
        QByteArray master = Crypto::hash(k + transcript(*helloFrame, answer), Crypto::KeySize, "qupil-wifi-pair-v1");
        Crypto::wipe(k);
        Secure secure(channel, master, false);
        Crypto::wipe(master);
        const auto theirs = secure.receiveJson();
        if (!theirs)
            return refuse("wrong_code", Status::WrongCode);
        const QByteArray pub = unb64(theirs->value(u"pub"_s));
        if (pub.size() != 32)
            return refuse("bad_key", Status::Failed);
        if (!secure.sendJson({ { u"pub"_s, b64(me.publicKey) }, { u"name"_s, me.name }, { u"hasData"_s, state.hasData } })) {
            r.error = u"connection lost"_s;
            return r;
        }
        r.peer = { clientId, theirs->value(u"name"_s).toString(), pub, theirs->value(u"hasData"_s).toBool() };
        r.status = Status::Ok;
        return r;
    }

    if (kind == u"sync"_s) {
        r.kind = ServeResult::Synced;
        const Peer *peer = nullptr;
        for (const Peer &p : state.peers)
            if (p.id == clientId)
                peer = &p;
        if (!peer)
            return refuse("unknown", Status::UnknownDevice);
        r.peer = *peer;
        if (state.paused.contains(clientId))
            return refuse("paused", Status::Refused);

        Crypto::KeyPair ephemeral = Crypto::x25519KeyPair();
        const QByteArray theirEphemeral = unb64(hello->value(u"eph"_s));
        const QByteArray answer = clearFrame({ { u"v"_s, ProtocolVersion }, { u"kind"_s, u"sync"_s },
                                               { u"id"_s, me.id }, { u"eph"_s, b64(ephemeral.publicKey) } });
        QByteArray ee = Crypto::x25519(ephemeral.secretKey, theirEphemeral);
        QByteArray es = Crypto::x25519(me.secretKey, theirEphemeral);         // their ephemeral, our static
        QByteArray se = Crypto::x25519(ephemeral.secretKey, peer->publicKey);  // their static, our ephemeral
        QByteArray ss = Crypto::x25519(me.secretKey, peer->publicKey);
        Crypto::wipe(ephemeral.secretKey);
        if (ee.isEmpty() || es.isEmpty() || se.isEmpty() || ss.isEmpty())
            return refuse("bad_key", Status::AuthFailed);
        if (!channel->send(answer)) {
            r.error = u"connection lost"_s;
            return r;
        }
        QByteArray master = syncMaster(ee, es, se, ss, transcript(*helloFrame, answer));
        for (QByteArray *secret : { &ee, &es, &se, &ss })
            Crypto::wipe(*secret);
        Secure secure(channel, master, false);
        Crypto::wipe(master);

        const auto theirs = secure.receiveJson();
        if (!theirs) {
            r.status = Status::AuthFailed;
            r.error = u"not authentic"_s;
            return r;
        }
        const VersionVector theirVersions = fromJson(theirs->value(u"vv"_s).toObject());
        if (!secure.sendJson({ { u"vv"_s, toJson(store->versionVector()) } })) {
            r.error = u"connection lost"_s;
            return r;
        }
        if (!receiveChanges(secure, store, &r.stats, &r.error))
            return r;
        // Everything newer than what they had, also what we got from third devices (relay)
        if (!sendChanges(secure, store, theirVersions, &r.stats)) {
            r.error = u"connection lost"_s;
            return r;
        }
        r.status = Status::Ok;
        return r;
    }

    return refuse("kind", Status::Failed);
}

} // namespace Qupil::WifiSync
