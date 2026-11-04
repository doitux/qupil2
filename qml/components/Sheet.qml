// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Editor container: a bottom sheet on phones, a centered card on wide screens.
// Header: Cancel · Title · Save. Children go into a scrolling column.
//
//   Sheet { id: sheet; title: qsTrId("note.new"); onAccepted: save() ; InputField { … } }

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Popup {
    id: sheet

    property string title
    property string acceptText: qsTrId("common.save")
    property string rejectText: qsTrId("common.cancel")
    property bool canAccept: true
    //! Why saving is not possible yet: the item that needs attention. Save then stays
    //! tappable and, instead of saving, scrolls there (acceptRefused: the editor marks it)
    property Item problem: null
    property bool showAccept: true
    //! Big button at the bottom instead of the text button in the header (phones), but not
    //! while typing: then Save is in the header, the big button would take room from the text
    property bool bottomButton: compact && !typing
    //! The on-screen keyboard is open (phones, tablets)
    readonly property bool keyboardShown: Qt.inputMethod.visible
    /*!
        Laid out for the on-screen keyboard. Nothing may change places while the sheet slides
        in or out (it flickered: the keyboard comes and goes at the same time, and on
        Android the window changes its height with it):
        - opening with the cursor in a field (the keyboard comes): laid out for it at once
        - while open: follows the keyboard (gone only after a moment: it may just be moving
          from one field to the next), Save moves over smoothly
        - closing: stays as it is, and with the keyboard open the sheet waits until the
          keyboard is gone before it fades out (dismiss)
    */
    property bool typing: false
    //! Closing, waiting for the keyboard to go down first (dismiss)
    property bool leaving: false
    default property alias content: column.data

    readonly property bool compact: parent ? parent.width < 720 : true

    signal accepted()
    signal rejected()
    signal acceptRefused()

    function accept() {
        if (leaving)
            return
        if (!canAccept) {
            if (problem) {
                reveal(problem)
                acceptRefused()
            }
            return
        }
        accepted()
        dismiss()
    }
    //! Scrolls the editor so that item is in view
    function reveal(item) {
        const top = item.mapToItem(flick.contentItem, 0, 0).y - 24
        const bottom = top + item.height + 48
        let y = flick.contentY
        if (top < y)
            y = top
        else if (bottom > y + flick.height)
            y = bottom - flick.height
        scroll.to = Math.max(0, Math.min(y, flick.contentHeight - flick.height))
        scroll.restart()
    }
    function reject() {
        if (leaving)
            return
        rejected()
        dismiss()
    }
    /*!
        Closes the sheet. With the on-screen keyboard open, it goes down first while the sheet
        stays (on Android the window grows back meanwhile, the pages under it with it), then
        the sheet fades out over the finished page.
    */
    function dismiss() {
        if (leaving || !visible)
            return
        if (App.desktop || !keyboardShown) {
            close()
            return
        }
        leaving = true
        Qt.inputMethod.hide()
        leaveAtLatest.restart()
    }
    //! The keyboard is down (or never comes): Save in the header goes to the bottom
    Timer {
        id: keyboardGone
        interval: 400
        onTriggered: if (sheet.opened && !sheet.leaving && !sheet.keyboardShown) sheet.typing = false
    }
    onKeyboardShownChanged: {
        if (leaving) {
            // Android: the window only grows back after the keyboard has gone (below)
            if (!keyboardShown && Qt.platform.os !== "android")
                leaveSettled.restart()
            return
        }
        if (!opened)
            return
        if (keyboardShown) {
            keyboardGone.stop()
            typing = true
        } else {
            keyboardGone.restart()
        }
    }
    // Android: the window grows back when the keyboard has gone; the sheet fades out once
    // the pages under it have their full height again
    Connections {
        target: sheet.parent
        enabled: sheet.leaving
        function onHeightChanged() { leaveSettled.restart() }
    }
    Timer {
        id: leaveSettled
        interval: 90
        onTriggered: if (sheet.leaving) sheet.close()
    }
    Timer {
        id: leaveAtLatest
        interval: 450
        onTriggered: if (sheet.leaving) sheet.close()
    }
    //! The field with the cursor, if the focus is in one
    function typingField() {
        const item = contentItem.Window.activeFocusItem
        return item && item.cursorPosition !== undefined && !item.readOnly
    }
    onAboutToShow: {
        leaving = false
        closing = false
        typing = false
        // The editor puts the cursor in a field right after open(): the keyboard comes
        Qt.callLater(() => {
            if (!App.desktop && sheet.visible && !sheet.closing && sheet.typingField())
                sheet.typing = true
        })
    }
    onOpened: {
        if (keyboardShown)
            typing = true
        else if (typing)
            keyboardGone.restart()
    }
    // While it fades out, the sheet keeps its place and size (the window may still change)
    property bool closing: false
    property real heldHeight: 0
    property real heldY: 0
    onAboutToHide: {
        heldHeight = height
        heldY = y
        closing = true
        leaving = false
        leaveSettled.stop()
        leaveAtLatest.stop()
        keyboardGone.stop()
    }
    Binding {
        target: sheet
        property: "height"
        value: sheet.heldHeight
        when: sheet.closing
        restoreMode: Binding.RestoreBinding
    }
    Binding {
        target: sheet
        property: "y"
        value: sheet.heldY
        when: sheet.closing
        restoreMode: Binding.RestoreBinding
    }

    parent: Overlay.overlay
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape
    padding: 0

    width: compact ? parent.width : Math.min(620, parent.width - 48)
    height: compact ? parent.height - 24
                    : Math.min(parent.height - 48, header.implicitHeight + column.implicitHeight + 40
                                                   + (sheet.bottomButton ? 76 : 0))
    x: compact ? 0 : Math.round((parent.width - width) / 2)
    y: compact ? parent.height - height : Math.round((parent.height - height) / 2)

    Overlay.modal: Rectangle { color: Theme.overlay }

    enter: Transition {
        NumberAnimation {
            property: sheet.compact ? "y" : "opacity"
            from: sheet.compact ? sheet.parent.height : 0
            to: sheet.compact ? sheet.parent.height - sheet.height : 1
            duration: 220
            easing.type: Easing.OutCubic
        }
    }
    exit: Transition {
        NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 120 }
    }

    background: Rectangle {
        radius: sheet.compact ? 24 : Theme.radiusLarge
        color: Theme.surface
        // square bottom corners on phones
        Rectangle {
            visible: sheet.compact
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: 24
            color: Theme.surface
        }
    }

    contentItem: ColumnLayout {
        spacing: 0

        Rectangle {
            visible: sheet.compact
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 8
            implicitWidth: 40
            implicitHeight: 5
            radius: 3
            color: Theme.border
        }

        // Anchored, not a RowLayout: when Save came into the header (keyboard open), the
        // layout did not follow and Save lay over "Cancel"
        Item {
            id: header
            Layout.fillWidth: true
            Layout.leftMargin: 8
            Layout.rightMargin: 8
            Layout.topMargin: sheet.compact ? 4 : 10
            implicitHeight: Math.max(cancelButton.implicitHeight, titleText.implicitHeight)

            ActionButton {
                id: cancelButton
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                kind: "quiet"
                text: sheet.rejectText
                // A tap must not take the focus from the text: the keyboard would start to
                // go down (and the sheet change) before the sheet closes
                focusPolicy: Qt.TabFocus
                onClicked: sheet.reject()
            }
            // Centred, as far as it fits between the buttons (a long one moves a little to
            // the right and is shortened if need be)
            Text {
                id: titleText
                readonly property real leftRoom: cancelButton.width + 4
                readonly property real rightRoom: headerAccept.visible ? headerAccept.width + 4 : 0
                readonly property bool centred: implicitWidth <= header.width - 2 * Math.max(leftRoom, rightRoom)
                x: centred ? Math.round((header.width - implicitWidth) / 2) : leftRoom
                width: centred ? implicitWidth : Math.max(0, header.width - leftRoom - rightRoom)
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Text.AlignHCenter
                text: sheet.title
                elide: Text.ElideRight
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 17
                font.weight: Font.ExtraBold
            }
            ActionButton {
                id: headerAccept
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                visible: sheet.showAccept && !sheet.bottomButton
                kind: "quiet"
                text: sheet.acceptText
                enabled: sheet.canAccept || sheet.problem !== null
                focusPolicy: Qt.TabFocus
                onClicked: sheet.accept()
                // Comes in softly when the keyboard opens while the sheet is open
                opacity: visible ? 1 : 0
                Behavior on opacity {
                    enabled: sheet.opened && !sheet.leaving
                    NumberAnimation { duration: 160 }
                }
            }
        }

        Flickable {
            id: flick
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            contentHeight: column.implicitHeight + 32
            boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: ScrollBar {}

            NumberAnimation {
                id: scroll
                target: flick
                property: "contentY"
                duration: 250
                easing.type: Easing.OutCubic
            }

            ColumnLayout {
                id: column
                x: 20
                y: 12
                width: flick.width - 40
                spacing: 16
            }
        }

        // Big Save button (phones). Folds away and back smoothly when the keyboard comes or
        // goes while the sheet is open, instead of jumping
        Item {
            id: bottomArea
            property real shown: sheet.bottomButton && sheet.showAccept ? 1 : 0
            Behavior on shown {
                enabled: sheet.opened && !sheet.leaving
                NumberAnimation { duration: 180; easing.type: Easing.OutCubic }
            }
            visible: shown > 0
            clip: true
            Layout.fillWidth: true
            Layout.preferredHeight: shown * (8 + 54 + 20)
            ActionButton {
                x: 20
                y: 8
                width: parent.width - 40
                height: 54
                opacity: bottomArea.shown
                kind: "primary"
                text: sheet.acceptText
                enabled: sheet.canAccept || sheet.problem !== null
                focusPolicy: Qt.TabFocus
                font.pixelSize: 17
                font.weight: Font.ExtraBold
                onClicked: sheet.accept()
            }
        }
    }
}
