// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "Try the new idea": a small text button within a card ("Pause", "Take up again").

import QtQuick
import QtQuick.Controls
import Qupil

AbstractButton {
    id: link

    property string iconName

    implicitHeight: 34
    implicitWidth: row.implicitWidth + 4
    // Also used in a window with fields: a tap must not close the on-screen keyboard
    focusPolicy: Qt.TabFocus
    Accessible.role: Accessible.Button
    Accessible.name: text

    contentItem: Item {
        Row {
            id: row
            anchors.verticalCenter: parent.verticalCenter
            spacing: 6
            Icon {
                visible: link.iconName !== ""
                anchors.verticalCenter: parent.verticalCenter
                width: 15
                height: 15
                name: link.iconName
                color: Theme.accentText
                strokeWidth: 2.2
            }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: link.text
                color: Theme.accentText
                font.family: Theme.fontFamily
                font.pixelSize: 14
                font.weight: Font.DemiBold
                font.underline: link.visualFocus
                opacity: link.down ? 0.6 : 1
            }
        }
    }
    background: Item {}
}
