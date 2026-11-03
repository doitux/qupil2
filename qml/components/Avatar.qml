// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Round avatar with a student's initials.

import QtQuick

Rectangle {
    id: avatar

    property string text: "?"
    //! Picks one of the avatar colours; same name, same colour
    property string seed: text
    property int size: 42
    //! Soft tint (on coloured cards) instead of a filled circle
    property bool soft: false
    property color ring: "transparent"
    //! Data.birthdayNote: a small cake on the circle (lime today, soft main colour since the last lesson)
    property var birthday: null
    readonly property bool cake: !!birthday && (birthday.kind === "today" || birthday.kind === "past")

    function hash(s) {
        let h = 0
        for (let i = 0; i < s.length; ++i)
            h = (h * 31 + s.charCodeAt(i)) % 997
        return h
    }

    implicitWidth: size
    implicitHeight: size
    radius: size / 2
    color: soft ? Theme.accentSoft : Theme.avatar[hash(seed) % Theme.avatar.length]
    border.width: ring.a > 0 ? 2 : 0
    border.color: ring

    Text {
        anchors.centerIn: parent
        text: avatar.text
        color: avatar.soft ? Theme.accentText : Theme.textOnAccent
        font.family: Theme.fontFamily
        font.pixelSize: Math.round(avatar.size * 0.36)
        font.weight: Font.ExtraBold
    }

    Rectangle {
        visible: avatar.cake
        readonly property bool small: avatar.size < 36
        width: small ? 16 : Math.max(20, Math.round(avatar.size * 0.44))
        height: width
        radius: width / 2
        x: avatar.size - width + (small ? 5 : 4)
        y: avatar.size - height + (small ? 5 : 4)
        color: avatar.cake && avatar.birthday.kind === "today" ? Theme.lime : Theme.accentSoft
        border.width: 2
        border.color: Theme.surface
        Icon {
            anchors.centerIn: parent
            width: parent.width * 0.6
            height: width
            name: "cake"
            color: avatar.cake && avatar.birthday.kind === "today" ? Theme.textOnLime : Theme.accentText
            strokeWidth: 2.4
        }
    }
}
