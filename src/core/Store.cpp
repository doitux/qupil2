// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "Store.h"

#include <QJsonArray>
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

// ------------------------------------------------------------------ writes

bool Store::ensureRow(const QString &entity, const QString &id)
{
    QSqlQuery q(m_db->connection());
    q.prepare(u"INSERT OR IGNORE INTO %1 (id) VALUES (?)"_s.arg(entity));
    q.addBindValue(id);
    if (!q.exec())
        return fail(q.lastError().text());
    return true;
}

bool Store::logChange(const Change &c)
{
    QSqlQuery q(m_db->connection());
    q.prepare(u"INSERT OR IGNORE INTO change_log (origin, seq, hlc, entity, row_id, field, value) VALUES (?, ?, ?, ?, ?, ?, ?)"_s);
    q.addBindValue(c.origin);
    q.addBindValue(c.seq);
    q.addBindValue(c.hlc);
    q.addBindValue(c.entity);
    q.addBindValue(c.rowId);
    q.addBindValue(c.field);
    q.addBindValue(encodeValue(c.value));
    if (!q.exec())
        return fail(q.lastError().text());
    return true;
}

bool Store::writeField(const QString &entity, const QString &id, const QString &field,
                       const QVariant &value, const QString &hlc)
{
    // NULL never reaches a column: text fields fall back to '', numbers to 0
    const EntityDef *def = Schema::entity(entity);
    const bool integer = field == u"deleted"_s
                         || (def && std::any_of(def->fields.cbegin(), def->fields.cend(), [&](const FieldDef &f) {
                                return f.name == field && f.type == u"INTEGER"_s;
                            }));
    QSqlQuery q(m_db->connection());
    q.prepare(u"UPDATE %1 SET %2 = COALESCE(?, %3) WHERE id = ?"_s.arg(entity, field, integer ? u"0"_s : u"''"_s));
    q.addBindValue(value);
    q.addBindValue(id);
    if (!q.exec())
        return fail(q.lastError().text());

    QSqlQuery c(m_db->connection());
    c.prepare(u"INSERT INTO field_clock (entity, row_id, field, hlc) VALUES (?, ?, ?, ?) "
              u"ON CONFLICT(entity, row_id, field) DO UPDATE SET hlc = excluded.hlc"_s);
    c.addBindValue(entity);
    c.addBindValue(id);
    c.addBindValue(field);
    c.addBindValue(hlc);
    if (!c.exec())
        return fail(c.lastError().text());
    return true;
}

QString Store::create(const QString &entity, const QVariantMap &fields)
{
    const EntityDef *def = Schema::entity(entity);
    if (!def) {
        fail(u"unknown entity "_s + entity);
        return {};
    }
    const QString id = fields.value(u"id"_s).toString().isEmpty() ? newId() : fields.value(u"id"_s).toString();

    beginBatch();
    bool ok = ensureRow(entity, id);
    // Log every field, also defaults, so the record is complete on other devices
    for (const FieldDef &f : def->fields) {
        if (!ok)
            break;
        QVariant v = fields.contains(f.name) ? fields.value(f.name)
                                             : (f.type == u"INTEGER"_s ? QVariant(f.defaultValue.toLongLong())
                                                                       : QVariant(QString()));
        const QString hlc = m_db->clock().now();
        ok = writeField(entity, id, f.name, v, hlc)
             && logChange({ m_db->deviceId(), m_db->nextLocalSeq(), hlc, entity, id, f.name, v });
    }
    if (ok) {
        const QString hlc = m_db->clock().now();
        ok = writeField(entity, id, u"deleted"_s, 0, hlc)
             && logChange({ m_db->deviceId(), m_db->nextLocalSeq(), hlc, entity, id, u"deleted"_s, 0 });
    }
    for (auto it = fields.cbegin(); ok && it != fields.cend(); ++it) {
        if (it.key() != u"id"_s && !def->hasField(it.key()))
            ok = fail(u"unknown field %1.%2"_s.arg(entity, it.key()));
    }
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

    beginBatch();
    bool ok = true;
    bool any = false;
    for (auto it = fields.cbegin(); ok && it != fields.cend(); ++it) {
        if (!def->hasField(it.key())) {
            ok = fail(u"unknown field %1.%2"_s.arg(entity, it.key()));
            break;
        }
        const QVariant old = current.value(it.key());
        const QVariant &v = it.value();
        const bool same = (old.typeId() == QMetaType::LongLong || old.typeId() == QMetaType::Int)
                              ? old.toLongLong() == v.toLongLong()
                              : old.toString() == v.toString();
        if (same)
            continue;
        const QString hlc = m_db->clock().now();
        ok = writeField(entity, id, it.key(), v, hlc)
             && logChange({ m_db->deviceId(), m_db->nextLocalSeq(), hlc, entity, id, it.key(), v });
        any = true;
    }
    if (any)
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
    q.exec(u"SELECT origin, MAX(seq) FROM change_log GROUP BY origin"_s);
    while (q.next())
        v.insert(q.value(0).toString(), q.value(1).toLongLong());
    return v;
}

QList<Change> Store::changesSince(const VersionVector &known, int limit) const
{
    QList<Change> out;
    const VersionVector mine = versionVector();
    for (auto it = mine.cbegin(); it != mine.cend(); ++it) {
        const qint64 from = known.value(it.key(), 0);
        if (it.value() <= from)
            continue;
        QSqlQuery q(m_db->connection());
        q.prepare(u"SELECT origin, seq, hlc, entity, row_id, field, value FROM change_log "
                  u"WHERE origin = ? AND seq > ? ORDER BY seq LIMIT ?"_s);
        q.addBindValue(it.key());
        q.addBindValue(from);
        q.addBindValue(limit);
        q.exec();
        while (q.next()) {
            out.append({ q.value(0).toString(), q.value(1).toLongLong(), q.value(2).toString(),
                         q.value(3).toString(), q.value(4).toString(), q.value(5).toString(),
                         decodeValue(q.value(6).toString()) });
        }
    }
    std::sort(out.begin(), out.end(), [](const Change &a, const Change &b) { return a.hlc < b.hlc; });
    if (out.size() > limit)
        out.resize(limit);
    return out;
}

int Store::applyRemote(const QList<Change> &changes)
{
    QList<Change> sorted = changes;
    std::sort(sorted.begin(), sorted.end(), [](const Change &a, const Change &b) { return a.hlc < b.hlc; });

    int applied = 0;
    beginBatch();
    bool ok = true;
    for (const Change &c : sorted) {
        const EntityDef *def = Schema::entity(c.entity);
        if (!def || !def->hasField(c.field) || c.rowId.isEmpty())
            continue;   // from a newer version: keep it in the log for relaying, ignore locally

        // Already known? Then nothing to do.
        QSqlQuery seen(m_db->connection());
        seen.prepare(u"SELECT 1 FROM change_log WHERE origin = ? AND seq = ?"_s);
        seen.addBindValue(c.origin);
        seen.addBindValue(c.seq);
        if (seen.exec() && seen.next())
            continue;

        m_db->clock().observe(c.hlc);
        if (!(ok = logChange(c)))
            break;

        QSqlQuery clock(m_db->connection());
        clock.prepare(u"SELECT hlc FROM field_clock WHERE entity = ? AND row_id = ? AND field = ?"_s);
        clock.addBindValue(c.entity);
        clock.addBindValue(c.rowId);
        clock.addBindValue(c.field);
        const QString localHlc = (clock.exec() && clock.next()) ? clock.value(0).toString() : QString();
        if (!localHlc.isEmpty() && localHlc >= c.hlc)
            continue;   // our value is newer

        if (!(ok = ensureRow(c.entity, c.rowId) && writeField(c.entity, c.rowId, c.field, c.value, c.hlc)))
            break;
        touched(c.entity);
        ++applied;
    }
    endBatch(ok);
    return ok ? applied : -1;
}

} // namespace Qupil
