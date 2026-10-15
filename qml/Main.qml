// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Main window. One layout for all devices:
//   narrow (phone, small window): tab bar at the bottom
//   wide (tablet, desktop):       sidebar on the left

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

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

    RowLayout {
        anchors.fill: parent
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

    // ---------------------------------------------------------------- tab bar (narrow)
    footer: Rectangle {
        visible: window.compact
        implicitHeight: window.compact ? 64 : 0
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
}
