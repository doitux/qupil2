// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Reminder shown when Qupil starts, at every lesson, at the lessons of one student or
// only at one lesson (e.g. the string quartet, or one student's individual lesson).

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: editor

    property string reminderId
    property var draft: ({})
    readonly property var students: { if (Data.revision < 0) return undefined; return Data.students("") }
    readonly property var lessons: { if (Data.revision < 0) return undefined; return Data.reminderLessons() }

    function edit(reminder, student, lesson) {
        reminderId = reminder
        draft = reminder ? Data.get("reminder", reminder)
                         : { text: "", mode: lesson ? 3 : student ? 2 : 0, student_id: student || "",
                             lesson_id: lesson || "", sound: 0 }
        // From a student's page: "only at a lesson" offers their individual lesson first
        preferredLesson = ""
        if (student) {
            const s = Data.student(student)
            const single = (s.memberships || []).find(m => m.active && m.kind === 1)
            preferredLesson = single ? single.lesson_id : ""
        }
        open()
    }
    property string preferredLesson
    function pickMode(mode) {
        set("mode", mode)
        if (mode === 3 && !(draft.lesson_id || "") && preferredLesson)
            set("lesson_id", preferredLesson)
    }
    function set(key, value) {
        const d = Object.assign({}, draft)
        d[key] = value
        draft = d
    }

    title: qsTrId("reminder.edit.title")
    canAccept: (draft.text || "").trim() !== "" && (draft.mode !== 2 || (draft.student_id || "") !== "")
               && (draft.mode !== 3 || (draft.lesson_id || "") !== "")
    onAccepted: {
        const fields = { text: draft.text.trim(), mode: draft.mode, student_id: draft.mode === 2 ? draft.student_id : "",
                         lesson_id: draft.mode === 3 ? draft.lesson_id : "", sound: draft.sound ? 1 : 0 }
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
            model: [0, 1, 2, 3]
            delegate: RadioDelegate {
                id: modeRow
                required property int modelData
                readonly property string hint: modelData === 2 ? qsTrId("reminder.mode.student_hint")
                                               : modelData === 3 ? qsTrId("reminder.mode.lesson_hint") : ""
                Layout.fillWidth: true
                text: modelData === 0 ? qsTrId("reminder.mode.startup_long")
                    : modelData === 1 ? qsTrId("reminder.mode.every_lesson_long")
                    : modelData === 2 ? qsTrId("reminder.mode.student_long")
                    : qsTrId("reminder.mode.lesson_long")
                Accessible.description: hint
                checked: (editor.draft.mode || 0) === modelData
                font.family: Theme.fontFamily
                font.pixelSize: 15
                // Title and a smaller line below (where the reminder comes)
                contentItem: ColumnLayout {
                    spacing: 1
                    Text {
                        Layout.fillWidth: true
                        rightPadding: modeRow.indicator.width + modeRow.spacing
                        wrapMode: Text.WordWrap
                        text: modeRow.text
                        font: modeRow.font
                        color: Theme.text
                    }
                    Text {
                        visible: modeRow.hint !== ""
                        Layout.fillWidth: true
                        rightPadding: modeRow.indicator.width + modeRow.spacing
                        wrapMode: Text.WordWrap
                        text: modeRow.hint
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                        color: Theme.textMuted
                    }
                }
                onClicked: editor.pickMode(modelData)
            }
        }
    }
    ChoiceField {
        visible: editor.draft.mode === 2
        label: qsTrId("common.student")
        options: editor.students.map(s => s.listName)
        value: (editor.students.find(s => s.id === editor.draft.student_id) || {}).listName || ""
        onPicked: (value) => {
            const s = editor.students.find(x => x.listName === value)
            editor.set("student_id", s ? s.id : "")
        }
    }
    LessonPicker {
        visible: editor.draft.mode === 3
        label: qsTrId("common.lesson")
        lessons: editor.lessons
        lessonId: editor.draft.lesson_id || ""
        onPicked: (id) => editor.set("lesson_id", id)
    }
    SwitchRow {
        text: qsTrId("reminder.play_sound")
        checked: editor.draft.sound === 1
        onToggled: editor.set("sound", checked ? 1 : 0)
    }
    ActionButton {
        focusPolicy: Qt.TabFocus
        visible: editor.reminderId !== ""
        kind: "quiet"
        iconName: "trash"
        text: qsTrId("common.delete")
        onClicked: {
            Data.remove("reminder", editor.reminderId)
            editor.dismiss()
        }
    }
}
