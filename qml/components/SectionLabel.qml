// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Small uppercase heading above a group ("LATER TODAY", "NOTICES").

import QtQuick

Text {
    color: Theme.textMuted
    font.family: Theme.fontFamily
    font.pixelSize: 13
    font.weight: Font.Bold
    font.letterSpacing: 0.5
    font.capitalization: Font.AllUppercase
    elide: Text.ElideRight
}
