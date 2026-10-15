// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Reminder shown when Qupil starts, at every lesson or at the lessons of one student.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: editor

    property string reminderId
    property var draft: ({})
    readonly property var students: { if (Data.revision < 0) return undefined; return Data.students("") }

    function edit(reminder, student) {
        reminderId = reminder
        draft = reminder ? Data.get("reminder", reminder)
                         : { text: "", mode: student ? 2 : 0, student_id: student || "", sound: 0 }
        open()
    }
    function set(key, value) {
        const d = Object.assign({}, draft)
        d[key] = value
        draft = d
    }

    title: qsTrId("reminder.edit.title")
    canAccept: (draft.text || "").trim() !== "" && (draft.mode !== 2 || (draft.student_id || "") !== "")
    onAccepted: {
        const fields = { text: draft.text.trim(), mode: draft.mode, student_id: draft.mode === 2 ? draft.student_id : "",
                         sound: draft.sound ? 1 : 0 }
        if (reminderId)
            Data.update("reminder", reminderId, fields)
        else
            Data.create("reminder", fields)
    }

    InputField {
        label: qsTrId("reminder.label.text")
        multiline: true
        text: editor.draft.text || ""
        onEdited: (value) => editor.set("text", value)
    }
    ColumnLayout {
        Layout.fillWidth: true
        spacing: 6
        FieldLabel { label: qsTrId("reminder.label.mode") }
        Repeater {
            model: [0, 1, 2]
            delegate: RadioDelegate {
                required property int modelData
                Layout.fillWidth: true
                text: modelData === 0 ? qsTrId("reminder.mode.startup_long")
                    : modelData === 1 ? qsTrId("reminder.mode.every_lesson_long")
                    : qsTrId("reminder.mode.student_long")
                checked: (editor.draft.mode || 0) === modelData
                font.family: Theme.fontFamily
                font.pixelSize: 15
                onClicked: editor.set("mode", modelData)
            }
        }
    }
    ChoiceField {
        visible: editor.draft.mode === 2
        label: qsTrId("common.label.student")
        options: editor.students.map(s => s.listName)
        value: (editor.students.find(s => s.id === editor.draft.student_id) || {}).listName || ""
        onPicked: (value) => {
            const s = editor.students.find(x => x.listName === value)
            editor.set("student_id", s ? s.id : "")
        }
    }
    SwitchRow {
        text: qsTrId("reminder.play_sound")
        checked: editor.draft.sound === 1
        onToggled: editor.set("sound", checked ? 1 : 0)
    }
    ActionButton {
        visible: editor.reminderId !== ""
        kind: "quiet"
        iconName: "trash"
        text: qsTrId("common.delete")
        onClicked: {
            Data.remove("reminder", editor.reminderId)
            editor.close()
        }
    }
}
