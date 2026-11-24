// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "Try the new idea": the invented students of the trial and what happens to them.
// Everything lives in memory only: nothing of it is stored, nothing reaches the sync,
// and no real student is touched. The trial starts afresh every time it is opened.
//
// A task runs until it is done, or rests for a while (paused). A lesson goes through the
// list; what was done in it becomes the entry of that lesson in the history. Tasks can
// also be changed in between (what comes that way stands in the entry of the next
// lesson), and an entry of the history can be put right afterwards: what is taken out
// of it is undone at the tasks too.

import QtQuick
import Qupil

QtObject {
    id: demo

    //! Counts up with every change: what shows the trial's data depends on it
    property int revision: 0
    //! The setting "use tasks": off, a lesson is just its note (as Qupil is today)
    property bool useTasks: true
    property var students: []
    property int counter: 100

    readonly property var amountText: ["", qsTrId("idea.fb.little"), qsTrId("idea.fb.some"), qsTrId("idea.fb.much")]
    readonly property var howText: [qsTrId("idea.fb.not_yet"), qsTrId("idea.fb.so_so"), qsTrId("idea.fb.well")]

    function newId() { return "i" + (++counter) }
    function addDays(iso, n) {
        const d = new Date(iso + "T12:00:00")
        d.setDate(d.getDate() + n)
        return Qt.formatDate(d, "yyyy-MM-dd")
    }
    function student(id) {
        for (const s of students)
            if (s.id === id)
                return s
        return null
    }
    function taskOf(s, id) { return s && id ? s.tasks.find(t => t.id === id) || null : null }
    //! The history shows a task and a point as they read now (a typing error put right at
    //! the task is right everywhere); what is gone keeps the words it had
    function lineTitle(s, line) {
        const t = taskOf(s, line.taskId)
        return t ? t.title : line.title
    }
    function changeText(s, line, change) {
        const t = taskOf(s, line.taskId)
        const p = t && change.pointId ? t.points.find(x => x.id === change.pointId) : null
        return p ? p.text : change.text
    }
    function tasksIn(s, state) { return s ? s.tasks.filter(t => t.state === state) : [] }
    function openPoints(t, withPrivate) { return t.points.filter(p => !p.done && (withPrivate || !p.priv)) }
    function feedbackText(fb) { return fb ? amountText[fb.amount] + ", " + howText[fb.how] : "" }
    //! "2 points open · practised some, so-so" under a task's title
    function status(s, t) {
        const parts = []
        const open = openPoints(t, false).length
        if (open > 0)
            parts.push(qsTrId("idea.points_open", open))
        if (s.coupled)
            parts.push(t.feedback ? feedbackText(t.feedback) : qsTrId("idea.fb.none"))
        return parts.join(" · ")
    }
    //! Tasks whose pause is over ("until about" has passed)
    function pauseOver(s) {
        return tasksIn(s, "pause").filter(t => t.until !== "" && t.until <= s.today)
    }
    //! Tasks are shown for this student: the setting, or there are tasks already (nothing
    //! is hidden silently), or the student reads along in their app
    function showsTasks(s) {
        return useTasks || s.coupled || s.tasks.some(t => t.state !== "done")
    }

    function resume(studentId, taskId) {
        const s = student(studentId)
        const t = s.tasks.find(x => x.id === taskId)
        t.state = "run"
        t.until = ""
        s.resumed.push(t.id)
        revision++
    }
    //! The student's answer of this week (the look into their app)
    function answer(studentId, taskId, amount, how) {
        const t = student(studentId).tasks.find(x => x.id === taskId)
        t.feedback = { amount: amount, how: how }
        revision++
    }

    /*!
        Saves a lesson. \a draft comes from the lesson's window:
        { tasks: [{ id, title, ticked, state, until, isNew, points: [{ id, text, done, priv, isNew }] }],
          extra: [{ text, done }], note }
        Writes the entry of the lesson into the history and moves on by a week.
    */
    function saveLesson(studentId, draft) {
        const s = student(studentId)
        const lines = []
        const line = (kind, t, changes, until) => ({ kind: kind, taskId: t.id, title: t.title, until: until || "", changes: changes })
        const resumed = s.resumed
        s.resumed = []
        for (const d of draft.tasks) {
            let t = taskOf(s, d.id)
            const changes = []
            // New in this lesson, or made in between since the lesson before
            const fresh = !t || !!t.fresh
            if (!t) {
                t = { id: d.id, title: d.title, state: "run", until: "", since: s.today, doneOn: "", points: [],
                      weeks: [], feedback: null }
                s.tasks.push(t)
            }
            t.fresh = false
            for (const dp of d.points) {
                let p = t.points.find(x => x.id === dp.id)
                if (!p) {
                    p = { id: dp.id, text: dp.text, done: false, priv: dp.priv }
                    t.points.push(p)
                    if (!dp.priv)
                        changes.push({ kind: fresh ? "–" : "+", pointId: p.id, text: p.text })
                } else if (p.fresh && !p.priv) {
                    changes.push({ kind: fresh ? "–" : "+", pointId: p.id, text: p.text })
                }
                p.fresh = false
                if (dp.done && !p.done) {
                    p.done = true
                    changes.push({ kind: p.priv ? "lock" : "✓", pointId: p.id, text: p.text })
                }
            }
            // What I had planned for the lesson and did is over; the rest stays for next time
            t.points = t.points.filter(p => !(p.priv && p.done))
            const was = t.state
            if (d.ticked) {
                t.state = "done"
                t.doneOn = s.today
                lines.push(line("done", t, changes))
            } else if (d.state === "pause") {
                t.state = "pause"
                t.until = d.until
                if (was !== "pause")
                    lines.push(line("paused", t, changes, d.until))
            } else {
                t.state = "run"
                t.until = ""
                lines.push(line(fresh ? "new" : was === "pause" || resumed.indexOf(t.id) >= 0 ? "resumed" : "on", t, changes))
            }
        }
        const did = draft.extra.filter(e => e.done).map(e => e.text)
        s.extra = draft.extra.filter(e => !e.done).map(e => e.text)
        s.log.unshift({ id: newId(), date: s.today, lines: lines, did: did, note: draft.note.trim() })
        s.prepared = null
        // A week goes by: the answers of this week join the weeks so far, and (so that the
        // trial is not silent) the student answers for the next one
        for (const t of s.tasks) {
            if (t.state !== "run")
                continue
            if (t.feedback)
                t.weeks.push(t.feedback.amount)
            t.feedback = s.coupled ? { amount: 1 + (t.title.length + t.weeks.length) % 3,
                                       how: (t.title.length + 2 * t.weeks.length) % 3 } : null
        }
        s.today = addDays(s.today, 7)
        s.weekPassed = true
        revision++
    }
    //! Saves the preparation: which tasks are due, what I plan for them, what else
    function savePrepare(studentId, draft) {
        const s = student(studentId)
        for (const d of draft.tasks) {
            const t = taskOf(s, d.id)
            if (!t)
                continue
            for (const dp of d.points)
                if (!t.points.find(x => x.id === dp.id))
                    t.points.push({ id: dp.id, text: dp.text, done: false, priv: dp.priv, fresh: !dp.priv })
        }
        s.extra = draft.extra.map(e => e.text)
        const due = draft.tasks.filter(d => d.due).map(d => d.id)
        // A student without tasks is prepared with a free note; the lesson starts from it
        const note = (draft.note || "").trim()
        s.prepared = due.length > 0 || s.extra.length > 0 || note !== "" ? { date: s.today, due: due, note: note } : null
        revision++
    }

    /*!
        Changes a task in between, without a lesson (or makes a new one: \a draft.id empty).
        { id, title, state, until, points: [{ id, text, done, priv }] } – points that are
        not in the list any more are gone. What is new stands in the entry of the next lesson.
    */
    function saveTask(studentId, draft) {
        const s = student(studentId)
        let t = taskOf(s, draft.id)
        const title = draft.title.trim()
        if (!t) {
            if (title === "")
                return
            t = { id: newId(), title: title, state: "run", until: "", since: s.today, doneOn: "", points: [],
                  weeks: [], feedback: null, fresh: true }
            s.tasks.push(t)
        }
        if (title !== "")
            t.title = title
        if (t.state !== "done") {
            if (draft.state === "run" && t.state === "pause")
                s.resumed.push(t.id)
            t.state = draft.state
            t.until = draft.state === "pause" ? draft.until : ""
        }
        const points = []
        for (const dp of draft.points) {
            const text = dp.text.trim()
            if (text === "")
                continue
            const p = t.points.find(x => x.id === dp.id)
            if (p) {
                p.text = text
                p.done = dp.done
                p.priv = dp.priv
                points.push(p)
            } else {
                points.push({ id: newId(), text: text, done: dp.done, priv: dp.priv, fresh: !dp.priv && !dp.done })
            }
        }
        t.points = points
        revision++
    }
    //! A task that should never have been there: gone (the history keeps its words)
    function deleteTask(studentId, taskId) {
        const s = student(studentId)
        const t = taskOf(s, taskId)
        if (!t)
            return
        for (const e of s.log)
            for (const l of e.lines)
                if (l.taskId === taskId) {
                    for (const c of l.changes)
                        c.text = changeText(s, l, c)
                    l.title = t.title
                }
        s.tasks = s.tasks.filter(x => x.id !== taskId)
        s.resumed = s.resumed.filter(x => x !== taskId)
        if (s.prepared)
            s.prepared.due = s.prepared.due.filter(x => x !== taskId)
        revision++
    }
    /*!
        Puts an entry of the history right. \a draft follows the entry's lines:
        { note, lines: [{ removed, title, changes: [{ removed, text }] }] }
        A changed text is changed at the task too. What is taken out is undone there: a new
        task or point is gone, what was ticked is open again, what was paused runs again.
    */
    function saveEntry(studentId, entryId, draft) {
        const s = student(studentId)
        const e = s.log.find(x => x.id === entryId)
        if (!e)
            return
        const undo = (t, c) => {
            if (!t)
                return
            if (c.kind === "+" || c.kind === "–") {
                t.points = t.points.filter(p => p.id !== c.pointId)
            } else if (c.kind === "✓") {
                const p = t.points.find(x => x.id === c.pointId)
                if (p)
                    p.done = false
            }
        }
        const kept = []
        e.lines.forEach((l, i) => {
            const d = draft.lines[i]
            const t = taskOf(s, l.taskId)
            if (d.removed) {
                for (const c of l.changes)
                    undo(t, c)
                if (t && l.kind === "new") {
                    deleteTask(studentId, t.id)
                } else if (t && (l.kind === "done" || l.kind === "paused")) {
                    t.state = "run"
                    t.doneOn = ""
                    t.until = ""
                } else if (t && l.kind === "resumed") {
                    t.state = "pause"
                }
                return
            }
            const title = d.title.trim()
            if (title !== "") {
                if (t)
                    t.title = title
                l.title = title
            }
            const changes = []
            l.changes.forEach((c, k) => {
                const dc = d.changes[k]
                if (dc.removed) {
                    undo(t, c)
                    return
                }
                const text = dc.text.trim()
                if (text !== "") {
                    const p = t ? t.points.find(x => x.id === c.pointId) : null
                    if (p)
                        p.text = text
                    c.text = text
                }
                changes.push(c)
            })
            l.changes = changes
            kept.push(l)
        })
        e.lines = kept
        e.note = draft.note.trim()
        if (e.lines.length === 0 && e.note === "" && e.did.length === 0)
            s.log = s.log.filter(x => x !== e)
        revision++
    }

    function reset() {
        const today = Data.today()
        const ago = n => addDays(today, -n)
        const p = (text, done, priv) => ({ id: newId(), text: text, done: !!done, priv: !!priv })
        const task = (title, state, points, weeks, fb, extra) => Object.assign(
            { id: newId(), title: title, state: state, until: "", since: ago(28), doneOn: "", points: points || [],
              weeks: weeks || [], feedback: fb || null }, extra || {})
        // An entry of the history: line(kind, task, [[mark, point], …])
        const line = (kind, t, changes, until) => ({
            kind: kind, taskId: t.id, title: t.title, until: until || "",
            changes: (changes || []).map(c => ({ kind: c[0], pointId: c[1].id, text: c[1].text })) })
        const entry = (days, note, lines) => ({ id: newId(), date: ago(days), did: [], note: note, lines: lines || [] })

        const mozart = task("Mozart: Violinkonzert G-Dur, 1. Satz", "run",
                            [p("Durchführung langsam, hohe Lage sauber", true), p("Bogeneinteilung im Mittelteil", true),
                             p("Exposition bis T. 94 auswendig"), p("Triller in T. 38 von oben")],
                            [1, 2, 2, 3], { amount: 2, how: 1 })
        const scaleA = task("Tonleiter A-Dur, drei Oktaven", "run", [p("Metronom 72, auch gebunden")], [2], { amount: 3, how: 2 })
        const kreutzer = task("Kreutzer: Etüde Nr. 2", "run", [p("Takt 1–16, ganze Bögen, Tempo 60")], [], { amount: 3, how: 1 },
                              { since: ago(7) })
        const doubleC = task("Bach: Doppelkonzert, 2. Satz", "pause", [p("Stimme durchsehen, Fingersätze eintragen")], [1, 1],
                             null, { until: addDays(today, 56) })
        const scaleG = task("Tonleiter G-Dur", "done", [], [], null, { doneOn: ago(7) })
        const bourree = task("Bach: Bourrée", "done", [], [], null, { doneOn: ago(21) })
        const shifts = task("Lagenwechsel 1.–3. Lage", "done", [], [], null, { doneOn: ago(35) })

        const song12 = task("Lied 12: Kuckuck", "run", [], [3], { amount: 3, how: 2 }, { since: ago(7) })
        const song13 = task("Lied 13: Hänschen klein", "run", [], [], { amount: 3, how: 2 }, { since: ago(7) })
        const strings = task("Leere Saiten: D und A im Wechsel", "run", [], [2], { amount: 2, how: 2 }, { since: ago(14) })
        const song14 = task("Lied 14: Summ, summ, summ", "run", [], [], { amount: 1, how: 0 }, { since: ago(7) })
        const song10 = task("Lied 10: Backe, backe Kuchen", "done", [], [], null, { doneOn: ago(7) })
        const song11 = task("Lied 11: Ist ein Mann in’ Brunn’ gefallen", "done", [], [], null, { doneOn: ago(7) })
        const song9 = task("Lied 9: Fuchs, du hast die Gans gestohlen", "done", [], [], null, { doneOn: ago(14) })

        students = [
            { id: "jonas", name: "Jonas Beispiel", sub: qsTrId("idea.student.jonas"), coupled: true, today: today,
              resumed: [], extra: [], prepared: null,
              tasks: [mozart, scaleA, kreutzer, doubleC, scaleG, bourree, shifts],
              log: [
                  entry(7, "Kam gut vorbereitet. Sehr schöner Ton!", [
                            line("new", kreutzer, [["–", kreutzer.points[0]]]),
                            line("done", scaleG),
                            line("on", mozart, [["+", mozart.points[2]], ["+", mozart.points[3]], ["✓", mozart.points[1]]]),
                            line("on", scaleA)]),
                  entry(14, "", [
                            line("new", scaleA, [["–", scaleA.points[0]]]),
                            line("on", mozart, [["✓", mozart.points[0]]]),
                            line("on", scaleG)]),
                  entry(21, "Vorspiel im Januar angesprochen.", [
                            line("done", bourree),
                            line("paused", doubleC, [], addDays(today, 56)),
                            line("on", mozart, [["+", mozart.points[1]]])])
              ] },
            { id: "mia", name: "Mia Muster", sub: qsTrId("idea.student.mia"), coupled: true, today: today,
              resumed: [], extra: [], prepared: null,
              tasks: [song12, song13, strings, song14, song10, song11, song9],
              log: [
                  entry(7, "", [line("new", song12), line("new", song13), line("new", song14),
                                line("done", song10), line("done", song11), line("on", strings)]),
                  entry(14, "Bogenhaltung: kleiner Finger rund.", [line("new", strings), line("done", song9)])
              ] },
            { id: "paul", name: "Paul Probe", sub: qsTrId("idea.student.paul"), coupled: false, today: today,
              resumed: [], extra: [], prepared: null, tasks: [],
              log: [
                  entry(7, "Menuett neu angefangen, Fingersatz eingetragen. Tonleiter D-Dur, eine Oktave."),
                  entry(14, "Vorspiel war sehr schön! Neues Stück ausgesucht: Menuett."),
                  entry(21, "Generalprobe fürs Vorspiel. Auf die Verbeugung achten.")
              ] }
        ]
        useTasks = true
        revision++
    }

    Component.onCompleted: reset()
}
