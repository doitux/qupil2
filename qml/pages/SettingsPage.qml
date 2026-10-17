// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Settings: sync at the top, then what applies to this device (appearance,
// alerts), what is the same on every device (lessons and events, lists), the
// data and backups, and about Qupil. Each opens its own page.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page
    title: qsTrId("settings.title")

    signal openSync()
    signal openAppearance()
    signal openAlerts()
    signal openLessons()
    signal openLists()
    signal openData()
    signal openAbout()

    SyncCard {
        Layout.fillWidth: true
        onOpenSync: page.openSync()
    }

    SectionLabel { Layout.topMargin: 6; text: qsTrId("settings.this_device") }
    MenuGroup {
        MenuRow {
            iconName: "palette"
            tile: Theme.tile.plum
            text: qsTrId("settings.appearance")
            value: Theme.mode === "dark" ? qsTrId("appearance.dark")
                 : Theme.mode === "light" ? qsTrId("appearance.light") : qsTrId("appearance.system")
            onClicked: page.openAppearance()
        }
        MenuRow {
            divider: true
            iconName: "bell"
            tile: Theme.tile.orange
            text: qsTrId("settings.alerts")
            onClicked: page.openAlerts()
        }
    }

    SectionLabel { Layout.topMargin: 6; text: qsTrId("settings.all_devices") }
    MenuGroup {
        MenuRow {
            iconName: "calendar"
            tile: Theme.tile.blue
            text: qsTrId("settings.lessons_events")
            onClicked: page.openLessons()
        }
        MenuRow {
            divider: true
            iconName: "tools"
            tile: Theme.tile.yellow
            text: qsTrId("settings.lists")
            onClicked: page.openLists()
        }
    }

    SectionLabel { Layout.topMargin: 6; text: qsTrId("more.data") }
    MenuGroup {
        MenuRow {
            iconName: "database"
            tile: Theme.tile.green
            text: qsTrId("settings.data")
            onClicked: page.openData()
        }
    }

    MenuGroup {
        Layout.topMargin: 6
        MenuRow {
            iconName: "info"
            text: qsTrId("about.title")
            value: App.versionText
            onClicked: page.openAbout()
        }
    }
}
