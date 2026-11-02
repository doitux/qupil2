// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// After the lesson: "What did you cover?" All points of the plan are ticked; take away
// what was not covered. The ticked ones become the note of the lesson (saved at once, or
// first adjusted in the note editor), the rest stays planned for the next lesson.
// A plan with blocks (heading and lines, after an empty line) is ticked block by block;
// the badge shows a block's lines without changing its tick.
//
// The same sheet takes blocks of an earlier note into the plan of the next lesson
// (startFromNote): nothing is ticked at first, the note stays as it is.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: sheet

    property string membershipId
    property var plan: ({})
    //! The blocks of the plan or note (Data.blocks); without blocks one per line
    property var blocks: []
    property var checks: []
    //! Which blocks show their lines
    property var peeked: []
    //! Set when blocks of this note go into the plan of the next lesson
    property string fromNoteId
    property bool forAll: false
    //! The next lesson: where the rest (or the blocks of the note) is planned
    property string nextDate
    readonly property bool grouped: blocks.some(b => b.more > 0)
    readonly property int checkedCount: checks.filter(Boolean).length

    //! "Edit before saving": the note editor takes over
    signal editRequested(string membershipId, string planId, string content, string rest, int restCount)

    function start(membership, id) {
        membershipId = membership
        fromNoteId = ""
        plan = Data.plans(membership).find(p => p.id === id) || ({})
        blocks = plan.blocks || []
        checks = blocks.map(() => true)
        peeked = blocks.map(() => false)
        nextDate = plan.date ? Data.nextLessonDate(membership, plan.date) : ""
        open()
    }
    //! Blocks of the note \a noteId for the next lesson
    function startFromNote(membership, noteId) {
        membershipId = membership
        fromNoteId = noteId
        const n = Data.notes(membership).find(x => x.id === noteId) || ({})
        plan = ({ id: "", date: n.date || "" })
        blocks = Data.blocks(n.content || "")
        checks = blocks.map(() => false)
        peeked = blocks.map(() => false)
        forAll = Data.setting("save_for_all_students", 1) !== 0 && (n.shared || 1) > 1
        nextDate = Data.nextLessonDate(membership, "")
        open()
    }
    //! [ticked blocks, the others, how many others]
    function parts() {
        const done = []
        const left = []
        blocks.forEach((b, i) => (checks[i] ? done : left).push(b.text))
        const sep = grouped ? "\n\n" : "\n"
        return [done.join(sep), left.join(sep), left.length]
    }
    function toggle(index) {
        const c = checks.slice()
        c[index] = !c[index]
        checks = c
    }
    function peek(index) {
        const p = peeked.slice()
        p[index] = !p[index]
        peeked = p
    }
    function indexes(ticked) {
        return blocks.map((b, i) => i).filter(i => (checks[i] === true) === ticked)
    }
    //! The headings of these blocks: „Jazz“ und „Bach“
    function names(list) {
        const quoted = list.map(i => qsTrId("block.quoted").arg(blocks[i].heading))
        return quoted.length > 1 ? qsTrId("block.and").arg(quoted.slice(0, -1).join(", ")).arg(quoted[quoted.length - 1])
                                 : quoted.join("")
    }
    //! What happens with the ticks
    function info() {
        const next = nextDate ? Data.formatDate(nextDate, true) : ""
        const done = indexes(true)
        if (fromNoteId) {
            if (done.length === 0)
                return qsTrId("plan.from_note.none")
            const existing = Data.plans(membershipId).some(p => p.date === nextDate)
            return qsTrId(existing ? "plan.from_note.add" : "plan.from_note.new").arg(names(done)).arg(next)
        }
        if (done.length === 0)
            return qsTrId("plan.finish.none")
        const date = Data.formatDate(plan.date || "")
        if (!grouped)
            return qsTrId("plan.finish.info").arg(date).arg(next)
        const left = indexes(false)
        return qsTrId("plan.finish.blocks").arg(date)
               + (left.length > 0 ? " " + qsTrId("plan.finish.blocks_left").arg(names(left)).arg(next) : "")
    }

    title: fromNoteId ? qsTrId("plan.from_note.title") : qsTrId("plan.finish.title")
    acceptText: fromNoteId ? qsTrId("plan.from_note.save") : qsTrId("plan.finish.save")
    canAccept: checkedCount > 0
    onAccepted: {
        const [done, left] = parts()
        if (fromNoteId) {
            if (Data.savePlan(membershipId, "", nextDate, done, forAll))
                Nav.toast(qsTrId("plan.from_note.done").arg(Data.formatDate(nextDate, true)))
        } else {
            Data.planToNote(plan.id, done, left, (plan.shared || 1) > 1)
        }
    }

    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: sheet.fromNoteId ? qsTrId("plan.from_note.source").arg(Data.formatDate(sheet.plan.date || "", true))
                               : qsTrId("plan.title_for").arg((sheet.plan.when || ""))
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 14
    }

    Repeater {
        model: sheet.blocks
        delegate: AbstractButton {
            id: pointRow
            required property var modelData
            required property int index
            readonly property bool ticked: sheet.checks[index] === true
            readonly property bool hasMore: modelData.more > 0
            readonly property bool peeking: sheet.peeked[index] === true
            Layout.fillWidth: true
            implicitHeight: rowColumn.implicitHeight + 20
            focusPolicy: Qt.StrongFocus
            checkable: true
            checked: ticked
            Accessible.role: Accessible.CheckBox
            Accessible.name: modelData.heading
            onClicked: sheet.toggle(index)
            background: Rectangle {
                radius: 14
                color: pointRow.ticked ? Theme.accentSoft : Theme.surfaceAlt
                border.width: pointRow.visualFocus ? 2 : 0
                border.color: Theme.accentText
            }
            contentItem: Column {
                id: rowColumn
                spacing: 6
                Item {
                    width: parent.width
                    implicitHeight: Math.max(28, pointRow.hasMore ? 28 : pointText.implicitHeight)
                    Rectangle {
                        id: box
                        anchors.verticalCenter: pointRow.hasMore ? parent.verticalCenter : undefined
                        y: pointRow.hasMore ? 0 : 2
                        width: 24
                        height: 24
                        radius: 7
                        color: pointRow.ticked ? Theme.accent : "transparent"
                        border.width: pointRow.ticked ? 0 : 2
                        border.color: Theme.border
                        Icon {
                            visible: pointRow.ticked
                            anchors.centerIn: parent
                            width: 18
                            height: 18
                            name: "check"
                            color: Theme.textOnAccent
                            strokeWidth: 2.6
                        }
                    }
                    Text {
                        id: pointText
                        x: 36
                        anchors.verticalCenter: pointRow.hasMore ? parent.verticalCenter : undefined
                        width: parent.width - x - (pointRow.hasMore ? peekButton.width + 8 : 0)
                        wrapMode: pointRow.hasMore ? Text.NoWrap : Text.Wrap
                        elide: pointRow.hasMore ? Text.ElideRight : Text.ElideNone
                        textFormat: pointRow.hasMore ? Text.PlainText : Text.RichText
                        text: pointRow.hasMore ? pointRow.modelData.heading
                                               : Data.noteHtml(pointRow.modelData.title, Theme.noteMark)
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: pointRow.hasMore ? Font.Bold : Font.Normal
                    }
                    // Look into the block without ticking it
                    AbstractButton {
                        id: peekButton
                        visible: pointRow.hasMore
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        implicitWidth: badge.implicitWidth
                        implicitHeight: 36
                        Accessible.name: pointRow.peeking ? qsTrId("common.show_less") : qsTrId("common.show_more")
                        onClicked: sheet.peek(pointRow.index)
                        contentItem: Item {
                            MoreBadge {
                                id: badge
                                anchors.centerIn: parent
                                count: pointRow.modelData.more
                                open: pointRow.peeking
                                color: Theme.surface
                            }
                        }
                    }
                }
                Text {
                    visible: pointRow.peeking
                    x: 36
                    width: parent.width - x
                    wrapMode: Text.Wrap
                    textFormat: Text.RichText
                    text: pointRow.peeking ? Data.noteHtml(pointRow.modelData.body, Theme.noteMark) : ""
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 14
                }
            }
            leftPadding: 14
            rightPadding: 10
            topPadding: 10
            bottomPadding: 10
        }
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 8
        Icon {
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: 18
            Layout.preferredHeight: 18
            name: "info"
            color: Theme.textMuted
        }
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: sheet.info()
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 13
        }
    }

    ActionButton {
        Layout.fillWidth: true
        Layout.preferredHeight: 48
        kind: "secondary"
        iconName: "edit"
        visible: sheet.fromNoteId === ""
        enabled: sheet.checkedCount > 0
        text: qsTrId("plan.finish.edit")
        onClicked: {
            const [done, left, count] = sheet.parts()
            const membership = sheet.membershipId
            const id = sheet.plan.id
            sheet.close()
            sheet.editRequested(membership, id, done, left, count)
        }
    }
}
