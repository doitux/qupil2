// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QObject>
#include <QPointer>
#include <QString>
#include <QUrl>

#include <functional>

#include "Wav.h"

class QAudioBuffer;
class QAudioDecoder;
class QSoundEffect;
class AlertPlayer;

/*!
    The sound of an alert (lesson end, reminders): the one from Qupil 1.x or a
    file of your own, at a volume of 1 … 10. Per device (QSettings).

    Your own file is decoded once (WAV directly, other formats by the system)
    and kept as WAV, at most kMaxSeconds long. For playing and for the system
    notifications there is a WAV with the volume applied (file()).
*/
class AlertSound : public QObject
{
    Q_OBJECT

public:
    static constexpr int kMaxSeconds = 10;

    /*!
        name: file prefix ("lesson-end", "reminder"); builtIn: resource of the
        default sound; group: settings group; defaultVolume: without own setting
    */
    AlertSound(const QString &name, const QString &builtIn, const QString &group, std::function<int()> defaultVolume,
               QObject *parent);

    int volume() const;
    void setVolume(int volume);
    //! File name of your own sound, empty for the default sound
    QString customName() const;
    bool busy() const { return m_busy; }

    //! Your own sound (also content:// on Android); plays it when it is ready
    void choose(const QUrl &file);
    void useDefault();
    //! The sound with the volume applied (created when needed); empty on failure
    QString file();
    void play();

signals:
    //! Your own sound or the volume changed
    void changed();
    void busyChanged();
    void failed(const QString &message);

private:
    QString folder() const;
    QString customFile() const;
    QByteArray source() const;
    void store(const QString &key, const QVariant &value);
    void storeCustom(const Qupil::Wav::Sound &sound, const QString &name);
    void decoded(const QAudioBuffer &buffer);
    void finishDecoding(bool ok);
    void setBusy(bool busy);
    //! QSoundEffect in Qupil's own process
    void playHere(const QString &path);

    QString m_name;
    QString m_builtIn;
    QString m_group;
    std::function<int()> m_defaultVolume;
    QSoundEffect *m_effect = nullptr;
    AlertPlayer *m_player = nullptr;   // Linux: the system's player in a process of its own
    bool m_playWhenReady = false;
    QPointer<QAudioDecoder> m_decoder;
    Qupil::Wav::Sound m_decoding;
    QString m_decodingName;
    QString m_decodingFile;
    bool m_busy = false;
};
