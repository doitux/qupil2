// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "LessonEndAlert.h"

#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QJSEngine>
#include <QLocale>
#include <QSettings>
#include <QStandardPaths>

#include <algorithm>

#include "AlertSound.h"
#include "DataService.h"

using namespace Qt::StringLiterals;
using namespace Qupil;

namespace {

LessonEndAlert *s_instance = nullptr;

// "lessonEnd" was written by an earlier test build with its defaults (off, 5 minutes)
const QString kGroup = u"lessonAlert"_s;

QVariant stored(const QString &key)
{
    QSettings s;
    s.beginGroup(kGroup);
    return s.value(key);
}

void store(const QString &key, const QVariant &value)
{
    QSettings s;
    s.beginGroup(kGroup);
    s.setValue(key, value);
}

} // namespace

LessonEndAlert::LessonEndAlert(DataService *data, QObject *parent)
    : QObject(parent)
    , m_data(data)
{
    Q_ASSERT_X(!s_instance, "LessonEndAlert", "only one instance allowed");
    s_instance = this;
    QSettings().remove(u"lessonEnd"_s);
    // Your own sound of an earlier test build
    const QString sounds = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + u"/sounds/"_s;
    if (QFile::exists(sounds + u"custom.wav"_s))
        QFile::rename(sounds + u"custom.wav"_s, sounds + u"custom-lesson-end.wav"_s);

    m_sound = new AlertSound(u"lesson-end"_s, u":/qt/qml/Qupil/assets/sounds/lesson-end.wav"_s, kGroup,
                             [this] { return m_data ? m_data->setting(u"lesson_end_volume"_s, 7).toInt() : 7; }, this);
    connect(m_sound, &AlertSound::changed, this, [this] {
        m_notified = { enabled(), minutes(), volume() };
        emit settingsChanged();
        reschedule();
    });
    connect(m_sound, &AlertSound::busyChanged, this, &LessonEndAlert::busyChanged);
    connect(m_sound, &AlertSound::failed, this, &LessonEndAlert::message);

    m_apply.setSingleShot(true);
    m_apply.setInterval(400);
    connect(&m_apply, &QTimer::timeout, this, &LessonEndAlert::apply);
    m_tick.setInterval(10000);   // as in Qupil 1.x
    connect(&m_tick, &QTimer::timeout, this, &LessonEndAlert::check);
    if (m_data)
        connect(m_data, &DataService::changed, this, &LessonEndAlert::reschedule);

    if (background()) {
        SystemAlerts::setUp();
        connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, [this](Qt::ApplicationState state) {
            if (state != Qt::ApplicationActive)
                return;
            // Back from the system settings or the permission question; iOS keeps
            // only the next 60 alerts, so the list moves on
            refreshState();
            SystemAlerts::refresh();
        });
        refreshState();
    }
    m_notified = { enabled(), minutes(), volume() };
    reschedule();
}

LessonEndAlert::~LessonEndAlert()
{
    if (s_instance == this)
        s_instance = nullptr;
}

LessonEndAlert *LessonEndAlert::instance()
{
    return s_instance;
}

LessonEndAlert *LessonEndAlert::create(QQmlEngine *, QJSEngine *)
{
    Q_ASSERT_X(s_instance, "LessonEndAlert", "create the instance in main() before loading QML");
    QJSEngine::setObjectOwnership(s_instance, QJSEngine::CppOwnership);
    return s_instance;
}

// ---------------------------------------------------------------------------- settings
// Per device; without own values the ones taken over from Qupil 1.x (or its defaults)

bool LessonEndAlert::enabled() const
{
    const QVariant v = stored(u"enabled"_s);
    if (v.isValid())
        return v.toBool();
    return m_data ? m_data->setting(u"lesson_end_message"_s, 1).toInt() != 0 : true;
}

void LessonEndAlert::setEnabled(bool enabled)
{
    store(u"enabled"_s, enabled);
    notifyIfChanged();
    reschedule();
}

int LessonEndAlert::minutes() const
{
    const QVariant v = stored(u"minutes"_s);
    const int m = v.isValid() ? v.toInt() : m_data ? m_data->setting(u"lesson_end_minutes"_s, 3).toInt() : 3;
    return std::clamp(m, 1, 30);
}

void LessonEndAlert::setMinutes(int minutes)
{
    store(u"minutes"_s, std::clamp(minutes, 1, 30));
    notifyIfChanged();
    reschedule();
}

int LessonEndAlert::volume() const
{
    return m_sound->volume();
}

void LessonEndAlert::setVolume(int volume)
{
    m_sound->setVolume(volume);
}

QString LessonEndAlert::soundName() const
{
    return m_sound->customName();
}

bool LessonEndAlert::busy() const
{
    return m_sound->busy();
}

bool LessonEndAlert::background() const
{
    return SystemAlerts::available();
}

LessonEndAlert::Permission LessonEndAlert::permission() const
{
    switch (m_state.permission) {
    case SystemAlerts::Permission::NotNeeded:
        return NotNeeded;
    case SystemAlerts::Permission::NotAsked:
        return NotAsked;
    case SystemAlerts::Permission::Granted:
        return Granted;
    case SystemAlerts::Permission::Denied:
        return Denied;
    }
    return NotNeeded;
}

void LessonEndAlert::notifyIfChanged()
{
    const QVariantList now { enabled(), minutes(), volume() };
    if (now != m_notified) {
        m_notified = now;
        emit settingsChanged();
    }
}

// ---------------------------------------------------------------------------- sound

QString LessonEndAlert::soundFile()
{
    return m_sound->file();
}

void LessonEndAlert::chooseSound(const QUrl &file)
{
    m_sound->choose(file);
}

void LessonEndAlert::useDefaultSound()
{
    m_sound->useDefault();
}

void LessonEndAlert::test()
{
    m_sound->play();
}

// ---------------------------------------------------------------------------- schedule

void LessonEndAlert::reschedule()
{
    m_apply.start();
}

void LessonEndAlert::apply()
{
    notifyIfChanged();
    m_alerts.clear();
    if (enabled() && m_data) {
        QList<LessonAlerts::Lesson> lessons;
        for (int day = 0; day < 7; ++day) {
            const QVariantList list = m_data->lessonsOn(day);
            for (const QVariant &v : list) {
                const QVariantMap lesson = v.toMap();
                // Without a name of its own: the students
                lessons.append({ day, lesson.value(u"end_time"_s).toString(), lesson.value(u"title"_s).toString() });
            }
        }
        m_alerts = LessonAlerts::weekly(lessons, minutes());
    }

    if (background()) {
        QList<SystemAlerts::Alert> alerts;
        const QString sound = m_alerts.isEmpty() ? QString() : m_sound->file();
        const QString title = qtTrId("notify.lesson_end", minutes());
        for (const LessonAlerts::Alert &a : std::as_const(m_alerts)) {
            QString text = a.names.join(u", "_s);
            if (text.isEmpty()) {
                const int end = (a.at + a.lead) % LessonAlerts::kMinutesPerDay;
                text = QLocale().toString(QTime(end / 60, end % 60), QLocale::ShortFormat);
            }
            alerts.append({ QString::fromLatin1(SystemAlerts::kLessonEnd), a.at, a.lead, title, text, sound });
        }
        SystemAlerts::setAlerts(QString::fromLatin1(SystemAlerts::kLessonEnd), qtTrId("notify.channel.lesson_end"),
                                alerts);
        askOnce();
    } else if (m_alerts.isEmpty()) {
        m_tick.stop();
    } else if (!m_tick.isActive()) {
        m_lastCheck = QDateTime::currentDateTime();
        m_tick.start();
    }
}

void LessonEndAlert::check()
{
    const QDateTime now = QDateTime::currentDateTime();
    QDateTime after = m_lastCheck.isValid() ? m_lastCheck : now;
    // The clock was set back, or the computer slept: look back a bit, not further
    if (after > now)
        after = now.addSecs(-10);
    else if (after < now.addSecs(-3600))
        after = now.addSecs(-3600);
    m_lastCheck = now;

    const QList<LessonAlerts::Alert> due = LessonAlerts::due(m_alerts, after, now);
    if (due.isEmpty() || m_quiet)
        return;
    QStringList names;
    for (const LessonAlerts::Alert &a : due)
        names << a.names;
    m_sound->play();
    emit alert(qtTrId("notify.lesson_end", minutes()), names.join(u", "_s));
}

// ---------------------------------------------------------------------------- permission (iOS, Android)

void LessonEndAlert::refreshState()
{
    SystemAlerts::queryState(this, [this](SystemAlerts::State state) {
        const bool changed = !m_stateKnown || state.permission != m_state.permission
                             || state.exactAlarms != m_state.exactAlarms;
        m_state = state;
        m_stateKnown = true;
        if (changed)
            emit permissionChanged();
        askOnce();
    });
}

void LessonEndAlert::askOnce()
{
    // Asked once by itself, as soon as there are lessons; later only with the button
    if (!background() || !m_stateKnown || m_alerts.isEmpty() || m_state.permission != SystemAlerts::Permission::NotAsked)
        return;
    if (stored(u"asked"_s).toBool())
        return;
    store(u"asked"_s, true);
    requestPermission();
}

void LessonEndAlert::requestPermission()
{
    SystemAlerts::requestPermission(this, [this] {
        refreshState();
        SystemAlerts::refresh();
    });
}

void LessonEndAlert::openSystemSettings(bool exactAlarms)
{
    SystemAlerts::openSettings(exactAlarms);
}
