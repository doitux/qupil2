// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "SystemScheme.h"

#ifdef QUPIL_DBUS
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#endif

using namespace Qt::StringLiterals;

namespace {
#ifdef QUPIL_DBUS
const auto kService = u"org.freedesktop.portal.Desktop"_s;
const auto kPath = u"/org/freedesktop/portal/desktop"_s;
const auto kInterface = u"org.freedesktop.portal.Settings"_s;
const auto kGroup = u"org.freedesktop.appearance"_s;
const auto kKey = u"color-scheme"_s;
#endif
} // namespace

SystemScheme::SystemScheme(QObject *parent)
    : QObject(parent)
{
#ifdef QUPIL_DBUS
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected())
        return;
    // Changes in the system settings (light/dark switched while Qupil runs). The slot is found
    // by its signature: QDBusVariant must be known by name before (else connect() fails)
    qRegisterMetaType<QDBusVariant>();
    bus.connect(kService, kPath, kInterface, u"SettingChanged"_s, this,
                SLOT(settingChanged(QString,QString,QDBusVariant)));
    read(false);
#endif
}

void SystemScheme::read(bool oldInterface)
{
#ifdef QUPIL_DBUS
    // ReadOne (portal version 2) answers with the value; Read (version 1) wraps it once more
    QDBusMessage call = QDBusMessage::createMethodCall(kService, kPath, kInterface,
                                                       oldInterface ? u"Read"_s : u"ReadOne"_s);
    call << kGroup << kKey;
    auto *watcher = new QDBusPendingCallWatcher(QDBusConnection::sessionBus().asyncCall(call, 2000), this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher, oldInterface] {
        watcher->deleteLater();
        const QDBusMessage reply = watcher->reply();
        if (reply.type() != QDBusMessage::ReplyMessage) {
            // An older portal without ReadOne: try Read; no portal at all: Qt decides
            if (!oldInterface && reply.errorName() == u"org.freedesktop.DBus.Error.UnknownMethod")
                read(true);
            return;
        }
        setValue(reply.arguments().value(0));
    });
#else
    Q_UNUSED(oldInterface)
#endif
}

#ifdef QUPIL_DBUS
void SystemScheme::settingChanged(const QString &group, const QString &key, const QDBusVariant &value)
{
    if (group == kGroup && key == kKey)
        setValue(QVariant::fromValue(value));
}
#endif

void SystemScheme::setValue(const QVariant &value)
{
    QVariant v = value;
#ifdef QUPIL_DBUS
    for (int i = 0; i < 3 && v.metaType() == QMetaType::fromType<QDBusVariant>(); ++i)
        v = v.value<QDBusVariant>().variant();
#endif
    bool ok = false;
    const uint scheme = v.toUInt(&ok);
    const int preference = ok && scheme <= 2 ? int(scheme) : 0;
    if (preference == m_preference)
        return;
    m_preference = preference;
    emit preferenceChanged();
}
