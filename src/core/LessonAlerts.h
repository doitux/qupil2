// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QDateTime>
#include <QList>
#include <QString>
#include <QStringList>

/*!
    When the lesson-end alert sounds: a few minutes before each weekly lesson
    ends (as in Qupil 1.x). The alerts repeat every week; iOS and Android get
    them as system notifications, the desktop app checks them itself.
*/
namespace Qupil::LessonAlerts {

constexpr int kMinutesPerDay = 24 * 60;
constexpr int kMinutesPerWeek = 7 * kMinutesPerDay;

struct Lesson
{
    int weekday = 0;   //!< 0 = Monday
    QString end;       //!< "HH:MM"
    QString name;
};

struct Alert
{
    int at = 0;          //!< minute of the week (Monday 00:00 = 0) when the alert sounds
    int lead = 0;        //!< minutes until the lessons end
    QStringList names;   //!< the lessons that end then

    int weekday() const { return at / kMinutesPerDay; }
    int minuteOfDay() const { return at % kMinutesPerDay; }
};

//! One alert per end time (lessons ending together share it), sorted by time of the week
QList<Alert> weekly(const QList<Lesson> &lessons, int leadMinutes);

//! The first time after `after` (local time) the alert sounds
QDateTime nextTime(const Alert &alert, const QDateTime &after);

//! Alerts that sound in (after, upTo] and whose lessons have not ended at upTo, in time order
QList<Alert> due(const QList<Alert> &alerts, const QDateTime &after, const QDateTime &upTo);

//! The alerts in the order they sound from `now` on
QList<Alert> upcoming(QList<Alert> alerts, const QDateTime &now);

} // namespace Qupil::LessonAlerts
