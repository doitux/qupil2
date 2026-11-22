// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Desktop: no scanner (the desktop build is for tests and screenshots).

#include "QrScanner.h"

namespace QrScanner {

bool nativeAvailable()
{
    return false;
}

void nativeScan(const Texts &, std::function<void(const Result &)> done)
{
    done({});
}

} // namespace QrScanner
