// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Dates in the user's language. The booklet carries plain ISO dates ("2026-11-24");
// they are read at local noon so that no time zone moves them to another day.

pragma Singleton
import QtQuick

QtObject {
    function day(iso) {
        return iso ? new Date(iso + "T12:00:00") : new Date(NaN)
    }
    // formatId: an ID from the catalogue that holds the pattern for this language
    function date(iso, formatId) {
        const d = day(iso)
        return isNaN(d.getTime()) ? "" : d.toLocaleDateString(Qt.locale(), qsTrId(formatId))
    }
    // "today", "tomorrow", "in 4 days"
    function distance(days) {
        if (days === 0) return qsTrId("s.when.today")
        if (days === 1) return qsTrId("s.when.tomorrow")
        if (days > 1) return qsTrId("s.when.in_days", days)
        return ""
    }
    function time(dateTime) {
        return dateTime && !isNaN(dateTime.getTime()) ? dateTime.toLocaleTimeString(Qt.locale(), "HH:mm") : ""
    }
    function dateTime(dateTime) {
        if (!dateTime || isNaN(dateTime.getTime()))
            return ""
        const now = new Date()
        if (dateTime.toDateString() === now.toDateString())
            return qsTrId("s.when.today_at").arg(time(dateTime))
        return dateTime.toLocaleDateString(Qt.locale(), qsTrId("s.fmt.short")) + ", " + time(dateTime)
    }
}
