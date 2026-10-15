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
    property real strokeWidth: name === "more" || name === "grip" ? 2.4 : 1.8

    implicitWidth: 24
    implicitHeight: 24

    // One SVG path per icon; several subpaths are allowed
    readonly property var paths: ({
        calendar: "M5.5 4.5h13a2.5 2.5 0 0 1 2.5 2.5v11a2.5 2.5 0 0 1 -2.5 2.5h-13a2.5 2.5 0 0 1 -2.5 -2.5v-11a2.5 2.5 0 0 1 2.5 -2.5z M3 9.5h18 M8 2.5v4 M16 2.5v4",
        students: "M5.5 8a3.5 3.5 0 1 0 7 0a3.5 3.5 0 1 0 -7 0 M2.5 20c0 -3.6 2.9 -6 6.5 -6s6.5 2.4 6.5 6 M16 4.6a3.5 3.5 0 0 1 0 6.8 M18 14.3c2.1 0.7 3.5 2.8 3.5 5.7",
        events: "M9 18v-13l11 -2v13 M3 18a3 3 0 1 0 6 0a3 3 0 1 0 -6 0 M14 16a3 3 0 1 0 6 0a3 3 0 1 0 -6 0",
        metronome: "M8.5 3h7l4 18h-15z M12 16l5 -9 M6.6 16h10.8",
        more: "M3.8 12a1.2 1.2 0 1 0 2.4 0a1.2 1.2 0 1 0 -2.4 0 M10.8 12a1.2 1.2 0 1 0 2.4 0a1.2 1.2 0 1 0 -2.4 0 M17.8 12a1.2 1.2 0 1 0 2.4 0a1.2 1.2 0 1 0 -2.4 0",
        sync: "M7 18a4.5 4.5 0 0 1 -0.6 -9a6 6 0 0 1 11.6 0.5a4 4 0 0 1 -0.5 8.5z M9.5 13.5l2 2l3.5 -3.5",
        info: "M3 12a9 9 0 1 0 18 0a9 9 0 1 0 -18 0 M12 11v6 M12 7.5v0.01",
        tools: "M14.7 6.3a4 4 0 0 0 -5.4 5.4l-5.8 5.8a1.5 1.5 0 0 0 2.1 2.1l5.8 -5.8a4 4 0 0 0 5.4 -5.4l-2.4 2.4l-2.1 -2.1z",
        "import": "M12 3v12 M7.5 10.5l4.5 4.5l4.5 -4.5 M4 15v3.5a2.5 2.5 0 0 0 2.5 2.5h11a2.5 2.5 0 0 0 2.5 -2.5v-3.5",
        backup: "M3.5 4.5h17v4h-17z M5 8.5v10a2 2 0 0 0 2 2h10a2 2 0 0 0 2 -2v-10 M10 12.5h4",
        restore: "M4 12a8 8 0 1 0 2.34 -5.66 M4 4v4.5h4.5 M12 8v4.5l3 2",
        plus: "M12 5v14 M5 12h14",
        minus: "M5 12h14",
        folder: "M3.5 7a2 2 0 0 1 2 -2h4l2 2.5h7a2 2 0 0 1 2 2v8.5a2 2 0 0 1 -2 2h-13a2 2 0 0 1 -2 -2z",
        file: "M14 3h-7a2 2 0 0 0 -2 2v14a2 2 0 0 0 2 2h10a2 2 0 0 0 2 -2v-11z M14 3v5h5",
        check: "M5 12.5l4.5 4.5l9.5 -10",
        warning: "M12 4l9 16h-18z M12 10v4.5 M12 17.5v0.01",
        database: "M4 6c0 -1.7 3.6 -3 8 -3s8 1.3 8 3s-3.6 3 -8 3s-8 -1.3 -8 -3z M4 6v12c0 1.7 3.6 3 8 3s8 -1.3 8 -3v-12 M4 12c0 1.7 3.6 3 8 3s8 -1.3 8 -3",
        chevron: "M9 6l6 6l-6 6",
        chevronDown: "M6 9l6 6l6 -6",
        bell: "M6 9a6 6 0 0 1 12 0c0 6 2.5 7.5 2.5 7.5h-17s2.5 -1.5 2.5 -7.5z M10 20a2 2 0 0 0 4 0",
        cake: "M4 21h16v-8a2 2 0 0 0 -2 -2h-12a2 2 0 0 0 -2 2z M4 16c2 0 2 -1.5 4 -1.5s2 1.5 4 1.5s2 -1.5 4 -1.5s2 1.5 4 1.5 M12 11v-4 M12 4.5v0.01",
        star: "M12 3l2.7 5.5l6 0.9l-4.4 4.3l1 6l-5.3 -2.9l-5.3 2.9l1 -6l-4.4 -4.3l6 -0.9z",
        edit: "M4 20h4l11 -11l-4 -4l-11 11z",
        trash: "M4 7h16 M10 11v6 M14 11v6 M6 7l1 14h10l1 -14 M9 7v-3h6v3",
        search: "M4 11a7 7 0 1 0 14 0a7 7 0 1 0 -14 0 M16 16l4.5 4.5",
        close: "M6 6l12 12 M18 6l-12 12",
        grip: "M7.8 6a1.2 1.2 0 1 0 2.4 0a1.2 1.2 0 1 0 -2.4 0 M13.8 6a1.2 1.2 0 1 0 2.4 0a1.2 1.2 0 1 0 -2.4 0 M7.8 12a1.2 1.2 0 1 0 2.4 0a1.2 1.2 0 1 0 -2.4 0 M13.8 12a1.2 1.2 0 1 0 2.4 0a1.2 1.2 0 1 0 -2.4 0 M7.8 18a1.2 1.2 0 1 0 2.4 0a1.2 1.2 0 1 0 -2.4 0 M13.8 18a1.2 1.2 0 1 0 2.4 0a1.2 1.2 0 1 0 -2.4 0",
        arrowUp: "M12 19v-14 M6 11l6 -6l6 6",
        arrowDown: "M12 5v14 M6 13l6 6l6 -6",
        play: "M7 5l12 7l-12 7z",
        stop: "M7 7h10v10h-10z",
        speaker: "M4 9h4l5 -4v14l-5 -4h-4z M16.5 8.5a5 5 0 0 1 0 7 M19 6a8.5 8.5 0 0 1 0 12",
        userPlus: "M5.5 8a3.5 3.5 0 1 0 7 0a3.5 3.5 0 1 0 -7 0 M2.5 20c0 -3.6 2.9 -6 6.5 -6s6.5 2.4 6.5 6 M19 8v6 M16 11h6",
        book: "M4 5.5a2 2 0 0 1 2 -2h13v15h-13a2 2 0 0 0 -2 2z M4 20.5a2 2 0 0 0 2 2h13v-4",
        settings: "M9 12a3 3 0 1 0 6 0a3 3 0 1 0 -6 0 M12 2.5v3 M12 18.5v3 M2.5 12h3 M18.5 12h3 M5.3 5.3l2.1 2.1 M16.6 16.6l2.1 2.1 M5.3 18.7l2.1 -2.1 M16.6 7.4l2.1 -2.1",
        document: "M14 3h-7a2 2 0 0 0 -2 2v14a2 2 0 0 0 2 2h10a2 2 0 0 0 2 -2v-11z M14 3v5h5 M9 13h6 M9 17h6",
        clock: "M3 12a9 9 0 1 0 18 0a9 9 0 1 0 -18 0 M12 7v5l3 2",
        instrument: "M14 4l6 6 M16 8l-6.5 6.5 M5 14.5a3 3 0 0 0 -0.5 4.5c1.5 1.5 4 1 5.5 -0.5s1.5 -3.5 0 -4.5s-3.5 -1 -5 0.5z",
        fork: "M9 3v7a3 3 0 0 0 6 0v-7 M12 13v8",
        archive: "M3.5 4.5h17v4h-17z M5 8.5v10a2 2 0 0 0 2 2h10a2 2 0 0 0 2 -2v-10 M10 12.5h4",
        cloud: "M7 18a4.5 4.5 0 0 1 -0.6 -9a6 6 0 0 1 11.6 0.5a4 4 0 0 1 -0.5 8.5z",
        cloudOff: "M7 18a4.5 4.5 0 0 1 -0.6 -9a6 6 0 0 1 11.6 0.5a4 4 0 0 1 -0.5 8.5z M12 10.5v3 M12 16v0.01",
        wifi: "M2.5 9a14 14 0 0 1 19 0 M5.5 12.3a9.5 9.5 0 0 1 13 0 M8.6 15.6a5 5 0 0 1 6.8 0 M11.2 19a0.8 0.8 0 1 0 1.6 0a0.8 0.8 0 1 0 -1.6 0",
        key: "M5 15a4 4 0 1 0 8 0a4 4 0 1 0 -8 0 M12 12l8 -8 M17 7l2.5 2.5 M15 9l2 2",
        device: "M7.5 3h9a1.5 1.5 0 0 1 1.5 1.5v15a1.5 1.5 0 0 1 -1.5 1.5h-9a1.5 1.5 0 0 1 -1.5 -1.5v-15a1.5 1.5 0 0 1 1.5 -1.5z M11 18h2",
        laptop: "M5 5.5h14a1 1 0 0 1 1 1v9h-16v-9a1 1 0 0 1 1 -1z M2.5 18.5h19"
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
