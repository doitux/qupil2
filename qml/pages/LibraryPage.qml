// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// More › Sheet music library: everything, what is available, what is lent.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page
    title: qsTrId("library.title")

    property int mode: 0
    property string filter
    readonly property var items: { if (Data.revision < 0) return undefined; return Data.libraryItems(page.mode, page.filter) }

    actions: IconButton {
        filled: true
        iconName: "plus"
        text: qsTrId("library.new")
        onClicked: editor.edit("")
    }

    InputField {
        placeholder: qsTrId("library.search")
        text: page.filter
        inputMethodHints: Qt.ImhNoPredictiveText
        onEdited: (value) => page.filter = value
    }
    Segmented {
        options: [0, 1, 2]
        value: page.mode
        labelFor: function(v) {
            return v === 1 ? qsTrId("library.tab.available") : v === 2 ? qsTrId("library.tab.loaned") : qsTrId("common.all")
        }
        onActivated: (v) => page.mode = v
    }

    EmptyHint {
        visible: page.items.length === 0
        text: qsTrId("library.empty")
    }
    Repeater {
        model: page.items
        delegate: ListRow {
            required property var modelData
            title: modelData.title || "–"
            subtitle: [modelData.number > 0 ? String(modelData.number).padStart(4, "0") : "",
                       modelData.author, modelData.publisher].filter(x => x).join(" · ")
                      + (modelData.lent ? "\n" + qsTrId("library.lent_to_since").arg(modelData.borrower)
                                                         .arg(Data.formatDate(modelData.lent_since)) : "")
            subtitleColor: modelData.lent ? Theme.accentText : Theme.textMuted
            iconName: "book"
            tile: modelData.lent ? Theme.tile.plum : Theme.tile.gray
            onClicked: editor.edit(modelData.id)
        }
    }

    LibraryItemEditor { id: editor }
}
