// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// One event: details, programme in order, playing time. Setting it to "done"
// can enter it as an activity for every performer and finish the pieces.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Item {
    id: detail

    property string eventId
    property bool showBack: true
    property string backText: qsTrId("common.back")
    readonly property var event: { if (Data.revision < 0) return undefined; return Data.event(eventId) }
    readonly property var programme: event.programme || []

    signal back()

    function move(index, delta) {
        const ids = programme.map(p => p.itemId)
        const target = index + delta
        if (target < 0 || target >= ids.length)
            return
        const [item] = ids.splice(index, 1)
        ids.splice(target, 0, item)
        Data.reorderProgramme(eventId, ids)
    }

    Flickable {
        id: flick
        anchors.fill: parent
        clip: true
        contentHeight: column.implicitHeight + 60
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}

        ColumnLayout {
            id: column
            x: 20
            y: detail.showBack ? 12 : 28
            width: flick.width - 40
            spacing: 16

            PageHeader {
                visible: detail.showBack
                Layout.leftMargin: -8
                backText: detail.backText
                onBack: detail.back()
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: detail.event.title || ""
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 26
                    font.weight: Font.ExtraBold
                }
                IconButton {
                    iconName: "edit"
                    text: qsTrId("event.edit")
                    onClicked: editor.edit(detail.eventId)
                }
                IconButton {
                    id: moreButton
                    iconName: "more"
                    text: qsTrId("common.more_actions")
                    onClicked: eventMenu.popup(moreButton, moreButton.width - eventMenu.width, moreButton.height)
                }
            }

            Segmented {
                Layout.maximumWidth: 360
                options: [0, 1]
                value: detail.event.state || 0
                labelFor: (v) => v === 0 ? qsTrId("event.state.planned") : qsTrId("event.state.done")
                onActivated: (v) => {
                    if (v === 1 && detail.programme.length > 0)
                        finish.open()
                    else
                        Data.setEventState(detail.eventId, v, false, false)
                }
            }

            // ------------------------------------------------------------ details
            Card {
                Layout.fillWidth: true
                GridLayout {
                    Layout.fillWidth: true
                    columns: column.width > 520 ? 3 : 2
                    columnSpacing: 16
                    rowSpacing: 14
                    Info { label: qsTrId("common.label.date"); value: Data.formatDate(detail.event.date || "", true) }
                    Info { label: qsTrId("common.label.time"); value: detail.event.time || "" }
                    Info { label: qsTrId("event.label.location"); value: detail.event.location || "" }
                    Info { label: qsTrId("event.label.organizer"); value: detail.event.organizer || "" }
                    Info { label: qsTrId("event.label.accompanist"); value: detail.event.accompanist || "" }
                    Info {
                        label: qsTrId("event.label.contributions")
                        value: qsTrId("event.contributions", detail.programme.length)
                               + (detail.event.externalCount > 0 ? " (" + qsTrId("event.external_count", detail.event.externalCount) + ")" : "")
                    }
                }
            }

            // ------------------------------------------------------------ programme
            RowLayout {
                Layout.fillWidth: true
                Text {
                    Layout.fillWidth: true
                    text: qsTrId("event.programme")
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 20
                    font.weight: Font.ExtraBold
                }
            }

            EmptyHint {
                visible: detail.programme.length === 0
                text: qsTrId("event.programme_empty")
            }

            Rectangle {
                visible: detail.programme.length > 0
                Layout.fillWidth: true
                implicitHeight: programmeColumn.implicitHeight
                radius: Theme.radius
                color: Theme.surface
                clip: true
                ColumnLayout {
                    id: programmeColumn
                    width: parent.width
                    spacing: 0
                    Repeater {
                        model: detail.programme
                        delegate: Item {
                            id: item
                            required property var modelData
                            required property int index
                            Layout.fillWidth: true
                            implicitHeight: itemRow.implicitHeight + 24
                            Rectangle {
                                visible: item.index > 0
                                width: parent.width
                                height: 1
                                color: Theme.divider
                            }
                            RowLayout {
                                id: itemRow
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 14
                                anchors.rightMargin: 8
                                spacing: 10
                                Text {
                                    Layout.preferredWidth: 22
                                    text: item.index + 1
                                    color: Theme.accentText
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 15
                                    font.weight: Font.ExtraBold
                                }
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 2
                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 6
                                        Text {
                                            Layout.fillWidth: true
                                            wrapMode: Text.WordWrap
                                            text: (item.modelData.composer ? item.modelData.composer + " – " : "") + item.modelData.title
                                            color: Theme.text
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 15
                                            font.weight: Font.Bold
                                        }
                                        Rectangle {
                                            visible: item.modelData.external
                                            implicitWidth: externalLabel.implicitWidth + 12
                                            implicitHeight: externalLabel.implicitHeight + 4
                                            radius: 6
                                            color: Theme.tile.yellow.bg
                                            Text {
                                                id: externalLabel
                                                anchors.centerIn: parent
                                                text: qsTrId("event.external_badge")
                                                color: Theme.tile.yellow.fg
                                                font.family: Theme.fontFamily
                                                font.pixelSize: 11
                                                font.weight: Font.ExtraBold
                                            }
                                        }
                                    }
                                    Text {
                                        Layout.fillWidth: true
                                        wrapMode: Text.WordWrap
                                        text: item.modelData.performers
                                        color: Theme.textMuted
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 13
                                    }
                                }
                                Text {
                                    text: item.modelData.duration > 0 ? item.modelData.duration + " " + qsTrId("common.unit.min") : ""
                                    color: Theme.textSecondary
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 14
                                    font.weight: Font.DemiBold
                                }
                                ColumnLayout {
                                    spacing: 0
                                    IconButton {
                                        size: 32
                                        iconName: "arrowUp"
                                        fill: "transparent"
                                        tint: Theme.textMuted
                                        enabled: item.index > 0
                                        opacity: enabled ? 1 : 0.3
                                        text: qsTrId("event.move_up")
                                        onClicked: detail.move(item.index, -1)
                                    }
                                    IconButton {
                                        size: 32
                                        iconName: "arrowDown"
                                        fill: "transparent"
                                        tint: Theme.textMuted
                                        enabled: item.index < detail.programme.length - 1
                                        opacity: enabled ? 1 : 0.3
                                        text: qsTrId("event.move_down")
                                        onClicked: detail.move(item.index, 1)
                                    }
                                }
                                IconButton {
                                    size: 36
                                    iconName: "close"
                                    fill: "transparent"
                                    tint: Theme.textMuted
                                    text: qsTrId("common.remove")
                                    onClicked: Data.removeFromProgramme(item.modelData.itemId)
                                }
                            }
                        }
                    }
                }
            }

            // Totals
            Card {
                visible: detail.programme.length > 0
                Layout.fillWidth: true
                spacing: 6
                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        Layout.fillWidth: true
                        text: qsTrId("event.net_playing_time")
                        color: Theme.textSecondary
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                    }
                    Text {
                        text: (detail.event.playingMinutes || 0) + " " + qsTrId("common.unit.min")
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.Bold
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        Layout.fillWidth: true
                        text: qsTrId("event.estimated_total")
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 16
                        font.weight: Font.Bold
                    }
                    Text {
                        text: (detail.event.totalMinutes || 0) + " " + qsTrId("common.unit.min")
                        color: Theme.accentText
                        font.family: Theme.fontFamily
                        font.pixelSize: 18
                        font.weight: Font.ExtraBold
                    }
                }
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: qsTrId("event.duration_breakdown").arg(detail.event.moderationMinutes || 0).arg(detail.event.betweenMinutes || 0)
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                }
            }

            Flow {
                Layout.fillWidth: true
                spacing: 10
                ActionButton {
                    kind: "primary"
                    iconName: "plus"
                    text: qsTrId("event.add_ready")
                    onClicked: picker.start(detail.eventId)
                }
                ActionButton {
                    iconName: "plus"
                    text: qsTrId("event.add_external")
                    onClicked: external.start(detail.eventId)
                }
            }
        }
    }

    Menu {
        id: eventMenu
        MenuItem {
            text: qsTrId("event.export_program")
            onTriggered: Docs.programme(detail.eventId)
        }
        MenuItem {
            text: qsTrId("event.archive")
            onTriggered: {
                Docs.archiveEvent(detail.eventId)
                Nav.toast(qsTrId("archive.saved").arg(detail.event.title || ""))
            }
        }
        MenuSeparator {}
        MenuItem {
            text: qsTrId("event.archive_delete")
            onTriggered: archiveAndDelete.open()
        }
        MenuItem {
            text: qsTrId("event.delete.title")
            onTriggered: confirmDelete.open()
        }
    }

    AppDialog {
        id: archiveAndDelete
        iconName: "archive"
        destructive: true
        title: qsTrId("event.archive_delete")
        text: qsTrId("event.delete.confirm").arg(detail.event.title || "")
        rejectText: qsTrId("common.cancel")
        acceptText: qsTrId("event.archive_delete")
        onAccepted: {
            const title = detail.event.title || ""
            Docs.archiveEvent(detail.eventId)
            Data.deleteEvent(detail.eventId)
            Nav.toast(qsTrId("archive.saved").arg(title))
            detail.back()
        }
    }

    EventEditor { id: editor }
    ReadyPiecesPicker { id: picker }
    ExternalEditor { id: external }

    AppDialog {
        id: confirmDelete
        iconName: "trash"
        destructive: true
        title: qsTrId("event.delete.title")
        text: qsTrId("event.delete.confirm").arg(detail.event.title || "")
        rejectText: qsTrId("common.cancel")
        acceptText: qsTrId("common.delete")
        onAccepted: {
            Data.deleteEvent(detail.eventId)
            detail.back()
        }
    }

    AppDialog {
        id: finish
        iconName: "check"
        title: qsTrId("event.finish.title")
        text: qsTrId("event.finish.intro")
        rejectText: qsTrId("common.cancel")
        acceptText: qsTrId("common.ok")
        onAccepted: Data.setEventState(detail.eventId, 1, activitiesSwitch.checked, piecesSwitch.checked)
        SwitchRow {
            id: activitiesSwitch
            text: qsTrId("common.activities")
            hint: qsTrId("event.finish.activities")
            checked: true
        }
        SwitchRow {
            id: piecesSwitch
            text: qsTrId("common.pieces")
            hint: qsTrId("event.finish.pieces")
            checked: true
        }
    }

    component Info: ColumnLayout {
        property string label
        property string value
        Layout.fillWidth: true
        spacing: 2
        FieldLabel { label: parent.label }
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: parent.value !== "" ? parent.value : "–"
            color: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 15
            font.weight: Font.DemiBold
        }
    }
}
