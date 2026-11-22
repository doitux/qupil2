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
#include "NoteFormat.h"
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

QVariantList DataService::couplings() const
{
    return rows(u"SELECT * FROM coupling WHERE deleted = 0 AND ended = '' ORDER BY created, id"_s);
}

QVariantList DataService::studentTasks(const QString &studentId) const
{
    QVariantList out;
    QSet<QString> seen;
    for (const QVariant &m : rows(u"SELECT id FROM membership WHERE deleted = 0 AND student_id = ?"_s, { studentId }))
        for (const QVariant &v : tasks(m.toMap().value(u"id"_s).toString()))
            if (!seen.contains(v.toMap().value(u"id"_s).toString())) {
                seen.insert(v.toMap().value(u"id"_s).toString());
                out << v;
            }
    return out;
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

QVariantList DataService::questions(const QString &studentId) const
{
    QVariantList out;
    const QJsonArray stored = QJsonDocument::fromJson(coupling(studentId).value(u"questions"_s).toString().toUtf8()).array();
    for (const QJsonValue &v : stored) {
        const QJsonObject o = v.toObject();
        if (o.value(u"text"_s).toString().trimmed().isEmpty())
            continue;
        const double at = o.value(u"at"_s).toDouble();
        QVariantMap q { { u"id"_s, o.value(u"id"_s).toString() }, { u"text"_s, o.value(u"text"_s).toString() },
                        { u"at"_s, at }, { u"read"_s, o.value(u"read"_s).toBool() } };
        if (at > 0)
            q.insert(u"date"_s, QDateTime::fromMSecsSinceEpoch(qint64(at)).date().toString(Qt::ISODate));
        out.prepend(q);   // stored oldest first
    }
    return out;
}

QVariantList DataService::openQuestions(const QString &studentId) const
{
    QVariantList out;
    for (const QVariant &v : questions(studentId))
        if (!v.toMap().value(u"read"_s).toBool())
            out.prepend(v);
    return out;
}

bool DataService::markQuestionsRead(const QString &studentId)
{
    const QVariantMap c = coupling(studentId);
    if (c.isEmpty())
        return false;
    QJsonArray stored = QJsonDocument::fromJson(c.value(u"questions"_s).toString().toUtf8()).array();
    bool changed = false;
    for (qsizetype i = 0; i < stored.size(); ++i) {
        QJsonObject o = stored.at(i).toObject();
        if (o.value(u"read"_s).toBool())
            continue;
        o.insert(u"read"_s, true);
        stored.replace(i, o);
        changed = true;
    }
    if (!changed)
        return true;
    return m_store->update(u"coupling"_s, c.value(u"id"_s).toString(),
                           { { u"questions"_s, QString::fromUtf8(QJsonDocument(stored).toJson(QJsonDocument::Compact)) } });
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
    // Read meanwhile (while the answers were being fetched): stays read
    QSet<QString> read;
    for (const QJsonValue &v : QJsonDocument::fromJson(c.value(u"questions"_s).toString().toUtf8()).array())
        if (v.toObject().value(u"read"_s).toBool())
            read.insert(v.toObject().value(u"id"_s).toString());
    QJsonArray merged = QJsonArray::fromVariantList(questions);
    for (qsizetype i = 0; i < merged.size(); ++i) {
        QJsonObject o = merged.at(i).toObject();
        if (read.contains(o.value(u"id"_s).toString()) && !o.value(u"read"_s).toBool()) {
            o.insert(u"read"_s, true);
            merged.replace(i, o);
        }
    }
    const QString q = QString::fromUtf8(QJsonDocument(merged).toJson(QJsonDocument::Compact));
    if (q != c.value(u"questions"_s).toString())
        ok = ok && m_store->update(u"coupling"_s, c.value(u"id"_s).toString(), { { u"questions"_s, q } });
    return m_store->endBatch(ok) && ok;
}

QJsonObject DataService::bookletContent(const QString &studentId) const
{
    const QVariantMap c = coupling(studentId);
    if (c.isEmpty())
        return {};
    const QVariantMap student = m_store->get(u"student"_s, studentId);
    const QDate today = Qupil::Clock::today();
    const QString todayIso = today.toString(Qt::ISODate);
    const QStringList memberships = [&] {
        QStringList out;
        for (const QVariant &v : rows(u"SELECT id FROM membership WHERE deleted = 0 AND student_id = ? "
                                      u"AND (end_date = '' OR end_date > ?)"_s, { studentId, todayIso }))
            out << v.toMap().value(u"id"_s).toString();
        return out;
    }();

    QJsonObject booklet;
    booklet.insert(u"format"_s, 1);
    booklet.insert(u"written"_s, QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    booklet.insert(u"teacher"_s, setting(u"booklet_teacher_name"_s, QString()).toString());
    booklet.insert(u"student"_s, student.value(u"forename"_s).toString());
    booklet.insert(u"instrument"_s, student.value(u"instrument"_s).toString());
    const bool receives = c.value(u"receives"_s, 1).toInt() != 0;
    booklet.insert(u"allow"_s, QJsonObject { { u"feedback"_s, receives && c.value(u"allow_feedback"_s, 1).toInt() != 0 },
                                             { u"questions"_s, receives && c.value(u"allow_questions"_s, 1).toInt() != 0 } });

    // The next three lessons (weekly ones; irregular lessons have no date to tell)
    struct Next { QString date, start, end, place; };
    QList<Next> next;
    for (const QString &m : memberships) {
        const QVariantMap lesson = m_store->get(u"lesson"_s, m_store->get(u"membership"_s, m).value(u"lesson_id"_s).toString());
        if (lesson.isEmpty() || lesson.value(u"irregular"_s).toInt() != 0 || lesson.value(u"active"_s, 1).toInt() == 0)
            continue;
        QString after;
        for (int i = 0; i < 3; ++i) {
            const QString date = nextDateOf(m, after);
            if (date.isEmpty() || date == after)
                break;
            next.append({ date, lesson.value(u"start_time"_s).toString(), lesson.value(u"end_time"_s).toString(),
                          lesson.value(u"location"_s).toString() });
            after = date;
        }
    }
    std::sort(next.begin(), next.end(), [](const Next &a, const Next &b) { return a.date + a.start < b.date + b.start; });
    QJsonArray nextArray;
    for (const Next &n : next.mid(0, 3))
        nextArray.append(QJsonObject { { u"date"_s, n.date }, { u"start"_s, n.start }, { u"end"_s, n.end },
                                       { u"place"_s, n.place }, { u"cancelled"_s, false } });
    booklet.insert(u"next"_s, nextArray);

    // Open tasks with the answers so far (closed by the teacher: gone from the booklet)
    QJsonArray taskArray;
    QJsonObject feedback;
    QSet<QString> seen;
    for (const QString &m : memberships) {
        for (const QVariant &v : tasks(m)) {
            const QVariantMap t = v.toMap();
            const QString id = t.value(u"id"_s).toString();
            const QString closed = t.value(u"closed"_s).toString();
            if (seen.contains(id) || !closed.isEmpty())
                continue;
            seen.insert(id);
            taskArray.append(QJsonObject { { u"id"_s, id }, { u"date"_s, t.value(u"date"_s).toString() },
                                           { u"since"_s, t.value(u"since"_s).toString() },
                                           { u"title"_s, t.value(u"title"_s).toString() },
                                           { u"hints"_s, QJsonArray::fromStringList(t.value(u"hints"_s).toStringList()) },
                                           { u"kind"_s, t.value(u"kind"_s).toInt() == 1 ? u"todo"_s : u"practice"_s } });
            const QVariantMap fb = t.value(u"feedback"_s).toMap();
            if (!fb.isEmpty())
                feedback.insert(id, QJsonObject::fromVariantMap(fb));
        }
    }
    booklet.insert(u"tasks"_s, taskArray);
    booklet.insert(u"feedback"_s, feedback);

    // Pieces in progress or ready for a recital
    QJsonArray pieces;
    for (const QString &m : memberships) {
        for (const QVariant &v : rows(u"SELECT title, composer, start_date, state FROM piece WHERE deleted = 0 "
                                      u"AND membership_id = ? AND state IN (1, 3) ORDER BY start_date, title"_s, { m })) {
            const QVariantMap p = v.toMap();
            pieces.append(QJsonObject { { u"title"_s, p.value(u"title"_s).toString() },
                                        { u"composer"_s, p.value(u"composer"_s).toString() },
                                        { u"since"_s, p.value(u"start_date"_s).toString() },
                                        { u"ready"_s, p.value(u"state"_s).toInt() == 3 } });
        }
    }
    booklet.insert(u"pieces"_s, pieces);

    // Coming events the student plays in (with the piece)
    QJsonArray events;
    if (!memberships.isEmpty()) {
        for (const QVariant &v : rows(u"SELECT e.date, e.time, e.title, e.location, p.title AS piece FROM event e "
                                      u"JOIN event_item i ON i.event_id = e.id AND i.deleted = 0 "
                                      u"JOIN piece p ON p.id = i.piece_id AND p.deleted = 0 "
                                      u"WHERE e.deleted = 0 AND e.state = 0 AND e.date >= ? AND p.membership_id IN (%1) "
                                      u"ORDER BY e.date, e.time"_s.arg(placeholders(memberships.size())),
                                      QVariantList { todayIso } + QVariantList(memberships.cbegin(), memberships.cend()))) {
            const QVariantMap e = v.toMap();
            events.append(QJsonObject { { u"date"_s, e.value(u"date"_s).toString() }, { u"time"_s, e.value(u"time"_s).toString() },
                                        { u"name"_s, e.value(u"title"_s).toString() },
                                        { u"place"_s, e.value(u"location"_s).toString() },
                                        { u"contribution"_s, e.value(u"piece"_s).toString() } });
        }
    }
    booklet.insert(u"events"_s, events);
    return booklet;
}

QString DataService::bookletSummary(const QString &studentId) const
{
    const QVariantMap c = coupling(studentId);
    if (c.isEmpty())
        return {};
    const auto label = [](const char *prefix, int value) {
        return qtTrId(QByteArray(prefix + QByteArray::number(std::clamp(value, 1, 3))).constData());
    };
    QStringList lines;
    for (const QVariant &v : studentTasks(studentId)) {
        const QVariantMap t = v.toMap();
        const QVariantMap fb = t.value(u"feedback"_s).toMap();
        if (!t.value(u"closed"_s).toString().isEmpty() || fb.isEmpty())
            continue;
        QStringList parts;
        if (t.value(u"kind"_s).toInt() == 1) {
            if (fb.value(u"done"_s).toBool())
                parts << qtTrId("task.feedback.done");
        } else {
            if (fb.value(u"practiced"_s).toInt() > 0)
                parts << qtTrId("task.feedback.practiced").arg(label("task.practiced.", fb.value(u"practiced"_s).toInt()));
            if (fb.value(u"works"_s).toInt() > 0)
                parts << qtTrId("task.feedback.works").arg(label("task.works.", fb.value(u"works"_s).toInt()));
        }
        if (!parts.isEmpty())
            lines << t.value(u"title"_s).toString() + u": "_s + parts.join(u" · "_s);
    }
    const double since = double(Qupil::Clock::now().addDays(-14).toMSecsSinceEpoch());
    for (const QJsonValue &q : QJsonDocument::fromJson(c.value(u"questions"_s).toString().toUtf8()).array()) {
        const QJsonObject o = q.toObject();
        if (o.value(u"at"_s).toDouble() >= since && !o.value(u"read"_s).toBool())
            lines << qtTrId("booklet.question_line").arg(o.value(u"text"_s).toString());
    }
    return lines.join(u'\n');
}

QVariantList DataService::blocksAsTasks(const QString &content) const
{
    QVariantList out;
    for (const NoteFormat::Block &b : NoteFormat::blocks(NoteFormat::normalized(content))) {
        QString title = NoteFormat::toPlainText(b.title).simplified();
        if (title.endsWith(u':'))
            title.chop(1);
        QStringList hints;
        for (const QString &line : b.lines) {
            const QString hint = NoteFormat::toPlainText(line).simplified();
            if (!hint.isEmpty())
                hints << hint;
        }
        if (!title.isEmpty() || !hints.isEmpty())
            out << QVariantMap { { u"title"_s, title }, { u"hints"_s, hints } };
    }
    return out;
}

QString DataService::tasksAsBlocks(const QVariantList &list) const
{
    QStringList blocks;
    for (const QVariant &v : list) {
        const QVariantMap t = v.toMap();
        const QString title = t.value(u"title"_s).toString().simplified().remove(u"**"_s);
        const QStringList hints = hintList(t.value(u"hints"_s));
        if (title.isEmpty() && hints.isEmpty())
            continue;
        QStringList lines;
        // The heading bold as a whole: a block even without lines (NoteFormat::blocks)
        lines << (title.isEmpty() ? hints.first() : u"**"_s + title + u"**"_s);
        lines << (title.isEmpty() ? hints.mid(1) : hints);
        blocks << lines.join(u'\n');
    }
    return blocks.join(u"\n\n"_s);
}

bool DataService::finishPlan(const QString &planId, const QString &rest, bool forAll)
{
    const QStringList copies = forAll ? groupCopies(u"plan"_s, planId) : QStringList { planId };
    const QString left = NoteFormat::normalized(rest);
    m_store->beginBatch();
    bool ok = true;
    if (left.trimmed().isEmpty()) {
        for (const QString &copy : copies)
            ok = ok && m_store->remove(u"plan"_s, copy);
    } else {
        for (const QString &copy : copies)
            ok = ok && m_store->update(u"plan"_s, copy, { { u"content"_s, left } });
        ok = ok && !postponePlan(planId, forAll).isEmpty();
    }
    return m_store->endBatch(ok) && ok;
}

bool DataService::appendToNote(const QString &membershipId, const QString &date, const QString &text, bool forAll)
{
    if (text.trimmed().isEmpty())
        return true;
    for (const QVariant &v : notes(membershipId)) {
        const QVariantMap n = v.toMap();
        if (n.value(u"date"_s).toString() == date)
            return updateNote(n.value(u"id"_s).toString(), date,
                              n.value(u"content"_s).toString().trimmed() + u"\n\n"_s + text.trimmed(), forAll);
    }
    return !addNote(membershipId, date, text.trimmed(), forAll).isEmpty();
}

bool DataService::planToTasks(const QString &planId, const QString &covered, const QString &rest, bool forAll,
                              const QString &privateNote)
{
    const QVariantMap plan = m_store->get(u"plan"_s, planId);
    if (plan.isEmpty())
        return false;
    const QString membership = plan.value(u"membership_id"_s).toString();
    const QString date = plan.value(u"date"_s).toString();
    const bool all = forAll && groupCopies(u"plan"_s, planId).size() > 1;
    // The tasks the lesson has already, then the new ones
    QVariantList list;
    for (const QVariant &v : tasksOn(membership, date)) {
        const QVariantMap t = v.toMap();
        list << QVariantMap { { u"id"_s, t.value(u"id"_s) }, { u"title"_s, t.value(u"title"_s) },
                              { u"hints"_s, t.value(u"hints"_s) }, { u"kind"_s, t.value(u"kind"_s) } };
    }
    list << blocksAsTasks(covered);
    m_store->beginBatch();
    bool ok = saveTasks(membership, date, list, all);
    // Marked private: into the note of the lesson, never to the student
    ok = ok && appendToNote(membership, date, NoteFormat::normalized(privateNote), all);
    ok = ok && finishPlan(planId, rest, forAll);
    return m_store->endBatch(ok) && ok;
}
