# Sync in Qupil 2

Qupil keeps the same data on all devices of one teacher. Two ways, usable together:

* **Nextcloud**: through a folder in the user's own Nextcloud, end-to-end encrypted.
* **Wi-Fi**: devices in the same network sync directly, without any server.

Changes travel in both ways and are relayed: a phone that only uses Wi-Fi gets
what the laptop received from Nextcloud, and the other way round.

## Data model

Every change to synchronised data is a row in `change_log`:
`(origin, seq, hlc, entity, row_id, field, value)`.

* `origin` is the device id of the database (new after restoring a backup),
  `seq` counts the changes of that origin without gaps.
* `hlc` is a hybrid logical clock (`<ms>-<counter>-<device>`): ordered like time,
  but never behind a change the device has already seen.
* Merging is per field: the change with the larger `hlc` wins (last writer wins).
  Different fields of the same student changed on two devices both survive.
* A version vector (`origin → highest seq`) says what a device has; the other side
  sends everything above it. Applying the same changes twice changes nothing.

## Nextcloud

Sign-in uses Nextcloud's *Login Flow v2*: Qupil opens the browser, the user signs
in there and Qupil receives an app password (revocable in Nextcloud's security
settings). Qupil never sees the Nextcloud password. Files are written with WebDAV
under `remote.php/dav/files/<user id>/Qupil/`.

```
Qupil/
  key.json                               salt, Argon2 parameters, check value
  devices/<device>.bin                   name and last sync of each device
  changes/<origin>/<first>-<last>.bin    change log in batches of 2000
```

* The **sync password** (chosen on the first device) is stretched with Argon2id
  (64 MiB, 3 passes) into the key. Everything except `key.json` is encrypted with
  XChaCha20-Poly1305; the file path is authenticated as well, so files cannot be
  swapped. The password cannot be recovered; the key is kept on each device.
* Each origin has its own folder and every batch file is written once, so two
  devices never overwrite each other's files.
* Every sync first checks the key against `key.json`. If another device *started
  over* (new password) or the folder was deleted, Qupil asks for the password
  again instead of writing into the folder.
* A device that already has data and joins an existing folder chooses:
  **replace** its data with the folder's (default, a backup is kept), **merge**
  (only for data that exists nowhere else, otherwise entries appear twice), or
  **start over** with its data (the old folder is kept as `Qupil-<date>`).
* After an import or a restored backup the sync pauses until the password is
  entered again, so a replaced data set is never merged by accident.

Automatic sync: at start, 15 seconds after a change, every 10 minutes and when
the app comes back to the front. It runs in a worker thread with its own
database connection.

## Wi-Fi

**Finding devices**: DNS service discovery (`_qupil-sync._tcp`, "Bonjour").
macOS and iOS use the system's Bonjour (iOS asks once for local network access),
Windows, Linux and Android a small multicast DNS responder of Qupil's own
(`MdnsDiscovery`), compatible with Bonjour and Avahi. Each device announces
`Qupil-<id>` with TXT entries `id`, `n` (name), `pair` and `v`. Qupil listens on
TCP port 47123 (or any free port) and is only visible after pairing a device or
while the pairing screen is open.

**Pairing**: "Pair device" shows a 6-digit code on both devices and lists the
devices nearby that show one. Entering the other device's code runs **CPace**, a
password-authenticated key exchange (X25519 with an Elligator 2 generator from
the code): listening in reveals nothing about the code, and an attacker in the
middle gets one guess per code (a wrong guess shows a new code; after five the
pairing stops). Over the resulting channel the devices exchange their long-term
X25519 keys. Pairing by IP address works when discovery is blocked.

**Sync**: paired devices authenticate with their long-term keys plus fresh
ephemeral keys (four X25519 results, like Noise KK). All frames after the
greeting are encrypted with XChaCha20-Poly1305 and numbered. The client sends its
version vector and the changes the server lacks, then the server sends its own.

If both devices already had data when they were paired, the one that entered
the code asks: **replace** its data with the other device's, or **merge**. Until
then it refuses sync with that device.

Automatic sync: when a paired device appears, 15 seconds after a change, every
5 minutes and when the app comes back to the front.

## Code

| Part | File |
| --- | --- |
| Change log, merge, version vectors | `src/core/Store.*`, `src/core/Ids.*` (clock) |
| Encryption (Monocypher) | `src/core/Crypto.*`, `3rdparty/monocypher` |
| Folder sync (format, encryption) | `src/core/FolderSync.*` |
| Wi-Fi protocol (CPace, sessions) | `src/core/WifiSession.*` |
| mDNS messages | `src/core/Mdns.*` |
| Nextcloud: sign-in, WebDAV, scheduling | `src/app/SyncController.*`, `src/app/WebDavFolder.*` |
| Wi-Fi: discovery, server, scheduling | `src/app/WifiController.*`, `src/app/*Discovery.*`, `src/app/SocketChannel.h` |
| User interface | `qml/pages/SyncPage.qml`, `qml/editors/PairingSheet.qml`, More page card |

The unit tests (`tests/tst_core.cpp`) cover merging, relaying, the folder sync
(in memory), pairing and sync over an in-memory connection, and the mDNS
messages.
