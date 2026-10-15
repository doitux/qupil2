// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Modal message / confirmation in the Qupil look.
// Without rejectText it is a plain message with one button.
//
//   AppDialog { id: ask; title: "…"; text: "…"; rejectText: qsTrId("common.cancel")
//               onAccepted: … }
//   ask.open()

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Popup {
    id: dialog

    property string title
    property string text
    property string acceptText: qsTrId("common.ok")
    property string rejectText: ""
    property bool destructive: false
    property string iconName: ""
    //! Extra content between text and buttons
    default property alias extra: extraColumn.data

    signal accepted()
    signal rejected()

    parent: Overlay.overlay
    anchors.centerIn: parent
    width: Math.min(parent ? parent.width - 32 : 440, 460)
    modal: true
    focus: true
    padding: 22
    closePolicy: Popup.CloseOnEscape

    Overlay.modal: Rectangle { color: Theme.overlay }

    background: Rectangle {
        radius: Theme.radiusLarge
        color: Theme.surface
        border.width: Theme.dark ? 1 : 0
        border.color: Theme.border
    }

    contentItem: ColumnLayout {
        spacing: 14

        RowLayout {
            spacing: 12
            Layout.fillWidth: true

            Rectangle {
                visible: dialog.iconName !== ""
                implicitWidth: 40
                implicitHeight: 40
                radius: 12
                color: dialog.destructive ? Theme.tile.orange.bg : Theme.accentSoft
                Icon {
                    anchors.centerIn: parent
                    width: 22
                    height: 22
                    name: dialog.iconName
                    color: dialog.destructive ? Theme.tile.orange.fg : Theme.accentText
                }
            }
            Text {
                Layout.fillWidth: true
                text: dialog.title
                wrapMode: Text.WordWrap
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 19
                font.weight: Font.ExtraBold
            }
        }

        Text {
            visible: dialog.text !== ""
            Layout.fillWidth: true
            text: dialog.text
            wrapMode: Text.Wrap
            color: Theme.textSecondary
            font.family: Theme.fontFamily
            font.pixelSize: 15
            lineHeight: 1.15
        }

        ColumnLayout {
            id: extraColumn
            Layout.fillWidth: true
            spacing: 8
            visible: children.length > 0
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.topMargin: 4
            spacing: 10

            Item { Layout.fillWidth: true }
            ActionButton {
                visible: dialog.rejectText !== ""
                kind: "quiet"
                text: dialog.rejectText
                onClicked: {
                    dialog.close()
                    dialog.rejected()
                }
            }
            ActionButton {
                kind: dialog.destructive ? "danger" : "primary"
                text: dialog.acceptText
                onClicked: {
                    dialog.close()
                    dialog.accepted()
                }
            }
        }
    }
}
