// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Row of an AppMenu: optional icon, text (with a smaller line below: subtitle), a check
// mark when checked (toggle: a switch instead), a chevron for a submenu. destructive:
// red, for deleting.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

MenuItem {
    id: item

    property string iconName: subMenu && subMenu.iconName ? subMenu.iconName : ""
    property bool destructive: false
    //! Second line in small grey (day and time, an explanation); wraps
    property string subtitle
    //! A switch on the right instead of the check mark (for an on/off setting)
    property bool toggle: false
    readonly property color tint: !enabled ? Theme.textMuted : destructive ? Theme.danger : Theme.text

    implicitWidth: contentItem.implicitWidth + leftPadding + rightPadding
    // Hidden rows take no room
    implicitHeight: visible ? Math.max(Theme.touchTarget, contentItem.implicitHeight + topPadding + bottomPadding) : 0
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
    Accessible.description: subtitle

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
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 1
            Text {
                Layout.fillWidth: true
                text: item.text
                font: item.font
                color: item.tint
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
            }
            Text {
                visible: item.subtitle !== ""
                Layout.fillWidth: true
                // Wraps within the menu; does not make the menu wider than the title
                Layout.preferredWidth: Math.min(implicitWidth, 260)
                text: item.subtitle
                wrapMode: Text.WordWrap
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 13
            }
        }
        Rectangle {
            visible: item.toggle
            implicitWidth: 40
            implicitHeight: 24
            radius: height / 2
            color: item.checked ? Theme.switchOn : Theme.switchOff
            Rectangle {
                width: 18
                height: 18
                radius: 9
                y: 3
                x: item.checked ? parent.width - width - 3 : 3
                color: "#FFFFFF"
                Behavior on x { NumberAnimation { duration: 120 } }
            }
        }
        Icon {
            visible: item.checkable && item.checked && !item.toggle
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
