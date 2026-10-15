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
    objectName: "morePage"

    readonly property var reminders: { if (Data.revision < 0) return undefined; return Data.reminders() }
    readonly property var birthdays: { if (Data.revision < 0) return undefined; return Data.birthdays(0) }
    readonly property int candidates: { if (Data.revision < 0) return undefined; return Data.recitalCandidates().length }
    readonly property int ensembleCandidates: { if (Data.revision < 0) return undefined; return Data.ensembleCandidates().candidates.length }
    readonly property int lent: { if (Data.revision < 0) return undefined; return Data.lentCount() }
    readonly property int rentals: { if (Data.revision < 0) return undefined; return Data.instrumentOverview().rentalCount }
    readonly property int archived: { if (Data.revision < 0) return undefined; return Data.studentArchive().length }

    property date now: new Date()

    function openPage(component) {
        stack.pop(null)
        stack.push(component)
    }
    //! Back to the first level (screenshot tour)
    function home() {
        stack.pop(null)
    }
    //! A page of "More" by name (screenshot tour)
    function showSubPage(name) {
        switch (name) {
        case "library": openPage(libraryPage); break
        case "instruments": openPage(instrumentsPage); break
        case "ensemble": openPage(ensemblePage); break
        case "archive": openPage(studentArchivePage); break
        case "archiveEntry": {
            const first = Data.studentArchive()[0]
            if (first)
                showArchiveEntry(first.id)
            break
        }
        }
    }

    Timer {
        interval: 30000
        running: true
        repeat: true
        onTriggered: stack.now = new Date()
    }
    Connections {
        target: Nav
        function onShowSync() { stack.openPage(syncPage) }
    }
    Component.onCompleted: {
        if (Nav.openSyncOnStart) {
            Nav.openSyncOnStart = false
            Qt.callLater(Nav.showSync)
        }
    }

    // Title and detail line of the sync card
    readonly property bool wifiUsed: Wifi.peers.length > 0
    readonly property bool syncUsed: Sync.connected || wifiUsed
    readonly property bool syncSetUp: (Sync.connected && !Sync.needsPassword) || wifiUsed
    readonly property bool wifiPaused: Wifi.peers.some(p => p.paused || p.problem !== "")
    readonly property bool syncWarn: (Sync.connected && (Sync.needsPassword || Sync.error !== "")) || wifiPaused
    readonly property string syncTitle: {
        if (Sync.loggingIn || !syncUsed) return qsTrId("sync.title")
        if (Sync.connected && Sync.needsPassword) return Sync.folderExists ? qsTrId("sync.password.enter_title") : qsTrId("sync.password.choose_title")
        if (Sync.busy || Wifi.busy) return qsTrId("sync.in_progress")
        if ((Sync.connected && Sync.error !== "") || wifiPaused) return qsTrId("sync.status.problem")
        if (Sync.lastSync === "" && Wifi.lastSync === "") return qsTrId("sync.never")
        return qsTrId("sync.all_synced")
    }
    readonly property string syncDetail: {
        if (Sync.loggingIn) return qsTrId("sync.nextcloud.waiting")
        if (!syncUsed) return qsTrId("sync.status.off")
        if (Sync.connected && Sync.needsPassword) return qsTrId("sync.nextcloud.connected_as").arg(Sync.user).arg(Sync.server.replace(/^https?:\/\//, ""))
        if (Sync.connected && Sync.error !== "") return Sync.error
        const parts = []
        if (Sync.connected)
            parts.push(qsTrId("sync.nextcloud.when").arg(Labels.syncWhen(Sync.lastSync, stack.now)))
        if (wifiPaused)
            parts.push(qsTrId("wifi.paused"))
        else if (Wifi.lastPeer !== "")
            parts.push(qsTrId("sync.wifi.when").arg(Wifi.lastPeer).arg(Labels.syncWhen(Wifi.lastSync, stack.now)))
        return parts.join(" · ")
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

            RowLayout {
                Layout.fillWidth: true
                Text {
                    Layout.fillWidth: true
                    text: qsTrId("nav.more")
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 34
                    font.weight: Font.ExtraBold
                }
                BuildChip {}
            }

            // Sync status at the top, like in the design
            Rectangle {
                id: syncCard
                Layout.fillWidth: true
                implicitHeight: syncColumn.implicitHeight + 28
                radius: Theme.radiusLarge - 2
                color: Theme.surface
                readonly property bool warn: stack.syncWarn
                readonly property var tile: !stack.syncUsed ? Theme.tile.gray
                                          : warn ? Theme.tile.orange
                                          : Theme.tile.green

                ColumnLayout {
                    id: syncColumn
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 14
                    spacing: 12

                    AbstractButton {
                        id: syncHeader
                        Layout.fillWidth: true
                        implicitHeight: syncHeaderRow.implicitHeight
                        focusPolicy: Qt.StrongFocus
                        Accessible.role: Accessible.Button
                        Accessible.name: stack.syncTitle + ", " + stack.syncDetail
                        onClicked: stack.openPage(syncPage)
                        background: Rectangle {
                            radius: 10
                            color: "transparent"
                            border.width: syncHeader.visualFocus ? 2 : 0
                            border.color: Theme.accentText
                        }
                        contentItem: RowLayout {
                            id: syncHeaderRow
                            spacing: 12
                            Rectangle {
                                implicitWidth: 44
                                implicitHeight: 44
                                radius: 14
                                color: syncCard.tile.bg
                                Icon {
                                    anchors.centerIn: parent
                                    width: 24
                                    height: 24
                                    name: !stack.syncUsed ? "cloud" : syncCard.warn ? "cloudOff" : "sync"
                                    color: syncCard.tile.fg
                                }
                            }
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2
                                Text {
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                    text: stack.syncTitle
                                    color: Theme.text
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 16
                                    font.weight: Font.ExtraBold
                                }
                                Text {
                                    Layout.fillWidth: true
                                    wrapMode: Text.WordWrap
                                    maximumLineCount: 2
                                    elide: Text.ElideRight
                                    text: stack.syncDetail
                                    color: Theme.textMuted
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 13
                                }
                            }
                            Icon {
                                Layout.preferredWidth: 18
                                Layout.preferredHeight: 18
                                name: "chevron"
                                color: Theme.icon
                            }
                        }
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        ActionButton {
                            Layout.fillWidth: true
                            Layout.preferredWidth: 1
                            kind: "primary"
                            text: stack.syncSetUp ? qsTrId("sync.now") : qsTrId("sync.setup_short")
                            enabled: !stack.syncSetUp || !(Sync.busy || Wifi.busy)
                            onClicked: {
                                if (!stack.syncSetUp) {
                                    stack.openPage(syncPage)
                                    return
                                }
                                Sync.syncNow()
                                Wifi.syncNow()
                            }
                        }
                        ActionButton {
                            Layout.fillWidth: true
                            Layout.preferredWidth: 1
                            text: qsTrId("sync.pair")
                            onClicked: Nav.pairDevice()
                        }
                    }
                }
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
                    iconName: "book"
                    tile: Theme.tile.blue
                    text: qsTrId("library.title")
                    value: stack.lent > 0 ? qsTrId("more.lent_count", stack.lent) : ""
                    onClicked: stack.push(libraryPage)
                }
                MenuRow {
                    divider: true
                    iconName: "instrument"
                    tile: Theme.tile.green
                    text: qsTrId("common.instruments")
                    value: stack.rentals > 0 ? qsTrId("more.rental_count", stack.rentals) : ""
                    onClicked: stack.push(instrumentsPage)
                }
                MenuRow {
                    divider: true
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
                MenuRow {
                    divider: true
                    iconName: "students"
                    tile: Theme.tile.yellow
                    text: qsTrId("candidates.ensemble.title")
                    value: stack.ensembleCandidates > 0 ? String(stack.ensembleCandidates) : ""
                    onClicked: stack.push(ensemblePage)
                }
            }

            SectionLabel { Layout.topMargin: 6; text: qsTrId("more.documents") }
            Group {
                MenuRow {
                    iconName: "document"
                    tile: Theme.tile.gray
                    text: qsTrId("doc.dayview.title")
                    onClicked: dayOverviewSheet.start()
                }
                MenuRow {
                    divider: true
                    iconName: "calendar"
                    tile: Theme.tile.gray
                    text: qsTrId("doc.timetable.title")
                    chevron: false
                    onClicked: Docs.timetable()
                }
                MenuRow {
                    divider: true
                    iconName: "instrument"
                    tile: Theme.tile.gray
                    text: qsTrId("instruments.inventory_list")
                    chevron: false
                    onClicked: Docs.rentalList()
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
                MenuRow {
                    iconName: "archive"
                    tile: Theme.tile.gray
                    text: qsTrId("nav.student_archive")
                    value: stack.archived > 0 ? String(stack.archived) : ""
                    onClicked: stack.push(studentArchivePage)
                }
                MenuRow {
                    divider: true
                    iconName: "import"
                    tile: Theme.tile.gray
                    text: qsTrId("menu.import_csv").replace(/\s*(\.\.\.|…)\s*$/, "")
                    onClicked: csvSheet.start()
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
                    value: App.versionText
                    chevron: false
                }
            }
        }
    }

    Component { id: remindersPage; RemindersPage { onBack: stack.pop() } }
    Component { id: birthdaysPage; BirthdaysPage { onBack: stack.pop() } }
    Component { id: candidatesPage; CandidatesPage { onBack: stack.pop() } }
    Component { id: settingsPage; SettingsPage { onBack: stack.pop() } }
    Component { id: syncPage; SyncPage { onBack: stack.pop() } }
    Component { id: libraryPage; LibraryPage { onBack: stack.pop() } }
    Component { id: instrumentsPage; InstrumentsPage { onBack: stack.pop() } }
    Component { id: ensemblePage; EnsembleCandidatesPage { onBack: stack.pop() } }
    Component {
        id: studentArchivePage
        ArchivePage {
            entity: "student_archive"
            onBack: stack.pop()
            onOpen: (entryId) => stack.push(archiveEntryPage, { entity: "student_archive", entryId: entryId })
        }
    }
    Component { id: archiveEntryPage; ArchiveEntryPage { onBack: stack.pop() } }

    DayOverviewSheet { id: dayOverviewSheet }
    CsvImportSheet { id: csvSheet }

    //! Opens an archive entry (from the student page after archiving)
    function showArchiveEntry(entryId) {
        openPage(studentArchivePage)
        stack.push(archiveEntryPage, { entity: "student_archive", entryId: entryId })
    }

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
