// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Row of an AppMenu: optional icon, text, a check mark when checked, a chevron
// for a submenu. destructive: red, for deleting.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

MenuItem {
    id: item

    property string iconName: subMenu && subMenu.iconName ? subMenu.iconName : ""
    property bool destructive: false
    readonly property color tint: !enabled ? Theme.textMuted : destructive ? Theme.danger : Theme.text

    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    implicitHeight: Math.max(Theme.touchTarget, contentItem.implicitHeight + topPadding + bottomPadding)
    leftPadding: 12
    rightPadding: 12
    topPadding: 6
    bottomPadding: 6
    spacing: 12
    hoverEnabled: true
    font.family: Theme.fontFamily
    font.pixelSize: 15
    font.weight: Font.Medium
    Accessible.name: text

    indicator: null
    arrow: null

    background: Rectangle {
        radius: Theme.radiusSmall
        color: item.destructive ? Theme.danger : Theme.text
        opacity: item.down ? 0.14 : item.highlighted ? 0.07 : 0
        Behavior on opacity { NumberAnimation { duration: 90 } }
    }

    contentItem: RowLayout {
        spacing: item.spacing
        Icon {
            visible: item.iconName !== ""
            Layout.preferredWidth: 20
            Layout.preferredHeight: 20
            name: item.iconName
            color: item.destructive ? Theme.danger : item.enabled ? Theme.icon : Theme.textMuted
        }
        Text {
            Layout.fillWidth: true
            text: item.text
            font: item.font
            color: item.tint
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        Icon {
            visible: item.checkable && item.checked
            Layout.preferredWidth: 20
            Layout.preferredHeight: 20
            name: "check"
            color: Theme.accentText
            strokeWidth: 2.2
        }
        Icon {
            visible: item.subMenu !== null
            Layout.preferredWidth: 18
            Layout.preferredHeight: 18
            name: "chevron"
            color: Theme.icon
        }
    }
}
