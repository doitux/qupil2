// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// One of several choices with a title and an explanation (radio button look).

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

AbstractButton {
    id: option

    property string hint
    property bool selected

    Layout.fillWidth: true
    implicitHeight: optionRow.implicitHeight + 24
    focusPolicy: Qt.StrongFocus
    Accessible.role: Accessible.RadioButton
    Accessible.checked: selected
    Accessible.name: text

    background: Rectangle {
        radius: 14
        color: option.selected ? Theme.accentSoft : Theme.surfaceAlt
        border.width: option.visualFocus ? 2 : option.selected ? 1.5 : 0
        border.color: option.visualFocus ? Theme.accentText : Theme.accent
    }
    contentItem: RowLayout {
        id: optionRow
        spacing: 12
        Rectangle {
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: 1
            implicitWidth: 20
            implicitHeight: 20
            radius: 10
            color: "transparent"
            border.width: 2
            border.color: option.selected ? Theme.accent : Theme.icon
            Rectangle {
                anchors.centerIn: parent
                width: 10
                height: 10
                radius: 5
                visible: option.selected
                color: Theme.accent
            }
        }
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 2
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: option.text
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 15
                font.weight: Font.Bold
            }
            Text {
                visible: option.hint !== ""
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: option.hint
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 13
            }
        }
    }
    leftPadding: 14
    rightPadding: 14
}
