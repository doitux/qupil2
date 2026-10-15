// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Entry of the main navigation: icon above the label in the phone tab bar,
// icon left of the label in the tablet/desktop sidebar.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

AbstractButton {
    id: control

    property string iconName
    property bool selected: false
    property bool vertical: true

    implicitHeight: vertical ? 56 : 46
    implicitWidth: vertical ? 64 : 188
    hoverEnabled: true
    focusPolicy: Qt.TabFocus

    Accessible.role: Accessible.PageTab
    Accessible.name: text
    Accessible.selected: selected

    readonly property color tint: selected ? Theme.accentText : (vertical ? Theme.icon : Theme.textSecondary)

    background: Rectangle {
        radius: control.vertical ? 0 : Theme.radiusSmall + 2
        color: !control.vertical && control.selected ? Theme.accentSoft
             : !control.vertical && control.hovered ? Theme.surfaceAlt
             : "transparent"
        border.width: control.visualFocus ? 2 : 0
        border.color: Theme.accent
    }

    contentItem: Item {
        ColumnLayout {
            visible: control.vertical
            anchors.centerIn: parent
            spacing: 3
            Icon {
                Layout.alignment: Qt.AlignHCenter
                name: control.iconName
                color: control.tint
                strokeWidth: control.iconName === "more" ? 3 : (control.selected ? 2 : 1.8)
            }
            Text {
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: control.width - 4
                horizontalAlignment: Text.AlignHCenter
                text: control.text
                color: control.tint
                // Long labels ("Veranstaltungen", "Emploi du temps") shrink a little instead of being cut
                fontSizeMode: Text.HorizontalFit
                minimumPixelSize: 9
                elide: Text.ElideRight
                font.family: Theme.fontFamily
                font.pixelSize: 11
                font.weight: control.selected ? Font.Bold : Font.DemiBold
            }
        }
        RowLayout {
            visible: !control.vertical
            anchors.fill: parent
            anchors.leftMargin: 12
            anchors.rightMargin: 12
            spacing: 12
            Icon {
                name: control.iconName
                color: control.tint
                strokeWidth: control.iconName === "more" ? 3 : (control.selected ? 2 : 1.8)
            }
            Text {
                Layout.fillWidth: true
                text: control.text
                color: control.tint
                elide: Text.ElideRight
                font.family: Theme.fontFamily
                font.pixelSize: 15
                font.weight: control.selected ? Font.Bold : Font.DemiBold
            }
        }
    }
}
