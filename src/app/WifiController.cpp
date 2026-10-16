// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "WifiController.h"

#include <QDateTime>
#include <QGuiApplication>
#include <QJSEngine>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkInterface>
#include <QPointer>
#include <QQmlEngine>
#include <QSettings>
#include <QTcpSocket>
#include <QThread>
#include <QUuid>

#include "AppController.h"
#include "Crypto.h"
#include "Database.h"
#include "Discovery.h"
#include "SocketChannel.h"
#include "Store.h"
#include "SyncController.h"

using namespace Qt::StringLiterals;
namespace Wifi = Qupil::WifiSync;

namespace {

WifiController *s_instance = nullptr;

constexpr int kAfterChangeMs = 15 * 1000;
constexpr int kPeriodicMs = 5 * 60 * 1000;
constexpr int kPairingMs = 10 * 60 * 1000;
constexpr int kConnectMs = 5000;
constexpr int kMaxFailedAttempts = 5;

template<typename Result, typename Work, typename Done>
void inWorker(QObject *context, Work work, Done done)
{
    QPointer<QObject> guard(context);   // the app may be closing while the worker runs
    QThread *thread = QThread::create([guard, work, done] {
        Result result = work();
        if (guard)
            QMetaObject::invokeMethod(guard.data(), [result, done] { done(result); }, Qt::QueuedConnection);
    });
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

QString nowIso()
{
    return QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
}

} // namespace

WifiController::WifiController(AppController *app, SyncController *sync, QObject *parent)
    : QObject(parent)
    , m_app(app)
    , m_sync(sync)
{
    Q_ASSERT_X(!s_instance, "WifiController", "only one instance allowed");
    s_instance = this;
    loadSettings();

    connect(&m_server, &WifiServer::connectionWaiting, this, &WifiController::accept);
    m_soon.setSingleShot(true);
    m_soon.setInterval(kAfterChangeMs);
    connect(&m_soon, &QTimer::timeout, this, &WifiController::syncAll);
    m_periodic.setInterval(kPeriodicMs);
    connect(&m_periodic, &QTimer::timeout, this, &WifiController::syncAll);
    m_pairingTimeout.setSingleShot(true);
    m_pairingTimeout.setInterval(kPairingMs);
    connect(&m_pairingTimeout, &QTimer::timeout, this, &WifiController::stopPairing);

    connect(app->store(), &Qupil::Store::dataChanged, this, [this] {
        if (!m_peers.isEmpty())
            m_soon.start();
    });
    connect(sync, &SyncController::deviceNameChanged, this, &WifiController::announce);
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, [this](Qt::ApplicationState state) {
        if (state != Qt::ApplicationActive || !active())
            return;
        if (m_discovery)
            m_discovery->browse();
        QTimer::singleShot(2000, this, &WifiController::syncAll);
    });
    connect(app, &AppController::phaseChanged, this, [this] {
        updateRunning();
        if (m_app->phase() == AppController::Ready)
            QTimer::singleShot(3000, this, &WifiController::syncAll);
    });
    // After an import or restore the data here is a different one: ask before syncing it
    connect(app, &AppController::dataReplaced, this, [this] {
        if (m_replacingData)
            return;
        for (PeerInfo &p : m_peers)
            p.paused = true;
        if (!m_peers.isEmpty()) {
            saveSettings();
            emit peersChanged();
        }
    });
    connect(app, &AppController::busyChanged, this, [this] {
        if (!m_app->busy())
            m_soon.start(1000);
    });
    updateRunning();
}

WifiController::~WifiController()
{
    if (m_discovery)
        m_discovery->withdraw();
    if (s_instance == this)
        s_instance = nullptr;
}

WifiController *WifiController::instance()
{
    return s_instance;
}

WifiController *WifiController::create(QQmlEngine *, QJSEngine *)
{
    Q_ASSERT_X(s_instance, "WifiController", "create the instance in main() before loading QML");
    QJSEngine::setObjectOwnership(s_instance, QJSEngine::CppOwnership);
    return s_instance;
}

// ---------------------------------------------------------------------------- settings

void WifiController::loadSettings()
{
    QSettings s;
    s.beginGroup(u"wifi"_s);
    m_enabled = s.value(u"enabled"_s, true).toBool();
    m_id = s.value(u"id"_s).toString();
    m_secretKey = QByteArray::fromBase64(s.value(u"secret"_s).toByteArray());
    const QJsonArray peers = QJsonDocument::fromJson(s.value(u"peers"_s).toByteArray()).array();
    for (const QJsonValue &v : peers) {
        const QJsonObject o = v.toObject();
        PeerInfo p;
        p.peer.id = o.value(u"id"_s).toString();
        p.peer.name = o.value(u"name"_s).toString();
        p.peer.publicKey = QByteArray::fromBase64(o.value(u"key"_s).toString().toLatin1());
        p.host = o.value(u"host"_s).toString();
        p.port = quint16(o.value(u"port"_s).toInt());
        p.lastSync = o.value(u"lastSync"_s).toString();
        p.paused = o.value(u"paused"_s).toBool();
        p.problem = o.value(u"problem"_s).toString();
        if (!p.peer.id.isEmpty() && p.peer.publicKey.size() == 32)
            m_peers << p;
    }
}

void WifiController::saveSettings()
{
    QJsonArray peers;
    for (const PeerInfo &p : std::as_const(m_peers)) {
        peers.append(QJsonObject { { u"id"_s, p.peer.id }, { u"name"_s, p.peer.name },
                                   { u"key"_s, QString::fromLatin1(p.peer.publicKey.toBase64()) },
                                   { u"host"_s, p.host }, { u"port"_s, p.port }, { u"lastSync"_s, p.lastSync },
                                   { u"paused"_s, p.paused }, { u"problem"_s, p.problem } });
    }
    QSettings s;
    s.beginGroup(u"wifi"_s);
    s.setValue(u"enabled"_s, m_enabled);
    s.setValue(u"id"_s, m_id);
    s.setValue(u"secret"_s, m_secretKey.toBase64());
    s.setValue(u"peers"_s, QJsonDocument(peers).toJson(QJsonDocument::Compact));
}

void WifiController::ensureIdentity()
{
    if (!m_id.isEmpty() && m_secretKey.size() == 32) {
        if (m_publicKey.isEmpty()) {
            // The public key follows from the secret one
            m_publicKey = Qupil::Crypto::x25519PublicKey(m_secretKey);
        }
        return;
    }
    m_id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    const Qupil::Crypto::KeyPair keys = Qupil::Crypto::x25519KeyPair();
    m_secretKey = keys.secretKey;
    m_publicKey = keys.publicKey;
    saveSettings();
}

Wifi::Identity WifiController::identity() const
{
    return { m_id, m_sync->deviceName(), m_secretKey, m_publicKey };
}

void WifiController::setEnabled(bool on)
{
    if (on == m_enabled)
        return;
    m_enabled = on;
    saveSettings();
    if (!on)
        stopPairing();
    updateRunning();
    emit stateChanged();
}

// ---------------------------------------------------------------------------- visible in the network

void WifiController::updateRunning()
{
    const bool run = m_enabled && m_app->phase() == AppController::Ready && (pairing() || !m_peers.isEmpty());
    if (run == active())
        return;
    if (run) {
        ensureIdentity();
        if (!m_server.listen(QHostAddress::Any, PreferredPort) && !m_server.listen(QHostAddress::Any, 0)
            && !m_server.listen(QHostAddress::AnyIPv4, 0)) {
            qWarning("Wi-Fi sync: cannot listen (%s)", qPrintable(m_server.errorString()));
            return;
        }
        if (!m_discovery) {
            m_discovery = Discovery::create(this);
            connect(m_discovery, &Discovery::found, this,
                    [this](const QString &id, const QString &name, const QString &host, quint16 port, bool pairing) {
                        const bool wasKnown = m_found.contains(id);
                        m_found.insert(id, { name, host, port, pairing });
                        if (PeerInfo *p = peer(id)) {
                            const bool moved = p->host != host || p->port != port;
                            p->host = host;
                            p->port = port;
                            if (!name.isEmpty() && name != p->peer.name)
                                p->peer.name = name;
                            if (moved)
                                saveSettings();
                            emit peersChanged();
                            if (!wasKnown)
                                QTimer::singleShot(1000, this, [this, id] { syncWith(id); });
                        }
                        emit nearbyChanged();
                    });
            connect(m_discovery, &Discovery::lost, this, [this](const QString &id) {
                if (m_found.remove(id)) {
                    emit nearbyChanged();
                    if (peer(id))
                        emit peersChanged();
                }
            });
        }
        announce();
        m_discovery->browse();
        m_periodic.start();
    } else {
        if (m_discovery) {
            m_discovery->withdraw();
            m_discovery->stopBrowsing();
        }
        m_server.close();
        m_periodic.stop();
        m_found.clear();
        emit nearbyChanged();
    }
    emit stateChanged();
    emit peersChanged();
}

void WifiController::announce()
{
    if (m_discovery && active())
        m_discovery->announce(m_id, m_sync->deviceName(), m_server.serverPort(), pairing());
}

QStringList WifiController::addresses() const
{
    QStringList out;
    if (!active())
        return out;
    for (const QNetworkInterface &iface : QNetworkInterface::allInterfaces()) {
        if (!(iface.flags() & QNetworkInterface::IsUp) || (iface.flags() & QNetworkInterface::IsLoopBack))
            continue;
        for (const QNetworkAddressEntry &e : iface.addressEntries()) {
            if (e.ip().protocol() != QAbstractSocket::IPv4Protocol || e.ip().isLinkLocal())
                continue;
            QString a = e.ip().toString();
            if (m_server.serverPort() != PreferredPort)
                a += u':' + QString::number(m_server.serverPort());
            out << a;
        }
    }
    return out;
}

// ---------------------------------------------------------------------------- lists for QML

QVariantList WifiController::peers() const
{
    QVariantList out;
    for (const PeerInfo &p : m_peers) {
        out << QVariantMap { { u"id"_s, p.peer.id }, { u"name"_s, p.peer.name },
                             { u"online"_s, m_found.contains(p.peer.id) }, { u"lastSync"_s, p.lastSync },
                             { u"syncing"_s, p.syncing }, { u"paused"_s, p.paused }, { u"problem"_s, p.problem } };
    }
    return out;
}

QVariantList WifiController::nearby() const
{
    QVariantList out;
    for (auto it = m_found.cbegin(); it != m_found.cend(); ++it) {
        if (!it->pairing)
            continue;
        if (const PeerInfo *known = peer(it.key()); known && known->problem != u"unknown"_s)
            continue;   // paired already
        out << QVariantMap { { u"id"_s, it.key() }, { u"name"_s, it->name } };
    }
    return out;
}

QString WifiController::lastSync() const
{
    QString latest;
    for (const PeerInfo &p : m_peers)
        if (p.lastSync > latest)
            latest = p.lastSync;
    return latest;
}

QString WifiController::lastPeer() const
{
    QString latest, name;
    for (const PeerInfo &p : m_peers) {
        if (p.lastSync > latest) {
            latest = p.lastSync;
            name = p.peer.name;
        }
    }
    return name;
}

WifiController::PeerInfo *WifiController::peer(const QString &id)
{
    for (PeerInfo &p : m_peers)
        if (p.peer.id == id)
            return &p;
    return nullptr;
}

const WifiController::PeerInfo *WifiController::peer(const QString &id) const
{
    for (const PeerInfo &p : m_peers)
        if (p.peer.id == id)
            return &p;
    return nullptr;
}

bool WifiController::hasLocalData() const
{
    return m_app->database()->isOpen() && !m_app->database()->isEmpty() && !m_app->awaitingSync();
}

void WifiController::beginWork()
{
    ++m_running;
    m_app->beginSync();
    if (m_running == 1)
        emit busyChanged();
}

void WifiController::endWork()
{
    m_running = std::max(0, m_running - 1);
    if (m_running == 0)
        emit busyChanged();
    m_app->endSync();   // last: may finish an import that waited
}

// ---------------------------------------------------------------------------- pairing

void WifiController::startPairing()
{
    if (!m_enabled) {
        m_enabled = true;
        saveSettings();
    }
    m_pairingCode = Wifi::newCode();
    m_failedAttempts = 0;
    m_pairingTimeout.start();
    updateRunning();
    announce();
    if (m_discovery)
        m_discovery->browse();
    emit pairingChanged();
    emit stateChanged();
}

void WifiController::stopPairing()
{
    if (m_pairingCode.isEmpty())
        return;
    m_pairingCode.clear();
    m_pairingTimeout.stop();
    announce();
    updateRunning();
    emit pairingChanged();
    emit nearbyChanged();
}

void WifiController::pairWith(const QString &deviceId, const QString &code)
{
    const auto it = m_found.constFind(deviceId);
    if (it == m_found.cend()) {
        emit message(qtTrId("wifi.error.gone"));
        return;
    }
    pairAt(it->host, it->port, code);
}

void WifiController::pairWithAddress(const QString &address, const QString &code)
{
    QString host = address.trimmed();
    quint16 port = PreferredPort;
    const qsizetype colon = host.lastIndexOf(u':');
    if (colon > 0 && host.count(u':') == 1) {
        port = quint16(host.mid(colon + 1).toUInt());
        host = host.left(colon);
    }
    if (host.isEmpty() || port == 0) {
        emit message(qtTrId("wifi.error.address"));
        return;
    }
    pairAt(host, port, code);
}

void WifiController::pairAt(const QString &host, quint16 port, const QString &code)
{
    if (m_connecting)
        return;
    ensureIdentity();
    m_connecting = true;
    emit busyChanged();
    const Wifi::Identity me = identity();
    const bool hasData = hasLocalData();
    struct Result { Wifi::Status status = Wifi::Status::Failed; Wifi::Peer peer; QString error; };
    inWorker<Result>(this, [host, port, code, me, hasData] {
        Result r;
        QTcpSocket socket;
        socket.connectToHost(host, port);
        if (!socket.waitForConnected(kConnectMs)) {
            r.error = socket.errorString();
            return r;
        }
        SocketChannel channel(&socket);
        r.status = Wifi::pair(&channel, me, code, hasData, &r.peer, &r.error);
        socket.disconnectFromHost();
        return r;
    }, [this, host, port](const Result &r) {
        m_connecting = false;
        emit busyChanged();
        switch (r.status) {
        case Wifi::Status::Ok: {
            // Both had data: the user decides how they go together, until then nothing is synced
            const bool ask = hasLocalData() && r.peer.hasData;
            addPeer(r.peer, host, port, ask);
            stopPairing();
            //: %1 = device name
            emit message(qtTrId("wifi.paired").arg(r.peer.name));
            if (ask)
                emit joinChoiceNeeded(r.peer.id, r.peer.name);
            else
                syncWith(r.peer.id);
            break;
        }
        case Wifi::Status::WrongCode:
            emit message(qtTrId("wifi.error.wrong_code"));
            break;
        case Wifi::Status::NotPairing:
            emit message(qtTrId("wifi.error.not_pairing"));
            break;
        case Wifi::Status::Outdated:
            emit message(qtTrId("wifi.error.version"));
            break;
        default:
            //: %1 = technical reason
            emit message(qtTrId("wifi.error.connect").arg(r.error));
            break;
        }
    });
}

void WifiController::addPeer(const Wifi::Peer &newPeer, const QString &host, quint16 port, bool paused)
{
    if (PeerInfo *existing = peer(newPeer.id)) {
        existing->peer = newPeer;     // paired again: new keys
        existing->host = host;
        existing->port = port;
        existing->paused = paused;
        existing->problem.clear();
    } else {
        PeerInfo p;
        p.peer = newPeer;
        p.host = host;
        p.port = port;
        p.paused = paused;
        m_peers << p;
    }
    saveSettings();
    updateRunning();
    emit peersChanged();
    emit nearbyChanged();
}

void WifiController::resolveJoin(const QString &peerId, int mode)
{
    PeerInfo *p = peer(peerId);
    if (!p)
        return;
    if (mode == SyncController::ReplaceLocal && !m_app->awaitingSync()) {
        if (m_app->busy() || m_app->syncRunning()) {
            emit message(qtTrId("sync.failed").arg(qtTrId("sync.error.replace")));
            return;
        }
        m_replacingData = true;
        const bool replaced = m_app->replaceWithEmpty();
        m_replacingData = false;
        if (!replaced) {
            emit message(qtTrId("sync.failed").arg(qtTrId("sync.error.replace")));
            return;
        }
        p = peer(peerId);
    }
    // The decision covers the data as it is now: every device that waited may sync
    for (PeerInfo &other : m_peers)
        other.paused = false;
    saveSettings();
    emit peersChanged();
    if (p)
        syncWith(p->peer.id);
    syncAll();
}

void WifiController::forget(const QString &peerId)
{
    for (int i = 0; i < m_peers.size(); ++i) {
        if (m_peers.at(i).peer.id == peerId) {
            m_peers.removeAt(i);
            break;
        }
    }
    saveSettings();
    updateRunning();
    emit peersChanged();
    emit nearbyChanged();
}

// ---------------------------------------------------------------------------- sync

void WifiController::syncNow()
{
    if (m_discovery && active())
        m_discovery->browse();
    syncAll();
}

void WifiController::syncAll()
{
    for (const PeerInfo &p : std::as_const(m_peers))
        syncWith(p.peer.id);
}

void WifiController::syncWith(const QString &peerId)
{
    PeerInfo *p = peer(peerId);
    if (!p || p->paused || p->syncing || !m_enabled || m_app->phase() != AppController::Ready || m_app->busy())
        return;
    if (p->host.isEmpty() || p->port == 0)
        return;
    ensureIdentity();
    p->syncing = true;
    emit peersChanged();
    beginWork();
    const Wifi::Identity me = identity();
    const Wifi::Peer other = p->peer;
    const QString host = p->host;
    const quint16 port = p->port;
    const QString databasePath = m_app->databasePath();
    struct Result { Wifi::Status status = Wifi::Status::Failed; Wifi::Stats stats; QString error; bool reached = false; };
    inWorker<Result>(this, [me, other, host, port, databasePath] {
        Result r;
        QTcpSocket socket;
        socket.connectToHost(host, port);
        if (!socket.waitForConnected(kConnectMs)) {
            r.error = socket.errorString();
            return r;
        }
        r.reached = true;
        Qupil::Database db;
        if (!db.open(databasePath, &r.error))
            return r;
        Qupil::Store store(&db);
        SocketChannel channel(&socket);
        r.status = Wifi::sync(&channel, &store, me, other, &r.stats, &r.error);
        socket.disconnectFromHost();
        if (socket.state() != QAbstractSocket::UnconnectedState)
            socket.waitForDisconnected(1000);
        db.close();
        return r;
    }, [this, peerId](const Result &r) {
        if (PeerInfo *p = peer(peerId)) {
            p->syncing = false;
            if (r.status == Wifi::Status::Ok) {
                p->lastSync = nowIso();
                p->problem.clear();
            } else if (r.status == Wifi::Status::UnknownDevice || r.status == Wifi::Status::AuthFailed) {
                p->problem = u"unknown"_s;   // removed or paired again on the other device
            } else if (r.status == Wifi::Status::Outdated) {
                p->problem = u"version"_s;   // one of the two needs an update
            }
            saveSettings();
        }
        if (r.status == Wifi::Status::Ok)
            applied(r.stats);
        emit peersChanged();
        endWork();
    });
}

void WifiController::accept(qintptr socketDescriptor)
{
    if (m_app->phase() != AppController::Ready || m_app->busy()) {
        QTcpSocket socket;
        socket.setSocketDescriptor(socketDescriptor);
        socket.abort();
        return;
    }
    // What the worker may do: a copy of the current state
    Wifi::ServerState state;
    for (const PeerInfo &p : std::as_const(m_peers)) {
        state.peers << p.peer;
        if (p.paused)
            state.paused << p.peer.id;
    }
    state.pairingCode = m_pairingCode;
    state.hasData = hasLocalData();
    ensureIdentity();
    const Wifi::Identity me = identity();
    const QString databasePath = m_app->databasePath();
    beginWork();
    struct Result { Wifi::ServeResult serve; QString host; };
    inWorker<Result>(this, [socketDescriptor, state, me, databasePath] {
        Result r;
        QTcpSocket socket;
        if (!socket.setSocketDescriptor(socketDescriptor))
            return r;
        r.host = socket.peerAddress().toString();
        if (r.host.startsWith(u"::ffff:"_s))
            r.host = r.host.mid(7);   // IPv4 seen through an IPv6 socket
        Qupil::Database db;
        if (!db.open(databasePath, &r.serve.error))
            return r;
        Qupil::Store store(&db);
        SocketChannel channel(&socket);
        r.serve = Wifi::serve(&channel, &store, me, state);
        socket.disconnectFromHost();
        if (socket.state() != QAbstractSocket::UnconnectedState)
            socket.waitForDisconnected(1000);
        db.close();
        return r;
    }, [this](const Result &r) {
        const Wifi::ServeResult &s = r.serve;
        if (s.kind == Wifi::ServeResult::Paired) {
            if (s.status == Wifi::Status::Ok) {
                // Their port is unknown until discovery finds them; the default is a good guess
                const auto found = m_found.constFind(s.peer.id);
                addPeer(s.peer, r.host, found != m_found.cend() ? found->port : PreferredPort, false);
                stopPairing();
                emit message(qtTrId("wifi.paired").arg(s.peer.name));
            } else if (s.status == Wifi::Status::WrongCode && pairing()) {
                // One guess per code: show a new one
                if (++m_failedAttempts >= kMaxFailedAttempts) {
                    stopPairing();
                } else {
                    m_pairingCode = Wifi::newCode();
                    emit pairingChanged();
                }
                emit message(qtTrId("wifi.error.wrong_code_here"));
            }
        } else if (s.kind == Wifi::ServeResult::Synced) {
            if (PeerInfo *p = peer(s.peer.id)) {
                if (s.status == Wifi::Status::Ok) {
                    p->lastSync = nowIso();
                    p->problem.clear();
                    if (!r.host.isEmpty() && p->host != r.host && !m_found.contains(p->peer.id))
                        p->host = r.host;
                    saveSettings();
                } else if (s.status == Wifi::Status::Outdated) {
                    p->problem = u"version"_s;
                    saveSettings();
                }
            }
            if (s.status == Wifi::Status::Ok)
                applied(s.stats);
            emit peersChanged();
        }
        endWork();
    });
}

void WifiController::applied(const Wifi::Stats &stats)
{
    if (stats.applied > 0) {
        m_app->database()->clock().observe(stats.maxTime);
        m_app->store()->notifyReset();
    }
    if (m_app->awaitingSync() && stats.received > 0) {
        m_app->syncArrived();
        emit firstSyncDone();
    }
}
