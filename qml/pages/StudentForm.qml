// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Personal data of a student, edited in place and saved with one button.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

ColumnLayout {
    id: form

    property string studentId
    property var draft: ({})
    property bool dirty: false

    readonly property var instruments: { if (Data.revision < 0) return undefined; return Data.instrumentNames() }
    readonly property var sizes: { if (Data.revision < 0) return undefined; return Data.listSetting("instrument_sizes") }

    function load() {
        const s = Data.student(studentId)
        draft = s
        dirty = false
    }
    function set(key, value) {
        if (draft[key] === value)
            return
        const d = Object.assign({}, draft)
        d[key] = value
        draft = d
        dirty = true
    }
    function save() {
        const keys = ["forename", "surname", "birthday", "address", "email", "phone", "mobile", "notes",
                      "mother_name", "mother_job", "mother_phone", "father_name", "father_job", "father_phone",
                      "first_lesson", "instrument", "instrument_size", "next_size", "rental", "rental_desc",
                      "rental_since", "recital_interval", "ensemble_expected"]
        const fields = {}
        for (const k of keys)
            if (draft[k] !== undefined)
                fields[k] = draft[k]
        if (Data.updateStudent(studentId, fields)) {
            dirty = false
            Nav.toast(qsTrId("common.saved"))
        }
    }

    onStudentIdChanged: load()
    Component.onCompleted: load()

    spacing: 14

    Card {
        Layout.fillWidth: true
        GridLayout {
            Layout.fillWidth: true
            columns: form.width > 560 ? 2 : 1
            columnSpacing: 14
            rowSpacing: 14
            InputField {
                label: qsTrId("common.first_name")
                text: form.draft.forename || ""
                onEdited: (value) => form.set("forename", value)
            }
            InputField {
                label: qsTrId("common.last_name")
                text: form.draft.surname || ""
                onEdited: (value) => form.set("surname", value)
            }
            DateField {
                label: qsTrId("common.birthday")
                allowEmpty: true
                date: form.draft.birthday || ""
                onPicked: (value) => form.set("birthday", value)
            }
            DateField {
                label: qsTrId("student.lesson_since")
                allowEmpty: true
                date: form.draft.first_lesson || ""
                onPicked: (value) => form.set("first_lesson", value)
            }
        }
        InputField {
            label: qsTrId("common.address")
            text: form.draft.address || ""
            onEdited: (value) => form.set("address", value)
        }
        GridLayout {
            Layout.fillWidth: true
            columns: form.width > 560 ? 3 : 1
            columnSpacing: 14
            rowSpacing: 14
            InputField {
                label: qsTrId("common.email")
                inputMethodHints: Qt.ImhEmailCharactersOnly | Qt.ImhNoAutoUppercase
                text: form.draft.email || ""
                onEdited: (value) => form.set("email", value)
            }
            InputField {
                label: qsTrId("common.phone")
                inputMethodHints: Qt.ImhDialableCharactersOnly
                text: form.draft.phone || ""
                onEdited: (value) => form.set("phone", value)
            }
            InputField {
                label: qsTrId("common.mobile")
                inputMethodHints: Qt.ImhDialableCharactersOnly
                text: form.draft.mobile || ""
                onEdited: (value) => form.set("mobile", value)
            }
        }
    }

    SectionLabel { text: qsTrId("common.instrument") }
    Card {
        Layout.fillWidth: true
        GridLayout {
            Layout.fillWidth: true
            columns: form.width > 560 ? 2 : 1
            columnSpacing: 14
            rowSpacing: 14
            ChoiceField {
                label: qsTrId("common.instrument")
                options: form.instruments
                value: form.draft.instrument || ""
                onPicked: (value) => form.set("instrument", value)
            }
            ChoiceField {
                label: qsTrId("common.size")
                options: form.sizes
                value: form.draft.instrument_size || ""
                onPicked: (value) => form.set("instrument_size", value)
            }
        }
        SwitchRow {
            text: qsTrId("instruments.next_size")
            checked: form.draft.next_size === 1
            onToggled: form.set("next_size", checked ? 1 : 0)
        }
        SwitchRow {
            text: qsTrId("common.rental_instrument")
            checked: form.draft.rental === 1
            onToggled: form.set("rental", checked ? 1 : 0)
        }
        GridLayout {
            visible: form.draft.rental === 1
            Layout.fillWidth: true
            columns: form.width > 560 ? 2 : 1
            columnSpacing: 14
            rowSpacing: 14
            InputField {
                label: qsTrId("student.label.inventory_number")
                text: form.draft.rental_desc || ""
                onEdited: (value) => form.set("rental_desc", value)
            }
            DateField {
                label: qsTrId("student.rented_since")
                allowEmpty: true
                date: form.draft.rental_since || ""
                onPicked: (value) => form.set("rental_since", value)
            }
        }
    }

    SectionLabel { text: qsTrId("candidates.recital.title") }
    Card {
        Layout.fillWidth: true
        ChoiceField {
            id: interval
            label: qsTrId("student.label.recital_interval")
            allowEmpty: false
            // shown labels map to the stored index 0–9
            options: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9].map(i => Labels.recitalInterval(i))
            value: Labels.recitalInterval(form.draft.recital_interval !== undefined ? form.draft.recital_interval : 5)
            onPicked: (value) => {
                const index = options.indexOf(value)
                if (index >= 0)
                    form.set("recital_interval", index)
            }
        }
        SwitchRow {
            text: qsTrId("student.ensemble_expected")
            checked: form.draft.ensemble_expected !== 0
            onToggled: form.set("ensemble_expected", checked ? 1 : 0)
        }
    }

    SectionLabel { text: qsTrId("student.label.parents") }
    Card {
        Layout.fillWidth: true
        GridLayout {
            Layout.fillWidth: true
            columns: form.width > 560 ? 3 : 1
            columnSpacing: 14
            rowSpacing: 14
            InputField {
                label: qsTrId("student.mother_name")
                text: form.draft.mother_name || ""
                onEdited: (value) => form.set("mother_name", value)
            }
            InputField {
                label: qsTrId("student.mother_job")
                text: form.draft.mother_job || ""
                onEdited: (value) => form.set("mother_job", value)
            }
            InputField {
                label: qsTrId("student.mother_phone")
                inputMethodHints: Qt.ImhDialableCharactersOnly
                text: form.draft.mother_phone || ""
                onEdited: (value) => form.set("mother_phone", value)
            }
            InputField {
                label: qsTrId("student.father_name")
                text: form.draft.father_name || ""
                onEdited: (value) => form.set("father_name", value)
            }
            InputField {
                label: qsTrId("student.father_job")
                text: form.draft.father_job || ""
                onEdited: (value) => form.set("father_job", value)
            }
            InputField {
                label: qsTrId("student.father_phone")
                inputMethodHints: Qt.ImhDialableCharactersOnly
                text: form.draft.father_phone || ""
                onEdited: (value) => form.set("father_phone", value)
            }
        }
    }

    SectionLabel { text: qsTrId("common.notes") }
    Card {
        Layout.fillWidth: true
        InputField {
            multiline: true
            text: form.draft.notes || ""
            onEdited: (value) => form.set("notes", value)
        }
    }

    RowLayout {
        Layout.fillWidth: true
        Item { Layout.fillWidth: true }
        ActionButton {
            kind: "quiet"
            text: qsTrId("common.undo")
            enabled: form.dirty
            onClicked: form.load()
        }
        ActionButton {
            kind: "primary"
            text: qsTrId("common.save")
            enabled: form.dirty
            onClicked: form.save()
        }
    }
}
