// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Settings › Data and backups: the database, backups (manual and automatic),
// taking over Qupil 1.x data, importing an address book.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    title: qsTrId("settings.data")
    backText: qsTrId("settings.title")

    signal importCsv()

    // Anchored (not in a layout): its wrapped texts need a fixed width to measure
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
    MenuGroup {
        MenuRow {
            iconName: "import"
            tile: Theme.tile.gray
            text: qsTrId("menu.import_csv")
            onClicked: importCsv()
        }
    }
}
