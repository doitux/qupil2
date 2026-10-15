// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Whole-row switch with a title and an optional hint below it.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

AbstractButton {
    id: row

    property string hint
    property bool tinted: true

    checkable: true
    Layout.fillWidth: true
    implicitHeight: Math.max(56, content.implicitHeight + 24)
    focusPolicy: Qt.StrongFocus
    Accessible.role: Accessible.CheckBox
    Accessible.checked: checked
    Accessible.name: text

    background: Rectangle {
        radius: 14
        color: row.tinted ? Theme.surfaceAlt : "transparent"
        border.width: row.visualFocus ? 2 : 0
        border.color: Theme.accentText
    }

    contentItem: RowLayout {
        id: content
        spacing: 12
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: row.text
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 15
                font.weight: Font.Bold
            }
            Text {
                visible: row.hint !== ""
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: row.hint
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 13
            }
        }
        Rectangle {
            implicitWidth: 52
            implicitHeight: 32
            radius: 16
            color: row.checked ? Theme.switchOn : Theme.switchOff
            Behavior on color { ColorAnimation { duration: 120 } }
            Rectangle {
                x: row.checked ? parent.width - width - 3 : 3
                y: 3
                width: 26
                height: 26
                radius: 13
                color: "#FFFFFF"
                Behavior on x { NumberAnimation { duration: 120 } }
            }
        }
    }
    leftPadding: 14
    rightPadding: 14
}
