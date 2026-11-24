// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "Try the new idea": a folded group in a card ("Paused (2)", "Done (14)"); a tap shows
// what is in it.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

AbstractButton {
    id: fold

    property string iconName
    property color tint: Theme.textMuted
    property bool open: false

    Layout.fillWidth: true
    implicitHeight: 36
    focusPolicy: Qt.StrongFocus
    Accessible.role: Accessible.Button
    Accessible.name: text

    contentItem: RowLayout {
        spacing: 8
        Icon {
            Layout.preferredWidth: 18
            Layout.preferredHeight: 18
            name: fold.iconName
            color: fold.tint
            strokeWidth: 2.3
        }
        Text {
            Layout.fillWidth: true
            text: fold.text
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 15
            font.weight: Font.DemiBold
            font.underline: fold.visualFocus
        }
        Icon {
            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
            name: "chevronDown"
            color: Theme.icon
            rotation: fold.open ? 180 : 0
        }
    }
    background: Item {}
}
