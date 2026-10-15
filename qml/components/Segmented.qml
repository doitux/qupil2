// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Segmented control. options are values (static, so a language switch only
// re-evaluates labelFor()); value is the selected one.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: control

    property var options: []
    property var value
    property var labelFor: function(v) { return String(v) }
    property int segmentHeight: 40

    signal activated(var value)

    implicitHeight: segmentHeight + 8
    implicitWidth: 300
    Layout.fillWidth: true
    radius: 12
    color: Theme.segment

    RowLayout {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 4
        Repeater {
            model: control.options
            delegate: AbstractButton {
                id: segment
                required property var modelData
                readonly property bool selected: control.value === modelData
                Layout.fillWidth: true
                Layout.fillHeight: true
                checkable: true
                checked: selected
                text: control.labelFor(modelData)
                focusPolicy: Qt.StrongFocus
                Accessible.role: Accessible.RadioButton
                Accessible.checked: selected
                onClicked: {
                    control.value = modelData
                    control.activated(modelData)
                }
                background: Rectangle {
                    radius: 9
                    color: segment.selected ? Theme.segmentActive : "transparent"
                    border.width: segment.visualFocus ? 2 : 0
                    border.color: Theme.accentText
                }
                contentItem: Text {
                    text: segment.text
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    fontSizeMode: Text.HorizontalFit
                    minimumPixelSize: 10
                    color: segment.selected ? Theme.accentText : Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 14
                    font.weight: Font.Bold
                }
            }
        }
    }
}
