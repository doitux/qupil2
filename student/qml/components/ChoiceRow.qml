// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
// "Practised: a lot · some · hardly" – values 3, 2, 1 from left to right, 0 = nothing chosen
import QtQuick
import QupilStudent

Row {
    id: row
    property string label
    property var choices: []          // texts for 3, 2, 1
    property int value: 0
    property color selectedColor: Theme.inverse
    property color selectedText: Theme.textOnInverse
    signal picked(int value)

    spacing: 6
    T {
        width: 58
        anchors.verticalCenter: parent.verticalCenter
        text: row.label
        font.pixelSize: 12
        font.weight: Font.Bold
        color: Theme.textMuted
        wrapMode: Text.NoWrap
        elide: Text.ElideRight
    }
    Repeater {
        model: row.choices
        Pill {
            objectName: row.objectName + "-" + choice
            required property int index
            required property string modelData
            readonly property int choice: 3 - index
            text: modelData
            selected: row.value === choice
            selectedColor: row.selectedColor
            selectedText: row.selectedText
            onClicked: row.picked(selected ? 0 : choice)
        }
    }
}
