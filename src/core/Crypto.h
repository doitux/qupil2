// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QByteArray>
#include <QString>

#include <optional>

/*!
    End-to-end encryption for the sync (Monocypher):
    XChaCha20-Poly1305 for data, Argon2id for the sync password,
    X25519 for pairing devices, BLAKE2b for hashing.
*/
namespace Qupil::Crypto {

constexpr int KeySize = 32;
constexpr int SaltSize = 16;

QByteArray randomBytes(int size);

struct KdfParams
{
    int memoryKiB = 64 * 1024;
    int passes = 3;
};

//! 32-byte key from the sync password (slow on purpose)
QByteArray deriveKey(const QString &password, const QByteArray &salt, KdfParams params = {});

//! nonce (24) + MAC (16) + cipher text. ad is authenticated but not encrypted.
QByteArray seal(const QByteArray &key, const QByteArray &plainText, const QByteArray &ad = {});
//! nullopt if the key is wrong or the data was changed
std::optional<QByteArray> open(const QByteArray &key, const QByteArray &sealed, const QByteArray &ad = {});

//! BLAKE2b, optionally keyed
QByteArray hash(const QByteArray &data, int size = 32, const QByteArray &key = {});

struct KeyPair
{
    QByteArray secretKey;
    QByteArray publicKey;
};
KeyPair x25519KeyPair();
//! Shared key from our secret and their public key (hashed, ready to use)
QByteArray x25519Shared(const QByteArray &secretKey, const QByteArray &theirPublicKey);

//! Overwrites a secret in memory
void wipe(QByteArray &secret);

} // namespace Qupil::Crypto
