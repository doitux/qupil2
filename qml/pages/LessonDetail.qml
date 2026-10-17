// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// One lesson: time, place, students. Note for everybody, edit the lesson.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Item {
    id: detail

    property string lessonId
    property bool showBack: true
    property string backText: qsTrId("common.back")
    readonly property var lesson: { if (Data.revision < 0) return undefined; return Data.lesson(lessonId) }
    readonly property string lessonName: lesson.name || ""

    signal back()
    signal studentClicked(string studentId, string membershipId)

    Flickable {
        id: flick
        anchors.fill: parent
        clip: true
        contentHeight: column.implicitHeight + 60
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}

        ColumnLayout {
            id: column
            x: 20
            y: detail.showBack ? 12 : 28
            width: flick.width - 40
            spacing: 16

            PageHeader {
                visible: detail.showBack
                Layout.leftMargin: -8
                backText: detail.backText
                onBack: detail.back()
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 12
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 3
                    Text {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: detail.lessonName
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 26
                        font.weight: Font.ExtraBold
                    }
                    Text {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: {
                            const l = detail.lesson
                            const parts = [Labels.lessonKind(l.kind || 1)]
                            if (l.irregular === 1)
                                parts.push(qsTrId("common.irregular"))
                            else if (l.start_time)
                                parts.push(Labels.weekday(l.weekday) + " " + l.start_time + " – " + l.end_time)
                            if (l.location)
                                parts.push(l.location)
                            return parts.join(" · ")
                        }
                        color: Theme.textMuted
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                    }
                }
                IconButton {
                    iconName: "edit"
                    text: qsTrId("lesson.edit")
                    onClicked: Nav.editLesson(detail.lessonId, -1)
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                ActionButton {
                    kind: "primary"
                    iconName: "plus"
                    text: (detail.lesson.studentCount || 0) > 1 ? qsTrId("lesson.note_for_all") : qsTrId("common.note")
                    enabled: (detail.lesson.studentCount || 0) > 0
                    onClicked: Nav.editNote(detail.lesson.students[0].membership_id, "")
                }
                ActionButton {
                    iconName: "metronome"
                    text: qsTrId("metronome.title")
                    onClicked: Nav.showPage(3)
                }
            }

            SectionLabel { text: qsTrId("lesson.label.students") + " · " + (detail.lesson.studentCount || 0) }

            EmptyHint {
                visible: (detail.lesson.studentCount || 0) === 0
                text: qsTrId("lesson.no_students")
            }

            Rectangle {
                visible: (detail.lesson.studentCount || 0) > 0
                Layout.fillWidth: true
                implicitHeight: studentColumn.implicitHeight
                radius: Theme.radius
                color: Theme.surface
                clip: true
                ColumnLayout {
                    id: studentColumn
                    width: parent.width
                    spacing: 0
                    Repeater {
                        model: detail.lesson.students || []
                        delegate: AbstractButton {
                            id: row
                            required property var modelData
                            required property int index
                            Layout.fillWidth: true
                            implicitHeight: implicitContentHeight + topPadding + bottomPadding
                            topPadding: 12
                            bottomPadding: 12
                            focusPolicy: Qt.StrongFocus
                            hoverEnabled: true
                            Accessible.role: Accessible.Button
                            Accessible.name: modelData.name
                            onClicked: detail.studentClicked(modelData.id, modelData.membership_id)
                            background: Rectangle {
                                color: row.down ? Theme.surfaceAlt : row.hovered ? Theme.surfaceSunken : "transparent"
                                Rectangle {
                                    visible: row.index > 0
                                    width: parent.width
                                    height: 1
                                    color: Theme.divider
                                }
                            }
                            contentItem: RowLayout {
                                id: rowContent
                                spacing: 12
                                Avatar {
                                    size: 40
                                    text: row.modelData.initials
                                    seed: row.modelData.name
                                }
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 1
                                    Text {
                                        Layout.fillWidth: true
                                        elide: Text.ElideRight
                                        text: row.modelData.name
                                        color: Theme.text
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 15
                                        font.weight: Font.Bold
                                    }
                                    Text {
                                        Layout.fillWidth: true
                                        elide: Text.ElideRight
                                        text: row.modelData.lastNote
                                              ? Data.formatDate(row.modelData.lastNoteDate) + ": " + row.modelData.lastNote
                                              : [row.modelData.instrument, row.modelData.instrument_size].filter(Boolean).join(" ")
                                        color: Theme.textMuted
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 13
                                    }
                                }
                                Icon {
                                    Layout.preferredWidth: 20
                                    Layout.preferredHeight: 20
                                    name: "chevron"
                                    color: Theme.icon
                                }
                            }
                            leftPadding: 14
                            rightPadding: 14
                        }
                    }
                }
            }

            ColumnLayout {
                visible: (detail.lesson.reminders || []).length > 0
                Layout.fillWidth: true
                spacing: 8
                SectionLabel { text: qsTrId("reminders.title") }
                Repeater {
                    model: detail.lesson.reminders || []
                    delegate: AbstractButton {
                        id: reminderRow
                        required property var modelData
                        Layout.fillWidth: true
                        implicitHeight: implicitContentHeight + topPadding + bottomPadding
                        topPadding: 12
                        bottomPadding: 12
                        onClicked: Nav.editReminder(modelData.id, "")
                        background: Rectangle {
                            radius: Theme.radius
                            color: Theme.surface
                        }
                        contentItem: RowLayout {
                            id: reminderText
                            spacing: 10
                            Icon {
                                Layout.preferredWidth: 20
                                Layout.preferredHeight: 20
                                name: "bell"
                                color: Theme.accentText
                            }
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 1
                                Text {
                                    Layout.fillWidth: true
                                    wrapMode: Text.WordWrap
                                    text: reminderRow.modelData.text
                                    color: Theme.text
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 15
                                    font.weight: Font.DemiBold
                                }
                                Text {
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                    text: Labels.reminderMode(reminderRow.modelData.mode, reminderRow.modelData.studentName)
                                    color: Theme.textMuted
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 13
                                }
                            }
                        }
                        leftPadding: 14
                        rightPadding: 14
                    }
                }
            }
        }
    }
}
