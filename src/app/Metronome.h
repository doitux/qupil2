// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QElapsedTimer>
#include <QList>
#include <QObject>
#include <QTimer>
#include <QtQml/qqmlregistration.h>

#include <memory>

class QAudioSink;
class ClickGenerator;

/*!
    Metronome and tuning tone, available in QML as the singleton "Metronome".

    Clicks and the tone are synthesised sample-accurately in one audio stream,
    so the tempo stays exact and no sound files are needed. Keeps running while
    the user switches screens. Tempo, accent and pitch are remembered per device.
*/
class Metronome : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(int bpm READ bpm WRITE setBpm NOTIFY bpmChanged FINAL)
    //! Accent on beat 1 of every N beats; 1 = every beat the same
    Q_PROPERTY(int beatsPerBar READ beatsPerBar WRITE setBeatsPerBar NOTIFY beatsPerBarChanged FINAL)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged FINAL)
    //! Current beat in the bar (0 = accent), -1 when stopped
    Q_PROPERTY(int beat READ beat NOTIFY beatChanged FINAL)
    //! "A" or "Bb"
    Q_PROPERTY(QString toneNote READ toneNote WRITE setToneNote NOTIFY toneChanged FINAL)
    //! Concert pitch a′ in Hz
    Q_PROPERTY(int pitch READ pitch WRITE setPitch NOTIFY toneChanged FINAL)
    Q_PROPERTY(double toneFrequency READ toneFrequency NOTIFY toneChanged FINAL)
    Q_PROPERTY(bool tonePlaying READ tonePlaying NOTIFY tonePlayingChanged FINAL)
    Q_PROPERTY(QString error READ error NOTIFY errorChanged FINAL)

public:
    static constexpr int MinBpm = 40;
    static constexpr int MaxBpm = 300;
    static constexpr int MinPitch = 415;
    static constexpr int MaxPitch = 466;

    explicit Metronome(QObject *parent = nullptr);
    ~Metronome() override;

    int bpm() const { return m_bpm; }
    void setBpm(int bpm);
    int beatsPerBar() const { return m_beatsPerBar; }
    void setBeatsPerBar(int beats);
    bool running() const { return m_running; }
    int beat() const { return m_beat; }
    QString toneNote() const { return m_toneNote; }
    void setToneNote(const QString &note);
    int pitch() const { return m_pitch; }
    void setPitch(int pitch);
    double toneFrequency() const;
    bool tonePlaying() const { return m_tonePlaying; }
    QString error() const { return m_error; }

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void toggle();
    //! Tap in time: sets the tempo from the last taps
    Q_INVOKABLE void tap();
    Q_INVOKABLE void toggleTone();
    Q_INVOKABLE void stopTone();

signals:
    void bpmChanged();
    void beatsPerBarChanged();
    void runningChanged();
    void beatChanged();
    void toneChanged();
    void tonePlayingChanged();
    void errorChanged();

private:
    bool ensureAudio();
    void releaseAudioIfIdle();
    void updateBeat();
    void save();

    std::unique_ptr<ClickGenerator> m_generator;
    std::unique_ptr<QAudioSink> m_sink;
    QTimer m_beatTimer;
    QTimer m_idleTimer;
    QElapsedTimer m_tapClock;
    QList<qint64> m_taps;

    int m_bpm = 80;
    int m_beatsPerBar = 4;
    bool m_running = false;
    int m_beat = -1;
    QString m_toneNote = QStringLiteral("A");
    int m_pitch = 443;
    bool m_tonePlaying = false;
    QString m_error;
};
