// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "Database.h"

#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QSysInfo>
#include <QUuid>

#include "Schema.h"

using namespace Qt::StringLiterals;

namespace Qupil {

Database::Database(QObject *parent)
    : QObject(parent)
    , m_connectionName(u"qupil-"_s + QUuid::createUuid().toString(QUuid::WithoutBraces))
{
}

Database::~Database()
{
    close();
}

QSqlDatabase Database::connection() const
{
    return QSqlDatabase::database(m_connectionName, false);
}

bool Database::isOpen() const
{
    return QSqlDatabase::contains(m_connectionName) && connection().isOpen();
}

bool Database::exec(const QString &sql, QString *error)
{
    QSqlQuery q(connection());
    if (!q.exec(sql)) {
        if (error)
            *error = q.lastError().text() + u" ("_s + sql.left(80) + u")"_s;
        return false;
    }
    return true;
}

bool Database::open(const QString &path, QString *error)
{
    close();
    m_path = path;
    QDir().mkpath(QFileInfo(path).absolutePath());
    const bool existed = QFileInfo::exists(path) && QFileInfo(path).size() > 0;

    bool opened = false;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(u"QSQLITE"_s, m_connectionName);
        db.setDatabaseName(path);
        opened = db.open();
        if (!opened && error)
            *error = db.lastError().text();
    }
    if (!opened) {
        close();
        return false;
    }
    exec(u"PRAGMA foreign_keys = OFF"_s);
    exec(u"PRAGMA journal_mode = WAL"_s);
    exec(u"PRAGMA synchronous = NORMAL"_s);
    exec(u"PRAGMA busy_timeout = 10000"_s);   // the sync writes from a second connection

    if (!existed || !connection().tables().contains(u"meta"_s)) {
        if (existed && !connection().tables().isEmpty()) {
            if (error)
                *error = QStringLiteral("This file is not a Qupil 2 database.");
            close();
            return false;
        }
        if (!createSchema(error)) {
            close();
            return false;
        }
    }

    const int version = meta(u"schema_version"_s, u"0"_s).toInt();
    if (version > Schema::Version) {
        if (error)
            *error = QStringLiteral("The database was created by a newer version of Qupil.");
        close();
        return false;
    }
    if (version < Schema::Version && !migrate(version, error)) {
        close();
        return false;
    }

    m_deviceId = meta(u"device_id"_s);
    if (m_deviceId.isEmpty())
        resetDeviceId();
    m_localOrigin = originNumber(m_deviceId);
    m_clock.restore(meta(u"hlc_ms"_s, u"0"_s).toLongLong(), meta(u"hlc_counter"_s, u"0"_s).toInt());

    // The change log is the truth for sequence numbers and the clock, even after a crash
    QSqlQuery q(connection());
    q.prepare(u"SELECT seq, time FROM change_log WHERE origin = ? ORDER BY seq DESC LIMIT 1"_s);
    q.addBindValue(m_localOrigin);
    m_localSeq = 0;
    if (q.exec() && q.next()) {
        m_localSeq = q.value(0).toLongLong();
        m_clock.observe(q.value(1).toLongLong());
    }
    return true;
}

void Database::close()
{
    if (QSqlDatabase::contains(m_connectionName)) {
        {
            QSqlDatabase db = QSqlDatabase::database(m_connectionName, false);
            if (db.isOpen()) {
                persistClock();
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(m_connectionName);
    }
    m_txDepth = 0;
    m_originNumbers.clear();
    m_originIds.clear();
}

bool Database::createSchema(QString *error)
{
    if (!transaction())
        return false;
    for (const QString &sql : Schema::createStatements()) {
        if (!exec(sql, error)) {
            rollback();
            return false;
        }
    }
    setMeta(u"schema_version"_s, QString::number(Schema::Version));
    setMeta(u"created"_s, QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    return commit();
}

bool Database::migrate(int from, QString *error)
{
    // A copy of the file as it was, in case something goes wrong
    if (!m_migrationBackup.isEmpty() && !backupTo(m_migrationBackup, error))
        return false;
    if (!transaction()) {
        if (error)
            *error = connection().lastError().text();
        return false;
    }
    // One step per version (1 is the first)
    const QList<QPair<int, QString>> steps = {
        { 2, u"ALTER TABLE library_item ADD COLUMN number INTEGER NOT NULL DEFAULT 0"_s },
    };
    for (const auto &[version, sql] : steps) {
        if (from < version && !exec(sql, error)) {
            rollback();
            return false;
        }
    }
    if (from < 3 && !migrateSyncTables(error)) {
        rollback();
        return false;
    }
    if (!setMeta(u"schema_version"_s, QString::number(Schema::Version)) || !commit()) {
        if (error)
            *error = connection().lastError().text();
        rollback();
        return false;
    }
    if (from < 3)
        exec(u"VACUUM"_s);   // gives the space back
    return true;
}

namespace {

//! The CREATE statement of a sync table, optionally under another name
QString createStatement(const QString &table, const QString &as = {})
{
    const QString prefix = u"CREATE TABLE "_s + table + u" ("_s;
    for (const QString &sql : Schema::createStatements()) {
        if (sql.startsWith(prefix))
            return as.isEmpty() ? sql : u"CREATE TABLE "_s + as + sql.mid(prefix.size() - 2);
    }
    return {};
}

QString compactJson(const QJsonObject &o)
{
    return QString::fromUtf8(QJsonDocument(o).toJson(QJsonDocument::Compact));
}

} // namespace

/*!
    Schema 3: device ids become local numbers, the clock text
    ("<ms>.<counter>.<device>") a number, the log one row per record and write
    instead of one per field. Every change keeps its device and sequence number,
    so devices that were synchronised before stay in step.
*/
bool Database::migrateSyncTables(QString *error)
{
    QSqlQuery q(connection());
    bool old = false;
    if (q.exec(u"PRAGMA table_info(change_log)"_s)) {
        while (q.next())
            old = old || q.value(1).toString() == u"hlc"_s;
    }
    if (!old)
        return true;   // already the new layout

    const QStringList prepare = {
        u"DROP INDEX IF EXISTS idx_change_log_hlc"_s,
        u"ALTER TABLE change_log RENAME TO change_log_v2"_s,
        u"ALTER TABLE field_clock RENAME TO field_clock_v2"_s,
        createStatement(u"change_log"_s),
        createStatement(u"field_clock"_s),
        createStatement(u"origin"_s),
    };
    for (const QString &sql : prepare) {
        if (!exec(sql, error))
            return false;
    }
    if (const QString me = meta(u"device_id"_s); !me.isEmpty())
        originNumber(me);   // this device gets number 1

    const auto failed = [error](const QSqlQuery &query) {
        if (error)
            *error = query.lastError().text();
        return false;
    };
    QSqlQuery read(connection());
    read.setForwardOnly(true);
    QSqlQuery write(connection());
    if (!read.exec(u"SELECT origin, seq, hlc, entity, row_id, field, value FROM change_log_v2"_s))
        return failed(read);
    write.prepare(u"INSERT INTO change_log (origin, seq, time, entity, row_id, data) VALUES (?, ?, ?, ?, ?, ?)"_s);
    while (read.next()) {
        qint64 time = 0;
        HybridClock::parse(read.value(2).toString(), &time);
        const QJsonArray value = QJsonDocument::fromJson(read.value(6).toString().toUtf8()).array();
        write.addBindValue(originNumber(read.value(0).toString()));
        write.addBindValue(read.value(1).toLongLong());
        write.addBindValue(time);
        write.addBindValue(read.value(3).toString());
        write.addBindValue(read.value(4).toString());
        write.addBindValue(compactJson({ { read.value(5).toString(), value.isEmpty() ? QJsonValue() : value.first() } }));
        if (!write.exec())
            return failed(write);
    }
    if (!read.exec(u"SELECT entity, row_id, field, hlc FROM field_clock_v2"_s))
        return failed(read);
    write.prepare(u"INSERT INTO field_clock (entity, row_id, field, time, origin) VALUES (?, ?, ?, ?, ?)"_s);
    while (read.next()) {
        qint64 time = 0;
        QString device;
        if (!HybridClock::parse(read.value(3).toString(), &time, &device))
            continue;
        write.addBindValue(read.value(0).toString());
        write.addBindValue(read.value(1).toString());
        write.addBindValue(read.value(2).toString());
        write.addBindValue(time);
        write.addBindValue(originNumber(device));
        if (!write.exec())
            return failed(write);
    }
    read.finish();
    return exec(u"DROP TABLE change_log_v2"_s, error) && exec(u"DROP TABLE field_clock_v2"_s, error)
           && compactChangeLog(error);
}

bool Database::compactChangeLog(QString *error)
{
    // Two seconds: far less than anyone needs to change the same thing on another device
    constexpr qint64 kWindow = qint64(2000) << 16;
    const auto failed = [error](const QSqlQuery &query) {
        if (error)
            *error = query.lastError().text();
        return false;
    };
    if (!transaction())
        return false;

    // 1. Log rows of one record directly after each other become one row: the
    //    sequence number of the last, the newest values, the time of the last.
    //    The fields of the earlier rows get that time, too (raised).
    QHash<QPair<int, qint64>, qint64> raised;   // (device, old time) → new time
    bool ok = exec(u"DROP TABLE IF EXISTS change_log_new"_s, error)
              && exec(createStatement(u"change_log"_s, u"change_log_new"_s), error);
    QSqlQuery read(connection());
    read.setForwardOnly(true);
    QSqlQuery write(connection());
    write.prepare(u"INSERT INTO change_log_new (origin, seq, time, entity, row_id, data) VALUES (?, ?, ?, ?, ?, ?)"_s);
    struct Group
    {
        int origin = 0;
        qint64 seq = 0;
        qint64 firstTime = 0;
        qint64 time = 0;
        QString entity;
        QString rowId;
        QString data;          // as read (single row)
        QJsonObject merged;    // several rows
        QList<qint64> times;   // of the rows merged
    } g;
    const auto flush = [&]() {
        if (g.origin == 0)
            return true;
        write.addBindValue(g.origin);
        write.addBindValue(g.seq);
        write.addBindValue(g.time);
        write.addBindValue(g.entity);
        write.addBindValue(g.rowId);
        write.addBindValue(g.times.size() > 1 ? compactJson(g.merged) : g.data);
        for (const qint64 t : std::as_const(g.times)) {
            if (t != g.time)
                raised.insert({ g.origin, t }, g.time);
        }
        g = Group();
        return write.exec() || failed(write);
    };
    ok = ok && (read.exec(u"SELECT origin, seq, time, entity, row_id, data FROM change_log ORDER BY origin, seq"_s)
                || failed(read));
    while (ok && read.next()) {
        const int origin = read.value(0).toInt();
        const qint64 seq = read.value(1).toLongLong();
        const qint64 time = read.value(2).toLongLong();
        const QString entity = read.value(3).toString();
        const QString rowId = read.value(4).toString();
        const bool joins = g.origin == origin && seq == g.seq + 1 && g.entity == entity && g.rowId == rowId
                           && time >= g.time && time - g.firstTime <= kWindow;
        if (!joins) {
            ok = flush();
            g.origin = origin;
            g.firstTime = time;
            g.entity = entity;
            g.rowId = rowId;
            g.data = read.value(5).toString();
            g.merged = QJsonDocument::fromJson(g.data.toUtf8()).object();
        } else {
            const QJsonObject more = QJsonDocument::fromJson(read.value(5).toString().toUtf8()).object();
            for (auto it = more.constBegin(); it != more.constEnd(); ++it)
                g.merged.insert(it.key(), it.value());
        }
        g.seq = seq;
        g.time = time;
        g.times.append(time);
    }
    ok = ok && flush();
    read.finish();
    ok = ok && exec(u"DROP TABLE change_log"_s, error) && exec(u"ALTER TABLE change_log_new RENAME TO change_log"_s, error);

    // 2. One clock per record ('') for the fields that share it, a row of their own
    //    only for fields changed later
    ok = ok && exec(u"DROP TABLE IF EXISTS field_clock_new"_s, error)
         && exec(createStatement(u"field_clock"_s, u"field_clock_new"_s), error);
    write.prepare(u"INSERT INTO field_clock_new (entity, row_id, field, time, origin) VALUES (?, ?, ?, ?, ?)"_s);
    using Stamp = QPair<qint64, int>;   // time, device number
    QString entity;
    QString rowId;
    QHash<QString, Stamp> clocks;
    const auto flushRecord = [&]() {
        if (rowId.isEmpty())
            return true;
        QHash<QString, Stamp> rows = clocks;
        if (const EntityDef *def = Schema::entity(entity)) {
            QStringList fields = def->fieldNames();
            fields << u"deleted"_s;
            const bool hasRecord = rows.contains(QString());
            const Stamp record = rows.value(QString());
            QHash<Stamp, int> count;
            bool all = true;
            for (const QString &f : std::as_const(fields)) {
                if (!rows.contains(f) && !hasRecord) {
                    all = false;
                    break;
                }
                ++count[rows.value(f, record)];
            }
            if (all) {
                // The clock most fields share becomes the record's
                Stamp best;
                int most = 0;
                for (auto it = count.cbegin(); it != count.cend(); ++it) {
                    if (it.value() > most || (it.value() == most && best < it.key())) {
                        best = it.key();
                        most = it.value();
                    }
                }
                QHash<QString, Stamp> compact;
                compact.insert(QString(), best);
                for (const QString &f : std::as_const(fields)) {
                    const Stamp own = rows.value(f, record);
                    if (own != best)
                        compact.insert(f, own);
                }
                for (auto it = rows.cbegin(); it != rows.cend(); ++it) {
                    if (!it.key().isEmpty() && !fields.contains(it.key()))
                        compact.insert(it.key(), it.value());   // fields of a newer version
                }
                rows = compact;
            }
        }
        for (auto it = rows.cbegin(); it != rows.cend(); ++it) {
            write.addBindValue(entity);
            write.addBindValue(rowId);
            write.addBindValue(it.key().isNull() ? u""_s : it.key());
            write.addBindValue(it.value().first);
            write.addBindValue(it.value().second);
            if (!write.exec())
                return failed(write);
        }
        clocks.clear();
        return true;
    };
    ok = ok && (read.exec(u"SELECT entity, row_id, field, time, origin FROM field_clock ORDER BY entity, row_id"_s)
                || failed(read));
    while (ok && read.next()) {
        const QString e = read.value(0).toString();
        const QString r = read.value(1).toString();
        if (e != entity || r != rowId) {
            ok = flushRecord();
            entity = e;
            rowId = r;
        }
        const int origin = read.value(4).toInt();
        const qint64 time = read.value(3).toLongLong();
        const QString field = read.value(2).toString();
        clocks.insert(field.isEmpty() ? QString() : field, { raised.value({ origin, time }, time), origin });
    }
    ok = ok && flushRecord();
    read.finish();
    ok = ok && exec(u"DROP TABLE field_clock"_s, error) && exec(u"ALTER TABLE field_clock_new RENAME TO field_clock"_s, error);

    if (!ok) {
        rollback();
        return false;
    }
    return commit();
}

QString Database::meta(const QString &key, const QString &fallback) const
{
    QSqlQuery q(connection());
    q.prepare(u"SELECT value FROM meta WHERE key = ?"_s);
    q.addBindValue(key);
    if (q.exec() && q.next())
        return q.value(0).toString();
    return fallback;
}

bool Database::setMeta(const QString &key, const QString &value)
{
    QSqlQuery q(connection());
    q.prepare(u"INSERT INTO meta (key, value) VALUES (?, ?) ON CONFLICT(key) DO UPDATE SET value = excluded.value"_s);
    q.addBindValue(key);
    q.addBindValue(value);
    return q.exec();
}

void Database::persistClock()
{
    if (!QSqlDatabase::contains(m_connectionName) || !connection().isOpen())
        return;
    setMeta(u"hlc_ms"_s, QString::number(m_clock.lastMs()));
    setMeta(u"hlc_counter"_s, QString::number(m_clock.counter()));
}

qint64 Database::nextLocalSeq()
{
    return ++m_localSeq;
}

bool Database::transaction()
{
    if (m_txDepth++ == 0)
        return connection().transaction();
    return true;
}

bool Database::commit()
{
    if (m_txDepth == 0)
        return false;
    if (--m_txDepth == 0)
        return connection().commit();
    return true;
}

void Database::rollback()
{
    if (m_txDepth > 0) {
        m_txDepth = 0;
        connection().rollback();
        // Numbers given out in the transaction are gone again
        m_originNumbers.clear();
        m_originIds.clear();
    }
}

bool Database::backupTo(const QString &file, QString *error) const
{
    if (QFileInfo::exists(file) && !QFile::remove(file)) {
        if (error)
            *error = QStringLiteral("Could not replace %1.").arg(file);
        return false;
    }
    QSqlQuery q(connection());
    q.prepare(u"VACUUM INTO ?"_s);
    q.addBindValue(QDir::toNativeSeparators(file));
    if (!q.exec()) {
        if (error)
            *error = q.lastError().text();
        return false;
    }
    return true;
}

bool Database::resetDeviceId()
{
    m_localSeq = 0;
    m_deviceId = newId();
    const bool ok = setMeta(u"device_id"_s, m_deviceId);
    if (connection().tables().contains(u"origin"_s))
        m_localOrigin = originNumber(m_deviceId);
    return ok;
}

int Database::originNumber(const QString &deviceId, bool create)
{
    if (deviceId.isEmpty())
        return 0;
    if (const auto it = m_originNumbers.constFind(deviceId); it != m_originNumbers.constEnd())
        return *it;
    QSqlQuery q(connection());
    const auto lookup = [&q, &deviceId] {
        q.prepare(u"SELECT num FROM origin WHERE id = ?"_s);
        q.addBindValue(deviceId);
        return q.exec() && q.next() ? q.value(0).toInt() : 0;
    };
    int number = lookup();
    if (number == 0 && create) {
        q.prepare(u"INSERT OR IGNORE INTO origin (id) VALUES (?)"_s);
        q.addBindValue(deviceId);
        q.exec();
        number = lookup();
    }
    if (number == 0)
        return 0;
    m_originNumbers.insert(deviceId, number);
    m_originIds.insert(number, deviceId);
    return number;
}

QString Database::originId(int number) const
{
    if (const auto it = m_originIds.constFind(number); it != m_originIds.constEnd())
        return *it;
    QSqlQuery q(connection());
    q.prepare(u"SELECT id FROM origin WHERE num = ?"_s);
    q.addBindValue(number);
    if (!q.exec() || !q.next())
        return {};
    const QString id = q.value(0).toString();
    m_originNumbers.insert(id, number);
    m_originIds.insert(number, id);
    return id;
}

bool Database::isEmpty() const
{
    QSqlQuery q(connection());
    q.exec(u"SELECT (SELECT COUNT(*) FROM student WHERE deleted = 0) + (SELECT COUNT(*) FROM lesson WHERE deleted = 0)"
           u" + (SELECT COUNT(*) FROM event WHERE deleted = 0)"_s);
    return !q.next() || q.value(0).toInt() == 0;
}

int Database::detectFormat(const QString &file, QString *description)
{
    if (!QFileInfo(file).isFile())
        return 0;
    const QString name = u"qupil-detect-"_s + QUuid::createUuid().toString(QUuid::WithoutBraces);
    int format = 0;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(u"QSQLITE"_s, name);
        db.setDatabaseName(file);
        db.setConnectOptions(u"QSQLITE_OPEN_READONLY"_s);
        if (db.open()) {
            const QStringList tables = db.tables();
            if (tables.contains(u"meta"_s) && tables.contains(u"change_log"_s)) {
                format = 2;
                if (description) {
                    QSqlQuery q(db);
                    q.exec(u"SELECT COUNT(*) FROM student WHERE deleted = 0"_s);
                    *description = q.next() ? QString::number(q.value(0).toInt()) : QString();
                }
            } else if (tables.contains(u"pupil"_s) && tables.contains(u"lesson"_s)) {
                format = 1;
                if (description) {
                    QSqlQuery q(db);
                    q.exec(u"SELECT COUNT(*) FROM pupil"_s);
                    *description = q.next() ? QString::number(q.value(0).toInt()) : QString();
                }
            }
            db.close();
        }
    }
    QSqlDatabase::removeDatabase(name);
    return format;
}

int Database::schemaVersionOf(const QString &file)
{
    if (!QFileInfo(file).isFile())
        return 0;
    const QString name = u"qupil-version-"_s + QUuid::createUuid().toString(QUuid::WithoutBraces);
    int version = 0;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(u"QSQLITE"_s, name);
        db.setDatabaseName(file);
        db.setConnectOptions(u"QSQLITE_OPEN_READONLY"_s);
        if (db.open()) {
            QSqlQuery q(db);
            if (q.exec(u"SELECT value FROM meta WHERE key = 'schema_version'"_s) && q.next())
                version = q.value(0).toInt();
            db.close();
        }
    }
    QSqlDatabase::removeDatabase(name);
    return version;
}

} // namespace Qupil
