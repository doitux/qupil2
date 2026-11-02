// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "There is more text": a small pill with lines, how many, and an arrow that turns
// when the block is open. Shown next to the heading of a block.

import QtQuick
import Qupil

Rectangle {
    id: badge

    property int count
    property bool open
    property color tint: Theme.textMuted

    implicitWidth: row.implicitWidth + 14
    implicitHeight: 24
    radius: 12
    color: Theme.surfaceAlt

    Row {
        id: row
        anchors.centerIn: parent
        spacing: 3
        Icon {
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 15
            name: "lines"
            color: badge.tint
            strokeWidth: 2.2
        }
        Text {
            anchors.verticalCenter: parent.verticalCenter
            text: "+" + badge.count
            color: badge.tint
            font.family: Theme.fontFamily
            font.pixelSize: 12
            font.weight: Font.Bold
        }
        Icon {
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 15
            name: "chevronDown"
            color: badge.tint
            strokeWidth: 2.4
            rotation: badge.open ? 180 : 0
            Behavior on rotation { NumberAnimation { duration: 160 } }
        }
    }
}
