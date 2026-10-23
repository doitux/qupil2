// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// More › Pieces ready to perform (Qupil 1.x: event manager, tab "Ready for
// Concert"): every piece marked as ready, with who plays it, optionally only
// the students of one lesson location. Tapping one shows it in the student's
// pieces (several students: a menu at the finger asks which one).

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page
    title: qsTrId("event.tab.ready")

    property string location
    readonly property var pieces: { if (Data.revision < 0) return undefined; return Data.readyPieces("") }
    readonly property var locations: {
        const set = []
        for (const p of pieces)
            if (p.location && set.indexOf(p.location) < 0)
                set.push(p.location)
        return set.sort()
    }
    readonly property var shown: pieces.filter(p => location === "" || p.location === location)

    function open(piece, row) {
        const copies = Data.pieceCopies(piece.group_key)
        const current = copies.filter(c => c.current)
        if (current.length > 1) {
            studentMenu.copies = current
            studentMenu.openAt(row, row.pressX, row.pressY)
        } else if (copies.length > 0) {
            const c = current.length === 1 ? current[0] : copies[0]
            Nav.showPiece(c.student_id, c.membership_id, c.piece_id)
        } else {
            Nav.showPiece(piece.student_id, piece.membership_id, piece.id)
        }
    }

    ChoiceField {
        visible: page.locations.length > 1
        label: qsTrId("event.filter.location")
        options: [qsTrId("common.all")].concat(page.locations)
        value: page.location !== "" ? page.location : qsTrId("common.all")
        onPicked: (value) => page.location = page.locations.indexOf(value) >= 0 ? value : ""
    }

    EmptyHint {
        visible: page.shown.length === 0
        text: qsTrId("event.no_ready_pieces")
    }
    Repeater {
        model: page.shown
        delegate: ListRow {
            id: row
            required property var modelData
            title: (modelData.composer ? modelData.composer + " – " : "") + modelData.title
            subtitle: [modelData.performers, modelData.genre,
                       modelData.duration > 0 ? modelData.duration + " " + qsTrId("common.unit.min") : ""]
                      .filter(x => x).join(" · ")
            iconName: "events"
            tile: Theme.tile.green
            onClicked: page.open(modelData, row)
        }
    }

    // A piece of several students: whose pieces to open
    AppMenu {
        id: studentMenu
        property var copies: []
        Repeater {
            model: studentMenu.copies
            delegate: AppMenuItem {
                required property var modelData
                iconName: "students"
                text: modelData.name
                onTriggered: Nav.showPiece(modelData.student_id, modelData.membership_id, modelData.piece_id)
            }
        }
    }
}
