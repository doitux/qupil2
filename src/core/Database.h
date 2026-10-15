// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QObject>
#include <QSqlDatabase>
#include <QString>
#include <QVariantMap>

#include "Ids.h"

namespace Qupil {

/*!
    The Qupil 2 database of this device: one SQLite file.

    Opening creates or migrates the schema and gives the database a device id.
    Writes to synchronised data go through Store, which also records every change.
*/
class Database : public QObject
{
    Q_OBJECT

public:
    explicit Database(QObject *parent = nullptr);
    ~Database() override;

    bool open(const QString &path, QString *error = nullptr);
    void close();
    bool isOpen() const;

    QString path() const { return m_path; }
    QSqlDatabase connection() const;

    QString deviceId() const { return m_deviceId; }
    HybridClock &clock() { return m_clock; }
    //! Saves the clock state; called by Store after each write batch
    void persistClock();

    QString meta(const QString &key, const QString &fallback = {}) const;
    bool setMeta(const QString &key, const QString &value);

    //! Next local change sequence number (persisted)
    qint64 nextLocalSeq();

    bool transaction();
    bool commit();
    void rollback();

    //! Consistent copy of the whole database into a new file (SQLite VACUUM INTO)
    bool backupTo(const QString &file, QString *error = nullptr) const;

    //! Gives this database a fresh device id (after a restore, so two devices never share one)
    bool resetDeviceId();

    //! True if the database contains no students, lessons or events yet
    bool isEmpty() const;

    //! Version info of a file without opening it as the live database:
    //! 0 = not a database, 1 = Qupil 1.x, 2 = Qupil 2
    static int detectFormat(const QString &file, QString *description = nullptr);
    //! Schema version stored in a Qupil 2 file (0 if unknown)
    static int schemaVersionOf(const QString &file);

private:
    bool createSchema(QString *error);
    bool migrate(int from, QString *error);
    bool exec(const QString &sql, QString *error = nullptr);

    QString m_connectionName;
    QString m_path;
    QString m_deviceId;
    HybridClock m_clock;
    int m_txDepth = 0;
    qint64 m_localSeq = 0;
};

} // namespace Qupil
