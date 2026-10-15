// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Students whose last recital is longer ago than their personal interval.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page
    title: qsTrId("candidates.recital.title")
    readonly property var candidates: { if (Data.revision < 0) return undefined; return Data.recitalCandidates() }

    function ago(days) {
        return days > 30 ? qsTrId("candidates.ago.months", Math.floor(days / 30)) : qsTrId("candidates.ago.days", days)
    }

    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: page.candidates.length > 0 ? qsTrId("candidates.recital.found") : qsTrId("candidates.recital.none")
        color: Theme.textSecondary
        font.family: Theme.fontFamily
        font.pixelSize: 15
    }
    Rectangle {
        visible: page.candidates.length > 0
        Layout.fillWidth: true
        implicitHeight: list.implicitHeight
        radius: Theme.radius
        color: Theme.surface
        clip: true
        ColumnLayout {
            id: list
            width: parent.width
            spacing: 0
            Repeater {
                model: page.candidates
                delegate: AbstractButton {
                    id: row
                    required property var modelData
                    required property int index
                    Layout.fillWidth: true
                    implicitHeight: 64
                    onClicked: Nav.showStudent(modelData.id, "")
                    onPressAndHold: rowMenu.popup()
                    background: Rectangle {
                        color: row.down ? Theme.surfaceAlt : "transparent"
                        Rectangle {
                            visible: row.index > 0
                            width: parent.width
                            height: 1
                            color: Theme.divider
                        }
                    }
                    contentItem: RowLayout {
                        spacing: 12
                        Avatar {
                            size: 38
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
                                text: (row.modelData.hasRecital ? qsTrId("candidates.recital.col_last") : qsTrId("student.lesson_since"))
                                      + ": " + Data.formatDate(row.modelData.lastDate) + " · " + page.ago(row.modelData.days)
                                color: Theme.textMuted
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                            }
                        }
                        IconButton {
                            iconName: "bell"
                            fill: "transparent"
                            tint: Theme.accentText
                            text: qsTrId("candidates.recital.add_reminder")
                            onClicked: {
                                Data.create("reminder", { text: qsTrId("candidates.recital.reminder_text"), mode: 2,
                                                         student_id: row.modelData.id, sound: 1 })
                                Nav.toast(qsTrId("candidates.recital.reminder_created").arg(row.modelData.name))
                            }
                        }
                    }
                    leftPadding: 14
                    rightPadding: 6
                    Menu {
                        id: rowMenu
                        MenuItem {
                            text: qsTrId("candidates.recital.add_reminder")
                            onTriggered: Data.create("reminder", { text: qsTrId("candidates.recital.reminder_text"), mode: 2,
                                                                   student_id: row.modelData.id, sound: 1 })
                        }
                    }
                }
            }
        }
    }
    Text {
        visible: page.candidates.length > 0
        text: qsTrId("candidates.recital.total", page.candidates.length)
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 13
    }
}
