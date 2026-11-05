// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QColor>

/*!
    Android: the system's status bar (top) and navigation bar (bottom) in Qupil's colours,
    with light or dark symbols. Qupil's light/dark choice can differ from the system's,
    and the activity's theme only follows the system (QupilActivity.setSystemBars).
    Elsewhere nothing: iOS and the desktop draw under or around the bars themselves.
*/
namespace SystemBars {

#ifdef Q_OS_ANDROID
void set(const QColor &top, const QColor &bottom, bool dark);
#else
inline void set(const QColor &, const QColor &, bool) {}
#endif

} // namespace SystemBars
