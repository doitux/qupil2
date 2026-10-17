// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// The few fields needed to add a student; the rest is in the student's "Data" section.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: editor

    property var draft: ({})
    readonly property var instruments: { if (Data.revision < 0) return undefined; return Data.instrumentNames() }
    readonly property var sizes: { if (Data.revision < 0) return undefined; return Data.listSetting("instrument_sizes") }

    function start() {
        draft = { forename: "", surname: "", instrument: "", instrument_size: "", birthday: "" }
        open()
        first.forceActiveFocus()
    }
    function set(key, value) {
        const d = Object.assign({}, draft)
        d[key] = value
        draft = d
    }

    title: qsTrId("schedule.new_student")
    canAccept: (draft.forename || "").trim() !== "" || (draft.surname || "").trim() !== ""
    onAccepted: {
        const id = Data.createStudent({
            forename: draft.forename.trim(), surname: draft.surname.trim(), instrument: draft.instrument,
            instrument_size: draft.instrument_size, birthday: draft.birthday
        })
        if (id)
            Nav.showStudent(id, "")
    }

    GridLayout {
        Layout.fillWidth: true
        columns: editor.compact ? 1 : 2
        columnSpacing: 14
        rowSpacing: 16
        InputField {
            id: first
            label: qsTrId("common.first_name")
            text: editor.draft.forename || ""
            onEdited: (value) => editor.set("forename", value)
        }
        InputField {
            label: qsTrId("common.last_name")
            text: editor.draft.surname || ""
            onEdited: (value) => editor.set("surname", value)
        }
        ChoiceField {
            label: qsTrId("common.instrument")
            options: editor.instruments
            value: editor.draft.instrument || ""
            onPicked: (value) => editor.set("instrument", value)
        }
        ChoiceField {
            label: qsTrId("common.size")
            options: editor.sizes
            value: editor.draft.instrument_size || ""
            onPicked: (value) => editor.set("instrument_size", value)
        }
        DateField {
            label: qsTrId("common.birthday")
            allowEmpty: true
            date: editor.draft.birthday || ""
            onPicked: (value) => editor.set("birthday", value)
        }
    }
    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: qsTrId("student.new_hint")
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 14
    }
}
