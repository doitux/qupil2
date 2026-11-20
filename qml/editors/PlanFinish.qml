// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// After the lesson: "What did you cover?" All points of the plan are ticked; take away
// what was not covered. The ticked ones become the note of the lesson (saved at once, or
// first adjusted in the note editor), the rest stays planned for the next lesson.
// A plan with blocks (heading and lines, after an empty line) is ticked block by block;
// the badge opens a block, and there each line can be ticked on its own: the ticked lines
// go into the note under the block's heading, the others stay planned under the same
// heading (a block with some lines ticked shows a dash).
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
    //! Per block: the ticks of its lines once one differs from the block (else null)
    property var lineChecks: []
    //! Which blocks show their lines
    property var peeked: []
    //! Coupled with the practice book: blocks marked private go into the note of the lesson
    //! instead of the tasks (never to the student)
    property var privates: []
    //! Set when blocks of this note go into the plan of the next lesson
    property string fromNoteId
    property bool forAll: false
    //! The next lesson: where the rest (or the blocks of the note) is planned
    property string nextDate
    readonly property bool grouped: blocks.some(b => b.more > 0)
    readonly property int checkedCount: blocks.filter((b, i) => blockState(i) !== 0).length

    //! The lines of block \a i (raw text, as written)
    function linesOf(i) {
        const b = blocks[i]
        return b && b.more > 0 ? b.body.split("\n") : []
    }
    function lineTicked(i, j) {
        const l = lineChecks[i]
        return l ? l[j] === true : checks[i] === true
    }
    //! 0 nothing ticked, 1 some lines, 2 all
    function blockState(i) {
        const l = lineChecks[i]
        if (!l)
            return checks[i] === true ? 2 : 0
        const n = l.filter(Boolean).length
        return n === 0 ? 0 : n === l.length ? 2 : 1
    }
    function toggleLine(i, j) {
        const lines = linesOf(i).map((x, k) => lineTicked(i, k))
        lines[j] = !lines[j]
        const all = lines.every(Boolean)
        const none = !lines.some(Boolean)
        const c = checks.slice()
        c[i] = all
        const l = lineChecks.slice()
        l[i] = all || none ? null : lines
        checks = c
        lineChecks = l
    }

    //! "Edit before saving": the note editor takes over
    signal editRequested(string membershipId, string planId, string content, string rest, int restCount)
    //! The same for a coupled student: the tasks editor, with the private part as the note
    signal editTasksRequested(string membershipId, string planId, string covered, string privateText, string rest)

    function start(membership, id) {
        membershipId = membership
        fromNoteId = ""
        plan = Data.plans(membership).find(p => p.id === id) || ({})
        blocks = plan.blocks || []
        checks = blocks.map(() => true)
        lineChecks = blocks.map(() => null)
        peeked = blocks.map(() => false)
        privates = blocks.map(() => false)
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
        lineChecks = blocks.map(() => null)
        peeked = blocks.map(() => false)
        privates = blocks.map(() => false)
        forAll = Data.setting("save_for_all_students", 1) !== 0 && (n.shared || 1) > 1
        nextDate = Data.nextLessonDate(membership, "")
        open()
    }
    //! [ticked blocks, the others, how many others, ticked blocks marked private]; a block
    //! with some lines ticked goes into both, each time with its heading
    function parts() {
        const done = []
        const left = []
        const hidden = []
        blocks.forEach((b, i) => {
            const state = blockState(i)
            const into = privates[i] === true ? hidden : done
            if (state !== 1) {
                (state === 2 ? into : left).push(b.text)
                return
            }
            // The heading bold as a whole: alone, the part stays a block (NoteFormat::blocks)
            const t = b.title.trim()
            const title = /^\*\*[^*].*\*\*$/.test(t) && t.slice(2, -2).indexOf("**") < 0
                          ? t : "**" + t.replace(/\*\*/g, "") + "**"
            const lines = linesOf(i)
            into.push([title].concat(lines.filter((x, j) => lineTicked(i, j))).join("\n"))
            left.push([title].concat(lines.filter((x, j) => !lineTicked(i, j))).join("\n"))
        })
        const sep = grouped ? "\n\n" : "\n"
        return [done.join(sep), left.join(sep), left.length, hidden.join(sep)]
    }
    function togglePrivate(index) {
        const p = privates.slice()
        p[index] = !p[index]
        privates = p
    }
    function toggle(index) {
        const c = checks.slice()
        c[index] = blockState(index) !== 2
        const l = lineChecks.slice()
        l[index] = null
        checks = c
        lineChecks = l
    }
    function peek(index) {
        const p = peeked.slice()
        p[index] = !p[index]
        peeked = p
    }
    //! Blocks with something ticked (\a ticked) or something left
    function indexes(ticked) {
        return blocks.map((b, i) => i).filter(i => ticked ? blockState(i) !== 0 : blockState(i) !== 2)
    }
    //! The headings of these blocks: „Jazz“ und „Bach“ (a block with some lines ticked:
    //! how many of its lines)
    function names(list, ticked) {
        const quoted = list.map(i => {
            const name = qsTrId("block.quoted").arg(blocks[i].heading)
            if (blockState(i) !== 1)
                return name
            const lines = linesOf(i)
            const n = lines.filter((x, j) => lineTicked(i, j) === ticked).length
            return qsTrId("block.lines_of").arg(name).arg(n).arg(lines.length)
        })
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
            return qsTrId(existing ? "plan.from_note.add" : "plan.from_note.new").arg(names(done, true)).arg(next)
        }
        if (done.length === 0)
            return qsTrId("plan.finish.none")
        const date = Data.formatDate(plan.date || "")
        // Coupled with the practice book: tasks, and what is locked into the private note
        if (coupled) {
            const left = indexes(false)
            return qsTrId("plan.finish.tasks_info").arg(date)
                   + (privates.some(Boolean) ? " " + qsTrId("plan.finish.private_info") : "")
                   + (left.length > 0 && grouped ? " " + qsTrId("plan.finish.blocks_left").arg(names(left, false)).arg(next) : "")
        }
        if (!grouped)
            return qsTrId("plan.finish.info").arg(date).arg(next)
        const left = indexes(false)
        return qsTrId("plan.finish.blocks").arg(date)
               + (left.length > 0 ? " " + qsTrId("plan.finish.blocks_left").arg(names(left, false)).arg(next) : "")
    }

    title: fromNoteId ? qsTrId("plan.from_note.title") : qsTrId("plan.finish.title")
    //! Coupled with the practice book: what was covered becomes tasks, not a note
    readonly property bool coupled: Data.revision >= 0 && membershipId !== ""
                                    && Data.isCoupled(Data.get("membership", membershipId).student_id || "")
    acceptText: fromNoteId ? qsTrId("plan.from_note.save") : coupled ? qsTrId("plan.finish.tasks") : qsTrId("plan.finish.save")
    canAccept: checkedCount > 0
    onAccepted: {
        const [done, left] = parts()
        if (fromNoteId) {
            if (Data.savePlan(membershipId, "", nextDate, done, forAll))
                Nav.toast(qsTrId("plan.from_note.done").arg(Data.formatDate(nextDate, true)))
        } else if (coupled) {
            Data.planToTasks(plan.id, done, left, (plan.shared || 1) > 1, parts()[3])
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
            readonly property int tickState: sheet.blockState(index)
            readonly property bool ticked: tickState === 2
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
                color: pointRow.tickState !== 0 ? Theme.accentSoft : Theme.surfaceAlt
                border.width: pointRow.visualFocus ? 2 : 0
                border.color: Theme.accentText
            }
            contentItem: Column {
                id: rowColumn
                spacing: 6
                Item {
                    width: parent.width
                    implicitHeight: Math.max(lockButton.visible ? 32 : 28, pointRow.hasMore ? 28 : pointText.implicitHeight)
                    Rectangle {
                        id: box
                        anchors.verticalCenter: pointRow.hasMore ? parent.verticalCenter : undefined
                        y: pointRow.hasMore ? 0 : 2
                        width: 24
                        height: 24
                        radius: 7
                        color: pointRow.tickState !== 0 ? Theme.accent : "transparent"
                        border.width: pointRow.tickState !== 0 ? 0 : 2
                        border.color: Theme.border
                        Icon {
                            visible: pointRow.tickState !== 0
                            anchors.centerIn: parent
                            width: 18
                            height: 18
                            name: pointRow.tickState === 1 ? "minus" : "check"
                            color: Theme.textOnAccent
                            strokeWidth: 2.6
                        }
                    }
                    Text {
                        id: pointText
                        x: 36
                        anchors.verticalCenter: pointRow.hasMore ? parent.verticalCenter : undefined
                        width: parent.width - x - (pointRow.hasMore ? peekButton.width + 8 : 0)
                               - (lockButton.visible ? lockButton.width + 4 : 0)
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
                    // Coupled: keep this block out of the practice book (into the private note)
                    AbstractButton {
                        id: lockButton
                        readonly property bool hidden: sheet.privates[pointRow.index] === true
                        visible: sheet.coupled && sheet.fromNoteId === ""
                        anchors.right: pointRow.hasMore ? peekButton.left : parent.right
                        anchors.rightMargin: pointRow.hasMore ? 4 : 0
                        anchors.verticalCenter: pointRow.hasMore ? parent.verticalCenter : undefined
                        implicitWidth: 32
                        implicitHeight: 32
                        checkable: true
                        checked: hidden
                        Accessible.role: Accessible.CheckBox
                        Accessible.name: qsTrId("plan.finish.private").arg(pointRow.modelData.heading || "")
                        onClicked: sheet.togglePrivate(pointRow.index)
                        background: Rectangle {
                            radius: 16
                            color: lockButton.hidden ? Theme.inverse : "transparent"
                        }
                        contentItem: Item {
                            Icon {
                                anchors.centerIn: parent
                                width: 18
                                height: 18
                                name: "lock"
                                color: lockButton.hidden ? Theme.textOnInverse : Theme.textMuted
                            }
                        }
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
                // The lines of an open block, each with its own tick
                Repeater {
                    model: pointRow.peeking ? sheet.linesOf(pointRow.index) : []
                    delegate: AbstractButton {
                        id: lineRow
                        required property string modelData
                        required property int index
                        readonly property bool lineTicked: sheet.lineTicked(pointRow.index, index)
                        x: 36
                        width: rowColumn.width - x
                        implicitHeight: Math.max(30, lineText.implicitHeight + 8)
                        focusPolicy: Qt.StrongFocus
                        checkable: true
                        checked: lineTicked
                        Accessible.role: Accessible.CheckBox
                        Accessible.name: lineText.text.replace(/<[^>]*>/g, "")
                        onClicked: sheet.toggleLine(pointRow.index, index)
                        background: Rectangle {
                            radius: 8
                            color: "transparent"
                            border.width: lineRow.visualFocus ? 2 : 0
                            border.color: Theme.accentText
                        }
                        contentItem: Item {
                            Rectangle {
                                id: lineBox
                                y: 4
                                width: 20
                                height: 20
                                radius: 6
                                color: lineRow.lineTicked ? Theme.accent : Theme.surface
                                border.width: lineRow.lineTicked ? 0 : 2
                                border.color: Theme.border
                                Icon {
                                    visible: lineRow.lineTicked
                                    anchors.centerIn: parent
                                    width: 15
                                    height: 15
                                    name: "check"
                                    color: Theme.textOnAccent
                                    strokeWidth: 2.6
                                }
                            }
                            Text {
                                id: lineText
                                x: 30
                                y: 4 + (20 - Math.min(20, font.pixelSize * 1.4)) / 2
                                width: parent.width - x
                                wrapMode: Text.Wrap
                                textFormat: Text.RichText
                                text: Data.noteHtml(lineRow.modelData, Theme.noteMark)
                                color: Theme.text
                                font.family: Theme.fontFamily
                                font.pixelSize: 14
                            }
                        }
                    }
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
        // Coupled: the tasks editor, with the new tasks after those the lesson has (nothing
        // is saved before its Save)
        visible: sheet.fromNoteId === ""
        enabled: sheet.checkedCount > 0
        text: qsTrId("plan.finish.edit")
        onClicked: {
            const [done, left, count, hidden] = sheet.parts()
            const membership = sheet.membershipId
            const id = sheet.plan.id
            const coupled = sheet.coupled
            sheet.close()
            if (coupled)
                sheet.editTasksRequested(membership, id, done, hidden, left)
            else
                sheet.editRequested(membership, id, done, left, count)
        }
    }
}
