// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "More": the content tools – reports, management (library, instruments,
// reminders, birthdays, student archive) and documents. Everything that is a
// setting (sync, appearance, alerts, lists, data and backups, about) is one
// level down under Settings: the gear at the top or the row at the end.

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
    readonly property int readyPieces: { if (Data.revision < 0) return undefined; return Data.readyPieces("").length }


    function openPage(component) {
        stack.pop(null)
        stack.push(component)
    }
    //! System back button: closes the page on top
    function back() {
        return Nav.stackBack(stack)
    }
    //! The settings, optionally with one of their pages on top
    function openSettings(page) {
        openPage(settingsPage)
        switch (page) {
        case "sync": stack.push(syncPage); break
        case "appearance": stack.push(appearancePage); break
        case "alerts": stack.push(alertSettingsPage); break
        case "lessons": stack.push(lessonSettingsPage); break
        case "lists": stack.push(listsPage); break
        case "data": stack.push(dataPage); break
        case "about": stack.push(aboutPage); break
        case "license": stack.push(aboutPage); stack.push(licensePage); break
        }
    }
    //! Back to the first level: the "More" button of the navigation always lands
    //! there (at once when coming from another tab), and the screenshot tour
    function home() {
        stack.pop(null, stack.visible ? StackView.PopTransition : StackView.Immediate)
    }
    //! A page of "More" by name (screenshot tour)
    function showSubPage(name) {
        switch (name) {
        case "library": openPage(libraryPage); break
        case "instruments": openPage(instrumentsPage); break
        case "ensemble": openPage(ensemblePage); break
        case "archive": openPage(studentArchivePage); break
        case "settings": openSettings(""); break
        case "appearance": openSettings("appearance"); break
        case "alerts": openSettings("alerts"); break
        case "lessonSettings": openSettings("lessons"); break
        case "lists": openSettings("lists"); break
        case "data": openSettings("data"); break
        case "birthdays": openPage(birthdaysPage); break
        case "ready": openPage(readyPage); break
        case "reminders": openPage(remindersPage); break
        case "dayOverview": dayOverviewSheet.start(); break
        case "about": openSettings("about"); break
        case "license": openSettings("license"); break
        case "archiveEntry": {
            const first = Data.studentArchive()[0]
            if (first)
                showArchiveEntry(first.id)
            break
        }
        }
    }

    Connections {
        target: Nav
        function onShowSync() { stack.openSettings("sync") }
        function onShowMore(name) { stack.showSubPage(name) }
    }
    Component.onCompleted: {
        if (Nav.openSyncOnStart) {
            Nav.openSyncOnStart = false
            Qt.callLater(Nav.showSync)
        }
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
                IconButton {
                    iconName: "settings"
                    text: qsTrId("settings.title")
                    onClicked: stack.openSettings("")
                }
            }

            // Only when the sync needs attention; everything else about it is in the settings
            SyncCard {
                Layout.fillWidth: true
                visible: warn
                compact: true
                onOpenSync: stack.openSettings("sync")
            }

            SectionLabel { Layout.topMargin: 6; text: qsTrId("more.reports") }
            MenuGroup {
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
                MenuRow {
                    divider: true
                    iconName: "events"
                    tile: Theme.tile.green
                    text: qsTrId("event.tab.ready")
                    value: stack.readyPieces > 0 ? String(stack.readyPieces) : ""
                    onClicked: stack.push(readyPage)
                }
            }

            SectionLabel { Layout.topMargin: 6; text: qsTrId("more.manage") }
            MenuGroup {
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
                MenuRow {
                    divider: true
                    iconName: "archive"
                    tile: Theme.tile.gray
                    text: qsTrId("nav.student_archive")
                    value: stack.archived > 0 ? String(stack.archived) : ""
                    onClicked: stack.push(studentArchivePage)
                }
            }

            SectionLabel { Layout.topMargin: 6; text: qsTrId("more.documents") }
            MenuGroup {
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

            MenuGroup {
                Layout.topMargin: 6
                MenuRow {
                    iconName: "settings"
                    text: qsTrId("settings.title")
                    value: qsTrId("settings.summary")
                    onClicked: stack.openSettings("")
                }
            }
        }
    }

    Component { id: remindersPage; RemindersPage { onBack: stack.pop() } }
    Component { id: birthdaysPage; BirthdaysPage { onBack: stack.pop() } }
    Component { id: readyPage; ReadyPiecesPage { onBack: stack.pop() } }
    Component { id: candidatesPage; CandidatesPage { onBack: stack.pop() } }
    Component {
        id: settingsPage
        SettingsPage {
            onBack: stack.pop()
            onOpenSync: stack.push(syncPage)
            onOpenAppearance: stack.push(appearancePage)
            onOpenAlerts: stack.push(alertSettingsPage)
            onOpenLessons: stack.push(lessonSettingsPage)
            onOpenLists: stack.push(listsPage)
            onOpenData: stack.push(dataPage)
            onOpenAbout: stack.push(aboutPage)
        }
    }
    Component { id: appearancePage; AppearancePage { onBack: stack.pop() } }
    Component { id: alertSettingsPage; AlertSettingsPage { onBack: stack.pop() } }
    Component { id: lessonSettingsPage; LessonSettingsPage { onBack: stack.pop() } }
    Component { id: listsPage; ListsPage { onBack: stack.pop() } }
    Component { id: dataPage; DataPage { onBack: stack.pop(); onImportCsv: csvSheet.start() } }
    Component { id: aboutPage; AboutPage { backText: qsTrId("settings.title"); onBack: stack.pop(); onShowLicense: stack.push(licensePage) } }
    Component { id: licensePage; LicensePage { onBack: stack.pop() } }
    Component { id: syncPage; SyncPage { backText: qsTrId("settings.title"); onBack: stack.pop() } }
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
}
