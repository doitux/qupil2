// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QHash>
#include <QList>
#include <QObject>
#include <QSet>
#include <QVariantMap>

namespace Qupil {

class Database;

//! One field change, as stored in the change log and exchanged between devices
struct Change
{
    QString origin;     // device that made the change
    qint64 seq = 0;     // per-origin sequence number
    QString hlc;        // hybrid-clock timestamp
    QString entity;
    QString rowId;
    QString field;
    QVariant value;

    bool operator==(const Change &o) const { return origin == o.origin && seq == o.seq; }
};

//! Highest sequence number known per origin device
using VersionVector = QHash<QString, qint64>;

/*!
    All writes to synchronised data. Each changed field is written to the table,
    to the change log and to the field clock in one transaction, so any device
    can later merge the change field by field ("newest timestamp wins").
*/
class Store : public QObject
{
    Q_OBJECT

public:
    explicit Store(Database *db, QObject *parent = nullptr);

    Database *database() const { return m_db; }

    //! New record; returns its id (taken from fields["id"] if given)
    QString create(const QString &entity, const QVariantMap &fields);
    //! Changes only the fields whose value differs; returns false on error
    bool update(const QString &entity, const QString &id, const QVariantMap &fields);
    //! Marks the record as deleted (tombstone, synchronised like any other field)
    bool remove(const QString &entity, const QString &id);

    QVariantMap get(const QString &entity, const QString &id) const;
    //! Non-deleted rows; where/order are SQL fragments with ? placeholders
    QList<QVariantMap> select(const QString &entity, const QString &where = {},
                              const QVariantList &binds = {}, const QString &orderBy = {}) const;

    //! Groups many writes into one transaction and one notification
    void beginBatch();
    bool endBatch(bool commit = true);

    // ------------------------------------------------------------- sync
    VersionVector versionVector() const;
    //! Changes the other side has not seen yet, oldest first
    QList<Change> changesSince(const VersionVector &known, int limit = 5000) const;
    //! Changes of one device after a sequence number, in order
    QList<Change> changesOf(const QString &origin, qint64 afterSeq, int limit = 5000) const;
    //! Merges changes from another device; returns how many changed local data
    int applyRemote(const QList<Change> &changes);

    static QString encodeValue(const QVariant &value);
    static QVariant decodeValue(const QString &json);

    QString lastError() const { return m_lastError; }

    //! Tells all views to reload, e.g. after the database file was replaced
    void notifyReset();

signals:
    //! Data of these entities changed (local or remote); views reload
    void dataChanged(const QStringList &entities);

private:
    bool writeField(const QString &entity, const QString &id, const QString &field,
                    const QVariant &value, const QString &hlc);
    bool ensureRow(const QString &entity, const QString &id);
    bool logChange(const Change &c);
    void touched(const QString &entity);
    void flushNotifications();
    bool fail(const QString &message);

    Database *m_db;
    int m_batchDepth = 0;
    bool m_batchOk = true;
    QSet<QString> m_pending;
    QString m_lastError;
};

} // namespace Qupil
