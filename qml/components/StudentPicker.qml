// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// The students of a lesson: the chosen ones as a short list (× takes one out),
// all others in a drop-down with a search field and check marks. Several can be
// ticked at once; for an individual lesson (single) a tap picks one and closes it.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: picker

    property string label
    property var students: []     // { id, listName, instrument, … } of all students
    property var selected: []     // ids
    property bool single: false

    //! A student was ticked or unticked (the owner changes "selected")
    signal toggled(string id)

    property string query
    readonly property var chosen: students.filter(s => selected.indexOf(s.id) >= 0)
    readonly property var found: {
        const q = query.trim().toLowerCase()
        return q === "" ? students
                        : students.filter(s => (s.listName + " " + (s.instrument || "")).toLowerCase().indexOf(q) >= 0)
    }

    function openList() {
        query = ""
        // Below the field, or above it where the window ends first
        const rows = Math.min(students.length * 46, 320)
        const height = rows + (students.length > 8 ? 56 : 0) + (single ? 0 : 56) + 2 * popup.padding + 16
        const win = Window.window
        const below = openButton.mapToItem(null, 0, openButton.height).y
        if (win && below + 6 + height > win.height - 12 && below - openButton.height - 6 - height >= 12)
            popup.y = openButton.y - 6 - height
        else
            popup.y = openButton.y + openButton.height + 6
        popup.open()
    }
    function closeList() {
        popup.close()
    }
    function describe(s) {
        return s.listName + (s.instrument ? " · " + s.instrument : "")
    }

    spacing: 8
    Layout.fillWidth: true

    FieldLabel {
        label: picker.label + " · " + picker.selected.length
    }

    // The chosen ones
    Rectangle {
        visible: picker.chosen.length > 0
        Layout.fillWidth: true
        implicitHeight: chosenColumn.implicitHeight
        radius: 14
        color: Theme.surfaceAlt
        ColumnLayout {
            id: chosenColumn
            width: parent.width
            spacing: 0
            Repeater {
                model: picker.chosen
                delegate: RowLayout {
                    required property var modelData
                    required property int index
                    Layout.fillWidth: true
                    Layout.leftMargin: 14
                    Layout.rightMargin: 4
                    Layout.preferredHeight: 46
                    spacing: 8
                    Text {
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: picker.describe(modelData)
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                    }
                    IconButton {
                        size: 36
                        iconName: "close"
                        fill: "transparent"
                        tint: Theme.textMuted
                        text: qsTrId("common.remove") + " " + modelData.listName
                        onClicked: picker.toggled(modelData.id)
                    }
                }
            }
        }
    }

    // Drop-down
    AbstractButton {
        id: openButton
        Layout.fillWidth: true
        implicitHeight: 48
        focusPolicy: Qt.StrongFocus
        Accessible.role: Accessible.ComboBox
        Accessible.name: text
        text: picker.single && picker.chosen.length > 0 ? qsTrId("lesson.pick_other_student")
                                                        : picker.single ? qsTrId("lesson.pick_student")
                                                                        : qsTrId("lesson.pick_students")
        onClicked: picker.openList()
        background: Rectangle {
            radius: 12
            color: Theme.surface
            border.width: openButton.visualFocus || popup.opened ? 2 : 1
            border.color: openButton.visualFocus || popup.opened ? Theme.accent : Theme.border
        }
        contentItem: RowLayout {
            spacing: 10
            Icon {
                Layout.preferredWidth: 20
                Layout.preferredHeight: 20
                name: "userPlus"
                color: Theme.accentText
            }
            Text {
                Layout.fillWidth: true
                elide: Text.ElideRight
                text: openButton.text
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 16
            }
            Icon {
                Layout.preferredWidth: 18
                Layout.preferredHeight: 18
                name: "chevronDown"
                color: Theme.icon
            }
        }
        leftPadding: 14
        rightPadding: 14
    }

    Popup {
        id: popup
        width: picker.width
        margins: 12
        padding: 10
        modal: false
        focus: true
        background: Rectangle {
            radius: 16
            color: Theme.dark ? Theme.surfaceAlt : Theme.surface
            border.width: 1
            border.color: Theme.border
            Rectangle {
                z: -1
                anchors.fill: parent
                anchors.topMargin: 6
                anchors.bottomMargin: -6
                radius: parent.radius
                color: Theme.shadow
                opacity: 0.5
            }
        }
        enter: Transition {
            NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 120 }
        }
        contentItem: ColumnLayout {
            spacing: 8
            InputField {
                visible: picker.students.length > 8
                placeholder: qsTrId("student.search")
                text: picker.query
                onEdited: (value) => picker.query = value
            }
            ListView {
                id: list
                Layout.fillWidth: true
                Layout.preferredHeight: Math.min(contentHeight, 320)
                clip: true
                boundsBehavior: Flickable.StopAtBounds
                model: picker.found
                ScrollIndicator.vertical: ScrollIndicator {}
                delegate: AbstractButton {
                    id: row
                    required property var modelData
                    readonly property bool chosen: picker.selected.indexOf(modelData.id) >= 0
                    width: ListView.view.width
                    implicitHeight: 46
                    hoverEnabled: true
                    Accessible.role: picker.single ? Accessible.RadioButton : Accessible.CheckBox
                    Accessible.checked: chosen
                    Accessible.name: picker.describe(modelData)
                    onClicked: {
                        if (!(picker.single && chosen))
                            picker.toggled(modelData.id)
                        if (picker.single)
                            popup.close()
                    }
                    background: Rectangle {
                        radius: 10
                        color: Theme.text
                        opacity: row.down ? 0.12 : row.hovered ? 0.05 : 0
                    }
                    contentItem: RowLayout {
                        spacing: 10
                        Text {
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            text: picker.describe(row.modelData)
                            color: Theme.text
                            font.family: Theme.fontFamily
                            font.pixelSize: 15
                            font.weight: row.chosen ? Font.Bold : Font.Normal
                        }
                        Rectangle {
                            implicitWidth: 24
                            implicitHeight: 24
                            radius: picker.single ? 12 : 7
                            color: row.chosen ? Theme.accent : "transparent"
                            border.width: row.chosen ? 0 : 2
                            border.color: Theme.border
                            Icon {
                                anchors.centerIn: parent
                                visible: row.chosen
                                width: 16
                                height: 16
                                name: "check"
                                color: Theme.textOnAccent
                                strokeWidth: 2.6
                            }
                        }
                    }
                    leftPadding: 10
                    rightPadding: 10
                }
            }
            Text {
                visible: picker.found.length === 0
                Layout.fillWidth: true
                Layout.margins: 10
                wrapMode: Text.WordWrap
                text: qsTrId("students.no_match")
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 14
            }
            ActionButton {
                visible: !picker.single
                Layout.fillWidth: true
                kind: "primary"
                text: qsTrId("common.done")
                onClicked: popup.close()
            }
        }
    }
}
