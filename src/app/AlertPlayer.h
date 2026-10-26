// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QObject>
#include <QPointer>

class QProcess;

/*!
    Linux: plays the alert sounds (short WAV files) with the system's own player
    in a process of its own: PipeWire's pw-play, PulseAudio's paplay or ALSA's
    aplay, whichever is there.

    In Qupil's process the sound came out choppy when the screen was switched
    off: Qt feeds the audio output in small portions with the help of the main
    thread, and drawing the window can block that thread for a long time while
    the screen is off (no frames are asked for). A separate process plays the
    sound to the end whatever Qupil does meanwhile.
*/
class AlertPlayer : public QObject
{
    Q_OBJECT

public:
    explicit AlertPlayer(QObject *parent = nullptr);
    ~AlertPlayer() override;

    //! Starts the sound (a running one stops). False if no player program is
    //! there or it cannot be started: use another way.
    bool play(const QString &wavFile);

    //! The player program found, empty if none
    static QString program();

signals:
    //! The player program ended with an error: play the file another way
    void failed(const QString &wavFile);

private:
    QPointer<QProcess> m_process;
};
