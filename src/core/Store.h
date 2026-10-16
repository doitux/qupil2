// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QHash>
#include <QList>
#include <QObject>
#include <QSet>
#include <QStringList>
#include <QVariantMap>

namespace Qupil {

class Database;
struct EntityDef;

//! One write of a record (a new record or changed fields), as stored in the
//! change log and exchanged between devices
struct Change
{
    QString origin;       // device that made the change
    qint64 seq = 0;       // per-origin sequence number
    qint64 time = 0;      // hybrid clock (HybridClock)
    QString entity;
    QString rowId;
    QVariantMap fields;   // the new values

    //! Order of all changes, the same on every device: later time first, the device id decides a tie
    bool isNewerThan(qint64 otherTime, const QString &otherOrigin) const
    {
        return time != otherTime ? time > otherTime : origin > otherOrigin;
    }
};

//! Highest sequence number known per origin device
using VersionVector = QHash<QString, qint64>;

/*!
    All writes to synchronised data. A write goes to the table, to the change
    log and to the clock in one transaction, so any device can later merge it
    field by field ("newest time wins").

    The clock of a record is kept once for the whole record (when it was
    created) and once more only for each field changed after that.
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
    /*!
        Replaces a value by another form of the same content (packed, a newer
        layout) in the table and in the change log, without a new change: every
        device makes that form itself, so nothing needs to travel and the old
        form does not stay in the log. Only where the value is still from.
    */
    bool rewrite(const QString &entity, const QString &id, const QString &field, const QString &from,
                 const QString &to);

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

    //! Field values as stored in the change log ({"field": value, …})
    static QString encodeFields(const QVariantMap &fields);
    static QVariantMap decodeFields(const QString &json);
    //! One value as "[value]" (sync format 1)
    static QString encodeValue(const QVariant &value);
    static QVariant decodeValue(const QString &json);

    QString lastError() const { return m_lastError; }

    //! Tells all views to reload, e.g. after the database file was replaced
    void notifyReset();

signals:
    //! Data of these entities changed (local or remote); views reload
    void dataChanged(const QStringList &entities);

private:
    //! When a value was written and by which device (local number)
    struct Clock
    {
        qint64 time = 0;
        int origin = 0;
        bool isNull() const { return origin == 0; }
    };
    bool isNewer(const Clock &a, const Clock &b) const;
    //! Clocks of a record: field '' is the record's own
    QHash<QString, Clock> clocks(const QString &entity, const QString &id) const;
    bool setClocks(const EntityDef *def, const QString &id, const QStringList &written, bool complete,
                   const QHash<QString, Clock> &before, const Clock &clock);
    bool writeFields(const EntityDef *def, const QString &id, const QVariantMap &fields);
    bool writeLocal(const EntityDef *def, const QString &id, const QVariantMap &fields);
    bool logChange(int origin, const Change &c);
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
