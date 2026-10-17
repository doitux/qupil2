// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Sheet music of the library: composer / author, title, publisher, and
// lending it to a student or taking it back.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: editor

    property string itemId
    property var draft: ({})
    property string borrowerName
    readonly property var current: { if (Data.revision < 0 || editor.itemId === "") return ({}); return Data.get("library_item", editor.itemId) }
    readonly property bool lent: (current.lent_to || "") !== "" && borrowerName !== ""
    readonly property var students: { if (Data.revision < 0) return undefined; return Data.students("") }

    function edit(id) {
        itemId = id
        draft = id ? Data.get("library_item", id) : { author: "", title: "", publisher: "", number: Data.nextLibraryNumber() }
        lendTo = ""
        open()
    }
    function set(key, value) {
        const d = Object.assign({}, draft)
        d[key] = value
        draft = d
    }
    property string lendTo

    onCurrentChanged: {
        const s = editor.students.find(x => x.id === (editor.current.lent_to || ""))
        borrowerName = s ? (s.name || s.listName) : ""
    }

    title: itemId ? qsTrId("library.edit") : qsTrId("library.new")
    canAccept: (draft.title || "").trim() !== ""
    onAccepted: {
        const fields = { author: (draft.author || "").trim(), title: draft.title.trim(),
                         publisher: (draft.publisher || "").trim(), number: Math.max(0, parseInt(draft.number) || 0) }
        if (itemId)
            Data.update("library_item", itemId, fields)
        else
            Data.create("library_item", fields)
    }

    InputField {
        // The index written on the music (Qupil 1.x: four digits)
        label: qsTrId("library.col.index")
        text: editor.draft.number ? String(editor.draft.number) : ""
        inputMethodHints: Qt.ImhDigitsOnly
        onEdited: (value) => editor.set("number", value.replace(/[^0-9]/g, ""))
    }
    InputField {
        label: qsTrId("library.col.author")
        text: editor.draft.author || ""
        completions: { if (Data.revision < 0) return []; return Data.libraryAuthors().concat(Data.composers()) }
        onEdited: (value) => editor.set("author", value)
    }
    InputField {
        label: qsTrId("common.title")
        text: editor.draft.title || ""
        onEdited: (value) => editor.set("title", value)
    }
    InputField {
        label: qsTrId("library.col.publisher")
        text: editor.draft.publisher || ""
        completions: { if (Data.revision < 0) return []; return Data.libraryPublishers() }
        onEdited: (value) => editor.set("publisher", value)
    }

    // ---------------------------------------------------------------- lending (saved at once)
    Rectangle {
        visible: editor.itemId !== ""
        Layout.fillWidth: true
        Layout.topMargin: 6
        implicitHeight: lending.implicitHeight + 28
        radius: Theme.radius
        color: Theme.surfaceAlt

        ColumnLayout {
            id: lending
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 14
            spacing: 10

            Text {
                visible: editor.lent
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: qsTrId("library.lent_to_since").arg(editor.borrowerName).arg(Data.formatDate(editor.current.lent_since || ""))
                color: Theme.accentText
                font.family: Theme.fontFamily
                font.pixelSize: 15
                font.weight: Font.Bold
            }
            ActionButton {
                visible: editor.lent
                iconName: "check"
                text: qsTrId("library.return")
                onClicked: Data.returnLibraryItem(editor.itemId)
            }
            ChoiceField {
                visible: !editor.lent
                label: qsTrId("library.lend_to_student")
                options: editor.students.map(s => s.listName)
                value: (editor.students.find(s => s.id === editor.lendTo) || {}).listName || ""
                onPicked: (value) => {
                    const s = editor.students.find(x => x.listName === value)
                    editor.lendTo = s ? s.id : ""
                }
            }
            ActionButton {
                visible: !editor.lent
                kind: "primary"
                iconName: "book"
                text: qsTrId("library.lend")
                enabled: editor.lendTo !== ""
                onClicked: {
                    Data.lendLibraryItem(editor.itemId, editor.lendTo)
                    editor.lendTo = ""
                }
            }
        }
    }

    ActionButton {
        visible: editor.itemId !== ""
        kind: "quiet"
        iconName: "trash"
        text: qsTrId("common.delete")
        onClicked: confirmDelete.open()
    }

    AppDialog {
        id: confirmDelete
        iconName: "trash"
        destructive: true
        title: qsTrId("common.delete")
        text: qsTrId("library.delete.confirm").arg(editor.draft.title || "")
        rejectText: qsTrId("common.cancel")
        acceptText: qsTrId("common.delete")
        onAccepted: {
            Data.remove("library_item", editor.itemId)
            editor.close()
        }
    }
}
