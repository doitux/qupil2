// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// New or changed lesson note with bold, italic, underline and marking (colour
// chosen under Appearance).
// In group lessons it can be saved for all students of the lesson.
// Also the plan of a lesson (editPlan) and a note made from a plan (fromPlan).

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
    //! The line above the text instead of the one for a copied note
    property string copiedText
    //! A lesson plan instead of a note (planId: the plan changed, "" for a new one)
    property bool plan: false
    property string planId
    //! A note made from a plan: the plan, and what stays planned (one point per line)
    property string fromPlanId
    property string rest

    readonly property int groupSize: info.groupSize || 1
    //! Qupil is quitting: the editor closing then keeps its draft
    property bool quitting: false
    //! Opened again with a draft: the line above the text has no "Clear" ("Cancel" drops it)
    property bool draft: false

    function edit(membership, note) {
        membershipId = membership
        noteId = note
        draft = false
        copiedFrom = ""
        copiedText = ""
        plan = false
        planId = ""
        fromPlanId = ""
        rest = ""
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

    //! The plan for a lesson: \a id, or (empty) the one for the next lesson, new if there is none
    function editPlan(membership, id) {
        edit(membership, "")
        plan = true
        const plans = Data.plans(membership)
        let p = id ? plans.find(x => x.id === id) : undefined
        if (!p) {
            date = Data.nextLessonDate(membership, "")
            p = plans.find(x => x.date === date)      // one plan per lesson
        }
        if (p) {
            planId = p.id
            date = p.date
            noteText.text = Data.noteHtml(p.content || "", Theme.noteMark)
            forAll = Data.setting("save_for_all_students", 1) !== 0 && p.shared > 1
        }
        noteText.cursorPosition = noteText.length
    }

    //! The note made from the ticked points of a plan, to adjust before saving
    function fromPlan(membership, id, content, restContent, restCount) {
        edit(membership, "")
        const p = Data.plans(membership).find(x => x.id === id) || {}
        fromPlanId = id
        rest = restContent
        date = p.date || Data.today()
        forAll = (p.shared || 1) > 1
        copiedFrom = date
        copiedText = restCount > 0 ? qsTrId("plan.from", restCount).arg(Data.formatDate(date))
                                   : qsTrId("plan.from_all").arg(Data.formatDate(date))
        noteText.text = Data.noteHtml(content, Theme.noteMark)
        noteText.cursorPosition = noteText.length
    }

    //! Opens the editor again with a draft kept by Nav.saveNoteDraft (Main.qml, at the start)
    function restoreDraft(d) {
        edit(d.membershipId, d.noteId && Data.get("note", d.noteId).id ? d.noteId : "")
        draft = true
        plan = !!d.plan
        planId = plan && d.planId && Data.plans(d.membershipId).some(x => x.id === d.planId) ? d.planId : ""
        fromPlanId = !plan && d.fromPlanId && Data.plans(d.membershipId).some(x => x.id === d.fromPlanId) ? d.fromPlanId : ""
        rest = fromPlanId ? (d.rest || "") : ""
        date = d.date || Data.today()
        forAll = !!d.forAll
        noteText.text = Data.noteHtml(d.content || "", Theme.noteMark)
        noteText.cursorPosition = noteText.length
        // The line above the text (as for a note started from an older one), with "Clear"
        copiedText = qsTrId("note.draft_restored").arg(d.savedOn ? Data.formatDate(d.savedOn) : "").arg(d.savedAt || "")
        copiedFrom = date
    }
    function keepDraft() {
        if (!opened || leaving || noteText.length === 0 || App.testMode)
            return
        const now = new Date()
        Nav.saveNoteDraft({
            membershipId: membershipId, noteId: noteId, date: date, forAll: forAll,
            plan: plan, planId: planId, fromPlanId: fromPlanId, rest: rest,
            content: formatter.markup(),
            savedOn: Qt.formatDate(now, "yyyy-MM-dd"), savedAt: Qt.formatTime(now, "hh:mm")
        })
    }
    // Android and iOS may end Qupil in the background: the text is kept as a draft
    Connections {
        target: Qt.application
        function onStateChanged() {
            if (Qt.application.state !== Qt.ApplicationActive)
                editor.keepDraft()
        }
        function onAboutToQuit() {
            editor.keepDraft()
            editor.quitting = true
        }
    }
    // Saved, cancelled or closed: no draft any more
    onClosed: if (!quitting) Nav.clearNoteDraft()

    title: plan ? (planId ? qsTrId("plan.edit") : qsTrId("plan.new")) : noteId ? qsTrId("note.edit") : qsTrId("note.new")
    acceptText: plan ? qsTrId("plan.save") : qsTrId("note.save")
    canAccept: noteText.length > 0
    onAccepted: {
        // Stored compactly: **bold**, *italic*, ++underlined++, ==marked==
        const content = formatter.markup()
        if (plan)
            Data.savePlan(membershipId, planId, date, content, forAll && groupSize > 1)
        else if (fromPlanId)
            Data.planToNote(fromPlanId, content, rest, forAll && groupSize > 1)
        else if (noteId)
            Data.updateNote(noteId, date, content, forAll && groupSize > 1)
        else
            Data.addNote(membershipId, date, content, forAll && groupSize > 1)
    }

    //! Phones: lesson and date in one line, the shorter hint, no "Note" label, so the
    //! text has room above the keyboard
    readonly property bool tight: editor.compact
    // Phones: lesson and date in one line
    RowLayout {
        visible: editor.tight
        Layout.fillWidth: true
        spacing: 10
        Row {
            spacing: -10
            Repeater {
                model: (editor.info.students || []).slice(0, 3)
                delegate: Avatar {
                    required property var modelData
                    size: 28
                    text: modelData.initials
                    seed: modelData.name
                    ring: Theme.surface
                }
            }
        }
        Text {
            Layout.fillWidth: true
            elide: Text.ElideRight
            text: editor.info.lessonName || ""
            color: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 15
            font.weight: Font.Bold
        }
        DateField {
            Layout.preferredWidth: 168
            date: editor.date
            onPicked: (value) => editor.date = value || Data.today()
        }
    }
    // Lesson and students the note is for
    Rectangle {
        visible: !editor.tight
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
        visible: !editor.tight
        label: editor.plan ? qsTrId("plan.for_lesson") : qsTrId("common.date")
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
                text: editor.copiedText !== "" ? editor.copiedText
                      : editor.tight ? qsTrId("note.copied_short").arg(Data.formatDate(copiedBar.from))
                                               // Desktop: right click; phones and tablets: press and hold
                                               : (App.desktop ? qsTrId("note.copied_from_desktop") : qsTrId("note.copied_from"))
                                                     .arg(Data.formatDate(copiedBar.from))
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 13
            }
            ActionButton {
                visible: !editor.draft
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

    // Bold, italic, underline, colour: in the bar at the bottom of the sheet (above the
    // keyboard) while the note is edited – right above the text the system's menu for a
    // selection (Ausschneiden, Kopieren …) covered them
    toolbarShown: noteText.activeFocus
    toolbar: [
        FormatButton {
            text: "B"
            font.weight: Font.ExtraBold
            checked: formatter.bold
            Accessible.name: qsTrId("format.bold")
            onClicked: formatter.toggleBold()
        },
        FormatButton {
            text: "I"
            font.italic: true
            checked: formatter.italic
            Accessible.name: qsTrId("format.italic")
            onClicked: formatter.toggleItalic()
        },
        FormatButton {
            text: "U"
            font.underline: true
            checked: formatter.underline
            Accessible.name: qsTrId("format.underline")
            onClicked: formatter.toggleUnderline()
        },
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
    ]

    ColumnLayout {
        Layout.fillWidth: true
        spacing: 8
        FieldLabel { visible: !editor.tight; label: editor.plan ? qsTrId("plan.label") : qsTrId("common.note") }
        TextArea {
            id: noteText
            Layout.fillWidth: true
            Layout.preferredHeight: Math.max(168, implicitHeight)
            textFormat: TextEdit.RichText
            wrapMode: TextEdit.Wrap
            padding: 14
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
            // Own placeholder: it wraps (the built-in one is cut off after one line on a phone)
            Text {
                x: noteText.leftPadding
                y: noteText.topPadding
                width: noteText.width - noteText.leftPadding - noteText.rightPadding
                visible: noteText.length === 0 && noteText.preeditText === ""
                wrapMode: Text.WordWrap
                text: editor.plan ? qsTrId("plan.placeholder") : qsTrId("note.placeholder")
                color: Theme.textMuted
                font: noteText.font
                Accessible.ignored: true
            }
            Accessible.description: noteText.length === 0 ? (editor.plan ? qsTrId("plan.placeholder") : qsTrId("note.placeholder")) : ""

            // Finger gestures: Qupil's own, the same on iPhone, iPad and Android (TextTouch)
            inputMethodHints: Qt.ImhNoTextHandles
            TextTouch { target: noteText }
        }
        Text {
            visible: editor.plan || noteText.length > 0
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            // How lines and blocks work: in the empty field itself, below it once there is text
            text: editor.plan ? (noteText.length > 0 ? qsTrId("plan.hint.lines") + " " : "") + qsTrId("plan.hint")
                              : qsTrId("note.hint.blocks")
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 13
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
        focusPolicy: Qt.TabFocus
        visible: editor.plan ? editor.planId !== "" : editor.noteId !== ""
        Layout.alignment: Qt.AlignLeft
        kind: "quiet"
        iconName: "trash"
        text: editor.plan ? qsTrId("plan.discard") : qsTrId("note.delete")
        onClicked: {
            if (editor.plan)
                Data.deletePlan(editor.planId, editor.forAll && editor.groupSize > 1)
            else
                Data.deleteNote(editor.noteId, editor.forAll && editor.groupSize > 1)
            editor.dismiss()
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
