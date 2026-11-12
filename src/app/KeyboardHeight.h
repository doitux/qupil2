// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QtGlobal>

#include <functional>

/*!
    The height of the on-screen keyboard over Qupil's window; the interface makes room for
    it itself (Nav.keyboardInset).

    Android: the window keeps its size when the keyboard comes and goes (adjustNothing in
    the manifest): with Android making it smaller and larger again, the interface was empty
    or squeezed for a frame each time (white or black flicker). QupilActivity measures the
    keyboard and reports here.

    iOS: the height the keyboard will have, as soon as it starts to come or go (Qt's
    keyboardRectangle has it only once the keyboard is there); 0 for a floating or split
    keyboard on the iPad.
*/
namespace KeyboardHeight {

//! \a changed gets the height in device-independent pixels (0: closed), on Qt's thread
void watch(std::function<void(qreal)> changed);

/*!
    iOS: while \a hold is on (an editor is open), Qt's moving of the whole view to show the
    cursor above the keyboard is undone right away. The editor makes room for the keyboard
    itself (Nav.keyboardInset) and keeps the cursor in view; with both moving, the editor
    went off the top of the screen.
*/
void holdView(bool hold);
//! iOS: undo such a move now (called after the cursor moved while typing)
void keepViewInPlace();
//! iOS: how far Qt's view is moved right now, as shown on screen (UI test; 0: in place)
qreal viewOffset();

} // namespace KeyboardHeight
