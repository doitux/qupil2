// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Line icons drawn as vector paths (24 × 24 grid, round caps), coloured from the theme.
// Usage: Icon { name: "calendar"; color: Theme.accentText }

import QtQuick
import QtQuick.Shapes

Item {
    id: icon

    property string name
    property color color: Theme.icon
    property real strokeWidth: name === "more" ? 3 : 1.8

    implicitWidth: 24
    implicitHeight: 24

    // One SVG path per icon; several subpaths are allowed
    readonly property var paths: ({
        calendar: "M5.5 4.5h13a2.5 2.5 0 0 1 2.5 2.5v11a2.5 2.5 0 0 1 -2.5 2.5h-13a2.5 2.5 0 0 1 -2.5 -2.5v-11a2.5 2.5 0 0 1 2.5 -2.5z M3 9.5h18 M8 2.5v4 M16 2.5v4",
        students: "M5.5 8a3.5 3.5 0 1 0 7 0a3.5 3.5 0 1 0 -7 0 M2.5 20c0 -3.6 2.9 -6 6.5 -6s6.5 2.4 6.5 6 M16 4.6a3.5 3.5 0 0 1 0 6.8 M18 14.3c2.1 0.7 3.5 2.8 3.5 5.7",
        events: "M9 18v-13l11 -2v13 M3 18a3 3 0 1 0 6 0a3 3 0 1 0 -6 0 M14 16a3 3 0 1 0 6 0a3 3 0 1 0 -6 0",
        metronome: "M8.5 3h7l4 18h-15z M12 16l5 -9 M6.6 16h10.8",
        more: "M5 12h0.01 M12 12h0.01 M19 12h0.01",
        sync: "M7 18a4.5 4.5 0 0 1 -0.6 -9a6 6 0 0 1 11.6 0.5a4 4 0 0 1 -0.5 8.5z M9.5 13.5l2 2l3.5 -3.5",
        info: "M3 12a9 9 0 1 0 18 0a9 9 0 1 0 -18 0 M12 11v6 M12 7.5v0.01",
        tools: "M14.7 6.3a4 4 0 0 0 -5.4 5.4l-5.8 5.8a1.5 1.5 0 0 0 2.1 2.1l5.8 -5.8a4 4 0 0 0 5.4 -5.4l-2.4 2.4l-2.1 -2.1z",
        "import": "M12 3v12 M7.5 10.5l4.5 4.5l4.5 -4.5 M4 15v3.5a2.5 2.5 0 0 0 2.5 2.5h11a2.5 2.5 0 0 0 2.5 -2.5v-3.5",
        backup: "M3.5 4.5h17v4h-17z M5 8.5v10a2 2 0 0 0 2 2h10a2 2 0 0 0 2 -2v-10 M10 12.5h4",
        restore: "M4 12a8 8 0 1 0 2.34 -5.66 M4 4v4.5h4.5 M12 8v4.5l3 2",
        plus: "M12 5v14 M5 12h14",
        folder: "M3.5 7a2 2 0 0 1 2 -2h4l2 2.5h7a2 2 0 0 1 2 2v8.5a2 2 0 0 1 -2 2h-13a2 2 0 0 1 -2 -2z",
        file: "M14 3h-7a2 2 0 0 0 -2 2v14a2 2 0 0 0 2 2h10a2 2 0 0 0 2 -2v-11z M14 3v5h5",
        check: "M5 12.5l4.5 4.5l9.5 -10",
        warning: "M12 4l9 16h-18z M12 10v4.5 M12 17.5v0.01",
        database: "M4 6c0 -1.7 3.6 -3 8 -3s8 1.3 8 3s-3.6 3 -8 3s-8 -1.3 -8 -3z M4 6v12c0 1.7 3.6 3 8 3s8 -1.3 8 -3v-12 M4 12c0 1.7 3.6 3 8 3s8 -1.3 8 -3",
        chevron: "M9 6l6 6l-6 6"
    })

    Shape {
        anchors.fill: parent
        // Multisampling gives smooth edges without blurring
        layer.enabled: true
        layer.samples: 4

        ShapePath {
            readonly property real k: Math.min(icon.width, icon.height) / 24

            scale: Qt.size(k, k)
            strokeColor: icon.color
            strokeWidth: icon.strokeWidth * k
            fillColor: "transparent"
            capStyle: ShapePath.RoundCap
            joinStyle: ShapePath.RoundJoin
            PathSvg { path: icon.paths[icon.name] ?? "" }
        }
    }
}
