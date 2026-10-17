// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "ReminderAlert.h"

#include <QDate>
#include <QGuiApplication>
#include <QJSEngine>
#include <QMap>
#include <QTime>

#include <algorithm>

#include "AlertSound.h"
#include "DataService.h"
#include "SystemAlerts.h"

using namespace Qt::StringLiterals;

namespace {

ReminderAlert *s_instance = nullptr;

//! Minutes after midnight for "HH:MM", -1 if it is no time
int minutesOf(const QString &time)
{
    const QTime t = QTime::fromString(time.trimmed(), u"H:mm"_s);
    return t.isValid() ? t.hour() * 60 + t.minute() : -1;
}

QString heading()
{
    return qtTrId("reminder.heading");
}

} // namespace

ReminderAlert::ReminderAlert(DataService *data, QObject *parent)
    : QObject(parent)
    , m_data(data)
{
    Q_ASSERT_X(!s_instance, "ReminderAlert", "only one instance allowed");
    s_instance = this;

    m_sound = new AlertSound(u"reminder"_s, u":/qt/qml/Qupil/assets/sounds/reminder.wav"_s, u"reminderAlert"_s,
                             [] { return 7; }, this);
    connect(m_sound, &AlertSound::changed, this, [this] {
        emit settingsChanged();
        reschedule();
    });
    connect(m_sound, &AlertSound::busyChanged, this, &ReminderAlert::busyChanged);
    connect(m_sound, &AlertSound::failed, this, &ReminderAlert::message);

    m_tick.setInterval(20000);
    connect(&m_tick, &QTimer::timeout, this, &ReminderAlert::check);
    m_apply.setSingleShot(true);
    m_apply.setInterval(500);
    connect(&m_apply, &QTimer::timeout, this, &ReminderAlert::apply);
    if (m_data)
        connect(m_data, &DataService::changed, this, &ReminderAlert::reschedule);
    // iOS, Android: back in front during a lesson
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, [this](Qt::ApplicationState state) {
        if (state == Qt::ApplicationActive)
            check();
    });
}

ReminderAlert::~ReminderAlert()
{
    if (s_instance == this)
        s_instance = nullptr;
}

ReminderAlert *ReminderAlert::instance()
{
    return s_instance;
}

ReminderAlert *ReminderAlert::create(QQmlEngine *, QJSEngine *)
{
    Q_ASSERT_X(s_instance, "ReminderAlert", "create the instance in main() before loading QML");
    QJSEngine::setObjectOwnership(s_instance, QJSEngine::CppOwnership);
    return s_instance;
}

// ---------------------------------------------------------------------------- sound

int ReminderAlert::volume() const
{
    return m_sound->volume();
}

void ReminderAlert::setVolume(int volume)
{
    m_sound->setVolume(volume);
}

QString ReminderAlert::soundName() const
{
    return m_sound->customName();
}

bool ReminderAlert::busy() const
{
    return m_sound->busy();
}

void ReminderAlert::chooseSound(const QUrl &file)
{
    m_sound->choose(file);
}

void ReminderAlert::useDefaultSound()
{
    m_sound->useDefault();
}

void ReminderAlert::test()
{
    m_sound->play();
}

// ---------------------------------------------------------------------------- pop-ups

void ReminderAlert::start(bool withStartReminders)
{
    if (!m_data)
        return;
    if (withStartReminders && !m_started) {
        QVariantList items;
        for (const QVariant &v : m_data->reminders()) {
            const QVariantMap r = v.toMap();
            if (r.value(u"mode"_s).toInt() == 0)
                items << r;
        }
        if (!items.isEmpty())
            add(heading(), {}, items);
    }
    m_started = true;
    m_tick.start();
    check();
    reschedule();
}

QVariantList ReminderAlert::itemsFor(const QVariantMap &lesson, const QVariantList &reminders) const
{
    QStringList students;
    for (const QVariant &s : lesson.value(u"students"_s).toList())
        students << s.toMap().value(u"id"_s).toString();
    QVariantList items;
    for (const QVariant &v : reminders) {
        const QVariantMap r = v.toMap();
        const int mode = r.value(u"mode"_s).toInt();
        if (mode == 1 || (mode == 2 && students.contains(r.value(u"student_id"_s).toString())))
            items << r;
    }
    return items;
}

void ReminderAlert::check()
{
    if (!m_started || !m_data)
        return;
    // iOS, Android in the background: the system notification comes instead
    if (SystemAlerts::available() && QGuiApplication::applicationState() != Qt::ApplicationActive)
        return;
    const QDateTime now = QDateTime::currentDateTime();
    const int minute = now.time().hour() * 60 + now.time().minute();
    const QString today = now.date().toString(Qt::ISODate);
    QVariantList reminders;
    bool loaded = false;
    // Lessons with a fixed time that run now (from their first minute on)
    for (const QVariant &v : m_data->lessonsOn(now.date().dayOfWeek() - 1)) {
        const QVariantMap lesson = v.toMap();
        const int start = minutesOf(lesson.value(u"start_time"_s).toString());
        const int end = minutesOf(lesson.value(u"end_time"_s).toString());
        if (start < 0 || minute < start || minute >= std::max(end, start + 1))
            continue;
        const QString key = lesson.value(u"id"_s).toString() + u'|' + today;
        if (m_seen.contains(key))
            continue;
        m_seen.insert(key);   // reminders added later during this lesson come next time
        if (!loaded) {
            reminders = m_data->reminders();
            loaded = true;
        }
        const QVariantList items = itemsFor(lesson, reminders);
        if (!items.isEmpty()) {
            const QString when = lesson.value(u"start_time"_s).toString() + u" – "_s + lesson.value(u"end_time"_s).toString();
            add(heading(), lesson.value(u"name"_s).toString() + u" · "_s + when, items);
        }
    }
}

void ReminderAlert::add(const QString &title, const QString &subtitle, const QVariantList &items)
{
    if (m_quiet)
        return;
    m_pending.append(QVariantMap { { u"title"_s, title }, { u"subtitle"_s, subtitle }, { u"items"_s, items } });
    emit pendingChanged();
    const bool sound = std::any_of(items.cbegin(), items.cend(),
                                   [](const QVariant &r) { return r.toMap().value(u"sound"_s).toInt() != 0; });
    if (sound)
        m_sound->play();
}

void ReminderAlert::showAll()
{
    const QVariantList items = m_data ? m_data->reminders() : QVariantList();
    if (items.isEmpty())
        return;
    m_pending.append(QVariantMap { { u"title"_s, heading() }, { u"subtitle"_s, QString() }, { u"items"_s, items } });
    emit pendingChanged();
}

void ReminderAlert::acknowledge(const QStringList &doneIds)
{
    for (const QString &id : doneIds)
        m_data->remove(u"reminder"_s, id);
    if (!m_pending.isEmpty()) {
        m_pending.removeFirst();
        emit pendingChanged();
    }
}

// ---------------------------------------------------------------------------- system notifications

void ReminderAlert::reschedule()
{
    if (SystemAlerts::available())
        m_apply.start();
}

void ReminderAlert::apply()
{
    if (!m_data)
        return;
    QMap<int, SystemAlerts::Alert> byTime;   // lessons starting together share one
    const QVariantList reminders = m_data->reminders();
    const bool any = std::any_of(reminders.cbegin(), reminders.cend(),
                                 [](const QVariant &r) { return r.toMap().value(u"mode"_s).toInt() != 0; });
    QString sound;
    for (int day = 0; any && day < 7; ++day) {
        for (const QVariant &v : m_data->lessonsOn(day)) {
            const QVariantMap lesson = v.toMap();
            const QVariantList items = itemsFor(lesson, reminders);
            const int start = minutesOf(lesson.value(u"start_time"_s).toString());
            if (items.isEmpty() || start < 0)
                continue;
            const int end = minutesOf(lesson.value(u"end_time"_s).toString());
            const int at = day * 24 * 60 + start;
            QStringList lines;
            bool withSound = false;
            for (const QVariant &item : items) {
                const QVariantMap r = item.toMap();
                const QString text = r.value(u"text"_s).toString().simplified();
                lines << (r.value(u"mode"_s).toInt() == 2 ? r.value(u"studentName"_s).toString() + u": "_s + text : text);
                withSound = withSound || r.value(u"sound"_s).toInt() != 0;
            }
            if (withSound && sound.isEmpty())
                sound = m_sound->file();
            SystemAlerts::Alert &alert = byTime[at];
            const QString name = lesson.value(u"name"_s).toString();
            if (alert.title.isEmpty()) {
                alert.kind = QString::fromLatin1(SystemAlerts::kReminder);
                alert.at = at;
                alert.lead = std::max(1, end - start);   // out of date when the lesson is over
                //: %1 = lesson name (more follow after a comma)
                alert.title = qtTrId("reminder.alert_title").arg(name);
                alert.text = lines.join(u'\n');
            } else {
                alert.title += u", "_s + name;
                alert.text += u'\n' + lines.join(u'\n');
            }
            if (withSound)
                alert.sound = sound;
        }
    }
    SystemAlerts::setAlerts(QString::fromLatin1(SystemAlerts::kReminder), qtTrId("reminders.title"), byTime.values());
}
