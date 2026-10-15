// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// More › Pieces ready to perform (Qupil 1.x: event manager, tab "Ready for
// Concert"): every piece marked as ready, with who plays it, optionally only
// the students of one lesson location.

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
            required property var modelData
            title: (modelData.composer ? modelData.composer + " – " : "") + modelData.title
            subtitle: [modelData.performers, modelData.genre,
                       modelData.duration > 0 ? modelData.duration + " " + qsTrId("common.unit.min") : ""]
                      .filter(x => x).join(" · ")
            iconName: "events"
            tile: Theme.tile.green
            chevron: false
        }
    }
}
