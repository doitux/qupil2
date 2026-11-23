// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "Try the new idea": a look into the student's app as it would be. No dates and no
// weeks: what runs (asked anew every week how it went), what is on pause (not asked),
// and what is done (collected). An answer given here arrives on the teacher's side.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: sheet

    property var demo
    property string studentId
    readonly property var s: demo && visible ? (demo.revision, demo.student(studentId)) : null
    readonly property var running: s ? (demo.revision, demo.tasksIn(s, "run")) : []
    readonly property var paused: s ? (demo.revision, demo.tasksIn(s, "pause")) : []
    readonly property var done: s ? (demo.revision, demo.tasksIn(s, "done")) : []

    title: qsTrId("booklet.title")
    showAccept: false
    rejectText: qsTrId("common.close")

    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: sheet.s ? qsTrId("idea.pupil.hint").arg(sheet.s.name.split(" ")[0]) : ""
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 14
    }

    SectionLabel { text: qsTrId("idea.pupil.running") + " · " + qsTrId("idea.tasks.count", sheet.running.length) }
    Repeater {
        model: sheet.running
        Card {
            id: card
            required property var modelData
            readonly property var fb: modelData.feedback
            Layout.fillWidth: true
            padding: 14
            spacing: 8
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: card.modelData.title
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 16
                font.weight: Font.Bold
            }
            Repeater {
                model: card.modelData.points.filter(p => !p.priv)
                IdeaPoint {
                    required property var modelData
                    text: modelData.text
                    done: modelData.done
                    mark: modelData.done ? "✓" : "–"
                }
            }
            Text {
                Layout.fillWidth: true
                Layout.topMargin: 2
                text: qsTrId("idea.pupil.question")
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 13
            }
            Flow {
                Layout.fillWidth: true
                spacing: 6
                Repeater {
                    model: [3, 2, 1]
                    Chip {
                        required property int modelData
                        kind: "filter"
                        text: sheet.demo.amountText[modelData]
                        selected: !!card.fb && card.fb.amount === modelData
                        Accessible.name: card.modelData.title + ": " + text
                        onClicked: sheet.demo.answer(sheet.studentId, card.modelData.id, modelData, card.fb ? card.fb.how : 1)
                    }
                }
            }
            Flow {
                Layout.fillWidth: true
                spacing: 6
                Repeater {
                    model: [2, 1, 0]
                    Chip {
                        required property int modelData
                        kind: "filter"
                        text: sheet.demo.howText[modelData]
                        selected: !!card.fb && card.fb.how === modelData
                        Accessible.name: card.modelData.title + ": " + text
                        onClicked: sheet.demo.answer(sheet.studentId, card.modelData.id, card.fb ? card.fb.amount : 2, modelData)
                    }
                }
            }
        }
    }
    Text {
        visible: sheet.running.length === 0
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: qsTrId("idea.tasks.none")
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 15
    }

    SectionLabel { visible: sheet.paused.length > 0; text: qsTrId("idea.paused.count").arg(sheet.paused.length) }
    Card {
        visible: sheet.paused.length > 0
        Layout.fillWidth: true
        padding: 14
        spacing: 6
        Repeater {
            model: sheet.paused
            Text {
                required property var modelData
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: modelData.title
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 15
            }
        }
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTrId("idea.pupil.paused_hint")
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 13
        }
    }

    SectionLabel { visible: sheet.done.length > 0; text: qsTrId("idea.pupil.done") + " · " + sheet.done.length }
    Card {
        visible: sheet.done.length > 0
        Layout.fillWidth: true
        padding: 14
        spacing: 8
        Repeater {
            model: sheet.done.slice().sort((a, b) => a.doneOn < b.doneOn ? 1 : -1)
            RowLayout {
                id: doneRow
                required property var modelData
                Layout.fillWidth: true
                spacing: 10
                Rectangle {
                    implicitWidth: 22
                    implicitHeight: 22
                    radius: 11
                    color: Theme.success
                    Icon { anchors.centerIn: parent; width: 14; height: 14; name: "check"; color: Theme.surface; strokeWidth: 2.8 }
                }
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: doneRow.modelData.title
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                    font.weight: Font.DemiBold
                }
            }
        }
    }
}
