// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Main window. One layout for all devices:
//   narrow (phone, small window): tab bar at the bottom
//   wide (tablet, desktop):       sidebar on the left

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

ApplicationWindow {
    id: window

    width: 1100
    height: 720
    minimumWidth: 360
    minimumHeight: 560
    visible: true
    color: Theme.bg
    title: qsTrId("app.window_title").arg(Qt.application.version)

    font.family: Theme.fontFamily

    // Standard controls (combo box popups, scroll bars, …) follow the theme
    palette.window: Theme.bg
    palette.windowText: Theme.text
    palette.base: Theme.surface
    palette.alternateBase: Theme.surfaceAlt
    palette.text: Theme.text
    palette.button: Theme.surfaceAlt
    palette.buttonText: Theme.text
    palette.brightText: Theme.textOnAccent
    palette.highlight: Theme.accent
    palette.highlightedText: Theme.textOnAccent
    palette.light: Theme.surfaceAlt
    palette.midlight: Theme.border
    palette.mid: Theme.border
    palette.dark: Theme.textMuted
    palette.placeholderText: Theme.textMuted
    palette.toolTipBase: Theme.inverse
    palette.toolTipText: Theme.textOnInverse

    readonly property bool compact: width < 720
    property int currentPage: 0

    // Static model: a language switch only re-evaluates the labels,
    // it does not destroy and re-create the navigation buttons.
    readonly property var navigation: ["calendar", "students", "events", "metronome", "more"]

    function navLabel(index) {
        switch (index) {
        case 0: return qsTrId("nav.schedule")
        case 1: return qsTrId("nav.students")
        case 2: return qsTrId("nav.events")
        case 3: return qsTrId("metronome.title")
        default: return qsTrId("nav.more")
        }
    }

    // First start (or unreadable data): setup screen, otherwise the app
    Loader {
        anchors.fill: parent
        sourceComponent: App.phase === App.Ready ? mainUi : setupUi
    }

    Component {
        id: setupUi
        SetupPage {}
    }

    Component {
        id: mainUi

        RowLayout {
            spacing: 0

            // ------------------------------------------------------------ sidebar (wide)
            Rectangle {
                visible: !window.compact
                Layout.fillHeight: true
                Layout.preferredWidth: 216
                color: Theme.surface

                Rectangle {
                    anchors.right: parent.right
                    width: 1
                    height: parent.height
                    color: Theme.border
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    anchors.topMargin: 28
                    spacing: 4

                    RowLayout {
                        Layout.leftMargin: 10
                        Layout.bottomMargin: 18
                        spacing: 8
                        Text {
                            text: "Qupil"
                            color: Theme.accentText
                            font.family: Theme.fontFamily
                            font.pixelSize: 24
                            font.weight: Font.ExtraBold
                        }
                        Rectangle {
                            implicitWidth: badge.implicitWidth + 12
                            implicitHeight: badge.implicitHeight + 4
                            radius: 6
                            color: Theme.lime
                            Text {
                                id: badge
                                anchors.centerIn: parent
                                text: "2"
                                color: Theme.textOnLime
                                font.family: Theme.fontFamily
                                font.pixelSize: 12
                                font.weight: Font.ExtraBold
                            }
                        }
                    }

                    Repeater {
                        model: window.navigation
                        delegate: NavButton {
                            required property string modelData
                            required property int index
                            Layout.fillWidth: true
                            vertical: false
                            text: window.navLabel(index)
                            iconName: modelData
                            selected: window.currentPage === index
                            onClicked: window.currentPage = index
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }

            // ------------------------------------------------------------ pages
            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: window.currentPage

                TodayPage {}
                PlaceholderPage { title: qsTrId("nav.students"); iconName: "students" }
                PlaceholderPage { title: qsTrId("nav.events"); iconName: "events" }
                PlaceholderPage { title: qsTrId("metronome.title"); iconName: "metronome" }
                MorePage {}
            }
        }
    }

    // ---------------------------------------------------------------- tab bar (narrow)
    footer: Rectangle {
        readonly property bool shown: window.compact && App.phase === App.Ready
        visible: shown
        implicitHeight: shown ? 64 : 0
        color: Theme.surface

        Rectangle {
            width: parent.width
            height: 1
            color: Theme.border
        }

        RowLayout {
            anchors.fill: parent
            anchors.topMargin: 4
            anchors.leftMargin: 4
            anchors.rightMargin: 4
            spacing: 0

            Repeater {
                model: window.navigation
                delegate: NavButton {
                    required property string modelData
                    required property int index
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    vertical: true
                    text: window.navLabel(index)
                    iconName: modelData
                    selected: window.currentPage === index
                    onClicked: window.currentPage = index
                }
            }
        }
    }

    // ---------------------------------------------------------------- take-over / restore feedback
    // Shared by the setup screen and More › Data

    Popup {
        id: progressPopup
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(parent.width - 32, 400)
        modal: true
        closePolicy: Popup.NoAutoClose
        visible: App.busy
        padding: 24
        Overlay.modal: Rectangle { color: Theme.overlay }
        background: Rectangle {
            radius: Theme.radiusLarge
            color: Theme.surface
        }
        contentItem: ColumnLayout {
            spacing: 14
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: qsTrId("migration.importing")
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 17
                font.weight: Font.Bold
            }
            ProgressBar {
                id: bar
                Layout.fillWidth: true
                from: 0
                to: 100
                value: App.progress
                background: Rectangle {
                    implicitHeight: 8
                    radius: 4
                    color: Theme.segment
                }
                contentItem: Item {
                    implicitHeight: 8
                    Rectangle {
                        width: bar.visualPosition * parent.width
                        height: parent.height
                        radius: 4
                        color: Theme.accent
                    }
                }
            }
            Text {
                Layout.fillWidth: true
                elide: Text.ElideRight
                text: window.stepLabel(App.progressStep)
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 14
            }
        }
    }

    function stepLabel(step) {
        switch (step) {
        case "students": return qsTrId("nav.students")
        case "archive": return qsTrId("common.archive")
        case "lessons": return qsTrId("summary.step.lessons")
        case "pieces": return qsTrId("common.pieces")
        case "notes": return qsTrId("common.notes")
        case "activities": return qsTrId("common.activities")
        case "library": return qsTrId("library.title")
        case "events": return qsTrId("nav.events")
        case "reminders": return qsTrId("reminders.title")
        case "settings": return qsTrId("settings.title")
        default: return ""
        }
    }

    AppDialog {
        id: failedDialog
        iconName: "warning"
        destructive: true
        title: qsTrId("takeover.failed")
    }

    // After a take-over from More › Data (the setup screen shows its own summary)
    AppDialog {
        id: summaryDialog
        title: App.lastTakeOver.kind === "backup" ? qsTrId("summary.title.backup")
             : App.lastTakeOver.kind === "demo" ? qsTrId("summary.title.demo")
             : qsTrId("summary.title.legacy")
        acceptText: qsTrId("common.done")
        onAccepted: App.dismissTakeOver()
        TakeOverSummary {
            Layout.fillWidth: true
            result: App.lastTakeOver
        }
    }

    Connections {
        target: App
        function onTakeOverFailed(message) {
            failedDialog.text = message
            failedDialog.open()
        }
        function onLastTakeOverChanged() {
            if (App.phase === App.Ready && App.lastTakeOver.kind !== undefined)
                summaryDialog.open()
        }
    }
}
