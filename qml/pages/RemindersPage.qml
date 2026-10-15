// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// All reminders with when they appear.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page
    title: qsTrId("reminders.title")
    readonly property var reminders: { if (Data.revision < 0) return undefined; return Data.reminders() }

    actions: IconButton {
        filled: true
        iconName: "plus"
        text: qsTrId("common.add")
        onClicked: Nav.editReminder("", "")
    }

    EmptyHint {
        visible: page.reminders.length === 0
        text: qsTrId("reminders.empty")
    }
    Repeater {
        model: page.reminders
        delegate: AbstractButton {
            id: row
            required property var modelData
            Layout.fillWidth: true
            implicitHeight: rowContent.implicitHeight + 24
            focusPolicy: Qt.StrongFocus
            Accessible.role: Accessible.Button
            Accessible.name: modelData.text
            onClicked: Nav.editReminder(modelData.id, "")
            background: Rectangle {
                radius: Theme.radius
                color: Theme.surface
                border.width: row.visualFocus ? 2 : 0
                border.color: Theme.accentText
            }
            contentItem: RowLayout {
                id: rowContent
                spacing: 12
                Icon {
                    Layout.alignment: Qt.AlignTop
                    Layout.preferredWidth: 22
                    Layout.preferredHeight: 22
                    name: "bell"
                    color: Theme.accentText
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2
                    Text {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: row.modelData.text
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.Bold
                    }
                    Text {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: Labels.reminderMode(row.modelData.mode, row.modelData.studentName)
                        color: Theme.textMuted
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                    }
                }
                Icon {
                    visible: row.modelData.sound === 1
                    Layout.preferredWidth: 18
                    Layout.preferredHeight: 18
                    name: "speaker"
                    color: Theme.textMuted
                }
            }
            leftPadding: 16
            rightPadding: 16
        }
    }
}
