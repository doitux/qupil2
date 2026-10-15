// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QDateTime>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QUrl>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

#include "LessonAlerts.h"
#include "SystemAlerts.h"
#include "Wav.h"

class DataService;
class QAudioBuffer;
class QAudioDecoder;
class QJSEngine;
class QQmlEngine;
class QSoundEffect;

/*!
    The sound and message a few minutes before a lesson ends (Qupil 1.x:
    "Notification at the end of lessons"), available in QML as "LessonEnd".

    Settings are per device. Without own settings the values taken over from
    Qupil 1.x apply (on, 3 minutes, volume 7). The sound is the "ahem" of
    Qupil 1.x or a file of your own (WAV, MP3, M4A, …; at most 10 seconds).

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

    static constexpr int kMaxSeconds = 10;

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
    bool busy() const { return m_busy; }
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
    void play();
    void refreshState();
    void askOnce();
    void setBusy(bool busy);
    void notifyIfChanged();
    QByteArray sourceSound() const;
    QString soundsFolder() const;
    void storeCustomSound(const Qupil::Wav::Sound &sound, const QString &name);
    void decoded(const QAudioBuffer &buffer);
    void finishDecoding(bool ok);
    QList<SystemAlerts::Alert> systemAlerts() const;

    DataService *m_data;
    QTimer m_apply;
    QTimer m_tick;
    QDateTime m_lastCheck;
    QList<Qupil::LessonAlerts::Alert> m_alerts;
    QSoundEffect *m_effect = nullptr;
    QPointer<QAudioDecoder> m_decoder;
    Qupil::Wav::Sound m_decoding;
    QString m_decodingName;
    QString m_decodingFile;
    SystemAlerts::State m_state;
    bool m_stateKnown = false;
    bool m_busy = false;
    bool m_playWhenReady = false;
    bool m_force = false;
    QByteArray m_scheduled;    //!< what the system has (skip unchanged)
    QVariantList m_notified;   //!< enabled, minutes, volume as last announced
};
