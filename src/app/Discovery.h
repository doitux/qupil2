// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QObject>
#include <QString>

/*!
    Finds other Qupil devices in the same network and makes this one visible,
    with DNS service discovery ("Bonjour") under the service type below.
    Apple systems use the system's Bonjour service (required on iOS), the
    other systems a small multicast DNS responder of our own.

    Every device is announced as "Qupil-<first 8 characters of its id>" with
    TXT entries id, n (name), pair (1 while it shows a pairing code) and v.
*/
class Discovery : public QObject
{
    Q_OBJECT

public:
    static constexpr const char *ServiceType = "_qupil-sync._tcp";

    //! The implementation for this platform
    static Discovery *create(QObject *parent);

    using QObject::QObject;

    //! Makes this device visible, or updates name and pairing mode
    virtual void announce(const QString &id, const QString &name, quint16 port, bool pairing) = 0;
    virtual void withdraw() = 0;
    //! Looks for other devices (now and then again from time to time)
    virtual void browse() = 0;
    virtual void stopBrowsing() = 0;

    static QString instanceName(const QString &id) { return QStringLiteral("Qupil-") + id.left(8); }

signals:
    //! A device appeared or changed; host is an IPv4 address
    void found(const QString &id, const QString &name, const QString &host, quint16 port, bool pairing);
    void lost(const QString &id);
};
