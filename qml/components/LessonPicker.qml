// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Choosing one lesson out of a whole week: a field that shows the lesson the way one
// knows it (the student for an individual lesson, else its name; below kind, day and
// time), and on a tap a page of all lessons by weekday starting today – a search for
// students and lesson names, a bar of the days with lessons to jump, the day's heading
// stays at the top while scrolling. The automatic names ("EU-45-Rau-LenMus") are not
// shown: they say little. Data from Data.reminderLessons().

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

ColumnLayout {
    id: picker

    property string label
    property var lessons: []        // Data.reminderLessons()
    property string lessonId        // the chosen one
    signal picked(string id)

    readonly property var chosen: lessons.find(l => l.id === lessonId) || null
    //! Today's weekday, 0 = Monday (Qupil's day, also when a day is pretended)
    readonly property int today: {
        if (Data.revision < 0)
            return 0
        const d = new Date(Data.today() + "T12:00:00")
        return (d.getDay() + 6) % 7
    }

    //! Kind and length ("Einzelunterricht · 45 Min") or kind and size ("Ensemble · 5 Schüler")
    function describe(l) {
        const kind = Labels.lessonKind(l.kind)
        if (l.kind === 3 || l.minutes <= 0)
            return kind + " · " + qsTrId("summary.students", (l.students || []).length)
        return kind + " · " + qsTrId("lessonpick.minutes").arg(l.minutes)
    }
    //! The day heading of a lesson: today first
    function dayHeading(l) {
        if (!l.time)
            return l.dayLong
        return l.weekday === today ? qsTrId("lessonpick.today").arg(l.dayLong) : l.dayLong
    }

    spacing: 6
    Layout.fillWidth: true

    FieldLabel { label: picker.label }

    // The field
    AbstractButton {
        id: field
        Layout.fillWidth: true
        implicitHeight: Math.max(56, fieldRow.implicitHeight + 16)
        focusPolicy: Qt.StrongFocus
        Accessible.role: Accessible.ComboBox
        Accessible.name: picker.label
        Accessible.description: picker.chosen ? picker.chosen.title + ", " + picker.chosen.dayLong + " " + picker.chosen.time : ""
        onClicked: sheet.openAt(picker.lessonId)
        background: Rectangle {
            radius: 12
            color: field.pressed ? Theme.surfaceAlt : Theme.surface
            border.width: field.visualFocus || sheet.opened ? 2 : 1
            border.color: field.visualFocus || sheet.opened ? Theme.accent : Theme.border
        }
        contentItem: RowLayout {
            id: fieldRow
            spacing: 10
            LessonBadge {
                visible: !!picker.chosen
                lesson: picker.chosen
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 1
                Text {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: picker.chosen ? picker.chosen.title : qsTrId("lesson.choose")
                    color: picker.chosen ? Theme.text : Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                    font.weight: picker.chosen ? Font.DemiBold : Font.Normal
                }
                Text {
                    visible: !!picker.chosen
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: picker.chosen ? Labels.lessonKind(picker.chosen.kind) + " · "
                                          + (picker.chosen.time ? picker.chosen.dayLong + " " + picker.chosen.time
                                                                : picker.chosen.dayLong) : ""
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                }
            }
            Icon {
                Layout.preferredWidth: 18
                Layout.preferredHeight: 18
                name: "chevron"
                color: Theme.icon
            }
        }
        leftPadding: 12
        rightPadding: 12
    }

    // The page of all lessons
    Popup {
        id: sheet
        parent: Overlay.overlay
        modal: true
        focus: true
        padding: 0
        readonly property bool compact: parent ? parent.width < 720 : true
        width: parent ? Math.min(parent.width, 560) : 400
        height: parent ? Math.min(parent.height - (sheet.compact ? 24 : 80), 820) : 700
        x: parent ? (parent.width - width) / 2 : 0
        y: parent ? (sheet.compact ? parent.height - height : (parent.height - height) / 2) : 0
        bottomMargin: Nav.keyboardInset
        property string query
        //! Sorted from today on; irregular ones at the end; filtered by the search
        readonly property var shown: {
            const q = query.trim().toLowerCase()
            const order = l => l.time ? (l.weekday - picker.today + 7) % 7 : 8
            return picker.lessons
                .filter(l => q === "" || (l.title + " " + l.name + " " + (l.students || []).join(" ")).toLowerCase().indexOf(q) >= 0)
                .map(l => Object.assign({ heading: picker.dayHeading(l), order: order(l) }, l))
                .sort((a, b) => a.order - b.order || (a.time < b.time ? -1 : a.time > b.time ? 1 : 0))
        }
        //! The days with lessons, for the bar
        readonly property var days: {
            const seen = []
            for (const l of shown)
                if (l.time && !seen.some(d => d.weekday === l.weekday))
                    seen.push({ weekday: l.weekday, day: l.day })
            return seen
        }
        property int topDay: -1

        function openAt(id) {
            query = ""
            open()
            const index = shown.findIndex(l => l.id === id)
            list.positionViewAtIndex(Math.max(0, index), index > 0 ? ListView.Center : ListView.Beginning)
            updateTopDay()
        }
        function jumpTo(weekday) {
            const index = shown.findIndex(l => l.time && l.weekday === weekday)
            if (index >= 0)
                list.positionViewAtIndex(index, ListView.Beginning)
            updateTopDay()
        }
        function updateTopDay() {
            const index = list.indexAt(10, list.contentY + 30)
            topDay = index >= 0 && shown[index].time ? shown[index].weekday : -1
        }

        enter: Transition { NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 140 } }
        exit: Transition { NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 120 } }
        background: Rectangle {
            color: Theme.surface
            radius: 20
            Rectangle {   // square bottom corners on the phone, where it comes up from the edge
                visible: sheet.compact
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                height: 20
                color: Theme.surface
            }
        }

        contentItem: ColumnLayout {
            spacing: 8
            // Head
            RowLayout {
                Layout.fillWidth: true
                Layout.topMargin: 12
                Layout.leftMargin: 8
                Layout.rightMargin: 16
                ActionButton {
                    kind: "quiet"
                    text: qsTrId("common.cancel")
                    onClicked: sheet.close()
                }
                Text {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    text: qsTrId("lesson.choose")
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 17
                    font.weight: Font.ExtraBold
                    elide: Text.ElideRight
                }
                Item { Layout.preferredWidth: 60 }
            }
            // Search
            InputField {
                id: search
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                placeholder: qsTrId("lessonpick.search")
                text: sheet.query
                onEdited: (value) => { sheet.query = value; list.positionViewAtBeginning(); sheet.updateTopDay() }
            }
            // The days
            RowLayout {
                visible: sheet.days.length > 1 && sheet.query.trim() === ""
                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16
                spacing: 6
                Repeater {
                    model: sheet.days
                    delegate: AbstractButton {
                        id: dayChip
                        required property var modelData
                        readonly property bool current: modelData.weekday === sheet.topDay
                        Layout.fillWidth: true
                        implicitHeight: 34
                        text: modelData.day
                        Accessible.name: Labels.weekday(modelData.weekday)
                        onClicked: sheet.jumpTo(modelData.weekday)
                        background: Rectangle {
                            radius: 10
                            color: dayChip.current ? Theme.accent : dayChip.pressed ? Theme.accentSoft : Theme.surfaceAlt
                        }
                        contentItem: Text {
                            text: dayChip.text
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: dayChip.current ? Theme.textOnAccent : Theme.textSecondary
                            font.family: Theme.fontFamily
                            font.pixelSize: 13
                            font.weight: Font.Bold
                        }
                    }
                }
            }
            // The lessons
            ListView {
                id: list
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: sheet.shown
                section.property: "heading"
                section.labelPositioning: ViewSection.InlineLabels | ViewSection.CurrentLabelAtStart
                section.delegate: Rectangle {
                    required property string section
                    width: ListView.view.width
                    height: 34
                    color: Theme.surface
                    z: 2
                    Text {
                        anchors.left: parent.left
                        anchors.leftMargin: 18
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 6
                        text: parent.section.toUpperCase()
                        color: Theme.accentText
                        font.family: Theme.fontFamily
                        font.pixelSize: 12
                        font.weight: Font.ExtraBold
                        font.letterSpacing: 0.8
                    }
                }
                onContentYChanged: sheet.updateTopDay()
                ScrollIndicator.vertical: ScrollIndicator {}
                footer: Item { width: 1; height: 16 }

                delegate: AbstractButton {
                    id: row
                    required property var modelData
                    readonly property bool chosen: modelData.id === picker.lessonId
                    readonly property string matchedStudent: {
                        const q = sheet.query.trim().toLowerCase()
                        if (q === "" || modelData.title.toLowerCase().indexOf(q) >= 0)
                            return ""
                        return (modelData.students || []).find(s => s.toLowerCase().indexOf(q) >= 0) || ""
                    }
                    width: ListView.view.width
                    implicitHeight: 60
                    Accessible.name: modelData.title + " · " + (modelData.time ? modelData.day + " " + modelData.time : modelData.dayLong)
                    onClicked: {
                        picker.picked(modelData.id)
                        sheet.close()
                    }
                    background: Rectangle {
                        anchors.fill: parent
                        anchors.leftMargin: 10
                        anchors.rightMargin: 10
                        radius: 12
                        color: row.pressed ? Theme.accentSoft : row.chosen ? Theme.accentSoft : "transparent"
                    }
                    contentItem: RowLayout {
                        spacing: 12
                        Text {
                            Layout.preferredWidth: 46
                            text: row.modelData.time
                            color: Theme.textSecondary
                            font.family: Theme.fontFamily
                            font.pixelSize: 14
                            font.weight: Font.Bold
                        }
                        LessonBadge { lesson: row.modelData }
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 1
                            Text {
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                                text: row.modelData.title
                                color: Theme.text
                                font.family: Theme.fontFamily
                                font.pixelSize: 15
                                font.weight: Font.DemiBold
                            }
                            Text {
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                                text: row.matchedStudent
                                      ? Labels.lessonKind(row.modelData.kind) + " · " + qsTrId("lessonpick.with").arg(row.matchedStudent)
                                      : picker.describe(row.modelData)
                                color: Theme.textMuted
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                            }
                        }
                        Icon {
                            visible: row.chosen
                            Layout.preferredWidth: 18
                            Layout.preferredHeight: 18
                            name: "check"
                            color: Theme.accentText
                            strokeWidth: 2.4
                        }
                    }
                    leftPadding: 18
                    rightPadding: 20
                }
            }
            EmptyHint {
                visible: sheet.shown.length === 0
                Layout.fillWidth: true
                Layout.margins: 16
                text: qsTrId("lessonpick.none")
            }
        }
    }
}
