// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// A row in a list: avatar or icon, title, subtitle, a value on the right and
// a chevron. Extra items (buttons) can be placed before the chevron.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

AbstractButton {
    id: row

    property string title
    property string subtitle
    property string trailing
    property string iconName
    property var tile: null              //!< icon in a coloured tile (Theme.tile.*)
    property string avatar               //!< initials: shows an avatar instead of the icon
    property string avatarSeed
    property bool chevron: true
    property bool card: true             //!< own rounded background (false: inside a group)
    property bool divider: false         //!< line above (inside a group)
    property color subtitleColor: Theme.textMuted
    default property alias extra: extraItems.data

    Layout.fillWidth: true
    implicitHeight: Math.max(58, implicitContentHeight + topPadding + bottomPadding)
    topPadding: 11
    bottomPadding: 11
    focusPolicy: Qt.StrongFocus
    hoverEnabled: true
    Accessible.role: Accessible.Button
    Accessible.name: title + (subtitle ? ", " + subtitle : "") + (trailing ? ", " + trailing : "")

    background: Rectangle {
        radius: row.card ? Theme.radius : 0
        color: row.down ? Theme.surfaceAlt : row.hovered && !row.card ? Theme.surfaceSunken : row.card ? Theme.surface : "transparent"
        border.width: row.visualFocus ? 2 : 0
        border.color: Theme.accentText
        Rectangle {
            visible: row.divider
            x: 14
            width: parent.width - 14
            height: 1
            color: Theme.divider
        }
    }

    contentItem: RowLayout {
        id: rowContent
        spacing: 12

        Avatar {
            visible: row.avatar !== ""
            size: 38
            text: row.avatar
            seed: row.avatarSeed || row.title
        }
        Rectangle {
            visible: row.avatar === "" && row.iconName !== "" && row.tile !== null
            implicitWidth: 34
            implicitHeight: 34
            radius: 10
            color: row.tile ? row.tile.bg : "transparent"
            Icon {
                anchors.centerIn: parent
                width: 20
                height: 20
                name: row.iconName
                color: row.tile ? row.tile.fg : Theme.icon
            }
        }
        Icon {
            visible: row.avatar === "" && row.iconName !== "" && row.tile === null
            Layout.preferredWidth: 22
            Layout.preferredHeight: 22
            name: row.iconName
            color: Theme.icon
        }
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 1
            Text {
                Layout.fillWidth: true
                elide: Text.ElideRight
                text: row.title
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 15
                font.weight: Font.Bold
            }
            Text {
                visible: row.subtitle !== ""
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                maximumLineCount: 2
                elide: Text.ElideRight
                text: row.subtitle
                color: row.subtitleColor
                font.family: Theme.fontFamily
                font.pixelSize: 13
            }
        }
        Text {
            visible: row.trailing !== ""
            text: row.trailing
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 14
        }
        RowLayout {
            id: extraItems
            spacing: 4
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
