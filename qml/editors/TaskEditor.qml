// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "Lesson of …" for a student coupled with the practice book app (variant A): the tasks
// of the lesson at the top – each a title and one-line hints – and below, folded away,
// the private note (it never goes to the student).
//
// Keyboard: Enter in the title goes to its first hint, Enter in a hint makes the next
// hint, Enter in an empty hint makes a new task. Ctrl/⌘+Enter saves. The bar above the
// keyboard: + Hint, + Task, Private.
// Open tasks of earlier lessons come along ("open since …"): kept, they move to this
// lesson; ticked as done, they stay where they were.
//
// The same editor writes the plan of a coupled student (editPlan): tasks and hints, saved
// as plan blocks (a task: its heading bold, its hints as lines), so that after the lesson
// "Turn into tasks" makes the same tasks again. And it takes the covered part of a plan
// (fromPlan) to adjust before saving; the rest stays planned.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: editor

    property string membershipId
    property string date
    property bool forAll: true
    property var info: ({})
    //! The private note of this lesson ("" if none yet)
    property string noteId
    property bool privateOpen: false
    readonly property int groupSize: info.groupSize || 1
    //! Task and hint that last had the focus (the bar adds there)
    property int currentTask: -1
    //! A plan instead of the lesson's tasks (planId: the plan changed, "" for a new one)
    property bool planMode: false
    property string planId
    //! Made from this plan: what is left of it stays planned (planRest)
    property string fromPlanId
    property string planRest
    property string fromText

    function edit(membership, day) {
        membershipId = membership
        info = Data.membershipInfo(membership)
        date = day || Data.today()
        forAll = Data.setting("save_for_all_students", 1) !== 0
        planMode = false
        planId = ""
        fromPlanId = ""
        planRest = ""
        fromText = ""
        load()
        open()
        Qt.callLater(() => focusTitle(tasks.count > carriedCount() ? carriedCount() : 0))
    }

    function load() {
        tasks.clear()
        // Open tasks of earlier lessons first, then those of this lesson
        for (const t of Data.openTasksBefore(membershipId, date))
            tasks.append(entry(t, true))
        for (const t of Data.tasksOn(membershipId, date))
            tasks.append(entry(t, false))
        if (tasks.count === carriedCount())
            tasks.append(blank())
        const note = Data.notes(membershipId).find(n => n.date === date)
        noteId = note ? note.id : ""
        privateText.text = note ? Data.noteHtml(note.content || "", Theme.noteMark) : ""
        privateOpen = noteId !== ""
        currentTask = -1
    }
    //! The plan of a coupled student, as tasks
    function editPlan(membership, id) {
        membershipId = membership
        info = Data.membershipInfo(membership)
        planMode = true
        planId = ""
        fromPlanId = ""
        fromText = ""
        privateOpen = false
        const plans = Data.plans(membership)
        let p = id ? plans.find(x => x.id === id) : undefined
        if (!p) {
            date = Data.nextLessonDate(membership, "")
            p = plans.find(x => x.date === date)      // one plan per lesson
        }
        forAll = Data.setting("save_for_all_students", 1) !== 0
        tasks.clear()
        if (p) {
            planId = p.id
            date = p.date
            forAll = forAll && (p.shared || 1) > 1
            for (const t of Data.blocksAsTasks(p.content || ""))
                tasks.append(Object.assign(blank(), { title: t.title, hints: t.hints.map(h => ({ text: h })) }))
        }
        // "+ Plan" with a plan for that lesson already: a new task at its end to write in
        if (tasks.count === 0 || !id)
            tasks.append(blank())
        currentTask = -1
        open()
        Qt.callLater(() => focusTitle(tasks.count - 1))
    }
    //! The covered part of a plan as new tasks of its lesson, to adjust before saving
    function fromPlan(membership, id, covered, privatePart, rest) {
        const p = Data.plans(membership).find(x => x.id === id) || {}
        edit(membership, p.date || Data.today())
        fromPlanId = id
        planRest = rest
        forAll = (p.shared || 1) > 1
        fromText = qsTrId("plan.from_all").arg(Data.formatDate(date))
        // A lone empty task goes; the plan's blocks come after the lesson's tasks
        if (tasks.count === 1 && tasks.get(0).tid === "" && tasks.get(0).title === "" && tasks.get(0).hints.count === 0)
            tasks.clear()
        for (const t of Data.blocksAsTasks(covered))
            tasks.append(Object.assign(blank(), { title: t.title, hints: t.hints.map(h => ({ text: h })) }))
        if (tasks.count === 0)
            tasks.append(blank())
        // Marked private: into the note of the lesson
        if (privatePart.trim() !== "") {
            const note = Data.notes(membershipId).find(n => n.date === date)
            const content = (note ? (note.content || "").trim() + "\n\n" : "") + privatePart.trim()
            privateText.text = Data.noteHtml(content, Theme.noteMark)
            privateOpen = true
        }
    }
    function entry(t, carried) {
        const fb = t.feedback || {}
        return { tid: t.id, title: t.title, kind: t.kind || 0, carried: carried, since: t.since || t.date,
                 done: (t.closed || "") !== "", shared: t.shared || 1, practiced: fb.practiced || 0,
                 works: fb.works || 0, studentDone: !!fb.done,
                 hints: (t.hints || []).map(h => ({ text: h })) }
    }
    function blank() {
        return { tid: "", title: "", kind: 0, carried: false, since: "", done: false, shared: 1,
                 practiced: 0, works: 0, studentDone: false, hints: [] }
    }
    function carriedCount() {
        let n = 0
        for (let i = 0; i < tasks.count; ++i)
            if (tasks.get(i).carried)
                ++n
        return n
    }

    // ---- moving the focus
    function taskItem(index) { return taskRepeater.itemAt(index) }
    function focusTitle(index) {
        const item = taskItem(index)
        if (item)
            item.focusTitle()
    }
    function focusHint(task, hint) {
        const item = taskItem(task)
        if (item)
            Qt.callLater(() => item.focusHint(hint))
    }
    function addHint(task, at) {
        if (task < 0 || task >= tasks.count)
            task = tasks.count - 1
        const hints = tasks.get(task).hints
        const index = at === undefined ? hints.count : at
        hints.insert(index, { text: "" })
        focusHint(task, index)
    }
    function addTask(after) {
        const index = after === undefined || after < 0 ? tasks.count : after + 1
        tasks.insert(index, blank())
        Qt.callLater(() => focusTitle(index))
    }
    function openPrivate() {
        privateOpen = true
        Qt.callLater(() => privateText.forceActiveFocus())
    }

    // Phones and tablets: + Hint, + Task, Private above the keyboard
    toolbarShown: editor.opened && (editor.currentTask >= 0 || privateText.activeFocus)
    toolbar: [
        BarButton {
            iconName: "plus"
            text: qsTrId("task.add_hint")
            enabled: !privateText.activeFocus
            onClicked: editor.addHint(editor.currentTask)
        },
        BarButton {
            iconName: "plus"
            text: qsTrId("task.add")
            onClicked: editor.addTask(editor.currentTask)
        },
        BarButton {
            visible: !editor.planMode
            iconName: "lock"
            text: qsTrId("task.private")
            onClicked: editor.openPrivate()
        }
    ]

    Shortcut {
        sequences: ["Ctrl+Return", "Ctrl+Enter"]
        enabled: editor.opened
        onActivated: editor.accept()
    }

    title: editor.planMode ? (editor.planId ? qsTrId("plan.edit") : qsTrId("plan.new"))
                           : qsTrId("task.lesson_of").arg(Data.formatDate(editor.date, true))
    acceptText: editor.planMode ? qsTrId("plan.save") : qsTrId("task.save")
    canAccept: {
        for (let i = 0; i < tasks.count; ++i)
            if (tasks.get(i).title.trim() !== "" || tasks.get(i).hints.count > 0)
                return true
        return !editor.planMode && (privateText.length > 0 || editor.noteId !== "")
    }
    onAccepted: {
        const list = []
        for (let i = 0; i < tasks.count; ++i) {
            const t = tasks.get(i)
            const hints = []
            for (let h = 0; h < t.hints.count; ++h)
                hints.push(t.hints.get(h).text)
            // Done in an earlier lesson: closed where it was, it does not come along
            if (t.carried && t.done) {
                Data.closeTask(t.tid, true)
                continue
            }
            if (t.title.trim() === "" && hints.every(h => h.trim() === ""))
                continue
            list.push({ id: t.tid, title: t.title, hints: hints, kind: t.kind, closed: t.done })
        }
        const all = editor.forAll && editor.groupSize > 1
        if (editor.planMode) {
            const content = Data.tasksAsBlocks(list)
            if (content.trim() !== "")
                Data.savePlan(editor.membershipId, editor.planId, editor.date, content, all)
            else if (editor.planId !== "")
                Data.deletePlan(editor.planId, all)
            return
        }
        Data.saveTasks(editor.membershipId, editor.date, list, all)
        if (editor.fromPlanId !== "")
            Data.finishPlan(editor.fromPlanId, editor.planRest, all)
        const content = formatter.markup()
        if (editor.noteId !== "") {
            if (content.trim() === "")
                Data.deleteNote(editor.noteId, all)
            else
                Data.updateNote(editor.noteId, editor.date, content, all)
        } else if (content.trim() !== "") {
            Data.addNote(editor.membershipId, editor.date, content, all)
        }
    }

    // ---- compact head: students, lesson, date in one line
    RowLayout {
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
            text: editor.groupSize > 1 ? (editor.info.lessonName || "") : (((editor.info.students || [])[0] || {}).name || "")
            color: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 15
            font.weight: Font.Bold
        }
        DateField {
            Layout.preferredWidth: 168
            date: editor.date
            onPicked: (value) => {
                editor.date = value || Data.today()
                if (!editor.planMode && editor.fromPlanId === "")
                    editor.load()
            }
        }
    }

    Text {
        visible: editor.fromText !== "" || editor.planMode
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: editor.planMode ? qsTrId("task.plan_hint") : editor.fromText
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 13
    }

    SwitchRow {
        visible: editor.groupSize > 1
        text: qsTrId("task.for_all", editor.groupSize)
        hint: qsTrId("task.for_all_hint")
        checked: editor.forAll
        onToggled: editor.forAll = checked
    }

    ListModel { id: tasks }

    // ---- the tasks
    Repeater {
        id: taskRepeater
        model: tasks
        delegate: Rectangle {
            id: taskCard
            required property int index
            required property string tid
            required property string title
            required property int kind
            required property bool carried
            required property string since
            required property bool done
            required property int practiced
            required property int works
            required property bool studentDone
            required property var hints

            function focusTitle() { titleField.forceActiveFocus() }
            function focusHint(h) {
                const item = hintRepeater.itemAt(h)
                if (item)
                    item.focusField()
            }

            objectName: "taskCard" + index
            Layout.fillWidth: true
            implicitHeight: taskColumn.implicitHeight + 24
            radius: 14
            color: Theme.surface
            border.width: 2
            border.color: editor.currentTask === index ? Theme.accent : Theme.border
            opacity: taskCard.carried && taskCard.done ? 0.6 : 1

            ColumnLayout {
                id: taskColumn
                x: 12
                y: 12
                width: parent.width - 24
                spacing: 6

                // From an earlier lesson: say so, and let it be ticked as done
                RowLayout {
                    visible: taskCard.carried
                    Layout.fillWidth: true
                    spacing: 8
                    Rectangle {
                        implicitHeight: 24
                        implicitWidth: sinceText.implicitWidth + 16
                        radius: 8
                        color: Theme.surfaceAlt
                        Text {
                            id: sinceText
                            anchors.centerIn: parent
                            text: qsTrId("task.open_since").arg(Data.formatDate(taskCard.since))
                            color: Theme.accentText
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                            font.weight: Font.Bold
                        }
                    }
                    Item { Layout.fillWidth: true }
                    CheckBox {
                        text: qsTrId("task.done")
                        checked: taskCard.done
                        font.family: Theme.fontFamily
                        onToggled: tasks.setProperty(taskCard.index, "done", checked)
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    TextField {
                        id: titleField
                        objectName: "taskTitle" + taskCard.index
                        Layout.fillWidth: true
                        text: taskCard.title
                        placeholderText: qsTrId("task.title_placeholder")
                        placeholderTextColor: Theme.textMuted
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 16
                        font.weight: Font.Bold
                        leftPadding: 4
                        background: null
                        Accessible.name: qsTrId("task.title")
                        onTextEdited: tasks.setProperty(taskCard.index, "title", text)
                        onActiveFocusChanged: if (activeFocus) editor.currentTask = taskCard.index
                        // Enter: to the first hint (a new one if there is none)
                        onAccepted: {
                            if (taskCard.hints.count === 0)
                                editor.addHint(taskCard.index, 0)
                            else
                                taskCard.focusHint(0)
                        }
                    }
                    // Practise or only to get done (get the music: the student only ticks it)
                    AbstractButton {
                        id: kindButton
                        visible: !editor.planMode
                        implicitWidth: kindLabel.implicitWidth + 16
                        implicitHeight: 28
                        focusPolicy: Qt.TabFocus
                        Accessible.name: kindLabel.text
                        background: Rectangle { radius: 14; color: taskCard.kind === 1 ? Theme.inverse : Theme.surfaceAlt }
                        contentItem: Text {
                            id: kindLabel
                            text: taskCard.kind === 1 ? qsTrId("task.kind.todo") : qsTrId("task.kind.practice")
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: taskCard.kind === 1 ? Theme.textOnInverse : Theme.textMuted
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                            font.weight: Font.Bold
                        }
                        onClicked: tasks.setProperty(taskCard.index, "kind", taskCard.kind === 1 ? 0 : 1)
                    }
                    IconButton {
                        iconName: "close"
                        text: qsTrId("task.remove")
                        focusPolicy: Qt.TabFocus
                        onClicked: {
                            tasks.remove(taskCard.index)
                            if (tasks.count === 0)
                                tasks.append(editor.blank())
                            editor.currentTask = -1
                        }
                    }
                }

                // The hints: one line each
                Repeater {
                    id: hintRepeater
                    model: taskCard.hints
                    delegate: RowLayout {
                        id: hintRow
                        required property int index
                        required property string text
                        function focusField() { hintField.forceActiveFocus() }
                        Layout.fillWidth: true
                        spacing: 8
                        Rectangle { implicitWidth: 6; implicitHeight: 6; radius: 3; color: Theme.accentText; Layout.leftMargin: 6 }
                        TextField {
                            id: hintField
                            objectName: "taskHint" + taskCard.index + "_" + hintRow.index
                            Layout.fillWidth: true
                            text: hintRow.text
                            placeholderText: qsTrId("task.hint_placeholder")
                            placeholderTextColor: Theme.textMuted
                            color: Theme.textSecondary
                            font.family: Theme.fontFamily
                            font.pixelSize: 15
                            leftPadding: 2
                            background: Rectangle {
                                color: "transparent"
                                Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: hintField.activeFocus ? Theme.accent : Theme.divider }
                            }
                            Accessible.name: qsTrId("task.hint")
                            onTextEdited: taskCard.hints.setProperty(hintRow.index, "text", text)
                            onActiveFocusChanged: if (activeFocus) editor.currentTask = taskCard.index
                            // Enter: the next hint; in an empty hint: a new task
                            // (Removing the hint destroys this field: everything needed is taken first,
                            // and the removal comes last)
                            onAccepted: {
                                const ed = editor
                                const card = taskCard.index
                                const hints = taskCard.hints
                                const i = hintRow.index
                                if (text.trim() === "") {
                                    ed.addTask(card)
                                    hints.remove(i)
                                } else {
                                    ed.addHint(card, i + 1)
                                }
                            }
                            // Backspace in an empty hint: it goes, the cursor back up
                            Keys.onPressed: (event) => {
                                if (event.key === Qt.Key_Backspace && text === "") {
                                    const ed = editor
                                    const card = taskCard.index
                                    const hints = taskCard.hints
                                    const i = hintRow.index
                                    event.accepted = true
                                    if (i > 0)
                                        ed.focusHint(card, i - 1)
                                    else
                                        ed.focusTitle(card)
                                    hints.remove(i)
                                }
                            }
                        }
                    }
                }

                // What the student answered
                Text {
                    readonly property var parts: {
                        const p = []
                        if (taskCard.kind === 1) {
                            if (taskCard.studentDone) p.push(qsTrId("task.feedback.done"))
                        } else {
                            if (taskCard.practiced > 0) p.push(qsTrId("task.feedback.practiced").arg(Labels.practiced(taskCard.practiced)))
                            if (taskCard.works > 0) p.push(qsTrId("task.feedback.works").arg(Labels.works(taskCard.works)))
                        }
                        return p
                    }
                    visible: parts.length > 0
                    Layout.fillWidth: true
                    Layout.topMargin: 2
                    text: parts.join(" · ")
                    color: Theme.success
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                    font.weight: Font.DemiBold
                    wrapMode: Text.WordWrap
                }
            }
        }
    }

    ActionButton {
        Layout.alignment: Qt.AlignLeft
        kind: "quiet"
        iconName: "plus"
        text: qsTrId("task.add_more")
        focusPolicy: Qt.TabFocus
        onClicked: editor.addTask()
    }

    // ---- the private note: never goes to the student
    ActionButton {
        visible: !editor.privateOpen && !editor.planMode
        Layout.alignment: Qt.AlignLeft
        kind: "quiet"
        iconName: "lock"
        text: qsTrId("task.private_add")
        focusPolicy: Qt.TabFocus
        onClicked: editor.openPrivate()
    }
    ColumnLayout {
        visible: editor.privateOpen && !editor.planMode
        Layout.fillWidth: true
        spacing: 8
        FieldLabel { label: qsTrId("task.private_label") }
        TextArea {
            id: privateText
            objectName: "taskPrivate"
            Layout.fillWidth: true
            Layout.preferredHeight: Math.max(96, implicitHeight)
            textFormat: TextEdit.RichText
            wrapMode: TextEdit.Wrap
            padding: 14
            color: Theme.text
            selectedTextColor: Theme.textOnAccent
            selectionColor: Theme.accent
            font.family: Theme.fontFamily
            font.pixelSize: 16
            selectByMouse: true
            Accessible.name: qsTrId("task.private_label")
            background: Rectangle {
                radius: 14
                color: Theme.surface
                border.width: 2
                border.color: privateText.activeFocus ? Theme.accent : Theme.border
            }
            inputMethodHints: (Qt.platform.os === "ios" ? Qt.ImhNone : Qt.ImhNoTextHandles)
            TextTouch { target: privateText }
            onActiveFocusChanged: if (activeFocus) editor.currentTask = -1
        }
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTrId("task.private_hint")
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 13
        }
        TextFormatter {
            id: formatter
            document: privateText.textDocument
            selectionStart: privateText.selectionStart
            selectionEnd: privateText.selectionEnd
            cursorPosition: privateText.cursorPosition
        }
    }

    ActionButton {
        visible: editor.planMode && editor.planId !== ""
        Layout.alignment: Qt.AlignLeft
        kind: "quiet"
        iconName: "trash"
        text: qsTrId("plan.discard")
        focusPolicy: Qt.TabFocus
        onClicked: {
            Data.deletePlan(editor.planId, editor.forAll && editor.groupSize > 1)
            editor.dismiss()
        }
    }

    component BarButton: AbstractButton {
        id: bar
        property string iconName
        implicitHeight: 40
        implicitWidth: barRow.implicitWidth + 20
        focusPolicy: Qt.NoFocus
        Accessible.name: text
        opacity: enabled ? 1 : 0.4
        background: Rectangle { radius: 10; color: bar.pressed ? Theme.border : Theme.surface; border.width: 1; border.color: Theme.border }
        contentItem: Item {
            Row {
                id: barRow
                anchors.centerIn: parent
                spacing: 4
                Icon { name: bar.iconName; width: 16; height: 16; color: Theme.text; anchors.verticalCenter: parent.verticalCenter }
                Text {
                    text: bar.text
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 14
                    font.weight: Font.DemiBold
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }
}
