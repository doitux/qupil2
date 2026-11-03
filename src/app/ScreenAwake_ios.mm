// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// iOS: no auto-lock while the metronome or the tone plays (Qupil's event loop runs on
// the main thread, where UIKit wants this set).

#include "ScreenAwake.h"

#import <UIKit/UIKit.h>

namespace ScreenAwake {

void set(bool awake)
{
    [UIApplication sharedApplication].idleTimerDisabled = awake ? YES : NO;
}

} // namespace ScreenAwake
