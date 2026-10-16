// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// One archive entry: the record as it was saved, PDF, delete.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page

    property string entity: "student_archive"
    property string entryId
    readonly property var entry: { if (Data.revision < 0) return undefined; return Data.get(page.entity, page.entryId) }
    // Stored packed; entries of Qupil 1.x come in the compact layout
    readonly property string html: { if (Data.revision < 0) return ""; return Docs.archiveHtml(page.entity, page.entryId) }
    readonly property string name: page.entity === "student_archive"
                                    ? [page.entry.forename, page.entry.surname].filter(x => x).join(" ")
                                    : (page.entry.title || "")

    title: page.name
    backText: page.entity === "student_archive" ? qsTrId("nav.student_archive") : qsTrId("events.archive.title")

    actions: [
        IconButton {
            iconName: "document"
            text: qsTrId("doc.share_pdf")
            onClicked: Docs.archiveEntry(page.entity, page.entryId)
        },
        IconButton {
            iconName: "trash"
            text: qsTrId("archive.delete_entry")
            onClicked: confirmDelete.open()
        }
    ]

    // The record on white paper, as it will be printed
    Rectangle {
        Layout.fillWidth: true
        implicitHeight: record.implicitHeight + 40
        radius: Theme.radius
        color: "#FFFFFF"
        border.width: Theme.dark ? 0 : 1
        border.color: Theme.border
        TextEdit {
            id: record
            x: 20
            y: 20
            width: parent.width - 40
            readOnly: true
            selectByMouse: true
            textFormat: TextEdit.RichText
            wrapMode: TextEdit.Wrap
            text: page.html
            color: "#1D1A22"
            font.family: Theme.fontFamily
            font.pixelSize: 14
        }
    }

    AppDialog {
        id: confirmDelete
        iconName: "trash"
        destructive: true
        title: qsTrId("archive.delete_entry")
        text: qsTrId("archive.delete.confirm").arg(page.name)
        rejectText: qsTrId("common.cancel")
        acceptText: qsTrId("common.delete")
        onAccepted: {
            Data.remove(page.entity, page.entryId)
            page.back()
        }
    }
}
