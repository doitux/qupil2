// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QtGlobal>

#include <functional>

class QWindow;

/*!
    The room the system's status bar (top) and home indicator (bottom) take of the app's
    window; the pages keep their content clear of it (Main.qml, topInset and bottomInset)
    and paint their background behind it (docs/student-app.md, "The screen's edges").

    iOS: the window covers the whole screen (Main.qml asks for it with
    Qt::MaximizeUsingFullscreenGeometryHint). Without that, Qt keeps the window inside the
    safe area and the strips above and below it are black, with a white status bar, also
    in the light look.

    Elsewhere: 0. Android colours its bars itself (res/values/student.xml), and the window
    ends at them.
*/
namespace SafeArea {

//! \a changed gets top and bottom in device-independent pixels, on Qt's thread
void watch(QWindow *window, std::function<void(qreal top, qreal bottom)> changed);

} // namespace SafeArea
