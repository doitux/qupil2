// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QAudioFormat>
#include <QIODevice>
#include <QMutex>

#include <atomic>

/*!
    Endless audio stream of the metronome: clicks at exact sample positions plus an
    optional sine tone. The tempo comes only from the number of samples, so it is as
    exact as the sound card's clock. readData() runs on the audio thread; parameters
    are atomics, the beat anchor is guarded by a mutex.
*/
class ClickGenerator : public QIODevice
{
public:
    explicit ClickGenerator(const QAudioFormat &format);

    std::atomic<int> bpm { 80 };
    std::atomic<int> beatsPerBar { 4 };
    std::atomic<bool> clicking { false };
    std::atomic<double> toneFrequency { 0 };   // 0 = off

    //! Clicks start with the next rendered sample
    void restartClicks();

    //! Beat (0-based since start) that is audible at the given number of played samples, -1 before the first
    qint64 beatAt(qint64 playedSamples) const;

    bool isSequential() const override { return true; }
    qint64 bytesAvailable() const override { return (1 << 16) + QIODevice::bytesAvailable(); }

protected:
    qint64 writeData(const char *, qint64) override { return -1; }
    qint64 readData(char *data, qint64 maxSize) override;

private:
    void setAnchor(qint64 sample, qint64 beat, double samplesPerBeat);

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
