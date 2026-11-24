// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
import QtQuick

/*
    A one-line field whose text is longer than the field (a long hint on a phone). Inside a
    TextField:

        TextField { id: hint; LineScroll { target: hint } }

    - A finger wiping sideways moves the text, letter by letter. The cursor and the selection
      stay as they are, so this is also for iPad and iPhone, where Qupil leaves both to the
      system. Typing brings the cursor back into view.
    - Without the focus the field shows the start of its text, not its end (the cursor is at
      the end after the text is set, and a field keeps its cursor in view).
    TextTouch brings this along; a field without TextTouch takes it by itself. In a field with
    several lines it does nothing.
*/
Item {
    id: line

    required property Item target
    //! A one-line field (TextInput has no lineCount)
    readonly property bool singleLine: target.lineCount === undefined && target.ensureVisible !== undefined
    readonly property bool longer: singleLine
                                   && target.contentWidth > target.width - target.leftPadding - target.rightPadding

    //! A wipe begins
    signal started()
    //! The text was moved (what follows the text has to follow)
    signal moved()

    anchors.fill: parent

    DragHandler {
        id: wipe
        objectName: "lineScrollWipe"
        target: null
        enabled: line.longer
        acceptedDevices: PointerDevice.TouchScreen
        yAxis.enabled: false
        // Also from a tap handler or the field itself, which took the touch on press
        grabPermissions: PointerHandler.CanTakeOverFromAnything
        property real done: 0
        //! The finger rested first (a long press selects, the system's loupe comes): no wiping
        property bool late: false
        onActiveChanged: {
            done = 0
            if (active) {
                late = centroid.timeHeld > 0.4
                if (!late)
                    line.started()
            }
        }
        onTranslationChanged: {
            if (!active || late)
                return
            const t = line.target
            const y = t.height / 2
            for (let guard = 0; guard < 400; ++guard) {
                const rest = translation.x - done
                if (rest > 0) {
                    // The finger goes to the right: what is before comes into view
                    const first = t.positionAt(t.leftPadding + 1, y)
                    if (first <= 0)
                        break
                    const w = t.positionToRectangle(first).x - t.positionToRectangle(first - 1).x
                    if (!(w > 0) || rest < w)
                        break
                    t.ensureVisible(first - 1)
                    done += w
                } else if (rest < 0) {
                    const last = t.positionAt(t.width - t.rightPadding - 1, y)
                    if (last >= t.length)
                        break
                    const w = t.positionToRectangle(last + 1).x - t.positionToRectangle(last).x
                    if (!(w > 0) || -rest < w)
                        break
                    t.ensureVisible(last + 1)
                    done -= w
                } else {
                    break
                }
            }
            line.moved()
        }
    }

    function toStart() {
        if (singleLine && !target.activeFocus && target.cursorPosition !== 0)
            target.cursorPosition = 0
    }
    Component.onCompleted: toStart()
    Connections {
        target: line.target
        enabled: line.singleLine
        function onTextChanged() { line.toStart() }
        function onActiveFocusChanged() {
            const t = line.target
            const reason = t.focusReason
            if (!t.activeFocus) {
                // Not when only the window or a popup took the focus for a moment: then the
                // cursor is still where the user was writing
                if (reason !== Qt.ActiveWindowFocusReason && reason !== Qt.PopupFocusReason)
                    t.cursorPosition = 0
            } else if (reason === Qt.OtherFocusReason && t.selectedText === "") {
                // Focused by the app (a new sheet, Enter to the next field): go on at the end
                t.cursorPosition = t.length
            }
        }
    }
}
