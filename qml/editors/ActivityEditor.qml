// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Activity outside the lessons: regular (orchestra, theory) or one-off (recital, competition).

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: editor

    property string studentId
    property string activityId
    property var draft: ({})

    function edit(student, activity, continuous) {
        studentId = student
        activityId = activity
        draft = activity ? Data.get("activity", activity)
                         : { continuous: continuous ? 1 : 0, description: "", weekday: 0, time: "17:00",
                             date: Data.today(), end_date: "", single_kind: 0, continuous_kind: 0 }
        open()
    }
    function set(key, value) {
        const d = Object.assign({}, draft)
        d[key] = value
        draft = d
    }

    readonly property bool continuous: draft.continuous === 1
    title: continuous ? qsTrId("activity.regular") : qsTrId("activity.irregular")
    canAccept: (draft.description || "").trim() !== ""
    onAccepted: {
        const fields = {
            student_id: studentId, continuous: draft.continuous, description: draft.description.trim(),
            weekday: draft.weekday, time: draft.time, date: draft.date, end_date: draft.end_date,
            single_kind: draft.single_kind, continuous_kind: draft.continuous_kind
        }
        if (activityId)
            Data.update("activity", activityId, fields)
        else
            Data.create("activity", fields)
    }

    InputField {
        label: qsTrId("common.label.description")
        text: editor.draft.description || ""
        onEdited: (value) => editor.set("description", value)
    }
    ChoiceField {
        label: qsTrId("common.label.type")
        allowEmpty: false
        options: editor.continuous ? [0, 1, 2, 3].map(k => Labels.continuousKind(k)) : [0, 1, 2].map(k => Labels.singleKind(k))
        value: editor.continuous ? Labels.continuousKind(editor.draft.continuous_kind || 0) : Labels.singleKind(editor.draft.single_kind || 0)
        onPicked: (value) => editor.set(editor.continuous ? "continuous_kind" : "single_kind", options.indexOf(value))
    }
    GridLayout {
        visible: editor.continuous
        Layout.fillWidth: true
        columns: 2
        columnSpacing: 14
        rowSpacing: 16
        ChoiceField {
            label: qsTrId("common.label.weekday")
            allowEmpty: false
            options: [0, 1, 2, 3, 4, 5, 6, 7].map(d => Labels.weekday(d))
            value: Labels.weekday(editor.draft.weekday || 0)
            onPicked: (value) => editor.set("weekday", options.indexOf(value))
        }
        TimeField {
            label: qsTrId("common.label.time")
            time: editor.draft.time || "17:00"
            onPicked: (value) => editor.set("time", value)
        }
    }
    GridLayout {
        Layout.fillWidth: true
        columns: editor.continuous && !editor.compact ? 2 : 1
        columnSpacing: 14
        rowSpacing: 16
        DateField {
            label: editor.continuous ? qsTrId("common.label.start") : qsTrId("common.label.date")
            date: editor.draft.date || ""
            onPicked: (value) => editor.set("date", value || Data.today())
        }
        DateField {
            visible: editor.continuous
            label: qsTrId("common.label.end")
            allowEmpty: true
            date: editor.draft.end_date || ""
            onPicked: (value) => editor.set("end_date", value)
        }
    }
    ActionButton {
        visible: editor.activityId !== ""
        kind: "quiet"
        iconName: "trash"
        text: qsTrId("common.delete")
        onClicked: {
            Data.remove("activity", editor.activityId)
            editor.close()
        }
    }
}
