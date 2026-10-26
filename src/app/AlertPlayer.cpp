// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "AlertPlayer.h"

#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>

AlertPlayer::AlertPlayer(QObject *parent)
    : QObject(parent)
{
}

AlertPlayer::~AlertPlayer()
{
    // Quitting while a sound plays: it may stop
    if (m_process && m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(200);
    }
}

QString AlertPlayer::program()
{
    // PipeWire (Kubuntu, Ubuntu, Fedora …), PulseAudio, plain ALSA
    for (const char *name : { "pw-play", "paplay", "aplay" }) {
        const QString path = QStandardPaths::findExecutable(QString::fromLatin1(name));
        if (!path.isEmpty())
            return path;
    }
    return {};
}

bool AlertPlayer::play(const QString &wavFile)
{
    static const QString player = program();
    if (player.isEmpty())
        return false;
    if (m_process && m_process->state() != QProcess::NotRunning)
        m_process->kill();   // a new alert or a preview replaces the one playing
    auto *process = new QProcess(this);
    process->setProgram(player);
    // aplay: no status lines on the terminal Qupil may have been started from
    process->setArguments(QFileInfo(player).fileName() == QLatin1String("aplay")
                              ? QStringList { QStringLiteral("-q"), wavFile }
                              : QStringList { wavFile });
    process->setStandardOutputFile(QProcess::nullDevice());
    process->setStandardErrorFile(QProcess::nullDevice());
    connect(process, &QProcess::finished, this, [this, process, wavFile](int code, QProcess::ExitStatus status) {
        process->deleteLater();
        // It could not play it (no sound server, …), not stopped by a newer sound
        if (status == QProcess::NormalExit && code != 0)
            emit failed(wavFile);
    });
    process->start();
    if (!process->waitForStarted(2000)) {
        process->deleteLater();
        return false;
    }
    m_process = process;
    return true;
}
