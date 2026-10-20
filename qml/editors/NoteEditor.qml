// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// New or changed lesson note with bold, italic, underline and marking (colour
// chosen under Appearance).
// In group lessons it can be saved for all students of the lesson.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: editor

    property string membershipId
    property string noteId
    property string date
    property bool forAll: true
    property var info: ({})
    //! Date of the note whose text a new note started with ("" if none)
    property string copiedFrom

    readonly property int groupSize: info.groupSize || 1

    function edit(membership, note) {
        membershipId = membership
        noteId = note
        copiedFrom = ""
        info = Data.membershipInfo(membership)
        const defaultForAll = Data.setting("save_for_all_students", 1) !== 0
        if (note) {
            const n = Data.get("note", note)
            date = n.date || Data.today()
            noteText.text = Data.noteHtml(n.content || "", Theme.noteMark)
            const copies = Data.notes(membership).find(x => x.id === note)
            forAll = defaultForAll && copies !== undefined && copies.shared > 1
        } else {
            date = Data.today()
            noteText.text = ""
            forAll = defaultForAll
        }
        open()
        noteText.forceActiveFocus()
    }

    //! New note for today with the text of an older one, to adjust for the next lesson
    function copy(membership, sourceNote) {
        const n = Data.get("note", sourceNote)
        edit(membership, "")
        copiedFrom = n.date || ""
        noteText.text = Data.noteHtml(n.content || "", Theme.noteMark)
        noteText.cursorPosition = noteText.length
    }

    title: noteId ? qsTrId("note.edit") : qsTrId("note.new")
    acceptText: qsTrId("note.save")
    canAccept: noteText.length > 0
    onAccepted: {
        // Stored compactly: **bold**, *italic*, ++underlined++, ==marked==
        const content = formatter.markup()
        if (noteId)
            Data.updateNote(noteId, date, content, forAll && groupSize > 1)
        else
            Data.addNote(membershipId, date, content, forAll && groupSize > 1)
    }

    // Lesson and students the note is for
    Rectangle {
        Layout.fillWidth: true
        implicitHeight: lessonRow.implicitHeight + 20
        radius: 14
        color: Theme.surfaceAlt
        RowLayout {
            id: lessonRow
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 12
            spacing: 10
            Row {
                spacing: -10
                Repeater {
                    model: (editor.info.students || []).slice(0, 4)
                    delegate: Avatar {
                        required property var modelData
                        size: 32
                        text: modelData.initials
                        seed: modelData.name
                        ring: Theme.surfaceAlt
                    }
                }
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 0
                Text {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: editor.info.lessonName || ""
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                    font.weight: Font.Bold
                }
                Text {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: Labels.lessonKind(editor.info.kind || 1) + " · "
                          + (editor.groupSize > 1 ? qsTrId("summary.students", editor.groupSize)
                                                  : ((editor.info.students || [])[0] || {}).name || "")
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                }
            }
        }
    }

    DateField {
        label: qsTrId("common.date")
        date: editor.date
        onPicked: (value) => editor.date = value || Data.today()
    }

    // Started from an older note: say so, and offer an empty start. Emptying keeps the
    // focus (and the keyboard) in the note: the line folds away, the cursor is at the start.
    Item {
        id: copiedBar
        property real shown: editor.copiedFrom !== "" ? 1 : 0
        // The date stays while the line folds away
        property string from
        Binding on from {
            when: editor.copiedFrom !== ""
            value: editor.copiedFrom
            restoreMode: Binding.RestoreNone
        }
        Behavior on shown {
            enabled: editor.opened
            NumberAnimation { duration: 180; easing.type: Easing.OutCubic }
        }
        visible: shown > 0
        clip: true
        Layout.fillWidth: true
        // The column's spacing folds away with it (no jump at the end)
        Layout.preferredHeight: shown * (copiedRow.implicitHeight + 16)
        Layout.bottomMargin: -16
        RowLayout {
            id: copiedRow
            width: parent.width
            opacity: copiedBar.shown
            spacing: 10
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: qsTrId("note.copied_from").arg(Data.formatDate(copiedBar.from))
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 13
            }
            ActionButton {
                kind: "quiet"
                iconName: "close"
                text: qsTrId("note.clear")
                // A tap must not take the focus from the note: the keyboard would close
                // and open again, and the sheet jump down and back up with it
                focusPolicy: Qt.TabFocus
                onClicked: {
                    noteText.clear()        // also ends a word the keyboard is still composing
                    noteText.cursorPosition = 0
                    editor.copiedFrom = ""
                    if (!noteText.activeFocus)
                        noteText.forceActiveFocus()
                }
            }
        }
    }

    ColumnLayout {
        Layout.fillWidth: true
        spacing: 8
        FieldLabel { label: qsTrId("common.note") }
        RowLayout {
            spacing: 6
            FormatButton {
                text: "B"
                font.weight: Font.ExtraBold
                checked: formatter.bold
                Accessible.name: qsTrId("format.bold")
                onClicked: formatter.toggleBold()
            }
            FormatButton {
                text: "I"
                font.italic: true
                checked: formatter.italic
                Accessible.name: qsTrId("format.italic")
                onClicked: formatter.toggleItalic()
            }
            FormatButton {
                text: "U"
                font.underline: true
                checked: formatter.underline
                Accessible.name: qsTrId("format.underline")
                onClicked: formatter.toggleUnderline()
            }
            FormatButton {
                checked: formatter.colored
                Accessible.name: qsTrId("format.text_color")
                onClicked: formatter.toggleColor(Theme.noteMark)
                Rectangle {
                    anchors.centerIn: parent
                    width: 18
                    height: 18
                    radius: 9
                    color: Theme.noteMark
                }
            }
        }
        TextArea {
            id: noteText
            Layout.fillWidth: true
            Layout.preferredHeight: Math.max(168, implicitHeight)
            textFormat: TextEdit.RichText
            wrapMode: TextEdit.Wrap
            padding: 14
            placeholderText: qsTrId("note.placeholder")
            placeholderTextColor: Theme.textMuted
            color: Theme.text
            selectedTextColor: Theme.textOnAccent
            selectionColor: Theme.accent
            font.family: Theme.fontFamily
            font.pixelSize: 16
            selectByMouse: true
            persistentSelection: true
            Accessible.name: qsTrId("common.note")
            background: Rectangle {
                radius: 14
                color: Theme.surface
                border.width: 2
                border.color: noteText.activeFocus ? Theme.accent : Theme.border
            }
        }
        TextFormatter {
            id: formatter
            document: noteText.textDocument
            selectionStart: noteText.selectionStart
            selectionEnd: noteText.selectionEnd
            cursorPosition: noteText.cursorPosition
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
        visible: editor.noteId !== ""
        Layout.alignment: Qt.AlignLeft
        kind: "quiet"
        iconName: "trash"
        text: qsTrId("note.delete")
        onClicked: {
            Data.deleteNote(editor.noteId, editor.forAll && editor.groupSize > 1)
            editor.close()
        }
    }

    component FormatButton: AbstractButton {
        id: format
        implicitWidth: 44
        implicitHeight: 40
        checkable: true
        focusPolicy: Qt.TabFocus
        font.family: Theme.fontFamily
        font.pixelSize: 16
        background: Rectangle {
            radius: 10
            color: format.checked ? Theme.inverse : Theme.surface
            border.width: 1
            border.color: Theme.border
        }
        contentItem: Text {
            text: format.text
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            color: format.checked ? Theme.textOnInverse : Theme.text
            font: format.font
        }
    }
}
