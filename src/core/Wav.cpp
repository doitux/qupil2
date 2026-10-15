// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "Wav.h"

#include <QByteArrayView>

#include <algorithm>
#include <cmath>
#include <cstring>

namespace Qupil::Wav {

namespace {

quint16 u16(const char *p)
{
    return quint16(quint8(p[0]) | quint16(quint8(p[1])) << 8);
}

quint32 u32(const char *p)
{
    return quint32(u16(p)) | quint32(u16(p + 2)) << 16;
}

void put16(QByteArray &out, quint16 value)
{
    out.append(char(value & 0xff));
    out.append(char(value >> 8));
}

void put32(QByteArray &out, quint32 value)
{
    put16(out, quint16(value & 0xffff));
    put16(out, quint16(value >> 16));
}

qint16 clip(double value)
{
    return qint16(std::clamp(std::lround(value), -32768L, 32767L));
}

} // namespace

int Sound::peak() const
{
    int peak = 0;
    for (qint16 v : samples)
        peak = std::max(peak, std::abs(int(v)));
    return peak;
}

std::optional<Sound> read(const QByteArray &data)
{
    if (data.size() < 12 || !data.startsWith("RIFF") || data.mid(8, 4) != "WAVE")
        return std::nullopt;

    int format = 0;
    int channels = 0;
    int rate = 0;
    int bits = 0;
    QByteArrayView pcm;
    qsizetype pos = 12;
    while (pos + 8 <= data.size()) {
        const QByteArrayView id(data.constData() + pos, 4);
        const quint32 size = u32(data.constData() + pos + 4);
        const qsizetype body = pos + 8;
        const qsizetype available = data.size() - body;
        // Some programs write a wrong size for the last chunk: take what is there
        const qsizetype length = std::min<qsizetype>(size, available);
        if (id == "fmt " && length >= 16) {
            const char *f = data.constData() + body;
            format = u16(f);
            channels = u16(f + 2);
            rate = int(u32(f + 4));
            bits = u16(f + 14);
            // WAVE_FORMAT_EXTENSIBLE: the sub format GUID starts with the format code
            if (format == 0xFFFE && length >= 26)
                format = u16(f + 24);
        } else if (id == "data") {
            pcm = QByteArrayView(data.constData() + body, length);
        }
        if (qsizetype(size) > available)
            break;
        pos = body + qsizetype(size) + (size & 1);
    }

    const bool isFloat = format == 3;
    if (channels <= 0 || rate <= 0 || pcm.isEmpty() || (format != 1 && !isFloat))
        return std::nullopt;
    if (isFloat ? bits != 32 : (bits != 8 && bits != 16 && bits != 24 && bits != 32))
        return std::nullopt;

    const int bytes = bits / 8;
    const qsizetype frames = pcm.size() / (qsizetype(bytes) * channels);
    if (frames == 0)
        return std::nullopt;

    Sound sound;
    sound.sampleRate = rate;
    sound.channels = std::min(channels, 2);
    sound.samples.reserve(frames * sound.channels);
    for (qsizetype i = 0; i < frames; ++i) {
        const char *frame = pcm.data() + i * bytes * channels;
        for (int c = 0; c < sound.channels; ++c) {
            const char *p = frame + c * bytes;
            qint16 value = 0;
            if (isFloat) {
                const quint32 raw = u32(p);
                float f;
                std::memcpy(&f, &raw, sizeof f);
                value = clip(double(std::isfinite(f) ? f : 0.0f) * 32767.0);
            } else if (bytes == 1) {
                value = qint16((int(quint8(p[0])) - 128) * 256);
            } else {
                // The two most significant bytes (little endian: the last two)
                value = qint16(u16(p + bytes - 2));
            }
            sound.samples.append(value);
        }
    }
    return sound;
}

QByteArray write(const Sound &sound)
{
    const quint32 dataBytes = quint32(sound.samples.size() * 2);
    const int channels = std::max(1, sound.channels);
    QByteArray out;
    out.reserve(44 + qsizetype(dataBytes));
    out.append("RIFF");
    put32(out, 36 + dataBytes);
    out.append("WAVE");
    out.append("fmt ");
    put32(out, 16);
    put16(out, 1);   // PCM
    put16(out, quint16(channels));
    put32(out, quint32(sound.sampleRate));
    put32(out, quint32(sound.sampleRate * channels * 2));
    put16(out, quint16(channels * 2));
    put16(out, 16);
    out.append("data");
    put32(out, dataBytes);
    for (qint16 v : sound.samples)
        put16(out, quint16(v));
    return out;
}

Sound scaled(Sound sound, double gain)
{
    for (qint16 &v : sound.samples)
        v = clip(v * gain);
    return sound;
}

Sound trimmed(Sound sound, double maxSeconds)
{
    if (!sound.isValid() || sound.seconds() <= maxSeconds)
        return sound;
    const qsizetype frames = qsizetype(maxSeconds * sound.sampleRate);
    sound.samples.resize(frames * sound.channels);
    // 30 ms fade-out, so the cut does not click
    const qsizetype fade = std::min<qsizetype>(frames, sound.sampleRate * 3 / 100);
    for (qsizetype i = 0; i < fade; ++i) {
        const double gain = double(i) / double(fade);
        const qsizetype frame = frames - 1 - i;
        for (int c = 0; c < sound.channels; ++c) {
            qint16 &v = sound.samples[frame * sound.channels + c];
            v = clip(v * gain);
        }
    }
    return sound;
}

} // namespace Qupil::Wav
