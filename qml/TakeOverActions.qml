// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// File pickers and confirmations for taking over Qupil 1.x data, restoring and
// creating backups. Used by the setup screen and by More › Data.
// Progress, errors and the summary are shown by Main.qml for both.

import QtCore
import QtQuick
import QtQuick.Dialogs
import Qupil

Item {
    id: actions

    // Existing data is replaced: ask first (on the setup screen there is nothing to lose)
    readonly property bool replacing: App.phase === App.Ready
    property bool exporting: false

    function chooseLegacyFolder() {
        folderDialog.open()
    }
    function chooseLegacyFile() {
        fileDialog.purpose = "legacy"
        fileDialog.open()
    }
    function chooseBackup() {
        fileDialog.purpose = "backup"
        fileDialog.open()
    }
    function start(source, purpose) {
        if (!replacing) {
            App.takeOver(source)
            return
        }
        confirm.source = source
        confirm.purpose = purpose
        confirm.open()
    }
    function createBackup() {
        if (App.saveDialogAvailable) {
            saveDialog.selectedFile = saveDialog.currentFolder + "/" + App.suggestedBackupName()
            saveDialog.open()
        } else if (App.exportPickerAvailable) {
            exporting = true
            App.exportBackup()   // iOS: "Save to Files", the result comes with backupExported
        } else {
            showBackupResult(App.createBackup(""))
        }
    }
    function showBackupResult(result) {
        backupResult.ok = result.ok
        backupResult.text = result.message
        backupResult.open()
    }

    Connections {
        target: App
        function onBackupExported(result) {
            if (!actions.exporting)
                return   // started from the other place that offers backups
            actions.exporting = false
            if (!result.cancelled)
                actions.showBackupResult(result)
        }
    }

    FolderDialog {
        id: folderDialog
        title: qsTrId("setup.legacy.choose_folder")
        currentFolder: StandardPaths.writableLocation(StandardPaths.HomeLocation)
        onAccepted: actions.start(selectedFolder, "legacy")
    }

    FileDialog {
        id: fileDialog
        property string purpose: "backup"
        title: purpose === "backup" ? qsTrId("backup.open.title") : qsTrId("setup.legacy.choose_file")
        fileMode: FileDialog.OpenFile
        // Mobile pickers filter by MIME type and would hide *.db files
        nameFilters: App.desktop ? [qsTrId("backup.filter"), qsTrId("common.filter_all")] : []
        onAccepted: actions.start(selectedFile, purpose)
    }

    FileDialog {
        id: saveDialog
        title: qsTrId("backup.create.title")
        fileMode: FileDialog.SaveFile
        defaultSuffix: "db"
        nameFilters: App.desktop ? [qsTrId("backup.filter")] : []
        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        onAccepted: actions.showBackupResult(App.createBackup(selectedFile))
    }

    AppDialog {
        id: confirm
        property url source
        property string purpose
        iconName: purpose === "backup" ? "restore" : "import"
        destructive: true
        title: purpose === "backup" ? qsTrId("backup.restore.title") : qsTrId("setup.legacy.title")
        text: (purpose === "backup" ? qsTrId("backup.restore.warning") : qsTrId("data.replace_warning"))
              + "\n\n" + qsTrId("data.safety_backup")
        rejectText: qsTrId("common.cancel")
        acceptText: purpose === "backup" ? qsTrId("backup.restore.action") : qsTrId("setup.legacy.action")
        onAccepted: App.takeOver(source)
    }

    AppDialog {
        id: backupResult
        property bool ok
        iconName: ok ? "check" : "warning"
        destructive: !ok
        title: qsTrId("backup.create.title")
    }
}
