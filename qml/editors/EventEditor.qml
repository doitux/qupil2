// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// New or changed event (recital, concert).

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: editor

    property string eventId
    property var draft: ({})
    readonly property var locations: { if (Data.revision < 0) return undefined; return Data.listSetting("locations") }

    signal saved(string id)

    function edit(id) {
        eventId = id
        draft = id ? Data.get("event", id)
                   : { title: "", date: Data.today(), time: "16:00", location: "", organizer: "", accompanist: "" }
        open()
    }
    function set(key, value) {
        const d = Object.assign({}, draft)
        d[key] = value
        draft = d
    }

    title: eventId ? qsTrId("event.edit") : qsTrId("event.add.title")
    canAccept: (draft.title || "").trim() !== ""
    onAccepted: {
        const id = Data.saveEvent(eventId, {
            title: draft.title.trim(), date: draft.date, time: draft.time, location: draft.location || "",
            organizer: draft.organizer || "", accompanist: draft.accompanist || ""
        })
        if (id)
            saved(id)
    }

    InputField {
        label: qsTrId("common.label.title")
        text: editor.draft.title || ""
        onEdited: (value) => editor.set("title", value)
    }
    GridLayout {
        Layout.fillWidth: true
        columns: 2
        columnSpacing: 14
        rowSpacing: 16
        DateField {
            label: qsTrId("common.label.date")
            date: editor.draft.date || ""
            onPicked: (value) => editor.set("date", value || Data.today())
        }
        TimeField {
            label: qsTrId("common.label.time")
            time: editor.draft.time || "16:00"
            onPicked: (value) => editor.set("time", value)
        }
    }
    ChoiceField {
        label: qsTrId("event.label.location")
        options: editor.locations
        editable: true
        value: editor.draft.location || ""
        onPicked: (value) => editor.set("location", value)
    }
    InputField {
        label: qsTrId("event.label.organizer")
        text: editor.draft.organizer || ""
        onEdited: (value) => editor.set("organizer", value)
    }
    InputField {
        label: qsTrId("event.label.accompanist")
        text: editor.draft.accompanist || ""
        onEdited: (value) => editor.set("accompanist", value)
    }
}
