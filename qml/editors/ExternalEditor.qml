// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Contribution to an event by musicians who are not your students.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: editor

    property string eventId
    property var draft: ({})
    readonly property var genres: { if (Data.revision < 0) return undefined; return Data.listSetting("genres") }
    readonly property var composers: { if (Data.revision < 0) return undefined; return Data.composers() }

    function start(id) {
        eventId = id
        draft = { composer: "", title: "", genre: "", duration: 3, performer: "" }
        open()
    }
    function set(key, value) {
        const d = Object.assign({}, draft)
        d[key] = value
        draft = d
    }

    title: qsTrId("event.external.title")
    canAccept: (draft.title || "").trim() !== "" && (draft.performer || "").trim() !== ""
    onAccepted: Data.addExternal(eventId, {
        composer: (draft.composer || "").trim(), title: draft.title.trim(), genre: draft.genre || "",
        duration: draft.duration || 0, performer: draft.performer.trim()
    })

    InputField {
        label: qsTrId("common.label.composer")
        text: editor.draft.composer || ""
        completions: editor.composers
        onEdited: (value) => editor.set("composer", value)
    }
    InputField {
        label: qsTrId("common.label.title")
        text: editor.draft.title || ""
        onEdited: (value) => editor.set("title", value)
    }
    GridLayout {
        Layout.fillWidth: true
        columns: editor.compact ? 1 : 2
        columnSpacing: 14
        rowSpacing: 16
        ChoiceField {
            label: qsTrId("settings.genre_label")
            options: editor.genres
            editable: true
            value: editor.draft.genre || ""
            onPicked: (value) => editor.set("genre", value)
        }
        Stepper {
            label: qsTrId("common.label.duration")
            value: editor.draft.duration || 0
            suffix: qsTrId("common.unit.min")
            onPicked: (value) => editor.set("duration", value)
        }
    }
    InputField {
        label: qsTrId("event.external.musicians")
        multiline: true
        placeholder: qsTrId("event.external.format_hint")
        text: editor.draft.performer || ""
        onEdited: (value) => editor.set("performer", value)
    }
}
