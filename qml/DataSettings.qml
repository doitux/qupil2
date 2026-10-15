// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// More › Data: what is stored, backups (manual and automatic), restoring
// a backup and taking over Qupil 1.x data again.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

ColumnLayout {
    id: root
    spacing: 12

    readonly property var info: App.info

    function formatDate(iso) {
        LanguageManager.effectiveLanguage   // re-evaluate on a language switch
        if (!iso)
            return ""
        return new Date(iso).toLocaleString(Qt.locale(), Locale.ShortFormat)
    }

    TakeOverActions { id: actions }

    RowLayout {
        spacing: 12
        Layout.fillWidth: true
        Rectangle {
            implicitWidth: 34
            implicitHeight: 34
            radius: 10
            color: Theme.tile.blue.bg
            Icon {
                anchors.centerIn: parent
                width: 20
                height: 20
                name: "database"
                color: Theme.tile.blue.fg
            }
        }
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 1
            Text {
                text: qsTrId("more.data")
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 15
                font.weight: Font.DemiBold
            }
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: [qsTrId("summary.students", root.info.students ?? 0),
                       qsTrId("summary.lessons", root.info.lessons ?? 0),
                       qsTrId("summary.events", root.info.events ?? 0)].join(" · ")
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 13
            }
        }
    }

    // Where the data came from
    ColumnLayout {
        Layout.fillWidth: true
        spacing: 4
        InfoLine {
            visible: root.info.demo === true
            text: qsTrId("data.demo")
        }
        InfoLine {
            visible: (root.info.importedAt ?? "") !== ""
            text: qsTrId("data.imported_at").arg(root.formatDate(root.info.importedAt))
            detail: root.info.importedFrom ?? ""
        }
        InfoLine {
            visible: (root.info.restoredAt ?? "") !== ""
            text: qsTrId("data.restored_at").arg(root.formatDate(root.info.restoredAt))
            detail: root.info.restoredFrom ?? ""
        }
        InfoLine {
            text: (root.info.lastAutoBackup ?? "") !== ""
                  ? qsTrId("data.last_auto_backup").arg(root.formatDate(root.info.lastAutoBackup))
                  : qsTrId("data.auto_backup_info")
            detail: App.desktop ? qsTrId("data.location").arg(root.info.databasePath ?? "") : ""
        }
    }

    Flow {
        Layout.fillWidth: true
        spacing: 10

        ActionButton {
            kind: "primary"
            iconName: "backup"
            text: qsTrId("data.backup_now")
            enabled: !App.busy
            onClicked: actions.createBackup()
        }
        ActionButton {
            iconName: "restore"
            text: qsTrId("data.restore")
            enabled: !App.busy
            onClicked: actions.chooseBackup()
        }
        ActionButton {
            id: takeOverButton
            iconName: "import"
            text: qsTrId("data.take_over")
            enabled: !App.busy
            onClicked: {
                App.refreshLegacyFolders()
                takeOverMenu.popup(takeOverButton, 0, takeOverButton.height + 4)
            }
        }
        ActionButton {
            visible: App.desktop
            kind: "quiet"
            iconName: "folder"
            text: qsTrId("data.open_backup_folder")
            onClicked: Qt.openUrlExternally(App.backupFolder)
        }
    }

    Menu {
        id: takeOverMenu

        Repeater {
            model: App.legacyFolders
            delegate: MenuItem {
                required property var modelData
                text: qsTrId("setup.legacy.found").arg(modelData.path)
                onTriggered: actions.start(modelData.url, "legacy")
            }
        }
        MenuItem {
            visible: App.desktop
            height: visible ? implicitHeight : 0
            text: qsTrId("setup.legacy.choose_folder")
            onTriggered: actions.chooseLegacyFolder()
        }
        MenuItem {
            text: qsTrId("setup.legacy.choose_file")
            onTriggered: actions.chooseLegacyFile()
        }
    }

    component InfoLine: ColumnLayout {
        id: line
        property string text
        property string detail
        Layout.fillWidth: true
        spacing: 0
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: line.text
            color: Theme.textSecondary
            font.family: Theme.fontFamily
            font.pixelSize: 14
        }
        Text {
            visible: line.detail !== ""
            Layout.fillWidth: true
            elide: Text.ElideMiddle
            text: line.detail
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 12
        }
    }
}
