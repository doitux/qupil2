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
}
