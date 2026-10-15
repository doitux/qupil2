// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "AlertSound.h"

#include <QAudioBuffer>
#include <QAudioDecoder>
#include <QAudioFormat>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QSettings>
#include <QSoundEffect>
#include <QStandardPaths>
#include <QTimer>

#include <algorithm>
#include <cmath>
#include <utility>

using namespace Qt::StringLiterals;
using namespace Qupil;

namespace {

constexpr qint64 kMaxFileBytes = 64 * 1024 * 1024;

//! Path QFile can open: local files, Android content:// URIs
QString openablePath(const QUrl &url)
{
    if (url.isLocalFile())
        return url.toLocalFile();
    if (url.scheme().isEmpty())
        return url.path();
    return url.toString();
}

//! "ding.mp3" also for Android documents (content://…/document/primary%3AMusic%2Fding.mp3)
QString displayName(const QUrl &url, const QString &path)
{
    QString name = QFileInfo(path).fileName();   // Android: the document's display name
    if (name.isEmpty() || name.contains(u':'))
        name = url.fileName(QUrl::FullyDecoded);
    const qsizetype cut = std::max(name.lastIndexOf(u'/'), name.lastIndexOf(u':'));
    return cut >= 0 ? name.mid(cut + 1) : name;
}

qint16 toSample(double value)
{
    return qint16(std::clamp(std::lround(value), -32768L, 32767L));
}

} // namespace

AlertSound::AlertSound(const QString &name, const QString &builtIn, const QString &group,
                       std::function<int()> defaultVolume, QObject *parent)
    : QObject(parent)
    , m_name(name)
    , m_builtIn(builtIn)
    , m_group(group)
    , m_defaultVolume(std::move(defaultVolume))
{
}

void AlertSound::store(const QString &key, const QVariant &value)
{
    QSettings s;
    s.beginGroup(m_group);
    if (value.isValid())
        s.setValue(key, value);
    else
        s.remove(key);
}

int AlertSound::volume() const
{
    QSettings s;
    s.beginGroup(m_group);
    const QVariant v = s.value(u"volume"_s);
    return std::clamp(v.isValid() ? v.toInt() : m_defaultVolume ? m_defaultVolume() : 7, 1, 10);
}

void AlertSound::setVolume(int volume)
{
    const int before = this->volume();
    store(u"volume"_s, std::clamp(volume, 1, 10));
    if (this->volume() != before)
        emit changed();
}

QString AlertSound::folder() const
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + u"/sounds"_s;
}

QString AlertSound::customFile() const
{
    // Not "<name>-…": those are the files with the volume applied
    return QDir(folder()).filePath(u"custom-%1.wav"_s.arg(m_name));
}

QString AlertSound::customName() const
{
    if (!QFileInfo::exists(customFile()))
        return {};
    QSettings s;
    s.beginGroup(m_group);
    const QString name = s.value(u"customName"_s).toString();
    return name.isEmpty() ? QFileInfo(customFile()).fileName() : name;
}

QByteArray AlertSound::source() const
{
    if (!customName().isEmpty()) {
        QFile custom(customFile());
        if (custom.open(QIODevice::ReadOnly))
            return custom.readAll();
    }
    QFile builtIn(m_builtIn);
    return builtIn.open(QIODevice::ReadOnly) ? builtIn.readAll() : QByteArray();
}

QString AlertSound::file()
{
    const QByteArray bytes = source();
    const auto sound = Wav::read(bytes);
    if (!sound)
        return {};
    // A new name for every sound and volume: iOS keeps notification sounds by name
    const QByteArray key = QCryptographicHash::hash(bytes + QByteArray::number(volume()), QCryptographicHash::Md5)
                               .toHex()
                               .left(12);
    const QDir dir(folder());
    const QString name = u"%1-%2.wav"_s.arg(m_name, QString::fromLatin1(key));
    const QString path = dir.filePath(name);
    if (QFileInfo::exists(path))
        return path;
    QDir().mkpath(dir.path());
    QSaveFile out(path);
    if (!out.open(QIODevice::WriteOnly))
        return {};
    out.write(Wav::write(Wav::scaled(*sound, volume() / 10.0)));
    if (!out.commit())
        return {};
    for (const QString &old : dir.entryList({ m_name + u"-*.wav"_s }, QDir::Files)) {
        if (old != name)
            QFile::remove(dir.filePath(old));
    }
    return path;
}

void AlertSound::play()
{
    const QString path = file();
    if (path.isEmpty())
        return;
    if (!m_effect) {
        m_effect = new QSoundEffect(this);
        connect(m_effect, &QSoundEffect::statusChanged, this, [this] {
            if (m_effect->status() == QSoundEffect::Error) {
                m_playWhenReady = false;
            } else if (m_playWhenReady && m_effect->status() == QSoundEffect::Ready) {
                m_playWhenReady = false;
                m_effect->play();
            }
        });
    }
    const QUrl url = QUrl::fromLocalFile(path);
    if (m_effect->source() != url)
        m_effect->setSource(url);
    if (m_effect->status() == QSoundEffect::Ready) {
        m_effect->stop();
        m_effect->play();
    } else {
        m_playWhenReady = true;   // plays when loaded
    }
}

void AlertSound::useDefault()
{
    QFile::remove(customFile());
    store(u"customName"_s, {});
    emit changed();
    play();
}

void AlertSound::choose(const QUrl &url)
{
    if (m_busy || url.isEmpty())
        return;
    const QString path = openablePath(url);
    QFile in(path);
    if (!in.open(QIODevice::ReadOnly) || in.size() > kMaxFileBytes) {
        emit failed(qtTrId("settings.sound.error"));
        return;
    }
    const QByteArray bytes = in.read(kMaxFileBytes);
    in.close();
    const QString name = displayName(url, path);

    if (const auto wav = Wav::read(bytes)) {
        storeCustom(*wav, name);
        return;
    }

    // Other formats: the system decodes them (FFmpeg, AVFoundation, Android's MediaCodec).
    // A local copy, so that the decoder needs no access to the picked document
    const QString cache = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir().mkpath(cache);
    const QString suffix = QFileInfo(name).suffix().toLower();
    m_decodingFile = QDir(cache).filePath(u"sound-import."_s + (suffix.isEmpty() ? u"audio"_s : suffix));
    QFile copy(m_decodingFile);
    if (!copy.open(QIODevice::WriteOnly | QIODevice::Truncate) || copy.write(bytes) != bytes.size()) {
        emit failed(qtTrId("settings.sound.error"));
        return;
    }
    copy.close();

    m_decoding = {};
    m_decodingName = name;
    auto *decoder = new QAudioDecoder(this);
    m_decoder = decoder;
    connect(decoder, &QAudioDecoder::bufferReady, this, [this, decoder] {
        while (m_decoder == decoder && decoder->bufferAvailable())
            decoded(decoder->read());
    });
    connect(decoder, &QAudioDecoder::finished, this, [this] { finishDecoding(true); });
    connect(decoder, qOverload<QAudioDecoder::Error>(&QAudioDecoder::error), this, [this] { finishDecoding(false); });
    QTimer::singleShot(30000, decoder, [this] { finishDecoding(false); });
    setBusy(true);
    decoder->setSource(QUrl::fromLocalFile(m_decodingFile));
    decoder->start();
}

void AlertSound::decoded(const QAudioBuffer &buffer)
{
    const QAudioFormat format = buffer.format();
    const int channels = format.channelCount();
    if (!buffer.isValid() || channels <= 0 || format.sampleRate() <= 0)
        return;
    if (m_decoding.sampleRate == 0) {
        m_decoding.sampleRate = format.sampleRate();
        m_decoding.channels = std::min(channels, 2);
    }
    if (format.sampleRate() != m_decoding.sampleRate)
        return;

    const qsizetype frames = buffer.frameCount();
    const int out = m_decoding.channels;
    for (qsizetype i = 0; i < frames; ++i) {
        for (int c = 0; c < out; ++c) {
            // Mono files fill both channels of a stereo start
            const qsizetype index = i * channels + std::min(c, channels - 1);
            qint16 v = 0;
            switch (format.sampleFormat()) {
            case QAudioFormat::UInt8:
                v = qint16((int(buffer.constData<quint8>()[index]) - 128) * 256);
                break;
            case QAudioFormat::Int16:
                v = buffer.constData<qint16>()[index];
                break;
            case QAudioFormat::Int32:
                v = qint16(buffer.constData<qint32>()[index] >> 16);
                break;
            case QAudioFormat::Float:
                v = toSample(double(buffer.constData<float>()[index]) * 32767.0);
                break;
            default:
                finishDecoding(false);
                return;
            }
            m_decoding.samples.append(v);
        }
    }
    if (m_decoding.seconds() >= kMaxSeconds)
        finishDecoding(true);   // enough: the rest is cut anyway
}

void AlertSound::finishDecoding(bool ok)
{
    if (!m_decoder)
        return;
    QAudioDecoder *decoder = m_decoder;
    m_decoder.clear();
    decoder->disconnect(this);
    decoder->stop();
    decoder->deleteLater();
    QFile::remove(m_decodingFile);
    setBusy(false);
    const Wav::Sound sound = std::exchange(m_decoding, {});
    // An error after a good part (some decoders complain at the very end) still counts
    if ((ok || sound.seconds() >= 0.2) && sound.isValid())
        storeCustom(sound, m_decodingName);
    else
        emit failed(qtTrId("settings.sound.error"));
}

void AlertSound::storeCustom(const Wav::Sound &sound, const QString &name)
{
    const Wav::Sound cut = Wav::trimmed(sound, kMaxSeconds);
    if (!cut.isValid() || cut.peak() == 0) {
        emit failed(qtTrId("settings.sound.error"));
        return;
    }
    QDir().mkpath(folder());
    QSaveFile out(customFile());
    if (!out.open(QIODevice::WriteOnly) || out.write(Wav::write(cut)) < 0 || !out.commit()) {
        emit failed(qtTrId("settings.sound.error"));
        return;
    }
    store(u"customName"_s, name);
    emit changed();
    play();   // so you hear what you chose
}

void AlertSound::setBusy(bool busy)
{
    if (m_busy == busy)
        return;
    m_busy = busy;
    emit busyChanged();
}
