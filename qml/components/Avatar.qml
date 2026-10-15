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
}
