// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QList>
#include <QMap>
#include <QObject>
#include <QString>

#include <functional>

/*!
    Alerts as system notifications on iOS and Android (lesson end, reminders at
    the start of a lesson): they come with their sound also when Qupil is in
    the background or not running.

    iOS: one repeating weekly notification per alert time (UserNotifications),
    at most 60 – the next ones; the list moves on whenever Qupil is opened.
    Android: an alarm for the next alert; the receiver (LessonAlarms.java)
    plays the sound, shows the notification and sets the following alarm.

    On desktop systems available() is false and Qupil shows the alerts itself.
*/
namespace SystemAlerts {

//! Kinds of alerts; on Android also the notification channels
inline constexpr char kLessonEnd[] = "lesson_end";
inline constexpr char kReminder[] = "reminder";

struct Alert
{
    QString kind;   //!< kLessonEnd, kReminder
    int at = 0;     //!< minute of the week (Monday 00:00 = 0)
    int lead = 0;   //!< minutes until the notification is out of date (end of the lesson)
    QString title;
    QString text;
    QString sound;  //!< WAV with the volume applied; empty: silent
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
    Each kind of alert is set by its source; all kinds are handed to the
    system together (only when something changed). channelName: shown in
    Android's notification settings.
*/
void setAlerts(const QString &kind, const QString &channelName, const QList<Alert> &alerts);
//! Hands everything over again (when Qupil is in front again: iOS keeps only the next 60)
void refresh();

//! done is called on context's thread, maybe later
void queryState(QObject *context, std::function<void(State)> done);
//! Shows the system question (or the settings if it may not ask any more); then done
void requestPermission(QObject *context, std::function<void()> done);
//! The app's notification settings, or (Android) the "Alarms & reminders" setting
void openSettings(bool exactAlarms);

namespace Platform {
//! All alerts in the order they come from now on; channels: kind → name
void schedule(const QList<Alert> &alerts, const QMap<QString, QString> &channels);
void cancel();
} // namespace Platform

} // namespace SystemAlerts
