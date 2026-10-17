// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// New or changed lesson: type, time (fixed or irregular), location, name
// (automatic like in Qupil 1.x or own) and the students in it.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: editor

    property string lessonId
    property var draft: ({})
    property var members: []
    property string filter
    property string error

    readonly property var locations: { if (Data.revision < 0) return undefined; return Data.listSetting("locations") }
    readonly property var allStudents: { if (Data.revision < 0) return undefined; return Data.students("") }
    readonly property string generatedName: Data.autoLessonName(draft.kind || 1, !!draft.irregular, draft.start_time || "",
                                                                 draft.end_time || "", draft.location || "", members)

    function edit(id, weekday) {
        lessonId = id
        error = ""
        filter = ""
        if (id) {
            const l = Data.lesson(id)
            draft = { kind: l.kind, auto_name: l.auto_name, name: l.name, irregular: l.irregular === 1,
                      weekday: l.weekday, start_time: l.start_time || "14:00", end_time: l.end_time || "14:45",
                      location: l.location }
            members = (l.students || []).map(s => s.id)
        } else {
            draft = { kind: 1, auto_name: 1, name: "", irregular: false, weekday: weekday >= 0 ? weekday : 0,
                      start_time: "14:00", end_time: "14:45", location: locations.length > 0 ? locations[0] : "" }
            members = []
        }
        open()
    }
    function set(key, value) {
        const d = Object.assign({}, draft)
        d[key] = value
        draft = d
    }
    function toggleMember(id) {
        const list = members.slice()
        const i = list.indexOf(id)
        if (i >= 0)
            list.splice(i, 1)
        else if (draft.kind === 1)
            list.splice(0, list.length, id)   // individual lesson: one student
        else
            list.push(id)
        members = list
    }
    function minutes(t) {
        const [h, m] = (t || "0:0").split(":").map(Number)
        return h * 60 + m
    }

    title: lessonId ? qsTrId("lesson.edit") : qsTrId("schedule.new_lesson")
    canAccept: draft.auto_name === 1 || (draft.name || "").trim() !== ""
    onAccepted: {
        const fields = {
            kind: draft.kind, auto_name: draft.auto_name, name: (draft.name || "").trim(), irregular: draft.irregular,
            weekday: draft.weekday, start_time: draft.start_time, end_time: draft.end_time, location: draft.location || ""
        }
        const result = Data.saveLesson(lessonId, fields, members)
        if (result !== "")
            Nav.toast(result)
    }

    Segmented {
        options: [1, 2, 3]
        value: editor.draft.kind || 1
        labelFor: (v) => Labels.lessonKind(v)
        onActivated: (v) => {
            editor.set("kind", v)
            if (v === 1 && editor.members.length > 1)
                editor.members = editor.members.slice(0, 1)
        }
    }

    SwitchRow {
        text: qsTrId("lesson.time.irregular")
        checked: !!editor.draft.irregular
        onToggled: editor.set("irregular", checked)
    }

    GridLayout {
        visible: !editor.draft.irregular
        Layout.fillWidth: true
        columns: editor.compact ? 2 : 3
        columnSpacing: 14
        rowSpacing: 16
        ChoiceField {
            Layout.columnSpan: editor.compact ? 2 : 1
            label: qsTrId("common.weekday")
            allowEmpty: false
            options: [0, 1, 2, 3, 4, 5, 6].map(d => Labels.weekday(d))
            value: Labels.weekday(editor.draft.weekday || 0)
            onPicked: (value) => editor.set("weekday", options.indexOf(value))
        }
        TimeField {
            label: qsTrId("common.start")
            time: editor.draft.start_time || "14:00"
            onPicked: (value) => {
                const length = editor.minutes(editor.draft.end_time) - editor.minutes(editor.draft.start_time)
                editor.set("start_time", value)
                // keep the duration when the start moves
                const end = Math.min(23 * 60 + 59, editor.minutes(value) + Math.max(length, 15))
                editor.set("end_time", String(Math.floor(end / 60)).padStart(2, "0") + ":" + String(end % 60).padStart(2, "0"))
            }
        }
        TimeField {
            label: qsTrId("common.end")
            time: editor.draft.end_time || "14:45"
            onPicked: (value) => editor.set("end_time", value)
        }
    }
    Text {
        visible: !editor.draft.irregular
        text: qsTrId("lesson.duration").arg(editor.minutes(editor.draft.end_time) - editor.minutes(editor.draft.start_time))
        color: editor.minutes(editor.draft.end_time) > editor.minutes(editor.draft.start_time) ? Theme.textMuted : Theme.danger
        font.family: Theme.fontFamily
        font.pixelSize: 13
    }

    ChoiceField {
        label: qsTrId("lesson.label.location")
        options: editor.locations
        editable: true
        value: editor.draft.location || ""
        onPicked: (value) => editor.set("location", value)
    }

    ColumnLayout {
        Layout.fillWidth: true
        spacing: 6
        FieldLabel { label: qsTrId("lesson.name.heading") }
        Segmented {
            options: [1, 0]
            value: editor.draft.auto_name === 0 ? 0 : 1
            labelFor: (v) => v === 1 ? qsTrId("lesson.name.auto_short") : qsTrId("lesson.name.manual_short")
            onActivated: (v) => {
                editor.set("auto_name", v)
                if (v === 0 && !editor.draft.name)
                    editor.set("name", editor.generatedName)
            }
        }
        Text {
            visible: editor.draft.auto_name !== 0
            Layout.fillWidth: true
            elide: Text.ElideRight
            text: editor.generatedName
            color: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 16
            font.weight: Font.Bold
        }
        InputField {
            visible: editor.draft.auto_name === 0
            text: editor.draft.name || ""
            onEdited: (value) => editor.set("name", value)
        }
    }

    ColumnLayout {
        Layout.fillWidth: true
        spacing: 8
        RowLayout {
            Layout.fillWidth: true
            FieldLabel {
                Layout.fillWidth: true
                label: qsTrId("lesson.label.students") + " · " + editor.members.length
            }
        }
        InputField {
            placeholder: qsTrId("student.search")
            text: editor.filter
            onEdited: (value) => editor.filter = value
        }
        Rectangle {
            Layout.fillWidth: true
            implicitHeight: studentList.implicitHeight
            radius: 14
            color: Theme.surfaceAlt
            clip: true
            ColumnLayout {
                id: studentList
                width: parent.width
                spacing: 0
                Repeater {
                    model: editor.allStudents.filter(s => editor.members.indexOf(s.id) >= 0
                                                         || editor.filter === ""
                                                         || (s.name + " " + s.instrument).toLowerCase().indexOf(editor.filter.toLowerCase()) >= 0)
                    delegate: CheckDelegate {
                        required property var modelData
                        Layout.fillWidth: true
                        text: modelData.listName + (modelData.instrument ? " · " + modelData.instrument : "")
                        checked: editor.members.indexOf(modelData.id) >= 0
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        onClicked: editor.toggleMember(modelData.id)
                    }
                }
            }
        }
    }

    ActionButton {
        visible: editor.lessonId !== ""
        kind: "quiet"
        iconName: "trash"
        text: qsTrId("lesson.delete")
        onClicked: confirmDelete.open()
    }

    AppDialog {
        id: confirmDelete
        iconName: "trash"
        destructive: true
        title: qsTrId("lesson.delete")
        text: qsTrId("lesson.delete.confirm")
        rejectText: qsTrId("common.cancel")
        acceptText: qsTrId("common.delete")
        onAccepted: {
            Data.deleteLesson(editor.lessonId)
            editor.close()
        }
    }
}
