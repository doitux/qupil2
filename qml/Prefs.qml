// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Settings of this device only (not synchronised), shared by all pages.
pragma Singleton
import QtQuick
import QtCore

QtObject {
    id: prefs

    //! Sound and message shortly before a lesson ends (Qupil 1.x: lesson end message)
    property bool lessonEndAlert: false
    property int lessonEndMinutes: 5

    property Settings _store: Settings {
        category: "lessonEnd"
        property alias enabled: prefs.lessonEndAlert
        property alias minutes: prefs.lessonEndMinutes
    }
}
