// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// More › Ensemble candidates: students who should play in an ensemble but
// currently do not (Qupil 1.x: "Ensemble candidates").

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page
    title: qsTrId("candidates.ensemble.title")
    readonly property var result: { if (Data.revision < 0) return undefined; return Data.ensembleCandidates() }
    readonly property var candidates: page.result.candidates

    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: page.candidates.length > 0 ? qsTrId("candidates.ensemble.found") : qsTrId("candidates.ensemble.none")
        color: Theme.textSecondary
        font.family: Theme.fontFamily
        font.pixelSize: 15
    }
    Repeater {
        model: page.candidates
        delegate: ListRow {
            id: row
            required property var modelData
            avatar: modelData.initials
            title: modelData.name
            subtitle: qsTrId("candidates.ensemble.col_last") + ": "
                      + (modelData.lastDate ? Data.formatDate(modelData.lastDate) + " (" + modelData.lastEnsemble + ")"
                                            : qsTrId("common.none"))
            chevron: false
            onClicked: Nav.showStudent(modelData.id, "")
            IconButton {
                iconName: "bell"
                fill: "transparent"
                tint: Theme.accentText
                text: qsTrId("candidates.ensemble.add_reminder")
                onClicked: {
                    Data.create("reminder", { text: qsTrId("candidates.ensemble.reminder_text"), mode: 2,
                                             student_id: row.modelData.id, sound: 1 })
                    Nav.toast(qsTrId("candidates.ensemble.reminder_created").arg(row.modelData.name))
                }
            }
        }
    }
    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: qsTrId("candidates.ensemble.total").arg(page.candidates.length).arg(page.result.active)
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 13
    }
}
