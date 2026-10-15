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

Popup {
    id: sheet

    property string title
    property string acceptText: qsTrId("common.save")
    property string rejectText: qsTrId("common.cancel")
    property bool canAccept: true
    property bool showAccept: true
    //! Big button at the bottom instead of the text button in the header (phones)
    property bool bottomButton: compact
    default property alias content: column.data

    readonly property bool compact: parent ? parent.width < 720 : true

    signal accepted()
    signal rejected()

    function accept() {
        if (!canAccept)
            return
        accepted()
        close()
    }
    function reject() {
        rejected()
        close()
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

        RowLayout {
            id: header
            Layout.fillWidth: true
            Layout.leftMargin: 8
            Layout.rightMargin: 8
            Layout.topMargin: sheet.compact ? 4 : 10
            spacing: 4

            ActionButton {
                id: cancelButton
                kind: "quiet"
                text: sheet.rejectText
                onClicked: sheet.reject()
            }
            Text {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: sheet.title
                elide: Text.ElideRight
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 17
                font.weight: Font.ExtraBold
            }
            ActionButton {
                visible: sheet.showAccept && !sheet.bottomButton
                kind: "quiet"
                text: sheet.acceptText
                enabled: sheet.canAccept
                onClicked: sheet.accept()
            }
            // keeps the title centred when the save button is at the bottom
            Item {
                visible: !sheet.showAccept || sheet.bottomButton
                implicitWidth: cancelButton.implicitWidth
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

            ColumnLayout {
                id: column
                x: 20
                y: 12
                width: flick.width - 40
                spacing: 16
            }
        }

        ActionButton {
            visible: sheet.bottomButton && sheet.showAccept
            Layout.fillWidth: true
            Layout.leftMargin: 20
            Layout.rightMargin: 20
            Layout.bottomMargin: 20
            Layout.topMargin: 8
            Layout.preferredHeight: 54
            kind: "primary"
            text: sheet.acceptText
            enabled: sheet.canAccept
            font.pixelSize: 17
            font.weight: Font.ExtraBold
            onClicked: sheet.accept()
        }
    }
}
