// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// The practice book's data in the teacher's Qupil: tasks of coupled students and their
// coupling (see docs/student-app.md). Kept apart from DataService.cpp.

#include "DataService.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "Clock.h"
#include "Ids.h"
#include "Store.h"

using namespace Qt::StringLiterals;

namespace {

QStringList hintList(const QVariant &value)
{
    QStringList lines = value.typeId() == QMetaType::QStringList || value.typeId() == QMetaType::QVariantList
                            ? value.toStringList()
                            : value.toString().split(u'\n');
    QStringList out;
    for (QString &line : lines) {
        line = line.simplified();
        if (!line.isEmpty())
            out << line;
    }
    return out;
}

QVariantMap feedbackMap(const QString &json)
{
    return json.isEmpty() ? QVariantMap() : QJsonDocument::fromJson(json.toUtf8()).object().toVariantMap();
}

} // namespace

QVariantMap DataService::coupling(const QString &studentId) const
{
    const QVariantList r = rows(u"SELECT * FROM coupling WHERE deleted = 0 AND student_id = ? AND ended = '' "
                                u"ORDER BY created DESC, id DESC LIMIT 1"_s, { studentId });
    return r.isEmpty() ? QVariantMap() : r.first().toMap();
}

bool DataService::isCoupled(const QString &studentId) const
{
    return !coupling(studentId).isEmpty();
}

QVariantList DataService::tasks(const QString &membershipId) const
{
    const QStringList all = together(membershipId);
    QVariantList out;
    for (const QVariant &v : rows(u"SELECT * FROM task WHERE deleted = 0 AND membership_id IN (%1) "
                                  u"ORDER BY date DESC, position, id"_s.arg(placeholders(all.size())),
                                  QVariantList(all.cbegin(), all.cend()))) {
        QVariantMap t = v.toMap();
        t.insert(u"hints"_s, hintList(t.value(u"hints"_s)));
        t.insert(u"feedback"_s, feedbackMap(t.value(u"feedback"_s).toString()));
        if (t.value(u"since"_s).toString().isEmpty())
            t.insert(u"since"_s, t.value(u"date"_s));
        const QString key = t.value(u"group_key"_s).toString();
        t.insert(u"shared"_s, key.isEmpty() ? 1 : scalar(u"SELECT COUNT(*) FROM task WHERE deleted = 0 AND group_key = ?"_s, { key }).toInt());
        out << t;
    }
    return out;
}

QVariantList DataService::tasksOn(const QString &membershipId, const QString &date) const
{
    QVariantList out;
    for (const QVariant &v : tasks(membershipId))
        if (v.toMap().value(u"date"_s).toString() == date)
            out << v;
    return out;
}

QVariantList DataService::openTasksBefore(const QString &membershipId, const QString &date) const
{
    QVariantList out;
    for (const QVariant &v : tasks(membershipId)) {
        const QVariantMap t = v.toMap();
        if (t.value(u"date"_s).toString() < date && t.value(u"closed"_s).toString().isEmpty())
            out << v;
    }
    // Oldest first, as they were given
    std::reverse(out.begin(), out.end());
    return out;
}

bool DataService::saveTasks(const QString &membershipId, const QString &date, const QVariantList &list, bool forAll)
{
    const QString own = forAll ? membershipId : membershipOn(membershipId, date);
    const QStringList targets = forAll ? activeMembershipsOfSameLesson(membershipId) : QStringList { own };
    const QString today = Qupil::Clock::today().toString(Qt::ISODate);

    m_store->beginBatch();
    bool ok = true;
    QSet<QString> kept;
    int position = 0;
    for (const QVariant &v : list) {
        const QVariantMap in = v.toMap();
        const QString title = in.value(u"title"_s).toString().simplified();
        const QStringList hints = hintList(in.value(u"hints"_s));
        if (title.isEmpty() && hints.isEmpty())
            continue;
        QVariantMap fields { { u"title"_s, title.isEmpty() ? hints.first() : title },
                             { u"hints"_s, (title.isEmpty() ? hints.mid(1) : hints).join(u'\n') },
                             { u"kind"_s, in.value(u"kind"_s).toInt() == 1 ? 1 : 0 },
                             { u"position"_s, position++ } };
        if (in.contains(u"closed"_s))
            fields.insert(u"closed"_s, in.value(u"closed"_s).toBool() ? today : QString());
        const QString id = in.value(u"id"_s).toString();
        if (!id.isEmpty()) {
            const QVariantMap existing = m_store->get(u"task"_s, id);
            if (existing.isEmpty() || existing.value(u"deleted"_s).toInt() != 0)
                continue;
            for (const QString &copy : forAll ? groupCopies(u"task"_s, id) : QStringList { id }) {
                QVariantMap f = fields;
                const QVariantMap c = m_store->get(u"task"_s, copy);
                // An open task of an earlier lesson moves on to this one
                if (c.value(u"date"_s).toString() < date) {
                    f.insert(u"date"_s, date);
                    if (c.value(u"since"_s).toString().isEmpty())
                        f.insert(u"since"_s, c.value(u"date"_s));
                    const QString m = membershipOn(c.value(u"membership_id"_s).toString(), date);
                    if (m != c.value(u"membership_id"_s).toString())
                        f.insert(u"membership_id"_s, m);
                }
                // Closed in an earlier lesson's copy: keep the day it was closed
                if (f.value(u"closed"_s).toString() == today && !c.value(u"closed"_s).toString().isEmpty())
                    f.insert(u"closed"_s, c.value(u"closed"_s));
                ok = ok && m_store->update(u"task"_s, copy, f);
                kept.insert(copy);
            }
            continue;
        }
        const QString key = targets.size() > 1 ? Qupil::newId() : QString();
        for (const QString &m : targets) {
            QVariantMap f = fields;
            f.insert(u"membership_id"_s, m);
            f.insert(u"group_key"_s, key);
            f.insert(u"date"_s, date);
            const QString created = m_store->create(u"task"_s, f);
            ok = ok && !created.isEmpty();
            kept.insert(created);
        }
    }
    // Tasks of this lesson that the editor no longer has
    for (const QString &m : targets) {
        const QStringList all = together(m);
        for (const QVariant &v : rows(u"SELECT id FROM task WHERE deleted = 0 AND membership_id IN (%1) AND date = ?"_s
                                          .arg(placeholders(all.size())),
                                      QVariantList(all.cbegin(), all.cend()) << date)) {
            const QString id = v.toMap().value(u"id"_s).toString();
            if (!kept.contains(id))
                ok = ok && m_store->remove(u"task"_s, id);
        }
    }
    return m_store->endBatch(ok) && ok;
}

bool DataService::closeTask(const QString &id, bool closed)
{
    return m_store->update(u"task"_s, id, { { u"closed"_s, closed ? Qupil::Clock::today().toString(Qt::ISODate) : QString() } });
}

bool DataService::deleteTask(const QString &id, bool forAll)
{
    m_store->beginBatch();
    bool ok = true;
    for (const QString &copy : forAll ? groupCopies(u"task"_s, id) : QStringList { id })
        ok = ok && m_store->remove(u"task"_s, copy);
    return m_store->endBatch(ok) && ok;
}

bool DataService::applyFeedback(const QString &studentId, const QVariantMap &feedback, const QVariantList &questions)
{
    const QVariantMap c = coupling(studentId);
    if (c.isEmpty())
        return false;
    // Only tasks of this student
    QSet<QString> own;
    for (const QVariant &v : rows(u"SELECT t.id FROM task t JOIN membership m ON m.id = t.membership_id "
                                  u"WHERE t.deleted = 0 AND m.student_id = ?"_s, { studentId }))
        own.insert(v.toMap().value(u"id"_s).toString());
    m_store->beginBatch();
    bool ok = true;
    for (auto it = feedback.constBegin(); it != feedback.constEnd(); ++it) {
        if (!own.contains(it.key()))
            continue;
        const QVariantMap theirs = it.value().toMap();
        const QVariantMap mine = feedbackMap(m_store->get(u"task"_s, it.key()).value(u"feedback"_s).toString());
        if (!mine.isEmpty() && theirs.value(u"at"_s).toDouble() <= mine.value(u"at"_s).toDouble())
            continue;
        ok = ok && m_store->update(u"task"_s, it.key(),
                                   { { u"feedback"_s, QString::fromUtf8(QJsonDocument(QJsonObject::fromVariantMap(theirs))
                                                                             .toJson(QJsonDocument::Compact)) } });
    }
    const QString q = QString::fromUtf8(QJsonDocument(QJsonArray::fromVariantList(questions)).toJson(QJsonDocument::Compact));
    if (q != c.value(u"questions"_s).toString())
        ok = ok && m_store->update(u"coupling"_s, c.value(u"id"_s).toString(), { { u"questions"_s, q } });
    return m_store->endBatch(ok) && ok;
}
