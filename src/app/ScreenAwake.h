// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

/*!
    Keeps the display on while the metronome or the tuning tone plays. On a phone
    or tablet the screen would go off after a while, and the app and its sound
    stop with it (Android). It only applies while Qupil is in front; the power
    button still turns the screen off.

    Android: FLAG_KEEP_SCREEN_ON on the activity's window; iOS: the idle timer
    is off. Desktop systems: nothing.
*/
namespace ScreenAwake {

void set(bool awake);

} // namespace ScreenAwake
