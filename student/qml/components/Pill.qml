// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
// One choice of a feedback row ("a lot", "works well"); tap again to clear
import QtQuick
import QupilStudent

Rectangle {
    id: pill
    property string text
    property bool selected: false
    property color selectedColor: Theme.inverse
    property color selectedText: Theme.textOnInverse
    signal clicked()

    implicitWidth: label.implicitWidth + 22
    implicitHeight: 34
    radius: 17
    color: selected ? selectedColor : (area.pressed ? Theme.border : Theme.surfaceAlt)
    Accessible.role: Accessible.Button
    Accessible.name: text
    Accessible.checked: selected

    T {
        id: label
        anchors.centerIn: parent
        text: pill.text
        font.pixelSize: 14
        font.weight: Font.Bold
        color: pill.selected ? pill.selectedText : Theme.textSecondary
        wrapMode: Text.NoWrap
    }
    MouseArea {
        id: area
        anchors.fill: parent
        anchors.margins: -4
        onClicked: pill.clicked()
    }
}
