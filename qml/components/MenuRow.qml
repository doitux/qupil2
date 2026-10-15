// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Row in a grouped list (More page): icon tile, title, value, chevron.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

AbstractButton {
    id: row

    property string iconName
    property var tile: Theme.tile.gray
    property string value
    property bool divider: false
    property bool chevron: true

    Layout.fillWidth: true
    implicitHeight: 58
    focusPolicy: Qt.StrongFocus
    hoverEnabled: true
    Accessible.role: Accessible.Button
    Accessible.name: text + (value ? ", " + value : "")

    background: Rectangle {
        color: row.down ? Theme.surfaceAlt : row.hovered ? Theme.surfaceSunken : "transparent"
        border.width: row.visualFocus ? 2 : 0
        border.color: Theme.accentText
        Rectangle {
            visible: row.divider
            x: 62
            width: parent.width - 62
            height: 1
            color: Theme.divider
        }
    }
    contentItem: RowLayout {
        spacing: 12
        Rectangle {
            implicitWidth: 34
            implicitHeight: 34
            radius: 10
            color: row.tile.bg
            Icon {
                anchors.centerIn: parent
                width: 20
                height: 20
                name: row.iconName
                color: row.tile.fg
            }
        }
        Text {
            Layout.fillWidth: true
            elide: Text.ElideRight
            text: row.text
            color: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 15
            font.weight: Font.DemiBold
        }
        Text {
            visible: row.value !== ""
            text: row.value
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 14
        }
        Icon {
            visible: row.chevron
            Layout.preferredWidth: 18
            Layout.preferredHeight: 18
            name: "chevron"
            color: Theme.icon
        }
    }
    leftPadding: 14
    rightPadding: 12
}
