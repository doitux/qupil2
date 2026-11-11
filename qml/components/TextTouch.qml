// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
import QtQuick
import QtQuick.Controls
import Qupil

/*
    Selecting and editing text with a finger – the same on iPhone, iPad and Android
    (docs/text-touch.md). Inside a TextArea or TextField:

        TextArea { id: notes; inputMethodHints: Qt.ImhNoTextHandles; TextTouch { target: notes } }

    The system's own selection (iOS: magnifier, handles, menu; Android: long press, handles,
    "Einfügen") is switched off – Qt.ImhNoTextHandles on the field, QT_QPA_NO_TEXT_HANDLES on
    Android (main.cpp) – so only one set of rules answers a finger:
    - tap: cursor there (the first tap focuses the field and brings the keyboard);
      a tap on the cursor or into the selection shows or hides the menu
    - double tap: the word, at once on the second touch; triple tap: the paragraph
    - long press (0.45 s): the word, while the finger is still down; stays when it lifts
    - handles: drag to change the selection
    - wiping scrolls (the Flickable takes the touch over), it never selects
    Typing, autocorrection and dictation stay with the system. A mouse or trackpad goes to
    the field as usual; only its double and triple click use the same word and paragraph.
*/
Item {
    id: touch

    required property Item target
    readonly property bool readOnly: target.readOnly === true
    readonly property bool hasSelection: target.selectionStart !== target.selectionEnd
    //! Handles only after a touch (not for a mouse selection)
    property bool touchMode: false
    property bool menuShown: false

    anchors.fill: parent

    // ---------------------------------------------------------------- words, paragraphs
    // The same rule as TextFormatting::wordAt (C++): letters, digits, hyphens and apostrophes
    // are one word ("G-Dur", "Bach’s"); punctuation around it is not
    function isWordChar(c) {
        if (c === "-" || c === "'" || c === "’")
            return true
        return c.toLowerCase() !== c.toUpperCase() || (c >= "0" && c <= "9")
            || /[²³¹¼-¾]/.test(c)
    }
    function isJoiner(c) {
        return c === "-" || c === "'" || c === "’"
    }
    function wordRange(position) {
        const text = target.getText(0, target.length)
        let from = position, to = position
        while (from > 0 && isWordChar(text[from - 1]))
            --from
        while (to < text.length && isWordChar(text[to]))
            ++to
        while (from < to && isJoiner(text[from]))
            ++from
        while (to > from && isJoiner(text[to - 1]))
            --to
        return [from, to]
    }
    function isLineEnd(c) {
        return c === "\n" || c === " " || c === " "
    }
    function paragraphRange(position) {
        const text = target.getText(0, target.length)
        let from = position, to = position
        while (from > 0 && !isLineEnd(text[from - 1]))
            --from
        while (to < text.length && !isLineEnd(text[to]))
            ++to
        return [from, to]
    }

    // ---------------------------------------------------------------- actions
    function positionAt(point) {
        return target.positionAt(point.x, point.y)
    }
    function focusField() {
        if (!target.activeFocus)
            target.forceActiveFocus()
        if (!readOnly)
            Qt.inputMethod.show()
    }
    function placeCursor(point) {
        Qt.inputMethod.commit()
        focusField()
        target.cursorPosition = positionAt(point)
    }
    function selectRange(range) {
        Qt.inputMethod.commit()
        focusField()
        if (range[0] === range[1])
            target.cursorPosition = range[0]
        else
            target.select(range[0], range[1])
    }
    //! The word under the point – none when the point is beside the text (behind the end
    //! of a line, in the empty part of the field)
    function wordUnder(point) {
        const p = positionAt(point)
        const r = wordRange(p)
        if (r[0] === r[1])
            return [p, p]
        const a = target.positionToRectangle(r[0])
        const b = target.positionToRectangle(r[1])
        const sameLine = Math.abs(a.y - b.y) < 2
        const inLine = point.y >= a.y - 4 && point.y <= b.y + b.height + 4
        const inWord = sameLine ? point.x >= a.x - 6 && point.x <= b.x + 6 : true
        return inLine && inWord ? r : [p, p]
    }
    function selectWordAt(point) {
        selectRange(wordUnder(point))
    }
    function selectParagraphAt(point) {
        selectRange(paragraphRange(positionAt(point)))
    }
    function showMenu() {
        menuShown = true
        menu.place()
    }
    function hideMenu() {
        menuShown = false
    }

    // ---------------------------------------------------------------- the finger
    property real lastPress: 0
    property point lastPoint
    property int taps: 0

    function pressed(point) {
        touchMode = true
        const now = Date.now()
        const interval = Qt.styleHints.mouseDoubleClickInterval > 0 ? Qt.styleHints.mouseDoubleClickInterval : 400
        const near = Math.abs(point.x - lastPoint.x) < 30 && Math.abs(point.y - lastPoint.y) < 30
        taps = now - lastPress <= interval && near ? taps + 1 : 1
        lastPress = now
        lastPoint = point
        // At once on the touch, not when the finger lifts
        if (taps === 2) {
            selectWordAt(point)
            showMenu()
        } else if (taps >= 3) {
            taps = 3
            selectParagraphAt(point)
            showMenu()
        }
    }
    function tapped(point) {
        if (taps >= 2)
            return
        if (!target.activeFocus) {
            placeCursor(point)
            hideMenu()
            return
        }
        const p = positionAt(point)
        if (hasSelection) {
            const from = Math.min(target.selectionStart, target.selectionEnd)
            const to = Math.max(target.selectionStart, target.selectionEnd)
            if (inSelection(point, p, from, to)) {
                menuShown ? hideMenu() : showMenu()
                focusField()
                return
            }
            placeCursor(point)
            hideMenu()
            return
        }
        if (p === target.cursorPosition) {
            menuShown ? hideMenu() : showMenu()
            focusField()
            return
        }
        placeCursor(point)
        hideMenu()
    }
    //! The point is on the selected text – not behind its last character (the end of a line,
    //! the empty part of the field) or before its first one
    function inSelection(point, p, from, to) {
        if (p > from && p < to)
            return true
        if (p !== from && p !== to)
            return false
        const a = target.positionToRectangle(from)
        const b = target.positionToRectangle(to)
        if (point.y < a.y || point.y > b.y + b.height)
            return false
        if (p === from && point.y <= a.y + a.height && point.x < a.x)
            return false
        if (p === to && point.y >= b.y && point.x > b.x)
            return false
        return true
    }
    function longPressed(point) {
        taps = 0
        selectWordAt(point)
        showMenu()
    }

    TapHandler {
        id: finger
        objectName: "textTouchFinger"
        acceptedDevices: PointerDevice.TouchScreen
        // Takes the touch on press: the field itself gets none (it would move the cursor
        // again); a Flickable around may still take it over to scroll
        gesturePolicy: TapHandler.ReleaseWithinBounds
        longPressThreshold: 0.45
        onPressedChanged: if (pressed) touch.pressed(point.position)
        onTapped: (eventPoint) => touch.tapped(eventPoint.position)
        onLongPressed: touch.longPressed(point.position)
    }

    // A mouse or trackpad: the field selects by itself; double and triple click then take
    // the same word and paragraph as a finger
    TapHandler {
        // On the field itself (on this layer it kept the press from the field: no focus)
        parent: touch.target
        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
        gesturePolicy: TapHandler.DragThreshold
        onPressedChanged: if (pressed) { touch.touchMode = false; touch.hideMenu() }
        // After the field's own handling of the click (it would widen a triple click to the
        // end of the paragraph again); still before the next frame
        onTapped: (eventPoint) => {
            const point = eventPoint.position
            if (tapCount === 2)
                Qt.callLater(touch.selectWordAt, point)
            else if (tapCount === 3)
                Qt.callLater(touch.selectParagraphAt, point)
        }
    }

    // ---------------------------------------------------------------- handles
    readonly property bool handlesShown: touchMode && hasSelection && target.activeFocus
    readonly property real handleSize: 14

    component Handle: Item {
        id: handle
        property bool atEnd: false
        readonly property int position: atEnd ? Math.max(touch.target.selectionStart, touch.target.selectionEnd)
                                               : Math.min(touch.target.selectionStart, touch.target.selectionEnd)
        readonly property rect line: {
            // re-evaluated when the text or the selection changes
            touch.target.length; touch.target.width; touch.target.contentHeight
            return touch.target.positionToRectangle(position)
        }
        visible: touch.handlesShown
        // A big area for the finger around the knob
        width: 44
        height: line.height + touch.handleSize + 22
        x: line.x - width / 2
        y: atEnd ? line.y : line.y - touch.handleSize - 22
        z: 10
        Accessible.role: Accessible.Grip
        Accessible.name: atEnd ? qsTrId("textmenu.handle_end") : qsTrId("textmenu.handle_start")

        Rectangle {
            x: (parent.width - width) / 2
            y: handle.atEnd ? 0 : touch.handleSize + 22
            width: 2
            height: handle.line.height
            color: Theme.accent
        }
        Rectangle {
            x: (parent.width - width) / 2
            y: handle.atEnd ? handle.line.height : 22
            width: touch.handleSize
            height: touch.handleSize
            radius: width / 2
            color: Theme.accent
        }
        DragHandler {
            id: drag
            target: null
            acceptedDevices: PointerDevice.TouchScreen | PointerDevice.Mouse
            // Not given away to the Flickable: dragging a handle never scrolls by itself
            grabPermissions: PointerHandler.CanTakeOverFromAnything
            property int anchorPosition: 0
            property point start
            onActiveChanged: {
                if (active) {
                    anchorPosition = handle.atEnd ? Math.min(touch.target.selectionStart, touch.target.selectionEnd)
                                                  : Math.max(touch.target.selectionStart, touch.target.selectionEnd)
                    start = Qt.point(handle.line.x, handle.line.y + handle.line.height / 2)
                    touch.hideMenu()
                } else {
                    touch.showMenu()
                }
            }
            onTranslationChanged: {
                if (!active)
                    return
                let p = touch.target.positionAt(start.x + translation.x, start.y + translation.y)
                if (p === anchorPosition)
                    p = handle.atEnd ? anchorPosition + 1 : anchorPosition - 1
                p = Math.max(0, Math.min(touch.target.length, p))
                touch.target.select(anchorPosition, p)
            }
        }
    }
    Handle { id: startHandle; objectName: "textTouchStart" }
    Handle { id: endHandle; objectName: "textTouchEnd"; atEnd: true }

    // ---------------------------------------------------------------- menu
    Connections {
        target: touch.target
        function onActiveFocusChanged() {
            if (!touch.target.activeFocus)
                touch.hideMenu()
        }
        function onTextChanged() {
            if (!menu.acting)
                touch.hideMenu()
        }
    }
    // Scrolling moves the text away from the menu
    Connections {
        target: touch.flickable
        ignoreUnknownSignals: true
        function onMovementStarted() { touch.hideMenu() }
    }
    readonly property Item flickable: {
        let p = target.parent
        while (p) {
            if (p.flicking !== undefined && p.contentY !== undefined)
                return p
            p = p.parent
        }
        return null
    }

    // Where the menu lives: the window, or the sheet the field is in (a Popup: its item is a
    // child of the overlay). A plain item, not a Popup: a popup above a modal sheet took the
    // next touch on the text away from it
    readonly property Item menuHost: {
        let p = target
        while (p && p.parent) {
            if (p.parent === touch.Overlay.overlay || p.parent === touch.Window.contentItem)
                return p
            p = p.parent
        }
        return p
    }

    Rectangle {
        id: menu
        objectName: "textTouchMenu"
        property bool acting: false
        parent: touch.menuHost
        visible: touch.menuShown && touch.target.activeFocus && items.actions.length > 0
        z: 1000
        width: items.implicitWidth
        height: items.implicitHeight
        radius: 10
        color: Theme.inverse

        function place() {
            if (!parent)
                return
            const from = Math.min(touch.target.selectionStart, touch.target.selectionEnd)
            const to = Math.max(touch.target.selectionStart, touch.target.selectionEnd)
            const a = touch.target.positionToRectangle(from)
            const b = touch.target.positionToRectangle(to)
            const top = touch.target.mapToItem(parent, a.x, a.y)
            const bottom = touch.target.mapToItem(parent, b.x, b.y + b.height)
            // Above the selection (clear of the start handle), else below it
            const yAbove = top.y - height - 12 - (touch.hasSelection ? touch.handleSize : 0)
            y = yAbove > 8 ? yAbove : bottom.y + 14 + (touch.hasSelection ? touch.handleSize : 0)
            const centre = touch.hasSelection && Math.abs(top.y - bottom.y) < a.height + 2 ? (top.x + bottom.x) / 2 : top.x
            x = Math.max(8, Math.min(parent.width - width - 8, centre - width / 2))
        }
        onWidthChanged: if (visible) place()
        onVisibleChanged: if (visible) place()

        Row {
            id: items
            // What the menu offers now, in this order
            readonly property var actions: {
                touch.target.length; touch.target.selectionStart; touch.target.selectionEnd; touch.target.cursorPosition
                const list = []
                const all = touch.target.length > 0
                            && Math.abs(touch.target.selectionEnd - touch.target.selectionStart) < touch.target.length
                const paste = !touch.readOnly && touch.target.canPaste === true
                if (touch.hasSelection) {
                    if (!touch.readOnly)
                        list.push("cut")
                    list.push("copy")
                } else if (touch.target.length > 0) {
                    const r = touch.wordRange(touch.target.cursorPosition)
                    if (r[0] !== r[1])
                        list.push("select")
                }
                if (paste)
                    list.push("paste")
                if (all)
                    list.push("select_all")
                return list
            }
            function label(action) {
                switch (action) {
                case "cut": return qsTrId("textmenu.cut")
                case "copy": return qsTrId("textmenu.copy")
                case "paste": return qsTrId("textmenu.paste")
                case "select": return qsTrId("textmenu.select")
                default: return qsTrId("textmenu.select_all")
                }
            }
            function run(action) {
                menu.acting = true
                const keep = action === "select" || action === "select_all"
                if (action === "cut")
                    touch.target.cut()
                else if (action === "copy")
                    touch.target.copy()
                else if (action === "paste")
                    touch.target.paste()
                else if (action === "select")
                    touch.selectRange(touch.wordRange(touch.target.cursorPosition))
                else if (action === "select_all")
                    touch.target.selectAll()
                menu.acting = false
                if (keep)
                    touch.showMenu()
                else
                    touch.hideMenu()
            }

            Repeater {
                model: items.actions
                delegate: Row {
                    id: entry
                    required property string modelData
                    required property int index
                    Rectangle {
                        visible: entry.index > 0
                        width: 1
                        height: 22
                        anchors.verticalCenter: parent.verticalCenter
                        color: Theme.inverseMuted
                    }
                    AbstractButton {
                        id: action
                        focusPolicy: Qt.NoFocus
                        height: 40
                        leftPadding: 14
                        rightPadding: 14
                        text: items.label(entry.modelData)
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        Accessible.name: text
                        onClicked: items.run(entry.modelData)
                        contentItem: Text {
                            text: action.text
                            color: Theme.textOnInverse
                            font: action.font
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            color: action.pressed ? Theme.inverseMuted : "transparent"
                            radius: 10
                        }
                    }
                }
            }
        }
    }
}
