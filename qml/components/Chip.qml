// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Pill button. kind "tab": dark when selected (sections of a student);
// kind "filter": tinted with an outline when selected (status filters).

import QtQuick
import QtQuick.Controls

AbstractButton {
    id: chip

    property bool selected: false
    property string kind: "tab"

    implicitHeight: kind === "tab" ? 38 : 34
    implicitWidth: label.implicitWidth + (kind === "tab" ? 32 : 24)
    focusPolicy: Qt.StrongFocus
    hoverEnabled: true
    Accessible.role: kind === "tab" ? Accessible.PageTab : Accessible.CheckBox
    Accessible.checked: selected
    Accessible.name: text

    background: Rectangle {
        radius: height / 2
        color: chip.kind === "tab" ? (chip.selected ? Theme.inverse : Theme.surface)
                                   : (chip.selected ? Theme.accentSoft : Theme.surface)
        border.width: chip.kind === "filter" ? 1.5 : (chip.visualFocus ? 2 : 0)
        border.color: chip.kind === "filter" ? (chip.selected ? Theme.accent : Theme.border) : Theme.accentText
        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: Theme.text
            opacity: chip.down ? 0.1 : chip.hovered && !chip.selected ? 0.04 : 0
        }
    }

    contentItem: Text {
        id: label
        text: chip.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        color: chip.kind === "tab" ? (chip.selected ? Theme.textOnInverse : Theme.text)
                                   : (chip.selected ? Theme.accentText : Theme.text)
        font.family: Theme.fontFamily
        font.pixelSize: chip.kind === "tab" ? 14 : 13
        font.weight: chip.selected ? Font.Bold : Font.DemiBold
    }
}
