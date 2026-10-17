// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// New or changed piece: composer (with suggestions), title, style, duration,
// status and dates. In group lessons it can apply to all students.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: editor

    property string membershipId
    property string pieceId
    property var draft: ({})
    property bool forAll: true
    property var info: ({})
    property bool showError: false

    readonly property int groupSize: info.groupSize || 1
    readonly property var genres: { if (Data.revision < 0) return undefined; return Data.listSetting("genres") }
    readonly property var composers: { if (Data.revision < 0) return undefined; return Data.composers() }

    function edit(membership, piece) {
        membershipId = membership
        pieceId = piece
        info = Data.membershipInfo(membership)
        showError = false
        const defaultForAll = Data.setting("save_for_all_students", 1) !== 0
        if (piece) {
            draft = Data.get("piece", piece)
            const listed = Data.pieces(membership).find(x => x.id === piece)
            forAll = defaultForAll && listed !== undefined && listed.shared > 1
        } else {
            draft = { composer: "", title: "", genre: "", duration: 0, state: 1, start_date: Data.today(), end_date: "" }
            forAll = defaultForAll
        }
        open()
    }
    function set(key, value) {
        const d = Object.assign({}, draft)
        d[key] = value
        draft = d
    }

    title: pieceId ? qsTrId("piece.edit") : qsTrId("piece.add")
    canAccept: (draft.title || "").trim() !== ""
    onAccepted: {
        const fields = {
            composer: (draft.composer || "").trim(), title: draft.title.trim(), genre: draft.genre || "",
            duration: draft.duration || 0, state: draft.state || 0, start_date: draft.start_date || "",
            end_date: draft.end_date || ""
        }
        if (pieceId)
            Data.updatePiece(pieceId, fields, forAll && groupSize > 1)
        else
            Data.addPiece(membershipId, fields, forAll && groupSize > 1)
    }

    InputField {
        label: qsTrId("common.composer")
        text: editor.draft.composer || ""
        completions: editor.composers
        onEdited: (value) => editor.set("composer", value)
    }
    InputField {
        label: qsTrId("common.title")
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
            label: qsTrId("common.duration")
            value: editor.draft.duration || 0
            suffix: qsTrId("common.unit.min")
            onPicked: (value) => editor.set("duration", value)
        }
    }
    ColumnLayout {
        Layout.fillWidth: true
        spacing: 6
        FieldLabel { label: qsTrId("common.status") }
        Flow {
            Layout.fillWidth: true
            spacing: 6
            Repeater {
                model: [0, 1, 2, 3, 4]
                delegate: Chip {
                    required property int modelData
                    kind: "filter"
                    selected: (editor.draft.state || 0) === modelData
                    text: Labels.pieceState(modelData)
                    onClicked: {
                        editor.set("state", modelData)
                        if (modelData === 4 && !editor.draft.end_date)
                            editor.set("end_date", Data.today())
                    }
                }
            }
        }
    }
    GridLayout {
        Layout.fillWidth: true
        columns: editor.compact ? 1 : 2
        columnSpacing: 14
        rowSpacing: 16
        DateField {
            label: qsTrId("common.start")
            allowEmpty: true
            date: editor.draft.start_date || ""
            onPicked: (value) => editor.set("start_date", value)
        }
        DateField {
            label: qsTrId("common.end")
            allowEmpty: true
            date: editor.draft.end_date || ""
            onPicked: (value) => editor.set("end_date", value)
        }
    }
    SwitchRow {
        visible: editor.groupSize > 1
        text: qsTrId("note.save_for_all", editor.groupSize)
        hint: qsTrId("note.save_for_all_hint")
        checked: editor.forAll
        onToggled: editor.forAll = checked
    }
    ActionButton {
        visible: editor.pieceId !== ""
        kind: "quiet"
        iconName: "trash"
        text: qsTrId("piece.delete")
        onClicked: {
            Data.deletePiece(editor.pieceId, editor.forAll && editor.groupSize > 1)
            editor.close()
        }
    }
}
