// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "MdnsDiscovery.h"

#include <QNetworkDatagram>
#include <QSet>
#include <QtEndian>

#include <algorithm>

#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QtCore/qcoreapplication_platform.h>
#endif

using namespace Qt::StringLiterals;
namespace Mdns = Qupil::Mdns;

namespace {

const QHostAddress kGroup(u"224.0.0.251"_s);
constexpr quint16 kPort = 5353;
constexpr quint32 kHostTtl = 120;
constexpr quint32 kServiceTtl = 4500;

#ifdef Q_OS_ANDROID
// Android drops multicast packets unless an app holds a multicast lock
class MulticastLock
{
public:
    MulticastLock()
    {
        QJniObject context = QNativeInterface::QAndroidApplication::context();
        if (!context.isValid())
            return;
        QJniObject wifi = context.callObjectMethod("getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;",
                                                   QJniObject::fromString(u"wifi"_s).object<jstring>());
        if (!wifi.isValid())
            return;
        m_lock = wifi.callObjectMethod("createMulticastLock",
                                       "(Ljava/lang/String;)Landroid/net/wifi/WifiManager$MulticastLock;",
                                       QJniObject::fromString(u"qupil-sync"_s).object<jstring>());
        if (m_lock.isValid()) {
            m_lock.callMethod<void>("setReferenceCounted", "(Z)V", jboolean(false));
            m_lock.callMethod<void>("acquire", "()V");
        }
    }
    ~MulticastLock()
    {
        if (m_lock.isValid())
            m_lock.callMethod<void>("release", "()V");
    }

private:
    QJniObject m_lock;
};
#endif

bool usable(const QNetworkInterface &iface)
{
    const auto flags = iface.flags();
    if (!(flags & QNetworkInterface::IsUp) || !(flags & QNetworkInterface::IsRunning)
        || !(flags & QNetworkInterface::CanMulticast) || (flags & QNetworkInterface::IsLoopBack))
        return false;
    for (const QNetworkAddressEntry &e : iface.addressEntries())
        if (e.ip().protocol() == QAbstractSocket::IPv4Protocol)
            return true;
    return false;
}

QByteArray ipv4Bytes(const QHostAddress &address)
{
    char b[4];
    qToBigEndian(address.toIPv4Address(), b);
    return QByteArray(b, 4);
}

} // namespace

Discovery *Discovery::create(QObject *parent)
{
    return new MdnsDiscovery(parent);
}

MdnsDiscovery::MdnsDiscovery(QObject *parent)
    : Discovery(parent)
{
#ifdef Q_OS_ANDROID
    static MulticastLock lock;   // for the life of the app, once sync over Wi-Fi is used
#endif
    connect(&m_socket, &QUdpSocket::readyRead, this, &MdnsDiscovery::readPending);
    m_reannounce.setSingleShot(true);
    connect(&m_reannounce, &QTimer::timeout, this, [this] {
        if (m_announced)
            sendAnnouncement(kServiceTtl);
    });
    m_queryTimer.setSingleShot(true);
    connect(&m_queryTimer, &QTimer::timeout, this, &MdnsDiscovery::sendQuery);
    m_expireTimer.setInterval(10000);
    connect(&m_expireTimer, &QTimer::timeout, this, &MdnsDiscovery::expire);
}

MdnsDiscovery::~MdnsDiscovery()
{
    withdraw();
}

QString MdnsDiscovery::serviceName() const
{
    return QString::fromLatin1(ServiceType) + u".local"_s;
}

QString MdnsDiscovery::ourInstance() const
{
    return instanceName(m_id) + u'.' + serviceName();
}

QString MdnsDiscovery::ourHost() const
{
    return instanceName(m_id).toLower() + u".local"_s;
}

bool MdnsDiscovery::ensureSocket()
{
    if (m_socket.state() == QAbstractSocket::BoundState) {
        joinInterfaces();
        return true;
    }
    m_onMdnsPort = m_socket.bind(QHostAddress::AnyIPv4, kPort, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    if (!m_onMdnsPort) {
        // The port is taken for good: we can still ask (answers come to our port), but not be found
        qWarning("Wi-Fi sync: port 5353 not available (%s)", qPrintable(m_socket.errorString()));
        if (!m_socket.bind(QHostAddress::AnyIPv4, 0))
            return false;
    }
    m_socket.setSocketOption(QAbstractSocket::MulticastTtlOption, 255);
    m_socket.setSocketOption(QAbstractSocket::MulticastLoopbackOption, 1);
    joinInterfaces();
    return true;
}

// Wi-Fi may come and go: join the group on every interface that is up now
void MdnsDiscovery::joinInterfaces()
{
    QList<QNetworkInterface> now;
    for (const QNetworkInterface &iface : QNetworkInterface::allInterfaces())
        if (usable(iface))
            now << iface;
    for (const QNetworkInterface &iface : std::as_const(now)) {
        const bool known = std::any_of(m_interfaces.cbegin(), m_interfaces.cend(),
                                       [&](const QNetworkInterface &i) { return i.index() == iface.index(); });
        if (!known && m_onMdnsPort)
            m_socket.joinMulticastGroup(kGroup, iface);
    }
    m_interfaces = now;
}

void MdnsDiscovery::sendToAll(const QByteArray &packet)
{
    if (m_interfaces.isEmpty()) {
        m_socket.writeDatagram(packet, kGroup, kPort);
        return;
    }
    for (const QNetworkInterface &iface : std::as_const(m_interfaces)) {
        m_socket.setMulticastInterface(iface);
        m_socket.writeDatagram(packet, kGroup, kPort);
    }
}

// ---------------------------------------------------------------------------- being found

QList<Mdns::Record> MdnsDiscovery::ourRecords(int interfaceIndex, quint32 ttl) const
{
    QList<Mdns::Record> records;
    Mdns::Record ptr;
    ptr.name = serviceName();
    ptr.type = Mdns::PTR;
    ptr.target = ourInstance();
    ptr.ttl = ttl ? kServiceTtl : 0;
    records << ptr;

    Mdns::Record srv;
    srv.name = ourInstance();
    srv.type = Mdns::SRV;
    srv.cacheFlush = true;
    srv.ttl = ttl ? kHostTtl : 0;
    srv.port = m_port;
    srv.target = ourHost();
    records << srv;

    Mdns::Record txt;
    txt.name = ourInstance();
    txt.type = Mdns::TXT;
    txt.cacheFlush = true;
    txt.ttl = ttl ? kServiceTtl : 0;
    txt.txt = { "v=1", "id=" + m_id.toUtf8(), "n=" + m_name.toUtf8().left(200),
                QByteArray("pair=") + (m_pairing ? "1" : "0") };
    records << txt;

    for (const QNetworkInterface &iface : m_interfaces) {
        if (interfaceIndex > 0 && iface.index() != interfaceIndex)
            continue;
        for (const QNetworkAddressEntry &e : iface.addressEntries()) {
            if (e.ip().protocol() != QAbstractSocket::IPv4Protocol)
                continue;
            Mdns::Record a;
            a.name = ourHost();
            a.type = Mdns::A;
            a.cacheFlush = true;
            a.ttl = ttl ? kHostTtl : 0;
            a.address = ipv4Bytes(e.ip());
            records << a;
        }
    }
    return records;
}

void MdnsDiscovery::announce(const QString &id, const QString &name, quint16 port, bool pairing)
{
    if (!ensureSocket())
        return;
    const bool changed = !m_announced || id != m_id || name != m_name || port != m_port || pairing != m_pairing;
    m_id = id;
    m_name = name;
    m_port = port;
    m_pairing = pairing;
    m_announced = true;
    if (!changed)
        return;
    // Twice, a second apart, as RFC 6762 asks (packets get lost in Wi-Fi)
    sendAnnouncement(kServiceTtl);
    m_reannounce.start(1000);
}

void MdnsDiscovery::sendAnnouncement(quint32 ttl)
{
    Mdns::Message m;
    m.response = true;
    m.answers = ourRecords(0, ttl);
    sendToAll(Mdns::encode(m));
}

void MdnsDiscovery::withdraw()
{
    if (!m_announced)
        return;
    sendAnnouncement(0);   // "goodbye": TTL 0
    m_announced = false;
    m_reannounce.stop();
}

void MdnsDiscovery::handleQuery(const Mdns::Message &query, const QNetworkDatagram &datagram)
{
    if (!m_announced)
        return;
    const QList<Mdns::Record> mine = ourRecords(datagram.interfaceIndex(), kServiceTtl);
    QList<Mdns::Record> answers;
    auto add = [&answers](const Mdns::Record &r) {
        for (const Mdns::Record &a : std::as_const(answers))
            if (a.type == r.type && a.name == r.name && a.address == r.address)
                return;
        answers << r;
    };
    for (const Mdns::Question &q : query.questions) {
        for (const Mdns::Record &r : mine) {
            if (Mdns::sameName(q.name, r.name) && (q.type == r.type || q.type == Mdns::ANY))
                add(r);
        }
    }
    if (answers.isEmpty())
        return;

    Mdns::Message reply;
    reply.response = true;
    reply.answers = answers;
    // Everything else about us, so the other side needs no second question
    for (const Mdns::Record &r : mine) {
        const bool answered = std::any_of(answers.cbegin(), answers.cend(), [&r](const Mdns::Record &a) {
            return a.type == r.type && a.name == r.name && a.address == r.address;
        });
        if (!answered)
            reply.additional << r;
    }

    if (datagram.senderPort() != kPort) {
        // "Legacy" one-shot question from an ordinary resolver: answer it directly
        reply.id = query.id;
        reply.questions = query.questions;
        for (auto *list : { &reply.answers, &reply.additional })
            for (Mdns::Record &r : *list)
                r.ttl = qMin<quint32>(r.ttl, 10);
        m_socket.writeDatagram(Mdns::encode(reply), datagram.senderAddress(), quint16(datagram.senderPort()));
        return;
    }
    const QByteArray packet = Mdns::encode(reply);
    for (const QNetworkInterface &iface : std::as_const(m_interfaces)) {
        if (datagram.interfaceIndex() > 0 && iface.index() != datagram.interfaceIndex())
            continue;
        m_socket.setMulticastInterface(iface);
        m_socket.writeDatagram(packet, kGroup, kPort);
    }
}

// ---------------------------------------------------------------------------- finding others

void MdnsDiscovery::browse()
{
    if (!ensureSocket())
        return;
    m_browsing = true;
    m_queryCount = 0;
    m_expireTimer.start();
    sendQuery();
}

void MdnsDiscovery::stopBrowsing()
{
    m_browsing = false;
    m_queryTimer.stop();
    m_expireTimer.stop();
}

void MdnsDiscovery::sendQuery()
{
    if (!m_browsing)
        return;
    joinInterfaces();
    ask(serviceName(), Mdns::PTR);
    // Soon again (lost packets), then once a minute
    static const int delays[] = { 1000, 3000, 10000 };
    m_queryTimer.start(m_queryCount < 3 ? delays[m_queryCount] : 60000);
    ++m_queryCount;
}

void MdnsDiscovery::ask(const QString &name, quint16 type)
{
    Mdns::Message m;
    m.questions << Mdns::Question { name, type, false };
    sendToAll(Mdns::encode(m));
}

void MdnsDiscovery::readPending()
{
    while (m_socket.hasPendingDatagrams()) {
        const QNetworkDatagram datagram = m_socket.receiveDatagram(9000);
        const auto message = Mdns::decode(datagram.data());
        if (!message)
            continue;
        if (message->response)
            handleResponse(*message);
        else
            handleQuery(*message, datagram);
    }
}

void MdnsDiscovery::handleResponse(const Mdns::Message &message)
{
    const QString suffix = u'.' + serviceName().toLower();
    QSet<QString> touched;
    QList<Mdns::Record> all = message.answers;
    all += message.additional;
    const QDateTime now = QDateTime::currentDateTimeUtc();

    for (const Mdns::Record &r : std::as_const(all)) {
        const QString name = r.name.toLower();
        if (r.type == Mdns::PTR && Mdns::sameName(r.name, serviceName())) {
            const QString key = r.target.toLower();
            if (r.ttl == 0) {
                if (m_instances.contains(key))
                    m_instances[key].expires = now;   // goodbye
            } else {
                m_instances[key].expires = now.addSecs(r.ttl);
            }
            touched << key;
        } else if (r.type == Mdns::SRV && name.endsWith(suffix)) {
            Instance &i = m_instances[name];
            i.host = r.target.toLower();
            i.port = r.port;
            i.haveSrv = true;
            if (!i.expires.isValid() || r.ttl == 0)
                i.expires = now.addSecs(r.ttl);
            touched << name;
        } else if (r.type == Mdns::TXT && name.endsWith(suffix)) {
            Instance &i = m_instances[name];
            i.txt = r.txt;
            i.haveTxt = true;
            if (!i.expires.isValid())
                i.expires = now.addSecs(r.ttl);
            touched << name;
        } else if (r.type == Mdns::A && r.address.size() == 4) {
            QList<QByteArray> &addresses = m_hosts[name];
            if (r.cacheFlush)
                addresses.clear();
            if (!addresses.contains(r.address))
                addresses << r.address;
            for (auto it = m_instances.cbegin(); it != m_instances.cend(); ++it)
                if (it->host == name)
                    touched << it.key();
        }
    }

    for (const QString &key : std::as_const(touched)) {
        if (!m_instances.contains(key))
            continue;
        if (m_instances[key].expires <= now) {
            expire();
            continue;
        }
        const Instance &i = m_instances[key];
        if (!i.haveSrv || !i.haveTxt)
            ask(key, i.haveSrv ? Mdns::TXT : Mdns::SRV);
        else if (m_hosts.value(i.host).isEmpty())
            ask(i.host, Mdns::A);
        else
            report(key);
    }
}

QString MdnsDiscovery::pickAddress(const QList<QByteArray> &addresses) const
{
    // One in the same network as us, if there is one
    for (const QByteArray &raw : addresses) {
        const QHostAddress address(qFromBigEndian<quint32>(raw.constData()));
        for (const QNetworkInterface &iface : m_interfaces)
            for (const QNetworkAddressEntry &e : iface.addressEntries())
                if (e.ip().protocol() == QAbstractSocket::IPv4Protocol
                    && address.isInSubnet(e.ip(), e.prefixLength()))
                    return address.toString();
    }
    return addresses.isEmpty() ? QString() : QHostAddress(qFromBigEndian<quint32>(addresses.first().constData())).toString();
}

void MdnsDiscovery::report(const QString &key)
{
    Instance &i = m_instances[key];
    const QString id = Mdns::txtValue(i.txt, "id");
    if (id.isEmpty() || id == m_id)
        return;   // not Qupil, or ourselves
    const QString name = Mdns::txtValue(i.txt, "n");
    const QString host = pickAddress(m_hosts.value(i.host));
    const bool pairing = Mdns::txtValue(i.txt, "pair") == u"1"_s;
    const QString state = u"%1|%2|%3|%4|%5"_s.arg(id, name, host).arg(i.port).arg(pairing);
    if (state == i.reported)
        return;
    i.reported = state;
    i.id = id;
    emit found(id, name, host, i.port, pairing);
}

void MdnsDiscovery::expire()
{
    const QDateTime now = QDateTime::currentDateTimeUtc();
    for (auto it = m_instances.begin(); it != m_instances.end();) {
        if (it->expires.isValid() && it->expires <= now) {
            if (!it->id.isEmpty())
                emit lost(it->id);
            it = m_instances.erase(it);
        } else {
            ++it;
        }
    }
}
