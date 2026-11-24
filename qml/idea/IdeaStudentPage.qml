// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "Try the new idea": the page of one invented student. At the top what they work on
// (running tasks, below them folded: paused and done), then the history: one entry per
// lesson. Two buttons: "Prepare" before a lesson, "+ Lesson" after it – both open the
// same list (IdeaLessonSheet).

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Item {
    id: root

    property var demo
    property string studentId
    readonly property var s: demo ? (demo.revision, demo.student(studentId)) : null
    readonly property bool showTasks: !!s && (demo.revision, demo.showsTasks(s))
    readonly property var running: s ? (demo.revision, demo.tasksIn(s, "run")) : []
    readonly property var paused: s ? (demo.revision, demo.tasksIn(s, "pause")) : []
    readonly property var done: s ? (demo.revision, demo.tasksIn(s, "done")) : []
    //! What is unfolded: ids of tasks, dates of lessons
    property var unfolded: ({})
    property bool pausedOpen: false
    property bool doneOpen: false

    signal back()

    function flip(key) {
        const u = Object.assign({}, unfolded)
        u[key] = !u[key]
        unfolded = u
    }
    function ago(date) {
        const days = Math.round((new Date(s.today + "T12:00:00") - new Date(date + "T12:00:00")) / 86400000)
        return days <= 0 ? qsTrId("idea.today") : days < 14 ? qsTrId("time.days_ago", days)
                                                            : qsTrId("time.weeks_ago", Math.round(days / 7))
    }

    SubPage {
        anchors.fill: parent
        title: root.s ? root.s.name : ""
        backText: qsTrId("idea.title")
        onBack: root.back()

        Text {
            Layout.fillWidth: true
            Layout.topMargin: -8
            wrapMode: Text.WordWrap
            text: root.s ? root.s.sub : ""
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 15
        }
        Text {
            Layout.fillWidth: true
            Layout.topMargin: -6
            wrapMode: Text.WordWrap
            text: !root.s ? "" : (root.demo.revision, qsTrId("idea.next").arg(Data.formatDate(root.s.today, true))
                  + (!root.s.weekPassed ? "" : "\n" + (root.s.coupled ? qsTrId("idea.week_passed.answered").arg(root.s.name.split(" ")[0])
                                                                      : qsTrId("idea.week_passed"))))
            color: Theme.accentText
            font.family: Theme.fontFamily
            font.pixelSize: 14
            font.weight: Font.DemiBold
        }
        IdeaLink {
            visible: !!root.s && root.s.coupled
            Layout.topMargin: -6
            iconName: "chevron"
            text: root.s ? qsTrId("idea.pupil.open").arg(root.s.name.split(" ")[0]) : ""
            onClicked: pupilSheet.open()
        }

        // The pause of a task is over
        Repeater {
            model: root.s ? (root.demo.revision, root.demo.pauseOver(root.s)) : []
            Card {
                id: overCard
                required property var modelData
                Layout.fillWidth: true
                color: Theme.accentSoft
                spacing: 4
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: qsTrId("idea.pause.over").arg(overCard.modelData.title)
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                    font.weight: Font.DemiBold
                }
                IdeaLink {
                    iconName: "play"
                    text: qsTrId("idea.resume")
                    onClicked: root.demo.resume(root.studentId, overCard.modelData.id)
                }
            }
        }

        // What is prepared for the next lesson
        Card {
            id: preparedCard
            readonly property var prepared: root.s ? (root.demo.revision, root.s.prepared) : null
            visible: !!prepared
            Layout.fillWidth: true
            border.width: 1.5
            border.color: Theme.accentText
            spacing: 6
            Accessible.role: Accessible.Button
            Accessible.name: prepared ? qsTrId("idea.prepared.for").arg(Data.formatDate(prepared.date, true)) : ""
            Accessible.onPressAction: lessonSheet.start(true)
            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                Icon { Layout.preferredWidth: 17; Layout.preferredHeight: 17; name: "plan"; color: Theme.accentText }
                Text {
                    Layout.fillWidth: true
                    text: preparedCard.prepared ? qsTrId("idea.prepared.for").arg(Data.formatDate(preparedCard.prepared.date, true)) : ""
                    color: Theme.accentText
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                    font.weight: Font.ExtraBold
                }
            }
            Repeater {
                model: preparedCard.prepared ? root.running.filter(t => preparedCard.prepared.due.indexOf(t.id) >= 0) : []
                ColumnLayout {
                    id: due
                    required property var modelData
                    Layout.fillWidth: true
                    spacing: 4
                    IdeaPoint { text: due.modelData.title }
                    Repeater {
                        model: due.modelData.points.filter(p => p.priv && !p.done)
                        IdeaPoint {
                            required property var modelData
                            Layout.leftMargin: 20
                            text: modelData.text
                            priv: true
                        }
                    }
                }
            }
            Repeater {
                model: preparedCard.prepared && root.s ? (root.demo.revision, root.s.extra.slice()) : []
                IdeaPoint {
                    required property string modelData
                    text: modelData
                    priv: true
                }
            }
            Text {
                visible: text !== ""
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: preparedCard.prepared && preparedCard.prepared.note ? preparedCard.prepared.note : ""
                Accessible.role: Accessible.StaticText
                Accessible.name: text
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 15
            }
            TapHandler { onTapped: lessonSheet.start(true) }
        }

        SectionLabel {
            visible: root.showTasks
            Layout.topMargin: 6
            text: qsTrId("idea.tasks.heading") + " · " + qsTrId("idea.tasks.running", root.running.length)
        }
        Card {
            visible: root.showTasks
            Layout.fillWidth: true
            spacing: 12
            Text {
                visible: root.running.length === 0
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: qsTrId("idea.tasks.none")
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 15
            }
            Repeater {
                model: root.running
                ColumnLayout {
                    id: runRow
                    required property var modelData
                    required property int index
                    Layout.fillWidth: true
                    spacing: 12
                    Rectangle { visible: runRow.index > 0; Layout.fillWidth: true; implicitHeight: 1; color: Theme.divider }
                    IdeaTask {
                        title: runRow.modelData.title
                        status: root.demo.status(root.s, runRow.modelData)
                        weeks: runRow.modelData.weeks
                        points: runRow.modelData.points
                        badge: runRow.modelData.since === root.demo.addDays(root.s.today, -7) && runRow.modelData.weeks.length === 0
                               ? qsTrId("idea.new") : ""
                        open: !!root.unfolded[runRow.modelData.id]
                        onToggled: root.flip(runRow.modelData.id)
                        editable: true
                        onEdit: taskSheet.start(runRow.modelData.id)
                    }
                }
            }
            // A task in between, without a lesson
            IdeaLink {
                iconName: "plus"
                text: qsTrId("idea.task.add")
                onClicked: taskSheet.start("")
            }
            Rectangle {
                visible: root.paused.length + root.done.length > 0
                Layout.fillWidth: true
                implicitHeight: 1
                color: Theme.divider
            }
            IdeaFold {
                visible: root.paused.length > 0
                iconName: "pause"
                text: qsTrId("idea.paused.count").arg(root.paused.length)
                open: root.pausedOpen
                onClicked: root.pausedOpen = !root.pausedOpen
            }
            Repeater {
                model: root.pausedOpen ? root.paused : []
                ColumnLayout {
                    id: pausedRow
                    required property var modelData
                    Layout.fillWidth: true
                    Layout.leftMargin: 26
                    spacing: 0
                    Text {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: pausedRow.modelData.title
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.DemiBold
                    }
                    Text {
                        visible: pausedRow.modelData.until !== ""
                        text: qsTrId("idea.until").arg(Data.formatDate(pausedRow.modelData.until))
                        color: Theme.textMuted
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                    }
                    RowLayout {
                        spacing: 18
                        IdeaLink {
                            iconName: "play"
                            text: qsTrId("idea.resume")
                            onClicked: root.demo.resume(root.studentId, pausedRow.modelData.id)
                        }
                        IdeaLink {
                            iconName: "edit"
                            text: qsTrId("common.edit")
                            Accessible.name: qsTrId("idea.task.edit.name").arg(pausedRow.modelData.title)
                            onClicked: taskSheet.start(pausedRow.modelData.id)
                        }
                    }
                }
            }
            IdeaFold {
                visible: root.done.length > 0
                iconName: "check"
                tint: Theme.success
                text: qsTrId("idea.done.count").arg(root.done.length)
                open: root.doneOpen
                onClicked: root.doneOpen = !root.doneOpen
            }
            Repeater {
                model: root.doneOpen ? root.done.slice().sort((a, b) => a.doneOn < b.doneOn ? 1 : -1) : []
                RowLayout {
                    id: doneRow
                    required property var modelData
                    Layout.fillWidth: true
                    Layout.leftMargin: 26
                    spacing: 8
                    Text {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: doneRow.modelData.title
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                    }
                    Text {
                        text: Data.formatDate(doneRow.modelData.doneOn)
                        color: Theme.textMuted
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                    }
                }
            }
        }

        SectionLabel { Layout.topMargin: 6; text: qsTrId("idea.history") }
        Repeater {
            model: root.s ? (root.demo.revision, root.s.log.slice()) : []
            IdeaEntry {
                required property var modelData
                entry: modelData
                demo: root.demo
                student: root.s
                onEdit: entrySheet.start(modelData.id)
                ago: root.ago(modelData.date)
                open: !!root.unfolded[modelData.id]
                onToggled: root.flip(modelData.id)
            }
        }
        // Room for the buttons
        Item { Layout.preferredHeight: 110 }
    }

    Fab {
        anchors.right: parent.right
        anchors.bottom: lessonFab.top
        anchors.rightMargin: 20
        anchors.bottomMargin: 12
        outlined: true
        iconName: "plan"
        text: qsTrId("idea.prepare")
        onClicked: lessonSheet.start(true)
    }
    Fab {
        id: lessonFab
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 20
        anchors.bottomMargin: 20 + Nav.playerInset
        text: qsTrId("idea.lesson")
        onClicked: lessonSheet.start(false)
    }

    IdeaLessonSheet {
        id: lessonSheet
        demo: root.demo
        studentId: root.studentId
    }
    IdeaTaskSheet {
        id: taskSheet
        demo: root.demo
        studentId: root.studentId
    }
    IdeaEntrySheet {
        id: entrySheet
        demo: root.demo
        studentId: root.studentId
    }
    IdeaPupilSheet {
        id: pupilSheet
        demo: root.demo
        studentId: root.studentId
    }
}
