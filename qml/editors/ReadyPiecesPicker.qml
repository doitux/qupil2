// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Pick pieces "ready to perform" for an event programme.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: picker

    property string eventId
    property var chosen: []
    property string location
    readonly property var pieces: { if (Data.revision < 0) return undefined; return eventId ? Data.readyPieces(eventId) : [] }
    readonly property var locations: {
        const set = []
        for (const p of pieces)
            if (p.location && set.indexOf(p.location) < 0)
                set.push(p.location)
        return set
    }

    function start(id) {
        eventId = id
        chosen = []
        location = ""
        open()
    }

    title: qsTrId("event.select_pieces.title")
    acceptText: qsTrId("common.add")
    canAccept: chosen.length > 0
    onAccepted: Data.addToProgramme(eventId, chosen)

    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: picker.pieces.length > 0 ? qsTrId("event.select_pieces.hint") : qsTrId("event.no_ready_pieces")
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 14
    }
    ChoiceField {
        visible: picker.locations.length > 1
        label: qsTrId("event.filter.location_alt")
        options: picker.locations
        value: picker.location
        onPicked: (value) => picker.location = value
    }
    Rectangle {
        visible: picker.pieces.length > 0
        Layout.fillWidth: true
        implicitHeight: pieceList.implicitHeight
        radius: 14
        color: Theme.surfaceAlt
        clip: true
        ColumnLayout {
            id: pieceList
            width: parent.width
            spacing: 0
            Repeater {
                model: picker.pieces.filter(p => picker.location === "" || p.location === picker.location)
                delegate: CheckDelegate {
                    id: row
                    required property var modelData
                    Layout.fillWidth: true
                    checked: picker.chosen.indexOf(modelData.id) >= 0
                    onClicked: {
                        const list = picker.chosen.slice()
                        const i = list.indexOf(modelData.id)
                        if (i >= 0) list.splice(i, 1); else list.push(modelData.id)
                        picker.chosen = list
                    }
                    contentItem: ColumnLayout {
                        spacing: 2
                        Text {
                            Layout.fillWidth: true
                            Layout.rightMargin: row.indicator.width + 8
                            wrapMode: Text.WordWrap
                            text: (row.modelData.composer ? row.modelData.composer + " – " : "") + row.modelData.title
                            color: Theme.text
                            font.family: Theme.fontFamily
                            font.pixelSize: 15
                            font.weight: Font.Bold
                        }
                        Text {
                            Layout.fillWidth: true
                            Layout.rightMargin: row.indicator.width + 8
                            wrapMode: Text.WordWrap
                            text: row.modelData.performers + (row.modelData.duration > 0
                                  ? " · " + row.modelData.duration + " " + qsTrId("common.unit.min") : "")
                            color: Theme.textMuted
                            font.family: Theme.fontFamily
                            font.pixelSize: 13
                        }
                    }
                }
            }
        }
    }
}
