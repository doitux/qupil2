// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Floating "+ Note" / "+ Piece" button in the lower right corner.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

AbstractButton {
    id: fab

    property string iconName: "plus"
    //! Outlined, for a second button above the main one
    property bool outlined: false

    implicitHeight: 56
    implicitWidth: row.implicitWidth + 40
    focusPolicy: Qt.StrongFocus
    Accessible.role: Accessible.Button
    Accessible.name: text

    background: Item {
        // Soft shadow
        Rectangle {
            anchors.fill: parent
            anchors.topMargin: 6
            anchors.leftMargin: 2
            anchors.rightMargin: 2
            anchors.bottomMargin: -4
            radius: height / 2
            color: Theme.shadow
        }
        Rectangle {
            anchors.fill: parent
            radius: height / 2
            color: fab.outlined ? Theme.surface : Theme.accent
            border.width: fab.outlined || fab.visualFocus ? 2 : 0
            border.color: fab.outlined && !fab.visualFocus ? Theme.accentText : Theme.lime
            Rectangle {
                anchors.fill: parent
                radius: parent.radius
                color: Theme.text
                opacity: fab.down ? 0.15 : 0
            }
        }
    }
    contentItem: Item {
        RowLayout {
            id: row
            anchors.centerIn: parent
            spacing: 8
            Icon {
                Layout.preferredWidth: 22
                Layout.preferredHeight: 22
                name: fab.iconName
                color: fab.outlined ? Theme.accentText : Theme.textOnAccent
                strokeWidth: 2.4
            }
            Text {
                text: fab.text
                color: fab.outlined ? Theme.accentText : Theme.textOnAccent
                font.family: Theme.fontFamily
                font.pixelSize: 16
                font.weight: Font.ExtraBold
            }
        }
    }
}
