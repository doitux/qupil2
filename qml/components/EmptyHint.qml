// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Muted card for empty lists ("No notes yet").

import QtQuick
import QtQuick.Layouts

Rectangle {
    id: hint
    property alias text: label.text
    Layout.fillWidth: true
    implicitHeight: label.implicitHeight + 36
    radius: Theme.radius
    color: Theme.surface
    Text {
        id: label
        anchors.centerIn: parent
        width: parent.width - 32
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 15
    }
}
