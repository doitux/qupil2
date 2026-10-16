// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "Store.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

#include <algorithm>

#include "Database.h"
#include "Ids.h"
#include "Schema.h"

using namespace Qt::StringLiterals;

namespace Qupil {

Store::Store(Database *db, QObject *parent)
    : QObject(parent)
    , m_db(db)
{
}

bool Store::fail(const QString &message)
{
    m_lastError = message;
    qWarning("Store: %s", qPrintable(message));
    return false;
}

QString Store::encodeFields(const QVariantMap &fields)
{
    return QString::fromUtf8(QJsonDocument(QJsonObject::fromVariantMap(fields)).toJson(QJsonDocument::Compact));
}

QVariantMap Store::decodeFields(const QString &json)
{
    return QJsonDocument::fromJson(json.toUtf8()).object().toVariantMap();
}

QString Store::encodeValue(const QVariant &value)
{
    QJsonArray a;
    a.append(QJsonValue::fromVariant(value));
    return QString::fromUtf8(QJsonDocument(a).toJson(QJsonDocument::Compact));
}

QVariant Store::decodeValue(const QString &json)
{
    const QJsonArray a = QJsonDocument::fromJson(json.toUtf8()).array();
    return a.isEmpty() ? QVariant() : a.first().toVariant();
}

// ------------------------------------------------------------------ batches

void Store::beginBatch()
{
    if (m_batchDepth++ == 0) {
        m_batchOk = true;
        m_db->transaction();
    }
}

bool Store::endBatch(bool commit)
{
    if (m_batchDepth == 0)
        return false;
    if (!commit)
        m_batchOk = false;
    if (--m_batchDepth > 0)
        return m_batchOk;
    bool ok = m_batchOk;
    if (ok) {
        ok = m_db->commit();
        m_db->persistClock();
    } else {
        m_db->rollback();
        m_pending.clear();
    }
    flushNotifications();
    return ok;
}

void Store::touched(const QString &entity)
{
    m_pending.insert(entity);
}

void Store::flushNotifications()
{
    if (m_pending.isEmpty())
        return;
    QStringList list(m_pending.cbegin(), m_pending.cend());
    std::sort(list.begin(), list.end());
    m_pending.clear();
    emit dataChanged(list);
}

void Store::notifyReset()
{
    QStringList all;
    for (const EntityDef &e : Schema::entities())
        all << e.name;
    emit dataChanged(all);
}

// ------------------------------------------------------------------ writes

namespace {

bool isInteger(const EntityDef *def, const QString &field)
{
    return field == u"deleted"_s || std::any_of(def->fields.cbegin(), def->fields.cend(), [&](const FieldDef &f) {
               return f.name == field && f.type == u"INTEGER"_s;
           });
}

//! A write with every field of the record (a new record): its clock becomes the record's
bool isComplete(const EntityDef *def, const QVariantMap &fields)
{
    if (!fields.contains(u"deleted"_s))
        return false;
    return std::all_of(def->fields.cbegin(), def->fields.cend(), [&](const FieldDef &f) { return fields.contains(f.name); });
}

} // namespace

bool Store::isNewer(const Clock &a, const Clock &b) const
{
    if (b.isNull())
        return true;
    if (a.time != b.time)
        return a.time > b.time;
    return a.origin != b.origin && m_db->originId(a.origin) > m_db->originId(b.origin);
}

QHash<QString, Store::Clock> Store::clocks(const QString &entity, const QString &id) const
{
    QHash<QString, Clock> out;
    QSqlQuery q(m_db->connection());
    q.prepare(u"SELECT field, time, origin FROM field_clock WHERE entity = ? AND row_id = ?"_s);
    q.addBindValue(entity);
    q.addBindValue(id);
    if (q.exec()) {
        while (q.next())
            out.insert(q.value(0).toString(), { q.value(1).toLongLong(), q.value(2).toInt() });
    }
    return out;
}

/*!
    The clocks after the fields were written: a complete write (a new record)
    that is newer than the record's clock becomes the record's clock, and the
    fields it wrote no longer need their own. Otherwise each field written
    gets its own clock.
*/
bool Store::setClocks(const EntityDef *def, const QString &id, const QStringList &written, bool complete,
                      const QHash<QString, Clock> &before, const Clock &clock)
{
    QSqlQuery q(m_db->connection());
    if (complete && isNewer(clock, before.value(QString()))) {
        q.prepare(u"INSERT INTO field_clock (entity, row_id, field, time, origin) VALUES (?, ?, '', ?, ?) "
                  u"ON CONFLICT(entity, row_id, field) DO UPDATE SET time = excluded.time, origin = excluded.origin"_s);
        q.addBindValue(def->name);
        q.addBindValue(id);
        q.addBindValue(clock.time);
        q.addBindValue(clock.origin);
        if (!q.exec())
            return fail(q.lastError().text());
        q.prepare(u"DELETE FROM field_clock WHERE entity = ? AND row_id = ? AND field = ?"_s);
        for (const QString &field : written) {
            if (!before.contains(field))
                continue;
            q.addBindValue(def->name);
            q.addBindValue(id);
            q.addBindValue(field);
            if (!q.exec())
                return fail(q.lastError().text());
        }
        return true;
    }
    q.prepare(u"INSERT INTO field_clock (entity, row_id, field, time, origin) VALUES (?, ?, ?, ?, ?) "
              u"ON CONFLICT(entity, row_id, field) DO UPDATE SET time = excluded.time, origin = excluded.origin"_s);
    for (const QString &field : written) {
        q.addBindValue(def->name);
        q.addBindValue(id);
        q.addBindValue(field);
        q.addBindValue(clock.time);
        q.addBindValue(clock.origin);
        if (!q.exec())
            return fail(q.lastError().text());
    }
    return true;
}

bool Store::writeFields(const EntityDef *def, const QString &id, const QVariantMap &fields)
{
    QSqlQuery q(m_db->connection());
    q.prepare(u"INSERT OR IGNORE INTO %1 (id) VALUES (?)"_s.arg(def->name));
    q.addBindValue(id);
    if (!q.exec())
        return fail(q.lastError().text());
    if (fields.isEmpty())
        return true;
    // NULL never reaches a column: text fields fall back to '', numbers to 0
    QStringList set;
    for (auto it = fields.cbegin(); it != fields.cend(); ++it)
        set << u"%1 = COALESCE(?, %2)"_s.arg(it.key(), isInteger(def, it.key()) ? u"0"_s : u"''"_s);
    q.prepare(u"UPDATE %1 SET %2 WHERE id = ?"_s.arg(def->name, set.join(u", "_s)));
    for (auto it = fields.cbegin(); it != fields.cend(); ++it)
        q.addBindValue(it.value());
    q.addBindValue(id);
    if (!q.exec())
        return fail(q.lastError().text());
    return true;
}

bool Store::logChange(int origin, const Change &c)
{
    QSqlQuery q(m_db->connection());
    q.prepare(u"INSERT OR IGNORE INTO change_log (origin, seq, time, entity, row_id, data) VALUES (?, ?, ?, ?, ?, ?)"_s);
    q.addBindValue(origin);
    q.addBindValue(c.seq);
    q.addBindValue(c.time);
    q.addBindValue(c.entity);
    q.addBindValue(c.rowId);
    q.addBindValue(encodeFields(c.fields));
    if (!q.exec())
        return fail(q.lastError().text());
    return true;
}

//! A write on this device: always the newest
bool Store::writeLocal(const EntityDef *def, const QString &id, const QVariantMap &fields)
{
    const Change c { m_db->deviceId(), m_db->nextLocalSeq(), m_db->clock().now(), def->name, id, fields };
    const QHash<QString, Clock> before = clocks(def->name, id);
    return writeFields(def, id, fields)
           && setClocks(def, id, fields.keys(), isComplete(def, fields), before, { c.time, m_db->localOrigin() })
           && logChange(m_db->localOrigin(), c);
}

QString Store::create(const QString &entity, const QVariantMap &fields)
{
    const EntityDef *def = Schema::entity(entity);
    if (!def) {
        fail(u"unknown entity "_s + entity);
        return {};
    }
    for (auto it = fields.cbegin(); it != fields.cend(); ++it) {
        if (it.key() != u"id"_s && !def->hasField(it.key())) {
            fail(u"unknown field %1.%2"_s.arg(entity, it.key()));
            return {};
        }
    }
    const QString id = fields.value(u"id"_s).toString().isEmpty() ? newId() : fields.value(u"id"_s).toString();

    // Every field, also defaults, so the record is complete on other devices
    QVariantMap values;
    for (const FieldDef &f : def->fields) {
        values.insert(f.name, fields.contains(f.name) ? fields.value(f.name)
                                                      : (f.type == u"INTEGER"_s ? QVariant(f.defaultValue.toLongLong())
                                                                                : QVariant(QString())));
    }
    values.insert(u"deleted"_s, 0);

    beginBatch();
    const bool ok = writeLocal(def, id, values);
    touched(entity);
    if (!ok) {
        endBatch(false);
        return {};
    }
    return endBatch() ? id : QString();
}

bool Store::update(const QString &entity, const QString &id, const QVariantMap &fields)
{
    const EntityDef *def = Schema::entity(entity);
    if (!def)
        return fail(u"unknown entity "_s + entity);
    const QVariantMap current = get(entity, id);
    if (current.isEmpty())
        return fail(u"%1 %2 does not exist"_s.arg(entity, id));

    QVariantMap changed;
    for (auto it = fields.cbegin(); it != fields.cend(); ++it) {
        if (!def->hasField(it.key()))
            return fail(u"unknown field %1.%2"_s.arg(entity, it.key()));
        const QVariant old = current.value(it.key());
        const QVariant &v = it.value();
        const bool same = (old.typeId() == QMetaType::LongLong || old.typeId() == QMetaType::Int)
                              ? old.toLongLong() == v.toLongLong()
                              : old.toString() == v.toString();
        if (!same)
            changed.insert(it.key(), v);
    }
    if (changed.isEmpty())
        return true;

    beginBatch();
    const bool ok = writeLocal(def, id, changed);
    touched(entity);
    return endBatch(ok) && ok;
}

bool Store::remove(const QString &entity, const QString &id)
{
    return update(entity, id, { { u"deleted"_s, 1 } });
}

// ------------------------------------------------------------------ reads

static QVariantMap recordToMap(const QSqlQuery &q)
{
    QVariantMap m;
    const QSqlRecord r = q.record();
    for (int i = 0; i < r.count(); ++i)
        m.insert(r.fieldName(i), q.value(i));
    return m;
}

QVariantMap Store::get(const QString &entity, const QString &id) const
{
    if (!Schema::entity(entity))
        return {};
    QSqlQuery q(m_db->connection());
    q.prepare(u"SELECT * FROM %1 WHERE id = ?"_s.arg(entity));
    q.addBindValue(id);
    if (q.exec() && q.next())
        return recordToMap(q);
    return {};
}

QList<QVariantMap> Store::select(const QString &entity, const QString &where, const QVariantList &binds,
                                 const QString &orderBy) const
{
    QList<QVariantMap> rows;
    if (!Schema::entity(entity))
        return rows;
    QString sql = u"SELECT * FROM %1 WHERE deleted = 0"_s.arg(entity);
    if (!where.isEmpty())
        sql += u" AND ("_s + where + u")"_s;
    if (!orderBy.isEmpty())
        sql += u" ORDER BY "_s + orderBy;
    QSqlQuery q(m_db->connection());
    q.prepare(sql);
    for (const QVariant &b : binds)
        q.addBindValue(b);
    if (!q.exec()) {
        qWarning("Store::select: %s", qPrintable(q.lastError().text()));
        return rows;
    }
    while (q.next())
        rows << recordToMap(q);
    return rows;
}

// ------------------------------------------------------------------ sync

VersionVector Store::versionVector() const
{
    VersionVector v;
    QSqlQuery q(m_db->connection());
    q.exec(u"SELECT id, (SELECT MAX(seq) FROM change_log WHERE change_log.origin = origin.num) FROM origin"_s);
    while (q.next()) {
        if (!q.value(1).isNull())
            v.insert(q.value(0).toString(), q.value(1).toLongLong());
    }
    return v;
}

QList<Change> Store::changesSince(const VersionVector &known, int limit) const
{
    QList<Change> out;
    const VersionVector mine = versionVector();
    for (auto it = mine.cbegin(); it != mine.cend(); ++it) {
        if (it.value() > known.value(it.key(), 0))
            out += changesOf(it.key(), known.value(it.key(), 0), limit);
    }
    std::sort(out.begin(), out.end(), [](const Change &a, const Change &b) { return b.isNewerThan(a.time, a.origin); });
    if (out.size() > limit)
        out.resize(limit);
    return out;
}

QList<Change> Store::changesOf(const QString &origin, qint64 afterSeq, int limit) const
{
    QList<Change> out;
    const int number = m_db->originNumber(origin, false);
    if (number == 0)
        return out;
    QSqlQuery q(m_db->connection());
    q.prepare(u"SELECT seq, time, entity, row_id, data FROM change_log WHERE origin = ? AND seq > ? ORDER BY seq LIMIT ?"_s);
    q.addBindValue(number);
    q.addBindValue(afterSeq);
    q.addBindValue(limit);
    q.exec();
    while (q.next()) {
        out.append({ origin, q.value(0).toLongLong(), q.value(1).toLongLong(), q.value(2).toString(),
                     q.value(3).toString(), decodeFields(q.value(4).toString()) });
    }
    return out;
}

int Store::applyRemote(const QList<Change> &changes)
{
    QList<Change> sorted = changes;
    std::sort(sorted.begin(), sorted.end(), [](const Change &a, const Change &b) { return b.isNewerThan(a.time, a.origin); });

    int applied = 0;
    beginBatch();
    bool ok = true;
    QSqlQuery seen(m_db->connection());
    seen.prepare(u"SELECT 1 FROM change_log WHERE origin = ? AND seq = ?"_s);
    for (const Change &c : std::as_const(sorted)) {
        if (c.origin.isEmpty() || c.seq <= 0)
            continue;
        const int origin = m_db->originNumber(c.origin);
        // Already known? Then nothing to do.
        seen.addBindValue(origin);
        seen.addBindValue(c.seq);
        if (seen.exec() && seen.next())
            continue;

        m_db->clock().observe(c.time);
        if (!(ok = logChange(origin, c)))
            break;
        const EntityDef *def = Schema::entity(c.entity);
        if (!def || c.rowId.isEmpty())
            continue;   // from a newer version: kept in the log for relaying, ignored here

        // Field by field: only what is newer than our value
        const Clock clock { c.time, origin };
        const QHash<QString, Clock> before = clocks(c.entity, c.rowId);
        const Clock record = before.value(QString());
        QVariantMap newer;
        for (auto it = c.fields.cbegin(); it != c.fields.cend(); ++it) {
            if (def->hasField(it.key()) && isNewer(clock, before.value(it.key(), record)))
                newer.insert(it.key(), it.value());
        }
        if (newer.isEmpty())
            continue;
        if (!(ok = writeFields(def, c.rowId, newer)
                   && setClocks(def, c.rowId, newer.keys(), isComplete(def, c.fields), before, clock)))
            break;
        touched(c.entity);
        ++applied;
    }
    endBatch(ok);
    return ok ? applied : -1;
}

} // namespace Qupil
