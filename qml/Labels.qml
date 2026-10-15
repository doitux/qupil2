// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Texts for the numbers stored in the database (weekdays, types, states).
// Functions, so they follow a language switch wherever they are bound.
pragma Singleton
import QtQuick

QtObject {
    //! 0 = Monday … 6 = Sunday, 7 = irregular
    function weekday(index) {
        switch (index) {
        case 0: return qsTrId("weekday.monday")
        case 1: return qsTrId("weekday.tuesday")
        case 2: return qsTrId("weekday.wednesday")
        case 3: return qsTrId("weekday.thursday")
        case 4: return qsTrId("weekday.friday")
        case 5: return qsTrId("weekday.saturday")
        case 6: return qsTrId("weekday.sunday")
        default: return qsTrId("common.irregular")
        }
    }
    function weekdayLabel(index) { return weekday(index) }

    function lessonKind(kind) {
        switch (kind) {
        case 2: return qsTrId("lesson.type.group")
        case 3: return qsTrId("lesson.type.ensemble")
        default: return qsTrId("lesson.type.individual")
        }
    }

    function pieceState(state) {
        switch (state) {
        case 1: return qsTrId("piece.status.in_progress")
        case 2: return qsTrId("piece.status.paused")
        case 3: return qsTrId("piece.status.ready")
        case 4: return qsTrId("piece.status.finished")
        default: return qsTrId("piece.status.planned")
        }
    }

    //! Regular activities: 0 ensemble, 1 theory, 2 accompanist rehearsal, 3 other
    function continuousKind(kind) {
        switch (kind) {
        case 1: return qsTrId("activity.type.theory")
        case 2: return qsTrId("activity.type.coaching")
        case 3: return qsTrId("common.other")
        default: return qsTrId("activity.type.ensemble")
        }
    }

    //! One-off activities: 0 solo, 1 ensemble participation, 2 other
    function singleKind(kind) {
        switch (kind) {
        case 1: return qsTrId("activity.type.ensemble_participation")
        case 2: return qsTrId("common.other")
        default: return qsTrId("activity.type.solo")
        }
    }

    //! Reminder: 0 when Qupil starts, 1 every lesson, 2 lessons of one student
    function reminderMode(mode, studentName) {
        switch (mode) {
        case 1: return qsTrId("reminder.mode.every_lesson")
        case 2: return qsTrId("reminder.mode.student").arg(studentName)
        default: return qsTrId("reminder.mode.startup")
        }
    }

    //! Recital interval index → label (Qupil 1.x: 0 never, then 1–24 months)
    readonly property var intervalMonths: [0, 1, 2, 3, 4, 6, 9, 12, 18, 24]
    function recitalInterval(index) {
        return index === 0 ? qsTrId("common.never") : qsTrId("common.duration_months", intervalMonths[index])
    }
}
