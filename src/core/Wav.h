// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QByteArray>
#include <QList>

#include <optional>

/*!
    Short sounds as 16-bit PCM and WAV files: the lesson-end alert is stored
    and handed to the system (iOS and Android notifications) as WAV, with the
    chosen volume already applied.
*/
namespace Qupil::Wav {

struct Sound
{
    int sampleRate = 0;
    int channels = 0;
    QList<qint16> samples;   //!< interleaved

    bool isValid() const { return sampleRate > 0 && channels > 0 && !samples.isEmpty(); }
    double seconds() const { return isValid() ? double(samples.size()) / channels / sampleRate : 0; }
    //! Largest absolute sample value
    int peak() const;
};

/*!
    Reads PCM WAV (8, 16, 24 or 32 bit integer, 32 bit float), any sample rate.
    More than two channels are reduced to the first two.
*/
std::optional<Sound> read(const QByteArray &data);

//! A plain 44-byte-header WAV file (16 bit PCM)
QByteArray write(const Sound &sound);

//! Every sample multiplied by gain (clipped to 16 bit)
Sound scaled(Sound sound, double gain);

//! At most maxSeconds long, with a short fade-out where it was cut
Sound trimmed(Sound sound, double maxSeconds);

} // namespace Qupil::Wav
