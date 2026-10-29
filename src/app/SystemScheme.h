// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QObject>
#include <QVariant>
#include <QtQml/qqmlregistration.h>
#ifdef QUPIL_DBUS
#include <QDBusVariant>
#endif

/*!
    Light or dark as the desktop asks for it, where Qt cannot tell on its own.

    Linux: the colour scheme of the desktop portal (org.freedesktop.appearance
    color-scheme), the same answer for KDE Plasma, GNOME and others; it follows
    changes in the system settings. Qt's own guess on KDE only looks at the name
    of the colour scheme ("BreezeDark") and misses schemes like Kubuntu's dark one.

    Elsewhere (and without a portal) \c preference stays 0: Theme.qml then uses
    Qt.styleHints.colorScheme.
*/
class SystemScheme : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    //! 0 = no answer (ask Qt), 1 = dark, 2 = light
    Q_PROPERTY(int preference READ preference NOTIFY preferenceChanged FINAL)

public:
    explicit SystemScheme(QObject *parent = nullptr);

    int preference() const { return m_preference; }

signals:
    void preferenceChanged();

#ifdef QUPIL_DBUS
private slots:
    void settingChanged(const QString &group, const QString &key, const QDBusVariant &value);
#endif

private:
    void read(bool oldInterface);
    void setValue(const QVariant &value);

    int m_preference = 0;
};
