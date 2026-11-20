// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
// Round icon button for the top bar
import QtQuick
import QupilStudent

Rectangle {
    id: button
    property string icon
    property string label
    property color iconColor: Theme.text
    signal clicked()

    width: Theme.touchTarget
    height: Theme.touchTarget
    radius: width / 2
    color: area.pressed ? Theme.border : "transparent"
    Accessible.role: Accessible.Button
    Accessible.name: label

    Icon { anchors.centerIn: parent; name: button.icon; color: button.iconColor; width: 22; height: 22 }
    MouseArea { id: area; anchors.fill: parent; onClicked: button.clicked() }
}
