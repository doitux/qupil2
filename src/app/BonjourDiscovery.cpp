// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "BonjourDiscovery.h"

#include <QHostAddress>
#include <QtEndian>

#include <arpa/inet.h>
#include <netinet/in.h>

using namespace Qt::StringLiterals;

namespace {

QString txtValue(uint16_t length, const unsigned char *txt, const char *key)
{
    uint8_t valueLength = 0;
    const void *value = TXTRecordGetValuePtr(length, txt, key, &valueLength);
    if (!value)
        return {};
    return QString::fromUtf8(static_cast<const char *>(value), valueLength);
}

void DNSSD_API onRegister(DNSServiceRef, DNSServiceFlags, DNSServiceErrorType error, const char *, const char *,
                          const char *, void *)
{
    if (error != kDNSServiceErr_NoError)
        qWarning("Wi-Fi sync: Bonjour registration failed (%d)", int(error));
}

void DNSSD_API onBrowse(DNSServiceRef, DNSServiceFlags flags, uint32_t interfaceIndex, DNSServiceErrorType error,
                        const char *serviceName, const char *regtype, const char *domain, void *context)
{
    if (error == kDNSServiceErr_NoError)
        static_cast<BonjourDiscovery *>(context)->browsed(flags, interfaceIndex, serviceName, regtype, domain);
}

void DNSSD_API onResolve(DNSServiceRef, DNSServiceFlags, uint32_t, DNSServiceErrorType error, const char *,
                         const char *host, uint16_t port, uint16_t txtLength, const unsigned char *txt, void *context)
{
    auto *lookup = static_cast<BonjourDiscovery::Lookup *>(context);
    if (error != kDNSServiceErr_NoError) {
        lookup->self->finish(lookup);
        return;
    }
    lookup->self->resolved(lookup, host, port, txtLength, txt);
}

void DNSSD_API onAddress(DNSServiceRef, DNSServiceFlags, uint32_t, DNSServiceErrorType error, const char *,
                         const struct sockaddr *address, uint32_t, void *context)
{
    auto *lookup = static_cast<BonjourDiscovery::Lookup *>(context);
    if (error != kDNSServiceErr_NoError || !address) {
        lookup->self->finish(lookup);
        return;
    }
    lookup->self->addressed(lookup, address);
}

} // namespace

Discovery *Discovery::create(QObject *parent)
{
    return new BonjourDiscovery(parent);
}

BonjourDiscovery::BonjourDiscovery(QObject *parent)
    : Discovery(parent)
{
}

BonjourDiscovery::~BonjourDiscovery()
{
    reset();
}

bool BonjourDiscovery::ensureConnection()
{
    if (m_connection)
        return true;
    if (DNSServiceCreateConnection(&m_connection) != kDNSServiceErr_NoError) {
        m_connection = nullptr;
        qWarning("Wi-Fi sync: Bonjour is not available");
        return false;
    }
    m_notifier = new QSocketNotifier(DNSServiceRefSockFD(m_connection), QSocketNotifier::Read, this);
    connect(m_notifier, &QSocketNotifier::activated, this, [this] {
        if (DNSServiceProcessResult(m_connection) != kDNSServiceErr_NoError) {
            // The daemon went away (rare): start again with what we had
            const bool announced = m_register != nullptr;
            const bool browsing = m_browse != nullptr;
            reset();
            if (announced)
                announce(m_id, m_name, m_port, m_pairing);
            if (browsing)
                browse();
        }
    });
    return true;
}

void BonjourDiscovery::reset()
{
    for (Lookup *lookup : std::as_const(m_lookups)) {
        if (lookup->ref)
            DNSServiceRefDeallocate(lookup->ref);
        delete lookup;
    }
    m_lookups.clear();
    if (m_register)
        DNSServiceRefDeallocate(m_register);
    if (m_browse)
        DNSServiceRefDeallocate(m_browse);
    m_register = m_browse = nullptr;
    delete m_notifier;
    m_notifier = nullptr;
    if (m_connection)
        DNSServiceRefDeallocate(m_connection);
    m_connection = nullptr;
}

QByteArray BonjourDiscovery::txtRecord() const
{
    QByteArray txt;
    for (const QByteArray &entry : { QByteArray("v=1"), "id=" + m_id.toUtf8(), "n=" + m_name.toUtf8().left(200),
                                     QByteArray("pair=") + (m_pairing ? "1" : "0") }) {
        txt.append(char(entry.size()));
        txt.append(entry);
    }
    return txt;
}

void BonjourDiscovery::announce(const QString &id, const QString &name, quint16 port, bool pairing)
{
    const bool sameService = m_register && id == m_id && port == m_port;
    m_id = id;
    m_name = name;
    m_port = port;
    m_pairing = pairing;
    if (!ensureConnection())
        return;
    const QByteArray txt = txtRecord();
    if (sameService) {
        DNSServiceUpdateRecord(m_register, nullptr, 0, uint16_t(txt.size()), txt.constData(), 0);
        return;
    }
    if (m_register)
        DNSServiceRefDeallocate(m_register);
    m_register = m_connection;
    const DNSServiceErrorType error =
        DNSServiceRegister(&m_register, kDNSServiceFlagsShareConnection, 0, instanceName(id).toUtf8().constData(),
                           ServiceType, nullptr, nullptr, qToBigEndian(port), uint16_t(txt.size()), txt.constData(),
                           onRegister, this);
    if (error != kDNSServiceErr_NoError) {
        qWarning("Wi-Fi sync: Bonjour registration failed (%d)", int(error));
        m_register = nullptr;
    }
}

void BonjourDiscovery::withdraw()
{
    if (m_register)
        DNSServiceRefDeallocate(m_register);
    m_register = nullptr;
}

void BonjourDiscovery::browse()
{
    if (m_browse || !ensureConnection())
        return;
    m_browse = m_connection;
    if (DNSServiceBrowse(&m_browse, kDNSServiceFlagsShareConnection, 0, ServiceType, nullptr, onBrowse, this)
        != kDNSServiceErr_NoError) {
        qWarning("Wi-Fi sync: Bonjour browsing failed");
        m_browse = nullptr;
    }
}

void BonjourDiscovery::stopBrowsing()
{
    if (m_browse)
        DNSServiceRefDeallocate(m_browse);
    m_browse = nullptr;
}

void BonjourDiscovery::browsed(DNSServiceFlags flags, uint32_t interfaceIndex, const char *serviceName,
                               const char *regtype, const char *domain)
{
    const QString instance = QString::fromUtf8(serviceName);
    if (!(flags & kDNSServiceFlagsAdd)) {
        const QString id = m_idByInstance.take(instance);
        if (!id.isEmpty())
            emit lost(id);
        return;
    }
    auto *lookup = new Lookup;
    lookup->self = this;
    lookup->interfaceIndex = interfaceIndex;
    lookup->instance = instance;
    lookup->ref = m_connection;
    if (DNSServiceResolve(&lookup->ref, kDNSServiceFlagsShareConnection, interfaceIndex, serviceName, regtype, domain,
                          onResolve, lookup)
        != kDNSServiceErr_NoError) {
        delete lookup;
        return;
    }
    m_lookups << lookup;
}

void BonjourDiscovery::resolved(Lookup *lookup, const char *host, uint16_t port, uint16_t txtLength,
                                const unsigned char *txt)
{
    lookup->id = txtValue(txtLength, txt, "id");
    lookup->name = txtValue(txtLength, txt, "n");
    lookup->pairing = txtValue(txtLength, txt, "pair") == u"1"_s;
    lookup->port = qFromBigEndian(port);
    DNSServiceRefDeallocate(lookup->ref);
    lookup->ref = nullptr;
    if (lookup->id.isEmpty() || lookup->id == m_id) {
        finish(lookup);
        return;
    }
    // Next: the IPv4 address of the host
    lookup->ref = m_connection;
    if (DNSServiceGetAddrInfo(&lookup->ref, kDNSServiceFlagsShareConnection, lookup->interfaceIndex,
                              kDNSServiceProtocol_IPv4, host, onAddress, lookup)
        != kDNSServiceErr_NoError) {
        lookup->ref = nullptr;
        finish(lookup);
    }
}

void BonjourDiscovery::addressed(Lookup *lookup, const struct sockaddr *address)
{
    if (address->sa_family == AF_INET) {
        const QHostAddress host(address);
        m_idByInstance.insert(lookup->instance, lookup->id);
        emit found(lookup->id, lookup->name, host.toString(), lookup->port, lookup->pairing);
    }
    finish(lookup);
}

void BonjourDiscovery::finish(Lookup *lookup)
{
    if (lookup->ref)
        DNSServiceRefDeallocate(lookup->ref);
    lookup->ref = nullptr;
    m_lookups.removeOne(lookup);
    delete lookup;
}
