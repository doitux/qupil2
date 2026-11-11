// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#import <AVFoundation/AVFoundation.h>

/*
    Metronome and tuning tone on iPad and iPhone: the audio session for playing, so the
    clicks go on while Qupil is in the background (with UIBackgroundModes "audio" in the
    Info.plist, patch-info-plist.sh) and when the device is switched to silent – like a
    metronome app. Mixes with other audio (a recording played along) instead of
    stopping it.

    Qt sets the session back to "Ambient" whenever a QAudioSink opens (QDarwinAudioSink::
    open(), Qt 6.8 – also for a QSoundEffect): with "Ambient" iOS stops the sound when
    Qupil goes to the background or the screen locks, and the silent switch mutes it. So
    this is called after the sink has started and again while it plays.
*/
void qupilPlaybackAudioSession()
{
    AVAudioSession *session = [AVAudioSession sharedInstance];
    NSError *error = nil;
    if (![session.category isEqualToString:AVAudioSessionCategoryPlayback]
        || !(session.categoryOptions & AVAudioSessionCategoryOptionMixWithOthers)) {
        [session setCategory:AVAudioSessionCategoryPlayback
                        mode:AVAudioSessionModeDefault
                     options:AVAudioSessionCategoryOptionMixWithOthers
                       error:&error];
        if (error)
            NSLog(@"Qupil: audio session category: %@", error);
    }
    error = nil;
    [session setActive:YES error:&error];
    if (error)
        NSLog(@"Qupil: audio session active: %@", error);
}

//! The session is set for playing (for the UI test and the log)
bool qupilPlaybackAudioSessionSet()
{
    return [[AVAudioSession sharedInstance].category isEqualToString:AVAudioSessionCategoryPlayback];
}
