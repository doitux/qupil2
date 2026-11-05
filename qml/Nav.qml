// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// App-wide requests between pages and the editors that live in Main.qml.
//   Nav.editNote(membershipId, "")   → Main.qml opens the note editor
pragma Singleton
import QtCore
import QtQuick

QtObject {
    //! Height taken at the bottom right by the running metronome (Main.qml): floating
    //! buttons of a page ("+ Note") sit above it instead of under it
    property real playerInset: 0
    //! Height of the on-screen keyboard over the window (set by Main.qml): Android and iOS
    property real keyboardInset: 0
    //! iOS: an editor opened for typing, the keyboard is about to come (Main.qml makes room)
    property bool expectKeyboard: false
    //! Editors (sheets) open: they make room for the keyboard themselves
    property int openSheets: 0
    //! iOS: an editor expected the on-screen keyboard and it did not come (a hardware keyboard
    //! on the iPad): Main.qml stops making room in advance
    signal keyboardStayedAway()
    //! Students to point out once on More › Birthdays (from "Hatten Geburtstag: …" in the notices)
    property var birthdayFocus: []

    // ---------------------------------------------------------------- note draft
    // The text of an open note or plan editor, kept when Qupil goes to the background or
    // quits: Android and iOS may end Qupil there (Qt then ends the whole process), and the
    // editor comes back with it at the next start (Main.qml). Only on this device, never
    // synced, not in the test mode.
    property Settings _drafts: Settings {
        category: "draft"
        property string note: ""
    }
    function saveNoteDraft(draft) {
        _drafts.note = JSON.stringify(draft)
        try { _drafts.sync() } catch (e) {}
    }
    function clearNoteDraft() {
        if (_drafts.note === "")
            return
        _drafts.note = ""
        try { _drafts.sync() } catch (e) {}
    }
    //! The kept draft, or null
    function noteDraft() {
        if (_drafts.note === "")
            return null
        try { return JSON.parse(_drafts.note) } catch (e) { return null }
    }

    /*!
        One step back in a StackView of pages (system back button): the page on top
        closes the way its own back arrow does (it may tidy up first), otherwise
        it is popped. False when there is nothing to go back to.
    */
    function stackBack(stack) {
        if (!stack || stack.depth <= 1)
            return false
        const depth = stack.depth
        const top = stack.currentItem
        if (top && typeof top.back === "function")
            top.back()
        if (stack.depth === depth)
            stack.pop()
        return true
    }

    // ------------------------------------------------------------ editors (handled by Main.qml)
    signal newStudent()
    signal editLesson(string lessonId, int weekday)
    signal editNote(string membershipId, string noteId)
    //! A new note (today) that starts with the text of an older one – as in Qupil 1.x,
    //! where tapping a note copied it into the field for the next one
    signal newNoteFrom(string membershipId, string noteId)
    signal editPiece(string membershipId, string pieceId)
    //! The plan of a lesson (empty id: the one for the next lesson, new if there is none)
    signal editPlan(string membershipId, string planId)
    //! After the lesson: tick what was covered, the rest stays planned
    signal finishPlan(string membershipId, string planId)
    //! Blocks of a note into the plan of the next lesson
    signal planFromNote(string membershipId, string noteId)
    signal editEvent(string eventId)
    signal editActivity(string studentId, string activityId, bool continuous)
    signal editReminder(string reminderId, string studentId)
    //! Message at the bottom of the window ("Saved", errors)
    signal toast(string message)

    // ------------------------------------------------------------ navigation
    //! Switch the main tab: 0 schedule, 1 students, 2 events, 3 metronome, 4 more
    signal showPage(int index)
    //! Students tab with this student open
    signal showStudent(string studentId, string membershipId)
    //! Students tab with this student's pieces, this piece in view and marked for a moment
    signal showPiece(string studentId, string membershipId, string pieceId)
    //! Events tab with this event open
    signal showEvent(string eventId)
    //! More tab with the sync page open
    signal showSync()
    //! More tab with a page or sheet: "library", "birthdays", "settings", "dayOverview", …
    signal showMore(string name)
    //! Pairing sheet: show this device's code and the devices nearby
    signal pairDevice()
    //! Both devices have data: ask how they go together
    signal decideJoin(string peerId, string peerName)
    //! Set on the setup screen ("data from another device"): open the sync page once the app is ready
    property bool openSyncOnStart: false
}
