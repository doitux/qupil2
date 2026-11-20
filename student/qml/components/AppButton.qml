// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
// Full-width button: "primary" in plum, otherwise quiet
import QtQuick
import QupilStudent

Rectangle {
    id: button
    property string text
    property string icon
    property bool primary: false
    property bool enabled: true
    signal clicked()

    implicitHeight: 50
    radius: 14
    opacity: enabled ? 1 : 0.45
    color: primary ? (area.pressed ? Qt.darker(Theme.accent, 1.15) : Theme.accent)
                   : (area.pressed ? Theme.border : Theme.surface)
    border.width: primary ? 0 : 1
    border.color: Theme.border
    Accessible.role: Accessible.Button
    Accessible.name: text

    Row {
        anchors.centerIn: parent
        spacing: 8
        Icon {
            visible: button.icon !== ""
            name: button.icon
            width: 20; height: 20
            anchors.verticalCenter: parent.verticalCenter
            color: button.primary ? "white" : Theme.accentText
            strokeWidth: 2.1
        }
        T {
            text: button.text
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 16
            font.weight: Font.Bold
            color: button.primary ? "white" : Theme.text
            wrapMode: Text.NoWrap
        }
    }
    MouseArea {
        id: area
        anchors.fill: parent
        enabled: button.enabled
        onClicked: button.clicked()
    }
}
