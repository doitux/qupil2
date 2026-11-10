// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "ClickGenerator.h"

#include <QMutexLocker>

#include <algorithm>
#include <cmath>

namespace {
constexpr double kPi = 3.14159265358979323846;
}

ClickGenerator::ClickGenerator(const QAudioFormat &format)
    : m_format(format)
{
    open(QIODevice::ReadOnly);
}

void ClickGenerator::restartClicks()
{
    m_restart.store(true);
    clicking.store(true);
}

qint64 ClickGenerator::beatAt(qint64 playedSamples) const
{
    QMutexLocker lock(&m_anchorMutex);
    if (m_anchorSample < 0 || playedSamples < m_anchorSample)
        return m_anchorBeat - 1;
    return m_anchorBeat + qint64((playedSamples - m_anchorSample) / m_anchorSamplesPerBeat);
}

qint64 ClickGenerator::readData(char *data, qint64 maxSize)
{
    const int channels = std::max(1, m_format.channelCount());
    const int bytesPerSample = m_format.bytesPerSample();
    const qint64 frames = maxSize / (qint64(bytesPerSample) * channels);
    const double rate = m_format.sampleRate();
    const double tone = toneFrequency.load();
    const int beatsInBar = std::max(1, beatsPerBar.load());
    const bool click = clicking.load();
    char *out = data;

    for (qint64 i = 0; i < frames; ++i, ++m_sample) {
        if (click) {
            const double samplesPerBeat = rate * 60.0 / std::clamp(bpm.load(), 1, 1000);
            if (m_restart.exchange(false)) {
                m_nextBeat = double(m_sample);
                m_beatIndex = 0;
                setAnchor(m_sample, 0, samplesPerBeat);
            }
            if (double(m_sample) >= m_nextBeat) {
                const bool accent = beatsInBar > 1 && m_beatIndex % beatsInBar == 0;
                m_clickFrequency = accent ? 1760.0 : 1175.0;
                m_clickGain = accent ? 0.9 : 0.6;
                m_clickPos = 0;
                if (m_anchorSamplesPerBeat != samplesPerBeat)
                    setAnchor(m_sample, m_beatIndex, samplesPerBeat);
                ++m_beatIndex;
                // From the anchor, not by adding up: no rounding error builds up over a long time
                m_nextBeat = double(m_anchorSample) + double(m_beatIndex - m_anchorBeat) * samplesPerBeat;
                if (m_nextBeat < double(m_sample))   // after a big tempo jump
                    m_nextBeat = double(m_sample) + samplesPerBeat;
            }
        }

        double s = 0;
        const int clickLength = int(rate * 0.035);
        if (m_clickPos >= 0 && m_clickPos < clickLength) {
            const double t = m_clickPos / rate;
            const double envelope = std::exp(-t * 140.0) * std::min(1.0, m_clickPos / (rate * 0.0008));
            s += m_clickGain * envelope * std::sin(2 * kPi * m_clickFrequency * t);
            ++m_clickPos;
        }

        // Tone with short fades to avoid clicks
        const double targetGain = tone > 0 ? 0.35 : 0.0;
        m_toneGain += (targetGain - m_toneGain) * (1.0 / (rate * 0.01));
        if (tone > 0)
            m_toneFrequency = tone;
        if (m_toneGain > 1e-4) {
            m_tonePhase += 2 * kPi * m_toneFrequency / rate;
            if (m_tonePhase > 2 * kPi)
                m_tonePhase -= 2 * kPi;
            // a little second harmonic: easier to hear on small speakers
            s += m_toneGain * (0.8 * std::sin(m_tonePhase) + 0.2 * std::sin(2 * m_tonePhase));
        }

        s = std::clamp(s, -1.0, 1.0);
        for (int c = 0; c < channels; ++c) {
            switch (m_format.sampleFormat()) {
            case QAudioFormat::UInt8:
                *reinterpret_cast<quint8 *>(out) = quint8(128 + s * 127);
                break;
            case QAudioFormat::Int16:
                *reinterpret_cast<qint16 *>(out) = qint16(s * 32767);
                break;
            case QAudioFormat::Int32:
                *reinterpret_cast<qint32 *>(out) = qint32(s * 2147483647.0);
                break;
            case QAudioFormat::Float:
                *reinterpret_cast<float *>(out) = float(s);
                break;
            default:
                break;
            }
            out += bytesPerSample;
        }
    }
    return frames * bytesPerSample * channels;
}

void ClickGenerator::setAnchor(qint64 sample, qint64 beat, double samplesPerBeat)
{
    QMutexLocker lock(&m_anchorMutex);
    m_anchorSample = sample;
    m_anchorBeat = beat;
    m_anchorSamplesPerBeat = samplesPerBeat;
}
