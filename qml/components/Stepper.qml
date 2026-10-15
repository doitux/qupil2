// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Number with − and + buttons (durations in minutes, counts).

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: stepper

    property string label
    property int value: 0
    property int from: 0
    property int to: 99
    property int step: 1
    property string suffix

    signal picked(int value)

    spacing: 6
    Layout.fillWidth: true

    FieldLabel {
        visible: stepper.label !== ""
        label: stepper.label
    }
    Rectangle {
        Layout.fillWidth: true
        implicitHeight: 48
        radius: 12
        color: Theme.surfaceAlt
        RowLayout {
            anchors.fill: parent
            anchors.margins: 4
            spacing: 4
            StepButton {
                text: "−"
                enabled: stepper.value > stepper.from
                onClicked: stepper.picked(Math.max(stepper.from, stepper.value - stepper.step))
                Accessible.name: "−"
            }
            Text {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: stepper.value + (stepper.suffix ? " " + stepper.suffix : "")
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 16
                font.weight: Font.Bold
            }
            StepButton {
                text: "+"
                enabled: stepper.value < stepper.to
                onClicked: stepper.picked(Math.min(stepper.to, stepper.value + stepper.step))
                Accessible.name: "+"
            }
        }
    }

    component StepButton: AbstractButton {
        id: button
        Layout.preferredWidth: 48
        Layout.fillHeight: true
        focusPolicy: Qt.StrongFocus
        autoRepeat: true
        background: Rectangle {
            radius: 10
            color: Theme.segmentActive
            opacity: button.enabled ? 1 : 0.4
            border.width: button.visualFocus ? 2 : 0
            border.color: Theme.accentText
        }
        contentItem: Text {
            text: button.text
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 20
            font.weight: Font.Bold
        }
    }
}
