// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "LessonAlerts.h"

#include <QMap>

#include <algorithm>

using namespace Qt::StringLiterals;

namespace Qupil::LessonAlerts {

namespace {

//! Minutes after midnight for "HH:MM" (also "H:MM"), -1 if it is no time
int minutes(const QString &time)
{
    const QStringList parts = time.trimmed().split(u':');
    if (parts.size() != 2)
        return -1;
    bool okH = false;
    bool okM = false;
    const int h = parts.at(0).toInt(&okH);
    const int m = parts.at(1).toInt(&okM);
    if (!okH || !okM || h < 0 || h > 23 || m < 0 || m > 59)
        return -1;
    return h * 60 + m;
}

} // namespace

QList<Alert> weekly(const QList<Lesson> &lessons, int leadMinutes)
{
    const int lead = std::clamp(leadMinutes, 0, kMinutesPerDay - 1);
    QMap<int, Alert> byTime;
    for (const Lesson &lesson : lessons) {
        const int end = minutes(lesson.end);
        if (end < 0 || lesson.weekday < 0 || lesson.weekday > 6)
            continue;
        const int at = (lesson.weekday * kMinutesPerDay + end - lead + kMinutesPerWeek) % kMinutesPerWeek;
        Alert &alert = byTime[at];
        alert.at = at;
        alert.lead = lead;
        const QString name = lesson.name.trimmed();
        if (!name.isEmpty() && !alert.names.contains(name))
            alert.names << name;
    }
    return byTime.values();
}

QDateTime nextTime(const Alert &alert, const QDateTime &after)
{
    const QTime time(alert.minuteOfDay() / 60, alert.minuteOfDay() % 60);
    const QDate first = after.date();
    // Eight days: the same weekday next week if today's time has passed
    for (int d = 0; d <= 7; ++d) {
        const QDate day = first.addDays(d);
        if (day.dayOfWeek() - 1 != alert.weekday())
            continue;
        const QDateTime candidate(day, time);
        if (candidate > after)
            return candidate;
    }
    return {};
}

QList<Alert> due(const QList<Alert> &alerts, const QDateTime &after, const QDateTime &upTo)
{
    QList<QPair<QDateTime, Alert>> found;
    for (const Alert &alert : alerts) {
        const QDateTime at = nextTime(alert, after);
        if (at.isValid() && at <= upTo && upTo < at.addSecs(qint64(alert.lead) * 60))
            found.append({ at, alert });
    }
    std::stable_sort(found.begin(), found.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
    QList<Alert> out;
    for (const auto &entry : std::as_const(found))
        out << entry.second;
    return out;
}

QList<Alert> upcoming(QList<Alert> alerts, const QDateTime &now)
{
    QList<QPair<QDateTime, Alert>> timed;
    for (const Alert &alert : std::as_const(alerts))
        timed.append({ nextTime(alert, now), alert });
    std::stable_sort(timed.begin(), timed.end(), [](const auto &a, const auto &b) { return a.first < b.first; });
    alerts.clear();
    for (const auto &entry : std::as_const(timed))
        alerts << entry.second;
    return alerts;
}

} // namespace Qupil::LessonAlerts
