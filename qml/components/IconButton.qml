// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Round icon-only button (44 px touch target). Set Accessible.name / text for screen readers.

import QtQuick
import QtQuick.Controls

AbstractButton {
    id: button

    property string iconName
    property bool filled: false
    property color tint: filled ? Theme.textOnAccent : Theme.text
    property color fill: filled ? Theme.accent : Theme.surface
    property int size: Theme.touchTarget

    implicitWidth: size
    implicitHeight: size
    focusPolicy: Qt.StrongFocus
    hoverEnabled: true
    Accessible.role: Accessible.Button
    Accessible.name: text

    Tip {
        text: button.text
    }

    background: Rectangle {
        radius: width / 2
        color: button.fill
        border.width: button.visualFocus ? 2 : 0
        border.color: Theme.accentText
        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: Theme.text
            opacity: button.down ? 0.12 : button.hovered ? 0.05 : 0
        }
    }
    contentItem: Item {
        Icon {
            anchors.centerIn: parent
            width: Math.round(button.size * 0.5)
            height: width
            name: button.iconName
            color: button.tint
            strokeWidth: button.iconName === "more" ? 2.4 : 2
        }
    }
}
