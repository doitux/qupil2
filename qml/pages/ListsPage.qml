// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Settings › Lists: lesson locations, styles and instrument sizes (the choices
// in the editors). Stored with the data, the same on every device.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page
    title: qsTrId("settings.lists")
    backText: qsTrId("settings.title")

    Card {
        Layout.fillWidth: true
        spacing: 18
        ListEditor {
            label: qsTrId("settings.locations")
            items: { if (Data.revision < 0) return undefined; return Data.listSetting("locations") }
            onChanged: (items) => Data.setSetting("locations", items)
        }
        ListEditor {
            label: qsTrId("settings.genres")
            hint: false
            items: { if (Data.revision < 0) return undefined; return Data.listSetting("genres") }
            onChanged: (items) => Data.setSetting("genres", items)
        }
        ListEditor {
            label: qsTrId("settings.sizes")
            hint: false
            items: { if (Data.revision < 0) return undefined; return Data.listSetting("instrument_sizes") }
            onChanged: (items) => Data.setSetting("instrument_sizes", items)
        }
    }
}
