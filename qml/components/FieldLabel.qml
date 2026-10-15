// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Label above a form field. The 1.x strings end with a colon ("First name:");
// above a field it is left out.

import QtQuick
import QtQuick.Layouts

Text {
    property string label
    text: label.replace(/\s*:\s*$/, "")
    Layout.fillWidth: true
    elide: Text.ElideRight
    color: Theme.textMuted
    font.family: Theme.fontFamily
    font.pixelSize: 12
    font.weight: Font.Bold
    font.letterSpacing: 0.5
    font.capitalization: Font.AllUppercase
}
