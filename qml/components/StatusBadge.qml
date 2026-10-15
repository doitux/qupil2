// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Coloured status of a piece (0 planned … 4 finished).

import QtQuick

Rectangle {
    id: badge

    property int status: 0

    function label(s) {
        switch (s) {
        case 1: return qsTrId("piece.status.in_progress")
        case 2: return qsTrId("piece.status.paused")
        case 3: return qsTrId("piece.status.ready")
        case 4: return qsTrId("piece.status.finished")
        default: return qsTrId("piece.status.planned")
        }
    }

    implicitWidth: text.implicitWidth + 20
    implicitHeight: text.implicitHeight + 8
    radius: height / 2
    color: Theme.statusBg[Math.max(0, Math.min(4, status))]

    Text {
        id: text
        anchors.centerIn: parent
        text: badge.label(badge.status)
        color: Theme.statusFg[Math.max(0, Math.min(4, badge.status))]
        font.family: Theme.fontFamily
        font.pixelSize: 12
        font.weight: Font.Bold
    }
}
