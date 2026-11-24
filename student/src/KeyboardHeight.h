// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QtGlobal>

#include <functional>

/*!
    The height of the on-screen keyboard over the app's window; the pages make room for it
    themselves (Main.qml, keyboardInset), as Qupil does (docs/student-app.md, "The keyboard").

    Android: the window keeps its size when the keyboard comes and goes (adjustNothing in
    the manifest): with Android making it smaller and larger again, the page jumped and was
    empty for a moment each time. StudentActivity measures the keyboard and reports here.

    Elsewhere: 0. iOS moves the view itself so that the cursor shows.
*/
namespace KeyboardHeight {

//! \a changed gets the height in device-independent pixels (0: closed), on Qt's thread
void watch(std::function<void(qreal)> changed);

} // namespace KeyboardHeight
