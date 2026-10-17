// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Settings › Appearance: light/dark, colours, language. Stored on this device only.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    title: qsTrId("settings.appearance")
    backText: qsTrId("settings.title")

    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: qsTrId("settings.device_only")
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 13
    }
    Card {
        Layout.fillWidth: true
        AppearanceSettings {
            Layout.fillWidth: true
        }
    }
}
