// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "Metronome.h"
#include "ClickGenerator.h"
#include "ScreenAwake.h"

#include <QAudioDevice>
#include <QAudioFormat>
#include <QAudioSink>
#include <QGuiApplication>
#include <QMediaDevices>
#include <QSettings>
#include <QThread>

#include <algorithm>
#include <atomic>
#include <cmath>

using namespace Qt::StringLiterals;

#ifdef Q_OS_IOS
// AudioSession_ios.mm
void qupilPlaybackAudioSession();
bool qupilPlaybackAudioSessionSet();
#endif

namespace {
//! Audio buffer: big enough that a busy moment does not let it run dry (crackling and
//! a gap that moves every later beat), small enough that a new tempo starts quickly
constexpr int kBufferMs = 120;
}

/*
    The sound output on a thread of its own. The system takes the samples from a buffer
    that Qt fills on the thread the QAudioSink lives on; on the user interface thread a
    page building up or an animation could keep it waiting (iPad: crackling, gaps, the
    tempo falling behind). Created, polled and closed only on that thread.
*/
class MetronomeOutput : public QObject
{
public:
    std::unique_ptr<ClickGenerator> generator;
    std::unique_ptr<QAudioSink> sink;
    //! A child: moveToThread() takes it along to the audio thread (a plain member stayed on
    //! the user interface thread and could not be started from the audio thread – the
    //! beat was never read)
    QTimer beatTimer { this };

    MetronomeOutput()
    {
        beatTimer.setInterval(10);
        QObject::connect(&beatTimer, &QTimer::timeout, this, [this] { updateBeat(); });
    }
    std::atomic<bool> open { false };
    std::atomic<qint64> beat { -1 };   // audible beat since the start, -1 before the first
#ifdef Q_OS_IOS
    int sessionChecks = 0;
#endif

    bool start(int bpm, int beatsPerBar)
    {
        if (sink && sink->state() != QAudio::StoppedState)
            return true;
        close();
        const QAudioDevice device = QMediaDevices::defaultAudioOutput();
        if (device.isNull())
            return false;
        QAudioFormat format;
        format.setSampleRate(48000);
        format.setChannelCount(1);
        format.setSampleFormat(QAudioFormat::Int16);
        if (!device.isFormatSupported(format))
            format = device.preferredFormat();
        generator = std::make_unique<ClickGenerator>(format);
        generator->bpm.store(bpm);
        generator->beatsPerBar.store(beatsPerBar);
        sink = std::make_unique<QAudioSink>(device, format);
        sink->setBufferSize(format.bytesForDuration(kBufferMs * 1000));
        sink->start(generator.get());
        if (sink->error() != QAudio::NoError) {
            close();
            return false;
        }
#ifdef Q_OS_IOS
        // After start(): opening the sink set the session to "Ambient" (AudioSession_ios.mm)
        qupilPlaybackAudioSession();
        sessionChecks = 0;
#endif
        beatTimer.start();
        open.store(true);
        return true;
    }

    void close()
    {
        beatTimer.stop();
        if (sink)
            sink->stop();
        sink.reset();
        generator.reset();
        open.store(false);
        beat.store(-1);
    }

private:
    void updateBeat()
    {
        if (!sink || !generator)
            return;
        if (sink->state() == QAudio::StoppedState) {
            open.store(false);
            return;
        }
        const qint64 played = sink->processedUSecs() * sink->format().sampleRate() / 1000000;
        beat.store(generator->beatAt(played));
#ifdef Q_OS_IOS
        // About twice a second: another sound (an alert, a preview: QSoundEffect opens a
        // sink of its own) set the session back to "Ambient" – playing again at once
        if (++sessionChecks >= 50) {
            sessionChecks = 0;
            if (!qupilPlaybackAudioSessionSet())
                qupilPlaybackAudioSession();
        }
#endif
    }
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
    const QString note = settings.value(u"note"_s, u"A"_s).toString();
    m_toneNote = (note == u"Bb"_s || note == u"B"_s) ? note : u"A"_s;

    m_audioThread = new QThread(this);
    m_audioThread->setObjectName(u"metronome audio"_s);
    m_output = new MetronomeOutput;
    m_output->moveToThread(m_audioThread);
    connect(m_audioThread, &QThread::finished, m_output, &QObject::deleteLater);
    m_audioThread->start(QThread::TimeCriticalPriority);

    m_beatTimer.setInterval(15);
    connect(&m_beatTimer, &QTimer::timeout, this, &Metronome::updateBeat);
    // Give the audio device back a while after the last sound (battery)
    m_idleTimer.setSingleShot(true);
    m_idleTimer.setInterval(10000);
    connect(&m_idleTimer, &QTimer::timeout, this, &Metronome::releaseAudioIfIdle);
    // Phones and tablets: the screen stays on while it plays (on Android the sound
    // stopped when the screen went off)
    const auto keepAwake = [this] { ScreenAwake::set(m_running || m_tonePlaying); };
    connect(this, &Metronome::runningChanged, this, keepAwake);
    connect(this, &Metronome::tonePlayingChanged, this, keepAwake);
#ifdef Q_OS_IOS
    // Into the background with the session for playing, or iOS stops the sound
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, [this](Qt::ApplicationState state) {
        if (state != Qt::ApplicationActive && (m_running || m_tonePlaying))
            qupilPlaybackAudioSession();
    });
#endif
}

Metronome::~Metronome()
{
    QMetaObject::invokeMethod(m_output, [this] { m_output->close(); }, Qt::BlockingQueuedConnection);
    m_audioThread->quit();
    m_audioThread->wait();
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
    if (ClickGenerator *g = generator())
        g->bpm.store(bpm);
    save();
    emit bpmChanged();
}

void Metronome::setBeatsPerBar(int beats)
{
    beats = std::clamp(beats, 1, 16);
    if (beats == m_beatsPerBar)
        return;
    m_beatsPerBar = beats;
    if (ClickGenerator *g = generator())
        g->beatsPerBar.store(beats);
    save();
    emit beatsPerBarChanged();
}

void Metronome::setToneNote(const QString &note)
{
    const QString n = (note == u"Bb"_s || note == u"B"_s) ? note : u"A"_s;
    if (n == m_toneNote)
        return;
    m_toneNote = n;
    if (ClickGenerator *g = generator(); m_tonePlaying && g)
        g->toneFrequency.store(toneFrequency());
    save();
    emit toneChanged();
}

void Metronome::setPitch(int pitch)
{
    pitch = std::clamp(pitch, MinPitch, MaxPitch);
    if (pitch == m_pitch)
        return;
    m_pitch = pitch;
    if (ClickGenerator *g = generator(); m_tonePlaying && g)
        g->toneFrequency.store(toneFrequency());
    save();
    emit toneChanged();
}

double Metronome::toneFrequency() const
{
    // B♭ is a semitone above A, B (German "H") a whole tone
    const int semitones = m_toneNote == u"Bb"_s ? 1 : m_toneNote == u"B"_s ? 2 : 0;
    return m_pitch * std::pow(2.0, semitones / 12.0);
}

ClickGenerator *Metronome::generator() const
{
    // Set and reset on the audio thread only while it is blocked for us (ensureAudio,
    // releaseAudioIfIdle), so reading the pointer here is safe
    return m_output && m_output->open.load() ? m_output->generator.get() : nullptr;
}

bool Metronome::ensureAudio()
{
    bool ok = false;
    const int bpm = m_bpm, beats = m_beatsPerBar;
    QMetaObject::invokeMethod(m_output, [this, &ok, bpm, beats] { ok = m_output->start(bpm, beats); },
                              Qt::BlockingQueuedConnection);
    if (!ok) {
        m_error = qtTrId("audio.init_failed");
        emit errorChanged();
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
    if (m_running || m_tonePlaying)
        return;
    QMetaObject::invokeMethod(m_output, [this] { m_output->close(); }, Qt::BlockingQueuedConnection);
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
    generator()->restartClicks();
    m_running = true;
    m_beatTimer.start();
    emit runningChanged();
}

void Metronome::stop()
{
    if (!m_running)
        return;
    m_running = false;
    if (ClickGenerator *g = generator())
        g->clicking.store(false);
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
    const qint64 beat = m_output ? m_output->beat.load() : -1;
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
    generator()->toneFrequency.store(toneFrequency());
    m_tonePlaying = true;
    emit tonePlayingChanged();
}

void Metronome::stopTone()
{
    if (!m_tonePlaying)
        return;
    if (ClickGenerator *g = generator())
        g->toneFrequency.store(0);
    m_tonePlaying = false;
    emit tonePlayingChanged();
    m_idleTimer.start();
}
