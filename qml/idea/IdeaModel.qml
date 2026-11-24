// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "Try the new idea": the invented students of the trial and what happens to them.
// Everything lives in memory only: nothing of it is stored, nothing reaches the sync,
// and no real student is touched. The trial starts afresh every time it is opened.
//
// A task runs until it is done, or rests for a while (paused). A lesson goes through the
// list; what was done in it becomes the entry of that lesson in the history.

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
        s.resumed.push(t.title)
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
        for (const title of s.resumed)
            lines.push({ kind: "resumed", title: title, changes: [] })
        s.resumed = []
        for (const d of draft.tasks) {
            let t = s.tasks.find(x => x.id === d.id)
            const changes = []
            if (!t) {
                t = { id: d.id, title: d.title, state: "run", until: "", since: s.today, doneOn: "", points: [],
                      weeks: [], feedback: null }
                s.tasks.push(t)
            }
            for (const dp of d.points) {
                let p = t.points.find(x => x.id === dp.id)
                if (!p) {
                    p = { id: dp.id, text: dp.text, done: false, priv: dp.priv }
                    t.points.push(p)
                    if (!dp.priv && !d.isNew)
                        changes.push({ kind: "+", text: dp.text })
                }
                if (dp.done && !p.done) {
                    p.done = true
                    changes.push({ kind: p.priv ? "lock" : "✓", text: dp.text })
                }
            }
            // What I had planned for the lesson and did is over; the rest stays for next time
            t.points = t.points.filter(p => !(p.priv && p.done))
            const was = t.state
            if (d.ticked) {
                t.state = "done"
                t.doneOn = s.today
                lines.push({ kind: "done", title: t.title, changes: changes })
            } else if (d.state === "pause") {
                t.state = "pause"
                t.until = d.until
                if (was !== "pause")
                    lines.push({ kind: "paused", title: t.title, until: d.until, changes: changes })
            } else {
                t.state = "run"
                t.until = ""
                if (d.isNew)
                    lines.push({ kind: "new", title: t.title,
                                 changes: d.points.filter(p => !p.priv).map(p => ({ kind: "–", text: p.text })) })
                else if (was === "pause")
                    lines.push({ kind: "resumed", title: t.title, changes: changes })
                else
                    lines.push({ kind: "on", title: t.title, changes: changes })
            }
        }
        const did = draft.extra.filter(e => e.done).map(e => e.text)
        s.extra = draft.extra.filter(e => !e.done).map(e => e.text)
        s.log.unshift({ date: s.today, lines: lines, did: did, note: draft.note.trim() })
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
            const t = s.tasks.find(x => x.id === d.id)
            if (!t)
                continue
            for (const dp of d.points)
                if (!t.points.find(x => x.id === dp.id))
                    t.points.push({ id: dp.id, text: dp.text, done: false, priv: dp.priv })
        }
        s.extra = draft.extra.map(e => e.text)
        const due = draft.tasks.filter(d => d.due).map(d => d.id)
        s.prepared = due.length > 0 || s.extra.length > 0 ? { date: s.today, due: due } : null
        revision++
    }

    function reset() {
        const today = Data.today()
        const ago = n => addDays(today, -n)
        const p = (text, done, priv) => ({ id: newId(), text: text, done: !!done, priv: !!priv })
        const task = (title, state, points, weeks, fb, extra) => Object.assign(
            { id: newId(), title: title, state: state, until: "", since: ago(28), doneOn: "", points: points || [],
              weeks: weeks || [], feedback: fb || null }, extra || {})
        students = [
            { id: "jonas", name: "Jonas Beispiel", sub: qsTrId("idea.student.jonas"), coupled: true, today: today,
              resumed: [], extra: [], prepared: null,
              tasks: [
                  task("Mozart: Violinkonzert G-Dur, 1. Satz", "run",
                       [p("Durchführung langsam, hohe Lage sauber", true), p("Bogeneinteilung im Mittelteil", true),
                        p("Exposition bis T. 94 auswendig"), p("Triller in T. 38 von oben")],
                       [1, 2, 2, 3], { amount: 2, how: 1 }),
                  task("Tonleiter A-Dur, drei Oktaven", "run", [p("Metronom 72, auch gebunden")], [2], { amount: 3, how: 2 }),
                  task("Kreutzer: Etüde Nr. 2", "run", [p("Takt 1–16, ganze Bögen, Tempo 60")], [], { amount: 3, how: 1 },
                       { since: ago(7) }),
                  task("Bach: Doppelkonzert, 2. Satz", "pause", [p("Stimme durchsehen, Fingersätze eintragen")], [1, 1],
                       null, { until: addDays(today, 56) }),
                  task("Tonleiter G-Dur", "done", [], [], null, { doneOn: ago(7) }),
                  task("Bach: Bourrée", "done", [], [], null, { doneOn: ago(21) }),
                  task("Lagenwechsel 1.–3. Lage", "done", [], [], null, { doneOn: ago(35) })
              ],
              log: [
                  { date: ago(7), did: [], note: "Kam gut vorbereitet. Sehr schöner Ton!", lines: [
                        { kind: "new", title: "Kreutzer: Etüde Nr. 2", changes: [{ kind: "–", text: "Takt 1–16, ganze Bögen, Tempo 60" }] },
                        { kind: "done", title: "Tonleiter G-Dur", changes: [] },
                        { kind: "on", title: "Mozart: Violinkonzert G-Dur, 1. Satz", changes: [
                              { kind: "+", text: "Exposition bis T. 94 auswendig" }, { kind: "+", text: "Triller in T. 38 von oben" },
                              { kind: "✓", text: "Bogeneinteilung im Mittelteil" }] },
                        { kind: "on", title: "Tonleiter A-Dur, drei Oktaven", changes: [] }] },
                  { date: ago(14), did: [], note: "", lines: [
                        { kind: "new", title: "Tonleiter A-Dur, drei Oktaven", changes: [{ kind: "–", text: "Metronom 72, auch gebunden" }] },
                        { kind: "on", title: "Mozart: Violinkonzert G-Dur, 1. Satz", changes: [{ kind: "✓", text: "Durchführung langsam, hohe Lage sauber" }] },
                        { kind: "on", title: "Tonleiter G-Dur", changes: [] }] },
                  { date: ago(21), did: [], note: "Vorspiel im Januar angesprochen.", lines: [
                        { kind: "done", title: "Bach: Bourrée", changes: [] },
                        { kind: "paused", title: "Bach: Doppelkonzert, 2. Satz", until: addDays(today, 56), changes: [] },
                        { kind: "on", title: "Mozart: Violinkonzert G-Dur, 1. Satz", changes: [{ kind: "+", text: "Bogeneinteilung im Mittelteil" }] }] }
              ] },
            { id: "mia", name: "Mia Muster", sub: qsTrId("idea.student.mia"), coupled: true, today: today,
              resumed: [], extra: [], prepared: null,
              tasks: [
                  task("Lied 12: Kuckuck", "run", [], [3], { amount: 3, how: 2 }, { since: ago(7) }),
                  task("Lied 13: Hänschen klein", "run", [], [], { amount: 3, how: 2 }, { since: ago(7) }),
                  task("Leere Saiten: D und A im Wechsel", "run", [], [2], { amount: 2, how: 2 }, { since: ago(14) }),
                  task("Lied 14: Summ, summ, summ", "run", [], [], { amount: 1, how: 0 }, { since: ago(7) }),
                  task("Lied 10: Backe, backe Kuchen", "done", [], [], null, { doneOn: ago(7) }),
                  task("Lied 11: Ist ein Mann in’ Brunn’ gefallen", "done", [], [], null, { doneOn: ago(7) }),
                  task("Lied 9: Fuchs, du hast die Gans gestohlen", "done", [], [], null, { doneOn: ago(14) })
              ],
              log: [
                  { date: ago(7), did: [], note: "", lines: [
                        { kind: "new", title: "Lied 12: Kuckuck", changes: [] }, { kind: "new", title: "Lied 13: Hänschen klein", changes: [] },
                        { kind: "new", title: "Lied 14: Summ, summ, summ", changes: [] },
                        { kind: "done", title: "Lied 10: Backe, backe Kuchen", changes: [] },
                        { kind: "done", title: "Lied 11: Ist ein Mann in’ Brunn’ gefallen", changes: [] },
                        { kind: "on", title: "Leere Saiten: D und A im Wechsel", changes: [] }] },
                  { date: ago(14), did: [], note: "Bogenhaltung: kleiner Finger rund.", lines: [
                        { kind: "new", title: "Leere Saiten: D und A im Wechsel", changes: [] },
                        { kind: "done", title: "Lied 9: Fuchs, du hast die Gans gestohlen", changes: [] }] }
              ] },
            { id: "paul", name: "Paul Probe", sub: qsTrId("idea.student.paul"), coupled: false, today: today,
              resumed: [], extra: [], prepared: null, tasks: [],
              log: [
                  { date: ago(7), did: [], lines: [], note: "Menuett neu angefangen, Fingersatz eingetragen. Tonleiter D-Dur, eine Oktave." },
                  { date: ago(14), did: [], lines: [], note: "Vorspiel war sehr schön! Neues Stück ausgesucht: Menuett." },
                  { date: ago(21), did: [], lines: [], note: "Generalprobe fürs Vorspiel. Auf die Verbeugung achten." }
              ] }
        ]
        useTasks = true
        revision++
    }

    Component.onCompleted: reset()
}
