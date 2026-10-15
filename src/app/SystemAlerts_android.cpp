// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Android: the alarms and notifications are handled by LessonAlarms.java
// (platform/android/src), which also works while Qupil is not running.
// The plan handed over: {"channels": {kind: name}, "alerts": [{kind, at, lead, title, text, sound}]}

#include "SystemAlerts.h"

#include <QCoreApplication>
#include <QJniObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPointer>

using namespace Qt::StringLiterals;

namespace SystemAlerts {

namespace {

constexpr char kClass[] = "de/qupil/app/LessonAlarms";

QJniObject appContext()
{
    return QNativeInterface::QAndroidApplication::context();
}

void later(QObject *context, std::function<void()> call)
{
    QPointer<QObject> guard(context);
    QMetaObject::invokeMethod(qApp, [guard, call] {
        if (guard)
            call();
    }, Qt::QueuedConnection);
}

} // namespace

bool available()
{
    return true;
}

void setUp()
{
}

void queryState(QObject *context, std::function<void(State)> done)
{
    const QJniObject ctx = appContext();
    State state;
    state.permission = Permission::Denied;
    if (ctx.isValid()) {
        const jint bits = QJniObject::callStaticMethod<jint>(kClass, "state", "(Landroid/content/Context;)I",
                                                             ctx.object());
        state.permission = (bits & 1) ? Permission::Granted : (bits & 4) ? Permission::NotAsked : Permission::Denied;
        state.exactAlarms = (bits & 2) != 0;
    }
    later(context, [done, state] { done(state); });
}

void requestPermission(QObject *context, std::function<void()> done)
{
    const QJniObject ctx = appContext();
    if (ctx.isValid()) {
        QJniObject::callStaticMethod<void>(kClass, "requestPermission", "(Landroid/content/Context;)V",
                                           ctx.object());
    }
    // The answer comes when Qupil is in front again (applicationStateChanged)
    later(context, done);
}

void openSettings(bool exactAlarms)
{
    const QJniObject ctx = appContext();
    if (ctx.isValid()) {
        QJniObject::callStaticMethod<void>(kClass, "openSettings", "(Landroid/content/Context;Z)V",
                                           ctx.object(), jboolean(exactAlarms));
    }
}

namespace Platform {

void schedule(const QList<Alert> &alerts, const QMap<QString, QString> &channels)
{
    const QJniObject ctx = appContext();
    if (!ctx.isValid())
        return;
    QJsonArray list;
    for (const Alert &a : alerts) {
        list.append(QJsonObject { { u"kind"_s, a.kind }, { u"at"_s, a.at }, { u"lead"_s, a.lead },
                                  { u"title"_s, a.title }, { u"text"_s, a.text }, { u"sound"_s, a.sound } });
    }
    QJsonObject names;
    for (auto it = channels.cbegin(); it != channels.cend(); ++it)
        names.insert(it.key(), it.value());
    const QJsonObject plan { { u"channels"_s, names }, { u"alerts"_s, list } };
    const QString json = QString::fromUtf8(QJsonDocument(plan).toJson(QJsonDocument::Compact));
    QJniObject::callStaticMethod<void>(kClass, "schedule", "(Landroid/content/Context;Ljava/lang/String;)V",
                                       ctx.object(), QJniObject::fromString(json).object<jstring>());
}

void cancel()
{
    const QJniObject ctx = appContext();
    if (ctx.isValid())
        QJniObject::callStaticMethod<void>(kClass, "cancel", "(Landroid/content/Context;)V", ctx.object());
}

} // namespace Platform

} // namespace SystemAlerts
