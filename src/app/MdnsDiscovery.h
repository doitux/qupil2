// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QDateTime>
#include <QHash>
#include <QNetworkInterface>
#include <QTimer>
#include <QUdpSocket>

#include "Discovery.h"
#include "Mdns.h"

class QNetworkDatagram;

/*!
    DNS service discovery over multicast DNS (224.0.0.251:5353), for Windows,
    Linux and Android. Shares the port with the system's responder (Avahi,
    Windows) where there is one. Answers questions for this device and asks
    for the others; understands the answers of Apple's Bonjour as well.
*/
class MdnsDiscovery : public Discovery
{
    Q_OBJECT

public:
    explicit MdnsDiscovery(QObject *parent);
    ~MdnsDiscovery() override;

    void announce(const QString &id, const QString &name, quint16 port, bool pairing) override;
    void withdraw() override;
    void browse() override;
    void stopBrowsing() override;

private:
    struct Instance
    {
        QString host;
        quint16 port = 0;
        QList<QByteArray> txt;
        bool haveSrv = false;
        bool haveTxt = false;
        QDateTime expires;
        QString reported;   //!< what was reported last ("id|name|host|port|pairing")
        QString id;
    };

    bool ensureSocket();
    void joinInterfaces();
    void readPending();
    void handleQuery(const Qupil::Mdns::Message &message, const QNetworkDatagram &datagram);
    void handleResponse(const Qupil::Mdns::Message &message);
    void sendAnnouncement(quint32 ttl);
    void sendQuery();
    void ask(const QString &name, quint16 type);
    void sendToAll(const QByteArray &packet);
    void expire();
    void report(const QString &key);
    QList<Qupil::Mdns::Record> ourRecords(int interfaceIndex, quint32 ttl) const;
    QString pickAddress(const QList<QByteArray> &addresses) const;

    QString serviceName() const;
    QString ourInstance() const;
    QString ourHost() const;

    QUdpSocket m_socket;
    bool m_onMdnsPort = false;
    QList<QNetworkInterface> m_interfaces;

    bool m_announced = false;
    QString m_id;
    QString m_name;
    quint16 m_port = 0;
    bool m_pairing = false;
    QTimer m_reannounce;

    bool m_browsing = false;
    QTimer m_queryTimer;
    int m_queryCount = 0;
    QTimer m_expireTimer;
    QHash<QString, Instance> m_instances;          // lower-case instance name
    QHash<QString, QList<QByteArray>> m_hosts;      // lower-case host name → IPv4 addresses
};
