// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "Try the new idea": the window of a lesson, and (prepare) of its preparation – the same
// list at two moments.
//   After the lesson: go through the running tasks (tick what is done, add points), write
//   down new tasks, put a task on pause, and a note of one's own. Saving writes the entry
//   of the lesson into the history (IdeaModel.saveLesson).
//   Before it: mark what is due, add what I plan (only for me).
// The window works on a copy; nothing changes before "Save".

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: sheet

    property var demo
    property string studentId
    property bool prepare: false
    property var s: null
    property bool tasksShown: true
    property string note
    //! Preparing a student without tasks: a free note instead of points (it is what the
    //! lesson's note starts from). Also when such a note is there and tasks are shown again
    property bool noteOnly: false
    //! The lesson's note starts from what was prepared
    property bool fromPrepared: false
    property string newTitle
    property string newExtra
    //! Counts up when tasks come, go or change their group
    property int rev: 0
    readonly property alias tasksModel: tasksM
    readonly property alias pointsModel: pointsM

    function count(test) {
        rev
        let n = 0
        for (let i = 0; i < tasksM.count; ++i)
            if (test(tasksM.get(i)))
                ++n
        return n
    }
    readonly property int dueCount: count(t => t.phase === "run" && t.due && !t.isNew)
    readonly property int openCount: count(t => t.phase === "run" && !t.isNew)
    readonly property int newCount: count(t => t.isNew)
    readonly property int pausedCount: count(t => t.phase === "pause")
    readonly property bool allTicked: count(t => t.phase === "run" && !t.isNew && !t.ticked) === 0

    //! Asked to open while on its way out (saved a moment ago): 0 lesson, 1 preparation
    property int queued: -1
    onClosed: if (queued >= 0) reopen.restart()
    Timer {
        id: reopen
        interval: 120
        onTriggered: {
            const again = sheet.queued === 1
            sheet.queued = -1
            sheet.start(again)
        }
    }

    function start(asPrepare) {
        if (visible) {
            // On its way out: opening it now would change what it shows and then close with
            // it – it opens as soon as it is gone. Fully there: a tap that reached a button
            // of the page behind it (seen on Android) must not open it a second time
            if (closing || leaving)
                queued = asPrepare ? 1 : 0
            return
        }
        prepare = asPrepare
        s = demo.student(studentId)
        tasksShown = demo.showsTasks(s)
        tasksM.clear()
        pointsM.clear()
        extraM.clear()
        note = s.prepared && s.prepared.note ? s.prepared.note : ""
        noteOnly = asPrepare && (!tasksShown || note !== "")
        fromPrepared = !asPrepare && note !== ""
        newTitle = ""
        newExtra = ""
        const dueIds = s.prepared ? s.prepared.due : []
        const first = s.name.split(" ")[0]
        for (const t of s.tasks) {
            if (t.state === "done")
                continue
            const due = dueIds.indexOf(t.id) >= 0
            tasksM.append({ tid: t.id, title: t.title, phase: t.state, ticked: false, due: due, until: t.until,
                            open: !asPrepare && due && t.points.some(p => p.priv && !p.done),
                            status: demo.status(s, t),
                            fbText: s.coupled && t.feedback ? qsTrId("idea.fb.of").arg(first).arg(demo.feedbackText(t.feedback)) : "",
                            isNew: false, pending: "", pendingPriv: asPrepare,
                            oldDone: t.points.filter(p => p.done).length })
            for (const p of t.points)
                pointsM.append({ pid: p.id, tid: t.id, text: p.text, done: p.done, priv: p.priv, isNew: false, hidden: p.done })
        }
        for (const e of s.extra)
            extraM.append({ text: e, done: false })
        rev++
        open()
    }
    function setTask(index, role, value) {
        tasksM.setProperty(index, role, value)
        if (role !== "pending" && role !== "open")
            rev++
    }
    function addPoint(index) {
        const t = tasksM.get(index)
        const text = t.pending.trim()
        if (text === "")
            return
        pointsM.append({ pid: demo.newId(), tid: t.tid, text: text, done: false, priv: t.pendingPriv, isNew: true, hidden: false })
        tasksM.setProperty(index, "pending", "")
    }
    function addTask() {
        const title = newTitle.trim()
        if (title === "")
            return
        tasksM.append({ tid: demo.newId(), title: title, phase: "run", ticked: false, due: false, until: "", open: true,
                        status: "", fbText: "", isNew: true, pending: "", pendingPriv: false, oldDone: 0 })
        newTitle = ""
        rev++
    }
    function removeTask(index) {
        const tid = tasksM.get(index).tid
        for (let i = pointsM.count - 1; i >= 0; --i)
            if (pointsM.get(i).tid === tid)
                pointsM.remove(i)
        tasksM.remove(index)
        rev++
    }
    function addExtra() {
        const text = newExtra.trim()
        if (text === "")
            return
        extraM.append({ text: text, done: false })
        newExtra = ""
    }
    function tickAll() {
        const to = !allTicked
        for (let i = 0; i < tasksM.count; ++i)
            if (tasksM.get(i).phase === "run" && !tasksM.get(i).isNew)
                tasksM.setProperty(i, "ticked", to)
        rev++
    }
    //! What the window holds, for IdeaModel. What is still in a line to type counts too
    function draft() {
        for (let i = 0; i < tasksM.count; ++i)
            addPoint(i)
        addTask()
        addExtra()
        const tasks = []
        for (let i = 0; i < tasksM.count; ++i) {
            const t = tasksM.get(i)
            const points = []
            for (let k = 0; k < pointsM.count; ++k) {
                const p = pointsM.get(k)
                if (p.tid === t.tid)
                    points.push({ id: p.pid, text: p.text, done: p.done, priv: p.priv, isNew: p.isNew })
            }
            tasks.push({ id: t.tid, title: t.title, ticked: t.ticked, state: t.phase, until: t.until, isNew: t.isNew,
                         due: t.due, points: points })
        }
        const extra = []
        for (let i = 0; i < extraM.count; ++i)
            extra.push({ text: extraM.get(i).text, done: extraM.get(i).done })
        return { tasks: tasks, extra: extra, note: note }
    }

    title: prepare ? qsTrId("idea.prepare") : qsTrId("idea.lesson.title")
    onAccepted: prepare ? demo.savePrepare(studentId, draft()) : demo.saveLesson(studentId, draft())

    ListModel { id: tasksM }
    ListModel { id: pointsM }
    ListModel { id: extraM }

    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: sheet.s ? sheet.s.name + " · " + Data.formatDate(sheet.s.today, true) + "\n"
                        + (sheet.prepare ? (sheet.tasksShown ? qsTrId("idea.prepare.hint") : qsTrId("idea.prepare.hint_note"))
                                         : sheet.fromPrepared ? qsTrId("idea.lesson.hint_prepared")
                                         : sheet.tasksShown ? qsTrId("idea.lesson.hint") : qsTrId("idea.lesson.hint_note")) : ""
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 14
    }

    // Prepared for this lesson: at the top
    SectionLabel {
        visible: sheet.tasksShown && !sheet.prepare && sheet.dueCount > 0
        text: qsTrId("idea.section.prepared")
    }
    Card {
        visible: sheet.tasksShown && !sheet.prepare && sheet.dueCount > 0
        Layout.fillWidth: true
        padding: 14
        spacing: 12
        Repeater {
            model: tasksM
            IdeaTaskEdit {
                required property var model
                visible: model.phase === "run" && model.due && !model.isNew
                win: sheet
            }
        }
    }

    RowLayout {
        visible: sheet.tasksShown && sheet.openCount - (sheet.prepare ? 0 : sheet.dueCount) > 0
        Layout.fillWidth: true
        SectionLabel {
            Layout.fillWidth: true
            text: !sheet.prepare && sheet.dueCount > 0 ? qsTrId("idea.section.also") : qsTrId("idea.section.running")
        }
        IdeaLink {
            visible: !sheet.prepare && sheet.openCount >= 3
            iconName: "check"
            text: sheet.allTicked ? qsTrId("idea.tick.none") : qsTrId("idea.tick.all")
            onClicked: sheet.tickAll()
        }
    }
    Card {
        visible: sheet.tasksShown && sheet.openCount - (sheet.prepare ? 0 : sheet.dueCount) > 0
        Layout.fillWidth: true
        padding: 14
        spacing: 12
        Repeater {
            model: tasksM
            IdeaTaskEdit {
                required property var model
                visible: model.phase === "run" && !model.isNew && (sheet.prepare || !model.due)
                win: sheet
            }
        }
    }

    // New tasks (after the lesson)
    SectionLabel {
        visible: sheet.tasksShown && !sheet.prepare && sheet.newCount > 0
        text: qsTrId("idea.section.new")
    }
    Card {
        visible: sheet.tasksShown && !sheet.prepare && sheet.newCount > 0
        Layout.fillWidth: true
        padding: 14
        spacing: 12
        Repeater {
            model: tasksM
            IdeaTaskEdit {
                required property var model
                visible: model.isNew
                win: sheet
            }
        }
    }
    RowLayout {
        visible: sheet.tasksShown && !sheet.prepare
        Layout.fillWidth: true
        spacing: 6
        InputField {
            placeholder: sheet.newCount + sheet.openCount > 0 ? qsTrId("idea.task.another") : qsTrId("idea.task.add")
            text: sheet.newTitle
            onEdited: value => sheet.newTitle = value
            onAccepted: sheet.addTask()
        }
        IconButton {
            focusPolicy: Qt.TabFocus
            Layout.alignment: Qt.AlignVCenter
            iconName: "plus"
            enabled: sheet.newTitle.trim() !== ""
            opacity: enabled ? 1 : 0.4
            text: qsTrId("idea.task.add_button")
            onClicked: sheet.addTask()
        }
    }

    // On pause: they stay in view, but nobody is asked about them
    SectionLabel {
        visible: sheet.tasksShown && sheet.pausedCount > 0
        text: qsTrId("idea.section.paused")
    }
    Card {
        visible: sheet.tasksShown && sheet.pausedCount > 0
        Layout.fillWidth: true
        padding: 14
        spacing: 12
        Repeater {
            model: tasksM
            ColumnLayout {
                id: pausedTask
                required property int index
                required property var model
                visible: model.phase === "pause"
                Layout.fillWidth: true
                spacing: 6
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: pausedTask.model.title
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 16
                    font.weight: Font.DemiBold
                }
                DateField {
                    label: qsTrId("idea.pause.until")
                    allowEmpty: true
                    date: pausedTask.model.until
                    onPicked: value => sheet.setTask(pausedTask.index, "until", value)
                }
                IdeaLink {
                    iconName: "play"
                    text: qsTrId("idea.resume")
                    Accessible.name: qsTrId("idea.resume.name").arg(pausedTask.model.title)
                    onClicked: sheet.setTask(pausedTask.index, "phase", "run")
                }
            }
        }
    }

    // Points of my own that belong to no task
    SectionLabel {
        visible: (sheet.prepare && sheet.tasksShown) || extraM.count > 0
        text: qsTrId("idea.section.extra")
    }
    Card {
        visible: extraM.count > 0
        Layout.fillWidth: true
        padding: 14
        spacing: 8
        Repeater {
            model: extraM
            IdeaPoint {
                required property int index
                required property var model
                text: model.text
                done: model.done
                priv: true
                tickable: !sheet.prepare
                onToggled: extraM.setProperty(index, "done", !model.done)
            }
        }
    }
    RowLayout {
        visible: sheet.prepare && sheet.tasksShown
        Layout.fillWidth: true
        spacing: 6
        InputField {
            wrap: true
            placeholder: qsTrId("idea.extra.add")
            text: sheet.newExtra
            onEdited: value => sheet.newExtra = value
            onAccepted: sheet.addExtra()
        }
        IconButton {
            focusPolicy: Qt.TabFocus
            Layout.alignment: Qt.AlignVCenter
            iconName: "plus"
            enabled: sheet.newExtra.trim() !== ""
            opacity: enabled ? 1 : 0.4
            text: qsTrId("idea.extra.add_button")
            onClicked: sheet.addExtra()
        }
    }

    // The note: a field of its own for every time the window opens. (Emptying the field of
    // the time before from here ended in a binding loop on iOS, whose keyboard still held
    // the last word.)
    Loader {
        id: noteLoader
        visible: !sheet.prepare || sheet.noteOnly
        Layout.fillWidth: true
        active: sheet.visible
        sourceComponent: InputField {
            width: noteLoader.width
            label: sheet.prepare ? qsTrId("idea.prepare.note") : sheet.tasksShown ? qsTrId("idea.note.label") : qsTrId("common.note")
            multiline: true
            // (set before the window opens: what was prepared)
            text: sheet.note
            onEdited: value => sheet.note = value
        }
    }
}
