// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "Database.h"

#include <QDir>
#include <QFileInfo>
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
    m_clock.setDeviceId(m_deviceId);
    m_clock.restore(meta(u"hlc_ms"_s, u"0"_s).toLongLong(), meta(u"hlc_counter"_s, u"0"_s).toInt());

    // The change log is the truth for sequence numbers and the clock, even after a crash
    QSqlQuery q(connection());
    q.prepare(u"SELECT MAX(seq), MAX(hlc) FROM change_log WHERE origin = ?"_s);
    q.addBindValue(m_deviceId);
    if (q.exec() && q.next()) {
        m_localSeq = q.value(0).toLongLong();
        qint64 ms = 0;
        int counter = 0;
        if (HybridClock::parse(q.value(1).toString(), &ms, &counter) && ms >= m_clock.lastMs())
            m_clock.restore(ms, counter);
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
    // One step per version (1 is the first)
    const QList<QPair<int, QString>> steps = {
        { 2, u"ALTER TABLE library_item ADD COLUMN number INTEGER NOT NULL DEFAULT 0"_s },
    };
    for (const auto &[version, sql] : steps) {
        if (from >= version)
            continue;
        QSqlQuery q(connection());
        if (!q.exec(sql)) {
            if (error)
                *error = q.lastError().text();
            return false;
        }
    }
    return setMeta(u"schema_version"_s, QString::number(Schema::Version));
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
    m_clock.setDeviceId(m_deviceId);
    return setMeta(u"device_id"_s, m_deviceId);
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
