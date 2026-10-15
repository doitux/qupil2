// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Editable list of short texts (lesson locations, styles, sizes) as removable chips.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: editor

    property string label
    property var items: []
    property string entry

    signal changed(var items)

    function add() {
        const v = entry.trim()
        if (v === "" || items.indexOf(v) >= 0)
            return
        changed(items.concat([v]))
        entry = ""
    }

    spacing: 8
    Layout.fillWidth: true

    FieldLabel {
        visible: editor.label !== ""
        label: editor.label
    }
    Flow {
        Layout.fillWidth: true
        spacing: 6
        Repeater {
            model: editor.items
            delegate: AbstractButton {
                id: chip
                required property string modelData
                required property int index
                implicitHeight: 34
                implicitWidth: chipRow.implicitWidth + 24
                focusPolicy: Qt.StrongFocus
                Accessible.role: Accessible.Button
                Accessible.name: qsTrId("common.remove") + " " + modelData
                onClicked: {
                    const list = editor.items.slice()
                    list.splice(index, 1)
                    editor.changed(list)
                }
                background: Rectangle {
                    radius: height / 2
                    color: Theme.surfaceAlt
                    border.width: chip.visualFocus ? 2 : 1
                    border.color: chip.visualFocus ? Theme.accent : Theme.border
                }
                contentItem: Item {
                    RowLayout {
                        id: chipRow
                        anchors.centerIn: parent
                        spacing: 6
                        Text {
                            text: chip.modelData
                            color: Theme.text
                            font.family: Theme.fontFamily
                            font.pixelSize: 14
                            font.weight: Font.DemiBold
                        }
                        Icon {
                            Layout.preferredWidth: 14
                            Layout.preferredHeight: 14
                            name: "close"
                            color: Theme.textMuted
                            strokeWidth: 2.4
                        }
                    }
                }
            }
        }
    }
    RowLayout {
        Layout.fillWidth: true
        spacing: 8
        InputField {
            Layout.fillWidth: true
            placeholder: qsTrId("common.add_ellipsis")
            text: editor.entry
            onEdited: (value) => editor.entry = value
            onAccepted: editor.add()
        }
        ActionButton {
            Layout.alignment: Qt.AlignBottom
            iconName: "plus"
            text: qsTrId("common.add")
            enabled: editor.entry.trim() !== ""
            onClicked: editor.add()
        }
    }
}
