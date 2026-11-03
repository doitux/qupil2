// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// The background of a row inside a card (selected, pressed, hovered): rounded where the row
// meets the card's corners, square towards the rows next to it, so a selection reads as a
// part of its card. The head of a card: roundTop only; the last row: roundBottom only.
//
//   background: CardRowHighlight {
//       anchors.margins: 4
//       roundTop: false
//       roundBottom: row.last
//       color: row.selected ? Theme.accent : "transparent"
//       focusFrame: row.visualFocus
//   }
//
// (Rectangle has corner radii of its own only from Qt 6.7: the square side is cut off here.)

import QtQuick

Item {
    id: root

    property color color: "transparent"
    property real radius: Theme.radius - 4
    property bool roundTop: true
    property bool roundBottom: true
    //! Keyboard focus: a frame around it
    property bool focusFrame: false
    property color frameColor: Theme.accentText

    clip: !roundTop || !roundBottom

    Rectangle {
        // The rounded corners of a square side lie outside and are cut off
        y: root.roundTop ? 0 : -root.radius
        width: parent.width
        height: parent.height + (root.roundTop ? 0 : root.radius) + (root.roundBottom ? 0 : root.radius)
        radius: root.radius
        color: root.color
        border.width: root.focusFrame ? 2 : 0
        border.color: root.frameColor
    }
    // The frame along a square side (cut off with the corners above)
    Rectangle {
        visible: root.focusFrame && !root.roundTop
        width: parent.width
        height: 2
        color: root.frameColor
    }
    Rectangle {
        visible: root.focusFrame && !root.roundBottom
        anchors.bottom: parent.bottom
        width: parent.width
        height: 2
        color: root.frameColor
    }
}
