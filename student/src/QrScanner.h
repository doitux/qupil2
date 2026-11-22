// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QString>

#include <functional>

/*!
    The QR code scanner of the phone, for the coupling code the teacher's Qupil shows.

    iOS: the camera with AVFoundation's own QR recognition, in a full-screen sheet.
    Android: the scanner of zxing-android-embedded (StudentActivity.startScan).
    Both ask for the camera permission themselves. The picture stays on the phone.
    Desktop: none (tests set a pretended result).
*/
namespace QrScanner {

struct Result
{
    QString text;          // what the code says; "" when closed without a code
    bool denied = false;   // the camera is not allowed for the app
};

struct Texts
{
    QString prompt;        // shown in the scanner
    QString cancel;        // the button that closes it (iOS)
};

bool available();
//! Opens the scanner; \a done is called once, on Qt's thread. A second call while the
//! scanner is open does nothing.
void scan(const Texts &texts, std::function<void(const Result &)> done);

//! Tests and screenshots: the scanner counts as available and gives this result
void pretend(const Result &result);

// ---- the platform's part (QrScanner_<platform>)
bool nativeAvailable();
//! \a done may be called on any thread
void nativeScan(const Texts &texts, std::function<void(const Result &)> done);

} // namespace QrScanner
