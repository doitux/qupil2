// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// A birthday as a small sign in lists: the cake on lime (today) or on the soft main
// colour (had a birthday since the last lesson); with how many when several students of
// one lesson are meant. note: Data.birthdayNote (of the first of them)

import QtQuick
import Qupil

Rectangle {
    id: tag

    property var note: ({})
    property int count: 1
    readonly property bool today: !!note && note.kind === "today"
    readonly property bool shown: !!note && (note.kind === "today" || note.kind === "past")
    readonly property color ink: today ? Theme.textOnLime : Theme.accentText

    visible: shown
    implicitWidth: count > 1 ? row.implicitWidth + 12 : 24
    implicitHeight: 24
    radius: 12
    color: today ? Theme.lime : Theme.accentSoft
    Accessible.role: Accessible.Graphic
    Accessible.name: today ? qsTrId("birthday.today") : qsTrId("birthday.past")

    Row {
        id: row
        anchors.centerIn: parent
        spacing: 3
        Icon {
            anchors.verticalCenter: parent.verticalCenter
            width: 15
            height: 15
            name: "cake"
            color: tag.ink
            strokeWidth: 2.2
        }
        Text {
            visible: tag.count > 1
            anchors.verticalCenter: parent.verticalCenter
            text: String(tag.count)
            color: tag.ink
            font.family: Theme.fontFamily
            font.pixelSize: 12
            font.weight: Font.ExtraBold
        }
    }
}
