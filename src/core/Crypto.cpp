// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "Crypto.h"

#include <QRandomGenerator>

#include <algorithm>

#include <vector>

extern "C" {
#include "monocypher.h"
}

namespace Qupil::Crypto {

namespace {

const uint8_t *bytes(const QByteArray &a)
{
    return reinterpret_cast<const uint8_t *>(a.constData());
}

uint8_t *bytes(QByteArray &a)
{
    return reinterpret_cast<uint8_t *>(a.data());
}

constexpr int NonceSize = 24;
constexpr int MacSize = 16;

} // namespace

QByteArray randomBytes(int size)
{
    QByteArray out(size, Qt::Uninitialized);
    QRandomGenerator *rng = QRandomGenerator::system();
    for (int i = 0; i < size; ++i)
        out[i] = char(rng->generate() & 0xff);
    return out;
}

QByteArray deriveKey(const QString &password, const QByteArray &salt, KdfParams params)
{
    QByteArray pass = password.normalized(QString::NormalizationForm_C).toUtf8();
    QByteArray key(KeySize, Qt::Uninitialized);
    crypto_argon2_config config;
    config.algorithm = CRYPTO_ARGON2_ID;
    config.nb_blocks = uint32_t(std::max(8, params.memoryKiB));
    config.nb_passes = uint32_t(std::max(1, params.passes));
    config.nb_lanes = 1;
    crypto_argon2_inputs inputs;
    inputs.pass = bytes(pass);
    inputs.pass_size = uint32_t(pass.size());
    inputs.salt = bytes(salt);
    inputs.salt_size = uint32_t(salt.size());
    std::vector<uint8_t> work(size_t(config.nb_blocks) * 1024);
    crypto_argon2(bytes(key), KeySize, work.data(), config, inputs, crypto_argon2_no_extras);
    crypto_wipe(work.data(), work.size());
    wipe(pass);
    return key;
}

QByteArray seal(const QByteArray &key, const QByteArray &plainText, const QByteArray &ad)
{
    Q_ASSERT(key.size() == KeySize);
    const QByteArray nonce = randomBytes(NonceSize);
    QByteArray out(NonceSize + MacSize + plainText.size(), Qt::Uninitialized);
    std::copy(nonce.cbegin(), nonce.cend(), out.begin());
    crypto_aead_lock(bytes(out) + NonceSize + MacSize, bytes(out) + NonceSize, bytes(key), bytes(nonce),
                     bytes(ad), size_t(ad.size()), bytes(plainText), size_t(plainText.size()));
    return out;
}

std::optional<QByteArray> open(const QByteArray &key, const QByteArray &sealed, const QByteArray &ad)
{
    if (key.size() != KeySize || sealed.size() < NonceSize + MacSize)
        return std::nullopt;
    const qsizetype textSize = sealed.size() - NonceSize - MacSize;
    QByteArray plain(textSize, Qt::Uninitialized);
    const uint8_t *nonce = bytes(sealed);
    const uint8_t *mac = nonce + NonceSize;
    const uint8_t *cipher = mac + MacSize;
    if (crypto_aead_unlock(bytes(plain), mac, bytes(key), nonce, bytes(ad), size_t(ad.size()), cipher,
                           size_t(textSize)) != 0)
        return std::nullopt;
    return plain;
}

QByteArray hash(const QByteArray &data, int size, const QByteArray &key)
{
    QByteArray out(size, Qt::Uninitialized);
    if (key.isEmpty())
        crypto_blake2b(bytes(out), size_t(size), bytes(data), size_t(data.size()));
    else
        crypto_blake2b_keyed(bytes(out), size_t(size), bytes(key), size_t(key.size()), bytes(data), size_t(data.size()));
    return out;
}

KeyPair x25519KeyPair()
{
    KeyPair pair;
    pair.secretKey = randomBytes(32);
    pair.publicKey = QByteArray(32, Qt::Uninitialized);
    crypto_x25519_public_key(bytes(pair.publicKey), bytes(pair.secretKey));
    return pair;
}

QByteArray x25519Shared(const QByteArray &secretKey, const QByteArray &theirPublicKey)
{
    if (secretKey.size() != 32 || theirPublicKey.size() != 32)
        return {};
    QByteArray raw(32, Qt::Uninitialized);
    crypto_x25519(bytes(raw), bytes(secretKey), bytes(theirPublicKey));
    QByteArray key = hash(raw, KeySize);
    wipe(raw);
    return key;
}

void wipe(QByteArray &secret)
{
    if (!secret.isEmpty())
        crypto_wipe(secret.data(), size_t(secret.size()));
    secret.clear();
}

} // namespace Qupil::Crypto
