// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QObject>
#include <QSet>
#include <QTimer>
#include <QUrl>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

class AlertSound;
class DataService;
class QJSEngine;
class QQmlEngine;

/*!
    Reminders popping up as in Qupil 1.x, available in QML as "ReminderAlert":

    - "When Qupil starts" (mode 0): when Qupil is started
    - "At the start of every lesson" (mode 1) and "for student …" (mode 2): when
      a lesson starts, or when Qupil is started or brought to the front while
      the lesson runs. Student reminders can be ticked as done (they are
      deleted then).

    With the reminder's sound option the reminder sound plays (Qupil 1.x's or
    your own, volume 1 … 10, per device). On iOS and Android the lesson
    reminders also come as system notifications while Qupil is not in front.
*/
class ReminderAlert : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(ReminderAlert)
    QML_SINGLETON

    //! 1 … 10
    Q_PROPERTY(int volume READ volume WRITE setVolume NOTIFY settingsChanged FINAL)
    //! File name of your own sound, empty for the default sound
    Q_PROPERTY(QString soundName READ soundName NOTIFY settingsChanged FINAL)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged FINAL)
    /*!
        Reminders to show, one group after the other:
        { title, subtitle, items: [{ id, text, mode, studentName }] }
    */
    Q_PROPERTY(QVariantList pending READ pending NOTIFY pendingChanged FINAL)

public:
    ReminderAlert(DataService *data, QObject *parent);
    ~ReminderAlert() override;

    static ReminderAlert *instance();
    static ReminderAlert *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    int volume() const;
    void setVolume(int volume);
    QString soundName() const;
    bool busy() const;
    QVariantList pending() const { return m_pending; }

    Q_INVOKABLE void chooseSound(const QUrl &file);
    Q_INVOKABLE void useDefaultSound();
    Q_INVOKABLE void test();
    //! The first group was seen; doneIds: student reminders ticked as done (deleted)
    Q_INVOKABLE void acknowledge(const QStringList &doneIds);

    //! No pop-ups (screenshot tour)
    void setQuiet(bool quiet) { m_quiet = quiet; }
    //! All reminders in one pop-up (screenshot tour)
    void showAll();

public slots:
    /*!
        The data is ready. withStartReminders: Qupil was just started (not after
        taking over data in the setup)
    */
    void start(bool withStartReminders);
    //! After changes to reminders, lessons or the language
    void reschedule();

signals:
    void settingsChanged();
    void busyChanged();
    void pendingChanged();
    void message(const QString &text);

private:
    void check();
    void apply();
    QVariantList itemsFor(const QVariantMap &lesson, const QVariantList &reminders) const;
    void add(const QString &title, const QString &subtitle, const QVariantList &items);

    DataService *m_data;
    AlertSound *m_sound;
    QTimer m_tick;
    QTimer m_apply;
    QSet<QString> m_seen;   //!< lesson id | date: already looked at
    QVariantList m_pending;
    bool m_started = false;
    bool m_quiet = false;
};
