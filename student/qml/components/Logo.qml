// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// The app's icon: Qupil's two figures on the lime tile (as tools/make_icons.py draws it).

import QtQuick
import QtQuick.Shapes
import QupilStudent

Rectangle {
    id: logo

    readonly property color tile: "#C9F36B"
    readonly property color front: "#6E3C5A"
    readonly property color behind: "#A35C88"
    readonly property real k: width / 64

    implicitWidth: 72
    implicitHeight: 72
    radius: width * 0.25
    color: tile

    Shape {
        anchors.fill: parent
        layer.enabled: true
        layer.samples: 4

        // The figure behind
        ShapePath {
            scale: Qt.size(logo.k, logo.k)
            strokeColor: "transparent"
            fillColor: logo.behind
            PathSvg { path: "M36 18a8 8 0 1 0 16 0a8 8 0 1 0 -16 0z" }
        }
        ShapePath {
            scale: Qt.size(logo.k, logo.k)
            strokeColor: logo.behind
            strokeWidth: 4 * logo.k
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap
            PathSvg { path: "M34 43.5c0 -7 4.5 -12 10 -12s10 5 10 12" }
        }
        // The figure in front, with a rim in the tile's colour that frees it from the one behind
        ShapePath {
            scale: Qt.size(logo.k, logo.k)
            strokeColor: logo.tile
            strokeWidth: 10 * logo.k
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap
            PathSvg { path: "M11 53.75c0 -8 5.8 -14 13 -14s13 6 13 14" }
        }
        ShapePath {
            scale: Qt.size(logo.k, logo.k)
            strokeColor: "transparent"
            fillColor: logo.tile
            PathSvg { path: "M12.4 25a11.6 11.6 0 1 0 23.2 0a11.6 11.6 0 1 0 -23.2 0z" }
        }
        ShapePath {
            scale: Qt.size(logo.k, logo.k)
            strokeColor: "transparent"
            fillColor: logo.front
            PathSvg { path: "M15 25a9 9 0 1 0 18 0a9 9 0 1 0 -18 0z" }
        }
        ShapePath {
            scale: Qt.size(logo.k, logo.k)
            strokeColor: logo.front
            strokeWidth: 4.5 * logo.k
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap
            PathSvg { path: "M11 53.75c0 -8 5.8 -14 13 -14s13 6 13 14" }
        }
    }
}
