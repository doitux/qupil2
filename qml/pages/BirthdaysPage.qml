// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Birthdays of the next weeks.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page
    title: qsTrId("birthday.title")
    readonly property var birthdays: { if (Data.revision < 0) return undefined; return Data.birthdays(60) }

    EmptyHint {
        visible: page.birthdays.length === 0
        text: qsTrId("birthday.none_soon")
    }
    Rectangle {
        visible: page.birthdays.length > 0
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
                model: page.birthdays
                delegate: AbstractButton {
                    id: row
                    required property var modelData
                    required property int index
                    Layout.fillWidth: true
                    implicitHeight: 60
                    onClicked: Nav.showStudent(modelData.id, "")
                    background: Rectangle {
                        color: row.modelData.days === 0 ? Theme.accentSoft : "transparent"
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
                                text: qsTrId("today.birthday_turns").arg(row.modelData.name).arg(row.modelData.turns)
                                color: Theme.text
                                font.family: Theme.fontFamily
                                font.pixelSize: 15
                                font.weight: Font.Bold
                            }
                            Text {
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                                text: row.modelData.days === 0 ? qsTrId("time.today")
                                                               : Data.formatDate(row.modelData.date, true) + " · " + qsTrId("time.in_days", row.modelData.days)
                                color: Theme.textMuted
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                            }
                        }
                        Icon {
                            visible: row.modelData.days === 0
                            Layout.preferredWidth: 22
                            Layout.preferredHeight: 22
                            name: "cake"
                            color: Theme.accentText
                        }
                    }
                    leftPadding: 14
                    rightPadding: 14
                }
            }
        }
    }
}
