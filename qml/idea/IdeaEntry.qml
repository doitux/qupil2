// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "Try the new idea": one lesson in the history. Folded: a line per kind (new, done,
// goes on, paused). After a tap: every task of the lesson with the points that came
// (+) or were done (✓) in it. The note of the lesson is the teacher's own.

import QtQuick
import QtQuick.Layouts
import Qupil

Card {
    id: card

    //! { date, lines: [{ kind, title, until, changes: [{ kind, text }] }], did: [text], note }
    property var entry: ({})
    property bool open: false
    property string ago
    readonly property var lines: entry.lines || []
    readonly property bool onlyNote: lines.length === 0 && (entry.did || []).length === 0

    signal toggled()

    function label(kind) {
        return kind === "new" ? qsTrId("idea.log.new") : kind === "done" ? qsTrId("idea.log.done")
             : kind === "paused" ? qsTrId("idea.log.paused") : kind === "resumed" ? qsTrId("idea.log.resumed")
             : qsTrId("idea.log.on")
    }
    //! "Lied 12: Kuckuck" -> "Lied 12", when several stand in one line
    function brief(title, several) {
        const at = title.indexOf(":")
        return several && at > 0 ? title.substring(0, at) : title
    }
    readonly property string summary: {
        const out = []
        for (const kind of ["new", "done", "resumed", "paused", "on"]) {
            const of = lines.filter(l => l.kind === kind)
            if (of.length === 0)
                continue
            out.push("<b>" + label(kind) + ":</b> " + of.map(l => {
                const added = l.changes.filter(c => c.kind === "+").length
                return brief(l.title, of.length > 1) + (kind === "on" && added > 0 ? " (" + qsTrId("idea.points_new", added) + ")" : "")
            }).join(", "))
        }
        return out.join("<br>")
    }

    Layout.fillWidth: true
    padding: 16
    spacing: 8
    Accessible.role: Accessible.Button
    Accessible.name: Data.formatDate(entry.date || "", true)
    Accessible.onPressAction: card.toggled()

    RowLayout {
        Layout.fillWidth: true
        Text {
            text: Data.formatDate(card.entry.date || "", true)
            color: Theme.accentText
            font.family: Theme.fontFamily
            font.pixelSize: 13
            font.weight: Font.ExtraBold
        }
        Item { Layout.fillWidth: true }
        Text {
            text: card.ago
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 12
        }
        Icon {
            visible: !card.onlyNote
            Layout.preferredWidth: 15
            Layout.preferredHeight: 15
            name: "chevronDown"
            color: Theme.icon
            rotation: card.open ? 180 : 0
        }
    }
    Text {
        visible: !card.open && !card.onlyNote
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        textFormat: Text.StyledText
        text: card.summary
        color: Theme.text
        font.family: Theme.fontFamily
        font.pixelSize: 15
        lineHeight: 1.15
    }
    ColumnLayout {
        visible: card.open && !card.onlyNote
        Layout.fillWidth: true
        spacing: 8
        Repeater {
            model: card.lines
            ColumnLayout {
                id: line
                required property var modelData
                Layout.fillWidth: true
                spacing: 4
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    textFormat: Text.StyledText
                    text: "<b>" + card.label(line.modelData.kind) + ":</b> " + line.modelData.title
                          + (line.modelData.kind === "paused" && (line.modelData.until || "") !== ""
                             ? " (" + qsTrId("idea.until").arg(Data.formatDate(line.modelData.until)) + ")" : "")
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                }
                Repeater {
                    model: line.modelData.changes
                    IdeaPoint {
                        required property var modelData
                        Layout.leftMargin: 14
                        text: modelData.text
                        mark: modelData.kind === "lock" ? "✓" : modelData.kind
                        priv: modelData.kind === "lock"
                    }
                }
                Text {
                    visible: line.modelData.kind === "on" && line.modelData.changes.length === 0
                    Layout.leftMargin: 14
                    text: qsTrId("idea.log.unchanged")
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                }
            }
        }
        Repeater {
            model: card.entry.did || []
            IdeaPoint {
                required property string modelData
                text: modelData
                mark: "✓"
                priv: true
            }
        }
    }
    RowLayout {
        visible: (card.entry.note || "") !== ""
        Layout.fillWidth: true
        spacing: 6
        Icon {
            visible: !card.onlyNote
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: 2
            Layout.preferredWidth: 14
            Layout.preferredHeight: 14
            name: "lock"
            color: Theme.textMuted
        }
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: card.entry.note || ""
            color: card.onlyNote ? Theme.text : Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: card.onlyNote ? 15 : 13
        }
    }
    TapHandler {
        enabled: !card.onlyNote
        onTapped: card.toggled()
    }
}
