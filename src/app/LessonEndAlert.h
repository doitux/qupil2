// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QDateTime>
#include <QList>
#include <QObject>
#include <QTimer>
#include <QUrl>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

#include "LessonAlerts.h"
#include "SystemAlerts.h"

class AlertSound;
class DataService;
class QJSEngine;
class QQmlEngine;

/*!
    The sound and message a few minutes before a lesson ends (Qupil 1.x:
    "Notification at the end of lessons"), available in QML as "LessonEnd".
    Also asks for and shows the system's permission for notifications, which
    the reminders use as well.

    Settings are per device. Without own settings the values taken over from
    Qupil 1.x apply (on, 3 minutes, volume 7). The sound is the "ahem" of
    Qupil 1.x or a file of your own (see AlertSound).

    Desktop: Qupil checks the time itself while it runs. iOS and Android: the
    alerts are system notifications, so they also come in the background.
*/
class LessonEndAlert : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(LessonEnd)
    QML_SINGLETON

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY settingsChanged FINAL)
    Q_PROPERTY(int minutes READ minutes WRITE setMinutes NOTIFY settingsChanged FINAL)
    //! 1 … 10
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY settingsChanged FINAL)
    //! File name of your own sound, empty for the default sound
    Q_PROPERTY(QString soundName READ soundName NOTIFY settingsChanged FINAL)
    //! Reading a chosen sound file
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged FINAL)
    //! Alerts are system notifications (iOS, Android)
    Q_PROPERTY(bool background READ background CONSTANT FINAL)
    Q_PROPERTY(Permission permission READ permission NOTIFY permissionChanged FINAL)
    //! Android 12+: may sound on the minute ("Alarms & reminders")
    Q_PROPERTY(bool exactAlarms READ exactAlarms NOTIFY permissionChanged FINAL)

public:
    enum Permission { NotNeeded, NotAsked, Granted, Denied };
    Q_ENUM(Permission)

    LessonEndAlert(DataService *data, QObject *parent);
    ~LessonEndAlert() override;

    static LessonEndAlert *instance();
    static LessonEndAlert *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    bool enabled() const;
    void setEnabled(bool enabled);
    int minutes() const;
    void setMinutes(int minutes);
    int volume() const;
    void setVolume(int volume);
    QString soundName() const;
    bool busy() const;
    bool background() const;
    Permission permission() const;
    bool exactAlarms() const { return m_state.exactAlarms; }

    //! Your own sound (also content:// on Android); plays it when it is ready
    Q_INVOKABLE void chooseSound(const QUrl &file);
    Q_INVOKABLE void useDefaultSound();
    //! Plays the sound as it will sound
    Q_INVOKABLE void test();
    Q_INVOKABLE void requestPermission();
    //! exactAlarms: Android's "Alarms & reminders" instead of the notification settings
    Q_INVOKABLE void openSystemSettings(bool exactAlarms = false);

    //! The sound file with the volume applied (created when needed); empty on failure
    QString soundFile();
    //! No alerts in the app (screenshot tour)
    void setQuiet(bool quiet) { m_quiet = quiet; }

public slots:
    //! After changes to lessons, settings or language (collected, applied shortly after)
    void reschedule();

signals:
    void settingsChanged();
    void busyChanged();
    void permissionChanged();
    //! Desktop: show the message in the app (the sound is already playing)
    void alert(const QString &title, const QString &text);
    //! Errors (e.g. a sound file that cannot be read)
    void message(const QString &text);

private:
    void apply();
    void check();
    void refreshState();
    void askOnce();
    void notifyIfChanged();

    DataService *m_data;
    AlertSound *m_sound;
    QTimer m_apply;
    QTimer m_tick;
    QDateTime m_lastCheck;
    QList<Qupil::LessonAlerts::Alert> m_alerts;
    SystemAlerts::State m_state;
    bool m_stateKnown = false;
    bool m_quiet = false;
    QVariantList m_notified;   //!< enabled, minutes, volume as last announced
};
