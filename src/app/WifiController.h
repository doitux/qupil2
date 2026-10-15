// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QHash>
#include <QObject>
#include <QStringList>
#include <QTcpServer>
#include <QTimer>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

#include "WifiSession.h"

class AppController;
class Discovery;
class QJSEngine;
class QQmlEngine;
class SyncController;

//! Hands accepted connections to WifiController as socket descriptors (for worker threads)
class WifiServer : public QTcpServer
{
    Q_OBJECT

public:
    using QTcpServer::QTcpServer;

signals:
    void connectionWaiting(qintptr socketDescriptor);

protected:
    void incomingConnection(qintptr socketDescriptor) override { emit connectionWaiting(socketDescriptor); }
};

/*!
    Sync over Wi-Fi, available in QML as the singleton "Wifi".

    Pairing: "Pair device" on both devices shows a code on each and lists the
    devices nearby that show one; entering the code of the other device pairs
    the two (see Qupil::WifiSync). Paired devices sync whenever they see each
    other: at start, shortly after changes and every few minutes.

    Only active once a device was paired (or while pairing), so nothing is
    visible in the network for people who do not use it.
*/
class WifiController : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Wifi)
    QML_SINGLETON

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY stateChanged FINAL)
    //! Visible in the network and listening
    Q_PROPERTY(bool active READ active NOTIFY stateChanged FINAL)
    Q_PROPERTY(bool pairing READ pairing NOTIFY pairingChanged FINAL)
    Q_PROPERTY(QString pairingCode READ pairingCode NOTIFY pairingChanged FINAL)
    //! Our IPv4 addresses, for pairing by address when devices do not find each other
    Q_PROPERTY(QStringList addresses READ addresses NOTIFY stateChanged FINAL)
    //! Paired devices: id, name, online, lastSync, syncing, paused, problem
    Q_PROPERTY(QVariantList peers READ peers NOTIFY peersChanged FINAL)
    //! Devices nearby that show a pairing code: id, name
    Q_PROPERTY(QVariantList nearby READ nearby NOTIFY nearbyChanged FINAL)
    //! A pairing attempt of ours is running
    Q_PROPERTY(bool connecting READ connecting NOTIFY busyChanged FINAL)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged FINAL)
    //! Latest sync with any device, and its name
    Q_PROPERTY(QString lastSync READ lastSync NOTIFY peersChanged FINAL)
    Q_PROPERTY(QString lastPeer READ lastPeer NOTIFY peersChanged FINAL)

public:
    static constexpr quint16 PreferredPort = 47123;

    WifiController(AppController *app, SyncController *sync, QObject *parent);
    ~WifiController() override;

    static WifiController *instance();
    static WifiController *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    bool enabled() const { return m_enabled; }
    void setEnabled(bool on);
    bool active() const { return m_server.isListening(); }
    bool pairing() const { return !m_pairingCode.isEmpty(); }
    QString pairingCode() const { return m_pairingCode; }
    QStringList addresses() const;
    QVariantList peers() const;
    QVariantList nearby() const;
    bool connecting() const { return m_connecting; }
    bool busy() const { return m_running > 0; }
    QString lastSync() const;
    QString lastPeer() const;

    //! Shows a code on this device and looks for devices that show one
    Q_INVOKABLE void startPairing();
    Q_INVOKABLE void stopPairing();
    //! Pairs with a device from "nearby" with the code it shows
    Q_INVOKABLE void pairWith(const QString &deviceId, const QString &code);
    //! Pairs by address ("192.168.1.23" or "192.168.1.23:47123")
    Q_INVOKABLE void pairWithAddress(const QString &address, const QString &code);
    /*!
        Both devices had data when they were paired (or the data here was
        replaced): mode is SyncController::Merge or SyncController::ReplaceLocal.
    */
    Q_INVOKABLE void resolveJoin(const QString &peerId, int mode);
    Q_INVOKABLE void forget(const QString &peerId);
    Q_INVOKABLE void syncNow();

signals:
    void stateChanged();
    void pairingChanged();
    void peersChanged();
    void nearbyChanged();
    void busyChanged();
    //! Short messages for the user (paired, wrong code, …)
    void message(const QString &text);
    //! Both devices have data: ask how they go together (then resolveJoin)
    void joinChoiceNeeded(const QString &peerId, const QString &peerName);
    //! The first sync of a device that waited for data is done
    void firstSyncDone();

private:
    struct PeerInfo
    {
        Qupil::WifiSync::Peer peer;
        QString host;
        quint16 port = 0;
        QString lastSync;          // ISO, UTC
        bool paused = false;       // waits for the join decision
        QString problem;           // the other device does not know us any more
        bool syncing = false;
    };
    struct Found
    {
        QString name;
        QString host;
        quint16 port = 0;
        bool pairing = false;
    };

    void loadSettings();
    void saveSettings();
    void ensureIdentity();
    Qupil::WifiSync::Identity identity() const;
    void updateRunning();
    void announce();
    PeerInfo *peer(const QString &id);
    const PeerInfo *peer(const QString &id) const;
    void addPeer(const Qupil::WifiSync::Peer &peer, const QString &host, quint16 port, bool paused);
    void pairAt(const QString &host, quint16 port, const QString &code);
    void syncWith(const QString &peerId);
    void syncAll();
    void accept(qintptr socketDescriptor);
    void applied(const Qupil::WifiSync::Stats &stats);
    bool hasLocalData() const;
    void beginWork();
    void endWork();

    AppController *m_app;
    SyncController *m_sync;
    Discovery *m_discovery = nullptr;
    WifiServer m_server;
    QTimer m_soon;
    QTimer m_periodic;
    QTimer m_pairingTimeout;

    bool m_enabled = true;
    QString m_id;
    QByteArray m_secretKey;
    QByteArray m_publicKey;
    QList<PeerInfo> m_peers;
    QHash<QString, Found> m_found;
    QString m_pairingCode;
    int m_failedAttempts = 0;
    bool m_connecting = false;
    int m_running = 0;
    bool m_replacingData = false;
};
