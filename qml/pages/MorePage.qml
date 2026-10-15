// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "More": appearance, management (reminders, birthdays), reports (recital
// candidates), data and backups, settings, about.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

StackView {
    id: stack

    readonly property var reminders: { if (Data.revision < 0) return undefined; return Data.reminders() }
    readonly property var birthdays: { if (Data.revision < 0) return undefined; return Data.birthdays(0) }
    readonly property int candidates: { if (Data.revision < 0) return undefined; return Data.recitalCandidates().length }

    function openPage(component) {
        stack.pop(null)
        stack.push(component)
    }

    initialItem: Flickable {
        clip: true
        contentHeight: column.implicitHeight + 64
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}

        ColumnLayout {
            id: column
            width: Math.min(parent.width - 40, 620)
            x: 20
            y: 40
            spacing: 14

            Text {
                text: qsTrId("nav.more")
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 34
                font.weight: Font.ExtraBold
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: appearance.implicitHeight + 32
                radius: Theme.radius + 2
                color: Theme.surface
                AppearanceSettings {
                    id: appearance
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 16
                }
            }

            SectionLabel { Layout.topMargin: 6; text: qsTrId("more.manage") }
            Group {
                MenuRow {
                    iconName: "bell"
                    tile: Theme.tile.plum
                    text: qsTrId("reminders.title")
                    value: stack.reminders.length > 0 ? qsTrId("more.active_count", stack.reminders.length) : ""
                    onClicked: stack.push(remindersPage)
                }
                MenuRow {
                    divider: true
                    iconName: "cake"
                    tile: Theme.tile.orange
                    text: qsTrId("birthday.title")
                    value: stack.birthdays.length > 0 ? qsTrId("more.birthdays_today", stack.birthdays.length) : ""
                    onClicked: stack.push(birthdaysPage)
                }
            }

            SectionLabel { Layout.topMargin: 6; text: qsTrId("more.reports") }
            Group {
                MenuRow {
                    iconName: "star"
                    tile: Theme.tile.blue
                    text: qsTrId("candidates.recital.title")
                    value: stack.candidates > 0 ? String(stack.candidates) : ""
                    onClicked: stack.push(candidatesPage)
                }
            }

            SectionLabel { Layout.topMargin: 6; text: qsTrId("more.data") }
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: dataSettings.implicitHeight + 32
                radius: Theme.radius + 2
                color: Theme.surface
                DataSettings {
                    id: dataSettings
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 16
                }
            }

            Group {
                Layout.topMargin: 6
                MenuRow {
                    iconName: "settings"
                    text: qsTrId("settings.title")
                    onClicked: stack.push(settingsPage)
                }
                MenuRow {
                    divider: true
                    iconName: "info"
                    text: qsTrId("about.title")
                    value: Qt.application.version
                    chevron: false
                }
            }
        }
    }

    Component { id: remindersPage; RemindersPage { onBack: stack.pop() } }
    Component { id: birthdaysPage; BirthdaysPage { onBack: stack.pop() } }
    Component { id: candidatesPage; CandidatesPage { onBack: stack.pop() } }
    Component { id: settingsPage; SettingsPage { onBack: stack.pop() } }

    component Group: Rectangle {
        default property alias rows: groupColumn.data
        Layout.fillWidth: true
        implicitHeight: groupColumn.implicitHeight
        radius: Theme.radius + 2
        color: Theme.surface
        clip: true
        ColumnLayout {
            id: groupColumn
            width: parent.width
            spacing: 0
        }
    }
}
