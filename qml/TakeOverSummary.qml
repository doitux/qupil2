// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// What a take-over or restore brought in: counts per kind of data and notes
// about anything that could not be taken over unchanged.

import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: root

    //! App.lastTakeOver
    property var result: ({})

    spacing: 12

    // Static keys: a language switch only re-evaluates the labels
    readonly property var keys: ["students", "archivedStudents", "lessons", "notes", "pieces", "activities",
                                 "events", "archivedEvents", "libraryItems", "reminders", "instruments"]

    function label(key, n) {
        switch (key) {
        case "students": return qsTrId("summary.students", n)
        case "archivedStudents": return qsTrId("summary.archived_students", n)
        case "lessons": return qsTrId("summary.lessons", n)
        case "notes": return qsTrId("summary.notes", n)
        case "pieces": return qsTrId("summary.pieces", n)
        case "activities": return qsTrId("summary.activities", n)
        case "events": return qsTrId("summary.events", n)
        case "archivedEvents": return qsTrId("summary.archived_events", n)
        case "libraryItems": return qsTrId("summary.library", n)
        case "reminders": return qsTrId("summary.reminders", n)
        default: return qsTrId("summary.instruments", n)
        }
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 10
        Rectangle {
            implicitWidth: 34
            implicitHeight: 34
            radius: 17
            color: Theme.successSoft
            Icon {
                anchors.centerIn: parent
                width: 20
                height: 20
                name: "check"
                color: Theme.success
                strokeWidth: 2.4
            }
        }
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 1
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: root.result.kind === "backup" ? qsTrId("summary.title.backup")
                    : root.result.kind === "demo" ? qsTrId("summary.demo_done")
                    : qsTrId("migration.done")
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 16
                font.weight: Font.Bold
            }
            Text {
                visible: (root.result.source ?? "") !== ""
                Layout.fillWidth: true
                elide: Text.ElideMiddle
                text: qsTrId("summary.source").arg(root.result.source ?? "")
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 13
            }
        }
    }

    // Counts in two columns on wide screens
    GridLayout {
        Layout.fillWidth: true
        columns: root.width > 420 ? 2 : 1
        columnSpacing: 16
        rowSpacing: 8

        Repeater {
            model: root.keys
            delegate: RowLayout {
                id: line
                required property string modelData
                readonly property int count: root.result[modelData] ?? 0
                visible: count > 0
                Layout.fillWidth: true
                spacing: 8
                Rectangle {
                    implicitWidth: 6
                    implicitHeight: 6
                    radius: 3
                    color: Theme.accentText
                }
                Text {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: root.label(line.modelData, line.count)
                    color: Theme.textSecondary
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                }
            }
        }
    }

    Text {
        visible: root.result.settingsFound === true
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: qsTrId("summary.settings")
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 14
    }

    // Anything that was not taken over 1:1
    Repeater {
        model: ["skipped", "unparsedDates"]
        delegate: RowLayout {
            id: notice
            required property string modelData
            readonly property int count: root.result[modelData] ?? 0
            visible: count > 0
            Layout.fillWidth: true
            spacing: 8
            Icon {
                Layout.alignment: Qt.AlignTop
                Layout.preferredWidth: 18
                Layout.preferredHeight: 18
                name: "warning"
                color: Theme.tile.orange.fg
            }
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: notice.modelData === "skipped" ? qsTrId("summary.skipped", notice.count)
                                                     : qsTrId("summary.unparsed_dates", notice.count)
                color: Theme.tile.orange.fg
                font.family: Theme.fontFamily
                font.pixelSize: 14
            }
        }
    }
}
