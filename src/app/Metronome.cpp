// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "Metronome.h"

#include <QAudioDevice>
#include <QAudioFormat>
#include <QAudioSink>
#include <QIODevice>
#include <QMediaDevices>
#include <QMutex>
#include <QMutexLocker>
#include <QSettings>

#include <algorithm>
#include <atomic>
#include <cmath>

using namespace Qt::StringLiterals;

namespace {
constexpr double kPi = 3.14159265358979323846;
}

/*
    Endless audio stream: clicks at exact sample positions plus an optional sine tone.
    readData() may run on the audio thread; parameters are atomics, the beat anchor
    is guarded by a mutex.
*/
class ClickGenerator : public QIODevice
{
public:
    explicit ClickGenerator(const QAudioFormat &format)
        : m_format(format)
    {
        open(QIODevice::ReadOnly);
    }

    std::atomic<int> bpm { 80 };
    std::atomic<int> beatsPerBar { 4 };
    std::atomic<bool> clicking { false };
    std::atomic<double> toneFrequency { 0 };   // 0 = off

    //! Clicks start with the next rendered sample
    void restartClicks()
    {
        m_restart.store(true);
        clicking.store(true);
    }

    //! Beat (0-based since start) that is audible at the given number of played samples, -1 before the first
    qint64 beatAt(qint64 playedSamples) const
    {
        QMutexLocker lock(&m_anchorMutex);
        if (m_anchorSample < 0 || playedSamples < m_anchorSample)
            return m_anchorBeat - 1;
        return m_anchorBeat + qint64((playedSamples - m_anchorSample) / m_anchorSamplesPerBeat);
    }

    bool isSequential() const override { return true; }
    qint64 bytesAvailable() const override { return (1 << 16) + QIODevice::bytesAvailable(); }

protected:
    qint64 writeData(const char *, qint64) override { return -1; }

    qint64 readData(char *data, qint64 maxSize) override
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
                    m_nextBeat += samplesPerBeat;
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

private:
    void setAnchor(qint64 sample, qint64 beat, double samplesPerBeat)
    {
        QMutexLocker lock(&m_anchorMutex);
        m_anchorSample = sample;
        m_anchorBeat = beat;
        m_anchorSamplesPerBeat = samplesPerBeat;
    }

    QAudioFormat m_format;
    std::atomic<bool> m_restart { false };
    qint64 m_sample = 0;
    double m_nextBeat = 0;
    qint64 m_beatIndex = 0;
    int m_clickPos = -1;
    double m_clickFrequency = 1000;
    double m_clickGain = 0.6;
    double m_tonePhase = 0;
    double m_toneGain = 0;
    double m_toneFrequency = 440;

    mutable QMutex m_anchorMutex;
    qint64 m_anchorSample = -1;
    qint64 m_anchorBeat = 0;
    double m_anchorSamplesPerBeat = 1;
};

// ---------------------------------------------------------------------------- Metronome

Metronome::Metronome(QObject *parent)
    : QObject(parent)
{
    QSettings settings;
    settings.beginGroup(u"metronome"_s);
    m_bpm = std::clamp(settings.value(u"bpm"_s, 80).toInt(), MinBpm, MaxBpm);
    m_beatsPerBar = std::clamp(settings.value(u"beats"_s, 4).toInt(), 1, 16);
    m_pitch = std::clamp(settings.value(u"pitch"_s, 443).toInt(), MinPitch, MaxPitch);
    m_toneNote = settings.value(u"note"_s, u"A"_s).toString() == u"Bb"_s ? u"Bb"_s : u"A"_s;

    m_beatTimer.setInterval(15);
    connect(&m_beatTimer, &QTimer::timeout, this, &Metronome::updateBeat);
    // Give the audio device back a while after the last sound (battery)
    m_idleTimer.setSingleShot(true);
    m_idleTimer.setInterval(10000);
    connect(&m_idleTimer, &QTimer::timeout, this, &Metronome::releaseAudioIfIdle);
}

Metronome::~Metronome()
{
    if (m_sink)
        m_sink->stop();
}

void Metronome::save()
{
    QSettings settings;
    settings.beginGroup(u"metronome"_s);
    settings.setValue(u"bpm"_s, m_bpm);
    settings.setValue(u"beats"_s, m_beatsPerBar);
    settings.setValue(u"pitch"_s, m_pitch);
    settings.setValue(u"note"_s, m_toneNote);
}

void Metronome::setBpm(int bpm)
{
    bpm = std::clamp(bpm, MinBpm, MaxBpm);
    if (bpm == m_bpm)
        return;
    m_bpm = bpm;
    if (m_generator)
        m_generator->bpm.store(bpm);
    save();
    emit bpmChanged();
}

void Metronome::setBeatsPerBar(int beats)
{
    beats = std::clamp(beats, 1, 16);
    if (beats == m_beatsPerBar)
        return;
    m_beatsPerBar = beats;
    if (m_generator)
        m_generator->beatsPerBar.store(beats);
    save();
    emit beatsPerBarChanged();
}

void Metronome::setToneNote(const QString &note)
{
    const QString n = note == u"Bb"_s ? u"Bb"_s : u"A"_s;
    if (n == m_toneNote)
        return;
    m_toneNote = n;
    if (m_tonePlaying && m_generator)
        m_generator->toneFrequency.store(toneFrequency());
    save();
    emit toneChanged();
}

void Metronome::setPitch(int pitch)
{
    pitch = std::clamp(pitch, MinPitch, MaxPitch);
    if (pitch == m_pitch)
        return;
    m_pitch = pitch;
    if (m_tonePlaying && m_generator)
        m_generator->toneFrequency.store(toneFrequency());
    save();
    emit toneChanged();
}

double Metronome::toneFrequency() const
{
    // B♭ is a semitone above A
    return m_toneNote == u"Bb"_s ? m_pitch * std::pow(2.0, 1.0 / 12.0) : double(m_pitch);
}

bool Metronome::ensureAudio()
{
    if (m_sink && m_sink->state() != QAudio::StoppedState)
        return true;
    const QAudioDevice device = QMediaDevices::defaultAudioOutput();
    if (device.isNull()) {
        m_error = qtTrId("audio.init_failed");
        emit errorChanged();
        return false;
    }
    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);
    if (!device.isFormatSupported(format))
        format = device.preferredFormat();

    m_sink.reset();
    m_generator = std::make_unique<ClickGenerator>(format);
    m_generator->bpm.store(m_bpm);
    m_generator->beatsPerBar.store(m_beatsPerBar);
    m_sink = std::make_unique<QAudioSink>(device, format);
    // ~40 ms buffer: low latency for the clicks, still safe on phones
    m_sink->setBufferSize(format.bytesForDuration(40000));
    m_sink->start(m_generator.get());
    if (m_sink->error() != QAudio::NoError) {
        m_error = qtTrId("audio.init_failed");
        emit errorChanged();
        m_sink.reset();
        return false;
    }
    if (!m_error.isEmpty()) {
        m_error.clear();
        emit errorChanged();
    }
    return true;
}

void Metronome::releaseAudioIfIdle()
{
    if (m_running || m_tonePlaying || !m_sink)
        return;
    m_sink->stop();
    m_sink.reset();
    m_generator.reset();
}

void Metronome::start()
{
    if (m_running)
        return;
    if (m_tonePlaying)
        stopTone();
    if (!ensureAudio())
        return;
    m_idleTimer.stop();
    m_generator->restartClicks();
    m_running = true;
    m_beatTimer.start();
    emit runningChanged();
}

void Metronome::stop()
{
    if (!m_running)
        return;
    m_running = false;
    if (m_generator)
        m_generator->clicking.store(false);
    m_beatTimer.stop();
    m_beat = -1;
    emit beatChanged();
    emit runningChanged();
    m_idleTimer.start();
}

void Metronome::toggle()
{
    m_running ? stop() : start();
}

void Metronome::updateBeat()
{
    if (!m_sink || !m_generator)
        return;
    const qint64 played = m_sink->processedUSecs() * m_sink->format().sampleRate() / 1000000;
    const qint64 beat = m_generator->beatAt(played);
    const int inBar = beat < 0 ? -1 : int(beat % std::max(1, m_beatsPerBar));
    if (inBar != m_beat) {
        m_beat = inBar;
        emit beatChanged();
    }
}

void Metronome::tap()
{
    if (!m_tapClock.isValid())
        m_tapClock.start();
    const qint64 now = m_tapClock.elapsed();
    if (!m_taps.isEmpty() && now - m_taps.last() > 2000)
        m_taps.clear();   // a new tap sequence
    m_taps << now;
    while (m_taps.size() > 5)
        m_taps.removeFirst();
    if (m_taps.size() >= 2) {
        const double average = double(m_taps.last() - m_taps.first()) / (m_taps.size() - 1);
        if (average > 0)
            setBpm(int(std::lround(60000.0 / average)));
    }
}

void Metronome::toggleTone()
{
    if (m_tonePlaying) {
        stopTone();
        return;
    }
    if (m_running)
        stop();
    if (!ensureAudio())
        return;
    m_idleTimer.stop();
    m_generator->toneFrequency.store(toneFrequency());
    m_tonePlaying = true;
    emit tonePlayingChanged();
}

void Metronome::stopTone()
{
    if (!m_tonePlaying)
        return;
    if (m_generator)
        m_generator->toneFrequency.store(0);
    m_tonePlaying = false;
    emit tonePlayingChanged();
    m_idleTimer.start();
}
