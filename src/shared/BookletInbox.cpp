// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

#include "BookletInbox.h"

#include "Booklet.h"

#include <QMap>
#include <QSet>

#include <algorithm>

using namespace Qt::StringLiterals;

namespace Qupil::Booklet {

namespace {

struct Name
{
    QString device;
    qint64 ms = 0;
};

Name parse(const QString &file)
{
    // fb-<device>-<ms>.qupil
    const QString core = file.mid(3, file.size() - 3 - int(qstrlen(FileSuffix)));
    const qsizetype dash = core.lastIndexOf(u'-');
    return {core.left(dash), core.mid(dash + 1).toLongLong()};
}

} // namespace

Inbox collect(const QByteArray &key, const QHash<QString, QByteArray> &files, const QJsonObject &known)
{
    Inbox inbox;
    inbox.feedback = known.value(u"feedback"_s).toObject();
    inbox.questions = known.value(u"questions"_s).toArray();

    // Newest file per device
    QMap<QString, QString> newest;   // device -> file
    for (auto it = files.constBegin(); it != files.constEnd(); ++it) {
        if (!isFeedbackFileName(it.key()))
            continue;
        const Name n = parse(it.key());
        const auto current = newest.constFind(n.device);
        if (current == newest.constEnd()) {
            newest.insert(n.device, it.key());
        } else if (parse(*current).ms < n.ms) {
            inbox.superseded.append(*current);
            newest.insert(n.device, it.key());
        } else {
            inbox.superseded.append(it.key());
        }
    }

    QSet<QString> questionIds;
    for (const QJsonValue &q : std::as_const(inbox.questions))
        questionIds.insert(q.toObject().value(u"id"_s).toString());

    for (const QString &file : std::as_const(newest)) {
        const auto content = open(key, Purpose::Feedback, files.value(file));
        if (!content) {
            inbox.unreadable.append(file);
            continue;
        }
        const QJsonObject feedback = content->value(u"feedback"_s).toObject();
        for (auto it = feedback.constBegin(); it != feedback.constEnd(); ++it) {
            const QJsonObject theirs = it.value().toObject();
            const QJsonObject mine = inbox.feedback.value(it.key()).toObject();
            if (theirs.isEmpty())
                continue;
            if (mine.isEmpty() || theirs.value(u"at"_s).toDouble() > mine.value(u"at"_s).toDouble()) {
                // Only the fields the app writes; values in range
                QJsonObject clean;
                clean.insert(u"practiced"_s, std::clamp(theirs.value(u"practiced"_s).toInt(), 0, 3));
                clean.insert(u"works"_s, std::clamp(theirs.value(u"works"_s).toInt(), 0, 3));
                clean.insert(u"done"_s, theirs.value(u"done"_s).toBool());
                clean.insert(u"at"_s, theirs.value(u"at"_s).toDouble());
                if (clean != mine) {
                    inbox.feedback.insert(it.key(), clean);
                    inbox.changed = true;
                }
            }
        }
        for (const QJsonValue &v : content->value(u"questions"_s).toArray()) {
            const QJsonObject q = v.toObject();
            const QString id = q.value(u"id"_s).toString();
            const QString text = q.value(u"text"_s).toString().trimmed().left(1000);
            if (id.isEmpty() || text.isEmpty() || questionIds.contains(id))
                continue;
            questionIds.insert(id);
            inbox.questions.append(QJsonObject{{u"id"_s, id.left(32)},
                                               {u"text"_s, text},
                                               {u"task"_s, q.value(u"task"_s).toString().left(64)},
                                               {u"at"_s, q.value(u"at"_s).toDouble()}});
            inbox.changed = true;
        }
    }

    // Questions oldest first
    QList<QJsonValue> list(inbox.questions.begin(), inbox.questions.end());
    std::stable_sort(list.begin(), list.end(), [](const QJsonValue &a, const QJsonValue &b) {
        return a.toObject().value(u"at"_s).toDouble() < b.toObject().value(u"at"_s).toDouble();
    });
    inbox.questions = QJsonArray();
    for (const QJsonValue &v : list)
        inbox.questions.append(v);
    inbox.superseded.sort();
    return inbox;
}

} // namespace Qupil::Booklet
