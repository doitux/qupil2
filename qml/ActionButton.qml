// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Button in the Qupil look.
//   kind: "primary" (filled accent), "secondary" (tinted), "danger", "quiet" (text only)
// Usage: ActionButton { kind: "primary"; iconName: "import"; text: qsTrId("…") }

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

AbstractButton {
    id: control

    property string kind: "secondary"
    property string iconName: ""

    readonly property color fill: kind === "primary" ? Theme.accent
                                : kind === "danger" ? Theme.dangerFill
                                : kind === "quiet" ? "transparent"
                                : Theme.accentSoft
    readonly property color ink: kind === "primary" ? Theme.textOnAccent
                               : kind === "danger" ? Theme.textOnDanger
                               : Theme.accentText

    implicitHeight: Theme.touchTarget
    implicitWidth: Math.max(implicitHeight, row.implicitWidth + 32)
    focusPolicy: Qt.StrongFocus
    font.family: Theme.fontFamily
    font.pixelSize: 15
    font.weight: Font.Bold
    Accessible.role: Accessible.Button
    Accessible.name: text

    background: Rectangle {
        radius: Theme.radiusSmall + 2
        color: control.fill
        opacity: control.enabled ? 1 : 0.45
        border.width: control.visualFocus ? 2 : 0
        border.color: Theme.accentText

        // Hover / press feedback that works on every fill colour
        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: Theme.text
            opacity: control.down ? 0.14 : control.hovered ? 0.06 : 0
        }
    }

    contentItem: Item {
        implicitWidth: row.implicitWidth
        implicitHeight: row.implicitHeight

        RowLayout {
            id: row
            anchors.centerIn: parent
            width: Math.min(implicitWidth, control.availableWidth)
            spacing: 8

            Icon {
                visible: control.iconName !== ""
                Layout.preferredWidth: 18
                Layout.preferredHeight: 18
                name: control.iconName
                color: control.ink
                opacity: control.enabled ? 1 : 0.6
            }
            Text {
                Layout.fillWidth: true
                text: control.text
                color: control.ink
                opacity: control.enabled ? 1 : 0.6
                font: control.font
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
