// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Hint for a control while a mouse rests on it (e.g. the name of an icon button),
// in Qupil's look. Declare it inside the control:  Tip { text: button.text }
//
// Only for a mouse or touchpad: on touch screens a tap would leave it standing,
// so iOS and Android never show it. It hides once the control is pressed until
// the pointer leaves. Screen readers use the control's Accessible.name instead.

import QtQuick
import QtQuick.Controls

Item {
    id: tip

    property string text
    property int delay: 600
    //! Below the control (most buttons sit at the top of a page), otherwise above
    property bool below: true

    readonly property bool showing: popup.opened
    readonly property bool targetPressed: parent !== null && parent.pressed === true
    property bool spent: false

    anchors.fill: parent
    onTargetPressedChanged: if (targetPressed) spent = true

    HoverHandler {
        id: hover
        enabled: Theme.hoverHints
        acceptedDevices: PointerDevice.Mouse | PointerDevice.TouchPad
        onHoveredChanged: if (!hovered) tip.spent = false
    }

    ToolTip {
        id: popup
        visible: hover.hovered && !tip.spent && tip.text !== ""
        text: tip.text
        delay: tip.delay
        timeout: 6000
        x: Math.round((tip.width - implicitWidth) / 2)
        y: tip.below ? tip.height + 6 : -implicitHeight - 6
        margins: 6
        leftPadding: 10
        rightPadding: 10
        topPadding: 6
        bottomPadding: 6
        font.family: Theme.fontFamily
        font.pixelSize: 13
        font.weight: Font.Medium
        contentItem: Text {
            text: popup.text
            font: popup.font
            color: Theme.textOnInverse
            wrapMode: Text.Wrap
        }
        background: Rectangle {
            radius: 8
            color: Theme.inverse
        }
        enter: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 120; easing.type: Easing.OutCubic }
        }
        exit: Transition {
            NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 80 }
        }
    }
}
