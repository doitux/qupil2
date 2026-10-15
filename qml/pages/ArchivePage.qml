// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Archived students or events: read-only records, as PDF to print or share.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page

    //! "student_archive" or "event_archive"
    property string entity: "student_archive"
    readonly property bool students: entity === "student_archive"
    readonly property var entries: {
        if (Data.revision < 0) return undefined
        return page.students ? Data.studentArchive() : Data.eventArchive()
    }

    signal open(string entryId)

    title: students ? qsTrId("nav.student_archive") : qsTrId("events.archive.title")

    Text {
        visible: page.students
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: qsTrId("archive.hint")
        color: Theme.textSecondary
        font.family: Theme.fontFamily
        font.pixelSize: 14
    }
    EmptyHint {
        visible: page.entries.length === 0
        text: page.students ? qsTrId("archive.empty") : qsTrId("events.archive.empty")
    }
    Repeater {
        model: page.entries
        delegate: ListRow {
            required property var modelData
            avatar: page.students ? modelData.initials : ""
            iconName: page.students ? "" : "archive"
            tile: page.students ? null : Theme.tile.gray
            title: page.students ? modelData.name : modelData.title
            subtitle: page.students && modelData.archived_on ? qsTrId("archive.archived_on").arg(Data.formatDate(modelData.archived_on)) : ""
            onClicked: page.open(modelData.id)
        }
    }
}
