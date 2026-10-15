// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Events (recitals, concerts): list and programme. Phones: one after the other;
// tablets/desktop: side by side.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Item {
    id: page

    readonly property bool wide: width >= 900
    property string eventId
    //! Wide layout: the archive (list or one entry) instead of an event
    property bool archive: false
    property string archiveId
    readonly property var events: { if (Data.revision < 0) return undefined; return Data.events() }
    readonly property int archived: { if (Data.revision < 0) return undefined; return Data.eventArchive().length }

    function open(id) {
        eventId = id
        archive = false
        archiveId = ""
        if (!wide && loader.item)
            loader.item.openEvent(id)
    }
    function openArchive() {
        eventId = ""
        archiveId = ""
        archive = true
        if (!wide && loader.item)
            loader.item.openArchive()
    }

    Connections {
        target: Nav
        function onShowEvent(id) { page.open(id) }
    }

    Loader {
        id: loader
        anchors.fill: parent
        sourceComponent: page.wide ? wideUi : compactUi
    }

    Component {
        id: compactUi
        StackView {
            id: stack
            function openEvent(id) {
                stack.pop(null)
                stack.push(detailComponent, { eventId: id, backText: qsTrId("nav.events") })
            }
            function openArchive() {
                stack.pop(null)
                stack.push(archiveList)
            }
            initialItem: EventList {
                events: page.events
                archived: page.archived
                onPicked: (id) => page.open(id)
                onAddRequested: newEvent.edit("")
                onArchiveRequested: page.openArchive()
            }
            Component {
                id: archiveList
                ArchivePage {
                    entity: "event_archive"
                    backText: qsTrId("nav.events")
                    onBack: stack.pop()
                    onOpen: (entryId) => stack.push(archiveEntry, { entryId: entryId })
                }
            }
            Component {
                id: archiveEntry
                ArchiveEntryPage {
                    entity: "event_archive"
                    onBack: stack.pop()
                }
            }
            Component {
                id: detailComponent
                EventDetail {
                    onBack: {
                        page.eventId = ""
                        stack.pop()
                    }
                }
            }
        }
    }

    Component {
        id: wideUi
        RowLayout {
            spacing: 0
            EventList {
                Layout.preferredWidth: Math.min(380, page.width * 0.4)
                Layout.fillHeight: true
                events: page.events
                selected: page.eventId
                archived: page.archived
                onPicked: (id) => page.open(id)
                onAddRequested: newEvent.edit("")
                onArchiveRequested: page.openArchive()
            }
            Rectangle {
                Layout.fillHeight: true
                implicitWidth: 1
                color: Theme.border
            }
            Loader {
                Layout.fillWidth: true
                Layout.fillHeight: true
                sourceComponent: page.archiveId ? wideArchiveEntry : page.archive ? wideArchive
                                 : page.eventId ? wideDetail : wideEmpty
            }
            Component {
                id: wideArchive
                ArchivePage {
                    entity: "event_archive"
                    backText: qsTrId("nav.events")
                    onBack: page.archive = false
                    onOpen: (entryId) => page.archiveId = entryId
                }
            }
            Component {
                id: wideArchiveEntry
                ArchiveEntryPage {
                    entity: "event_archive"
                    entryId: page.archiveId
                    onBack: page.archiveId = ""
                }
            }
            Component {
                id: wideDetail
                EventDetail {
                    showBack: false
                    eventId: page.eventId
                    onBack: page.eventId = ""
                }
            }
            Component {
                id: wideEmpty
                PlaceholderPage {
                    iconName: "events"
                    title: page.events.length > 0 ? qsTrId("events.pick_hint") : qsTrId("events.empty")
                    hint: ""
                }
            }
        }
    }

    EventEditor {
        id: newEvent
        onSaved: (id) => page.open(id)
    }

    component EventList: Item {
        id: list
        property var events: []
        property string selected
        property int archived
        signal picked(string id)
        signal addRequested()
        signal archiveRequested()

        Flickable {
            anchors.fill: parent
            clip: true
            contentHeight: listColumn.implicitHeight + 48
            boundsBehavior: Flickable.StopAtBounds
            ScrollBar.vertical: ScrollBar {}

            ColumnLayout {
                id: listColumn
                x: 20
                y: 28
                width: parent.width - 40
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: qsTrId("nav.events")
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 30
                        font.weight: Font.ExtraBold
                    }
                    IconButton {
                        filled: true
                        iconName: "plus"
                        text: qsTrId("event.add.title")
                        onClicked: list.addRequested()
                    }
                }

                EmptyHint {
                    visible: list.events.length === 0
                    text: qsTrId("events.empty")
                }

                Repeater {
                    model: [0, 1]
                    delegate: ColumnLayout {
                        id: group
                        required property int modelData
                        readonly property var items: list.events.filter(e => e.state === modelData)
                        visible: items.length > 0
                        Layout.fillWidth: true
                        spacing: 8
                        SectionLabel {
                            text: group.modelData === 0 ? qsTrId("event.state.planned") : qsTrId("event.state.done")
                        }
                        Repeater {
                            model: group.items
                            delegate: AbstractButton {
                                id: row
                                required property var modelData
                                readonly property bool isSelected: list.selected === modelData.id
                                Layout.fillWidth: true
                                implicitHeight: rowContent.implicitHeight + 24
                                focusPolicy: Qt.StrongFocus
                                hoverEnabled: true
                                Accessible.role: Accessible.Button
                                Accessible.name: modelData.title
                                onClicked: list.picked(modelData.id)
                                background: Rectangle {
                                    radius: Theme.radius
                                    color: row.isSelected ? Theme.accentSoft : Theme.surface
                                    border.width: row.isSelected ? 1.5 : row.visualFocus ? 2 : 0
                                    border.color: Theme.accent
                                }
                                contentItem: RowLayout {
                                    id: rowContent
                                    spacing: 12
                                    Rectangle {
                                        implicitWidth: 52
                                        implicitHeight: 52
                                        radius: 14
                                        color: row.modelData.state === 0 ? Theme.accentSoft : Theme.surfaceAlt
                                        ColumnLayout {
                                            anchors.centerIn: parent
                                            spacing: -2
                                            Text {
                                                Layout.alignment: Qt.AlignHCenter
                                                text: row.modelData.date ? Number(row.modelData.date.slice(8, 10)) : ""
                                                color: Theme.accentText
                                                font.family: Theme.fontFamily
                                                font.pixelSize: 19
                                                font.weight: Font.ExtraBold
                                            }
                                            Text {
                                                Layout.alignment: Qt.AlignHCenter
                                                text: {
                                                    if (LanguageManager.effectiveLanguage === "") return ""
                                                    return row.modelData.date
                                                        ? Qt.locale().standaloneMonthName(Number(row.modelData.date.slice(5, 7)) - 1, Locale.ShortFormat).replace(".", "")
                                                        : ""
                                                }
                                                color: Theme.accentText
                                                font.family: Theme.fontFamily
                                                font.pixelSize: 12
                                                font.weight: Font.Bold
                                            }
                                        }
                                    }
                                    ColumnLayout {
                                        Layout.fillWidth: true
                                        spacing: 2
                                        Text {
                                            Layout.fillWidth: true
                                            elide: Text.ElideRight
                                            text: row.modelData.title
                                            color: Theme.text
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 16
                                            font.weight: Font.Bold
                                        }
                                        Text {
                                            Layout.fillWidth: true
                                            elide: Text.ElideRight
                                            text: [Data.formatDate(row.modelData.date, true), row.modelData.time, row.modelData.location]
                                                  .filter(Boolean).join(" · ")
                                            color: Theme.textMuted
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 13
                                        }
                                        Text {
                                            Layout.fillWidth: true
                                            elide: Text.ElideRight
                                            text: qsTrId("event.contributions", row.modelData.itemCount)
                                            color: Theme.textMuted
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 13
                                        }
                                    }
                                }
                                leftPadding: 12
                                rightPadding: 14
                            }
                        }
                    }
                }

                ListRow {
                    visible: list.archived > 0
                    Layout.topMargin: 8
                    iconName: "archive"
                    tile: Theme.tile.gray
                    title: qsTrId("events.archive.title")
                    trailing: String(list.archived)
                    onClicked: list.archiveRequested()
                }
            }
        }
    }
}
