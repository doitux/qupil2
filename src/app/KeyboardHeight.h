// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QtGlobal>

#include <functional>

/*!
    Android: the height of the on-screen keyboard over Qupil's window.

    The window keeps its size when the keyboard comes and goes (adjustNothing in the
    manifest): with Android making it smaller and larger again, the interface was empty
    or squeezed for a frame each time (white or black flicker). QupilActivity measures the
    keyboard and reports here; the interface makes room for it itself (Nav.keyboardInset).
*/
namespace KeyboardHeight {

//! \a changed gets the height in device-independent pixels (0: closed), on Qt's thread
void watch(std::function<void(qreal)> changed);

} // namespace KeyboardHeight
