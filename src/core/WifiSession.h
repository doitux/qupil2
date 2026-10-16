// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QByteArray>
#include <QList>
#include <QString>
#include <QStringList>

#include <optional>

#include "Store.h"

namespace Qupil {

/*!
    A blocking connection that carries whole messages (frames): TCP in the
    app, an in-memory pipe in the tests. Used from one worker thread.
*/
class FrameChannel
{
public:
    virtual ~FrameChannel() = default;
    virtual bool send(const QByteArray &frame) = 0;
    //! nullopt on timeout, error or when the other side closed the connection
    virtual std::optional<QByteArray> receive() = 0;
};

/*!
    Sync between two devices in the same Wi-Fi, without a server.

    Pairing: one device shows a 6-digit code, the other enters it. Both run
    CPace (a password-authenticated key exchange on X25519 with Elligator 2):
    whoever listens in learns nothing about the code, and an attacker in the
    middle gets exactly one guess per attempt. Over that channel the devices
    exchange their long-term X25519 keys.

    Sync: the paired devices authenticate each other with their long-term keys
    and fresh ephemeral keys (like Noise KK), then exchange version vectors and
    the changes the other side is missing. Every frame after the greeting is
    encrypted (XChaCha20-Poly1305) and numbered, so nothing can be read,
    changed, repeated or reordered.

    Order, so neither side waits for the other: the client sends its changes,
    then the server sends its own.
*/
namespace WifiSync {

//! 2: sync format 2 (one change per record and write)
constexpr int ProtocolVersion = 2;

//! This installation (not the database: that one gets a new id on restore)
struct Identity
{
    QString id;
    QString name;
    QByteArray secretKey;
    QByteArray publicKey;
};

//! A paired device
struct Peer
{
    QString id;
    QString name;
    QByteArray publicKey;
    bool hasData = false;   //!< told while pairing: the device already had data
};

enum class Status {
    Ok,
    WrongCode,        //!< pairing: the code did not match
    NotPairing,       //!< pairing: the other device does not show a code (any more)
    UnknownDevice,    //!< sync: the other device does not know us (not paired or removed)
    Refused,          //!< sync: the other device paused the sync with us
    AuthFailed,       //!< sync: keys do not match (paired again elsewhere?)
    Outdated,         //!< the other device speaks another protocol version
    Failed            //!< connection or data problem
};

struct Stats
{
    int sent = 0;        //!< changes sent
    int received = 0;    //!< changes received
    int applied = 0;     //!< of those, newer than what we had
    qint64 maxTime = 0;  //!< latest received time (for our clock)
};

//! Client: pairs with the device that shows code. On success, *peer is that device.
Status pair(FrameChannel *channel, const Identity &me, const QString &code, bool hasData, Peer *peer,
            QString *error = nullptr);

//! Client: syncs with a paired device
Status sync(FrameChannel *channel, Store *store, const Identity &me, const Peer &peer, Stats *stats,
            QString *error = nullptr);

//! What the server allows when another device connects
struct ServerState
{
    QList<Peer> peers;          //!< paired devices
    QStringList paused;         //!< paired, but sync is paused until the user decides
    QString pairingCode;        //!< empty: not in pairing mode
    bool hasData = false;
};

struct ServeResult
{
    enum Kind { None, Paired, Synced };
    Status status = Status::Failed;
    Kind kind = None;
    Peer peer;                  //!< the other device (for Paired: new)
    Stats stats;
    QString error;
};

//! Server: answers one connection (pairing or sync)
ServeResult serve(FrameChannel *channel, Store *store, const Identity &me, const ServerState &state);

//! "482913" from "482 913", "482-913"
QString normalizeCode(const QString &code);
//! A new random 6-digit code
QString newCode();

} // namespace WifiSync
} // namespace Qupil
