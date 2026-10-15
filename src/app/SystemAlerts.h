// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QList>
#include <QObject>
#include <QString>

#include <functional>

/*!
    Lesson-end alerts as system notifications on iOS and Android: they come
    with their sound also when Qupil is in the background or not running.

    iOS: one repeating weekly notification per alert time (UserNotifications).
    Android: an alarm for the next alert; the receiver (LessonAlarms.java)
    plays the sound, shows the notification and sets the following alarm.

    On desktop systems available() is false and Qupil plays the alert itself.
*/
namespace SystemAlerts {

struct Alert
{
    int at = 0;     //!< minute of the week (Monday 00:00 = 0)
    int lead = 0;   //!< minutes from the alert to the end of the lesson
    QString title;
    QString text;
};

enum class Permission {
    NotNeeded,   //!< desktop
    NotAsked,    //!< the system will ask when requested
    Granted,
    Denied       //!< only the system settings can change it
};

struct State
{
    Permission permission = Permission::NotNeeded;
    bool exactAlarms = true;   //!< Android 12+: allowed to sound on the minute
};

bool available();

//! Once at start (iOS: shows notifications also while Qupil is in front)
void setUp();

/*!
    Replaces all lesson-end notifications. alerts are in the order they come
    from now on (iOS keeps the first 60). soundFile: WAV, volume applied.
*/
void schedule(const QList<Alert> &alerts, const QString &soundFile, const QString &channelName);
void cancel();

//! done is called on context's thread, maybe later
void queryState(QObject *context, std::function<void(State)> done);
//! Shows the system question (or the settings if it may not ask any more); then done
void requestPermission(QObject *context, std::function<void()> done);
//! The app's notification settings, or (Android) the "Alarms & reminders" setting
void openSettings(bool exactAlarms);

} // namespace SystemAlerts
