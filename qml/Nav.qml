// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// App-wide requests between pages and the editors that live in Main.qml.
//   Nav.editNote(membershipId, "")   → Main.qml opens the note editor
pragma Singleton
import QtQuick

QtObject {
    // ------------------------------------------------------------ editors (handled by Main.qml)
    signal newStudent()
    signal editLesson(string lessonId, int weekday)
    signal editNote(string membershipId, string noteId)
    //! A new note (today) that starts with the text of an older one – as in Qupil 1.x,
    //! where tapping a note copied it into the field for the next one
    signal newNoteFrom(string membershipId, string noteId)
    signal editPiece(string membershipId, string pieceId)
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
