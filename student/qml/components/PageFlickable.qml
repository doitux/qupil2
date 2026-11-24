// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// A page that scrolls and looks after the keyboard: the text field with the cursor, and
// what belongs to it below (its buttons), stays in view when the page gets lower (Android:
// the page makes room for the keyboard, Main.qml; iOS moves the view itself so the cursor
// shows). When the page gets higher again it does not stay scrolled past its end.
// A tap beside the fields puts the keyboard away.

import QtQuick
import QtQuick.Window

Flickable {
    id: flick

    //! Room below the field that has to stay in view with it
    property real revealBelow: 72

    function reveal() {
        // Never past the ends (the keyboard went: the page is higher again)
        if (!flick.dragging && !flick.flicking) {
            const end = Math.max(0, flick.contentHeight - flick.height)
            if (flick.contentY > end)
                flick.contentY = end
            else if (flick.contentY < 0)
                flick.contentY = 0
        }
        const item = Window.activeFocusItem
        if (!item || item.cursorRectangle === undefined)
            return
        // Only fields on this page
        let p = item.parent
        while (p && p !== flick.contentItem)
            p = p.parent
        if (!p)
            return
        const top = item.mapToItem(flick.contentItem, 0, 0).y
        const bottom = top + item.height + flick.revealBelow
        const last = Math.max(0, flick.contentHeight - flick.height)
        if (bottom > flick.contentY + flick.height)
            flick.contentY = Math.min(bottom - flick.height, last)
        if (top - 12 < flick.contentY)
            flick.contentY = Math.max(0, top - 12)
    }

    // (at once, not a frame later: the page glides with the keyboard)
    onHeightChanged: reveal()
    Window.onActiveFocusItemChanged: Qt.callLater(reveal)

    MouseArea {
        parent: flick.contentItem
        anchors.fill: parent
        z: -1
        onClicked: flick.forceActiveFocus()
    }
}
