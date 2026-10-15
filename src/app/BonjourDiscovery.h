// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QHash>
#include <QList>
#include <QSocketNotifier>

#include <dns_sd.h>

#include "Discovery.h"

/*!
    DNS service discovery with Apple's Bonjour (dns_sd), for macOS and iOS.
    iOS only allows multicast through this service; it asks the user once for
    access to the local network.
*/
class BonjourDiscovery : public Discovery
{
    Q_OBJECT

public:
    explicit BonjourDiscovery(QObject *parent);
    ~BonjourDiscovery() override;

    void announce(const QString &id, const QString &name, quint16 port, bool pairing) override;
    void withdraw() override;
    void browse() override;
    void stopBrowsing() override;

    // Called from the dns_sd callbacks
    struct Lookup
    {
        BonjourDiscovery *self = nullptr;
        DNSServiceRef ref = nullptr;
        uint32_t interfaceIndex = 0;
        QString instance;
        QString id;
        QString name;
        quint16 port = 0;
        bool pairing = false;
    };
    void browsed(DNSServiceFlags flags, uint32_t interfaceIndex, const char *serviceName, const char *regtype,
                 const char *domain);
    void resolved(Lookup *lookup, const char *host, uint16_t port, uint16_t txtLength, const unsigned char *txt);
    void addressed(Lookup *lookup, const struct sockaddr *address);
    void finish(Lookup *lookup);

private:
    bool ensureConnection();
    void reset();
    QByteArray txtRecord() const;

    DNSServiceRef m_connection = nullptr;
    DNSServiceRef m_register = nullptr;
    DNSServiceRef m_browse = nullptr;
    QSocketNotifier *m_notifier = nullptr;
    QList<Lookup *> m_lookups;
    QHash<QString, QString> m_idByInstance;

    QString m_id;
    QString m_name;
    quint16 m_port = 0;
    bool m_pairing = false;
};
