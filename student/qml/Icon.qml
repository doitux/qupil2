// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Line icons as vector paths (24 × 24, round caps), the same style as in Qupil.

import QtQuick
import QtQuick.Shapes
import QupilStudent

Item {
    id: icon

    property string name
    property color color: Theme.icon
    property real strokeWidth: 1.9

    implicitWidth: 24
    implicitHeight: 24

    readonly property var paths: ({
        book: "M4 4.5a2.5 2.5 0 0 1 2.5 -2.5h13.5v17h-13.5a2.5 2.5 0 0 0 -2.5 2.5z M4 19.5a2.5 2.5 0 0 1 2.5 -2.5h13.5",
        refresh: "M21 12a9 9 0 1 1 -3 -6.7l3 2.7 M21 3v5h-5",
        settings: "M12 9a3 3 0 1 0 0 6a3 3 0 1 0 0 -6z M19.4 15a1.7 1.7 0 0 0 .3 1.8l.1 .1a2 2 0 1 1 -2.8 2.8l-.1 -.1a1.7 1.7 0 0 0 -1.8 -.3a1.7 1.7 0 0 0 -1 1.5v.2a2 2 0 1 1 -4 0v-.1a1.7 1.7 0 0 0 -1.1 -1.5a1.7 1.7 0 0 0 -1.8 .3l-.1 .1a2 2 0 1 1 -2.8 -2.8l.1 -.1a1.7 1.7 0 0 0 .3 -1.8a1.7 1.7 0 0 0 -1.5 -1h-.2a2 2 0 1 1 0 -4h.1a1.7 1.7 0 0 0 1.5 -1.1a1.7 1.7 0 0 0 -.3 -1.8l-.1 -.1a2 2 0 1 1 2.8 -2.8l.1 .1a1.7 1.7 0 0 0 1.8 .3h.1a1.7 1.7 0 0 0 1 -1.5v-.2a2 2 0 1 1 4 0v.1a1.7 1.7 0 0 0 1 1.5a1.7 1.7 0 0 0 1.8 -.3l.1 -.1a2 2 0 1 1 2.8 2.8l-.1 .1a1.7 1.7 0 0 0 -.3 1.8v.1a1.7 1.7 0 0 0 1.5 1h.2a2 2 0 1 1 0 4h-.1a1.7 1.7 0 0 0 -1.5 1z",
        qr: "M3 3h7v7h-7z M14 3h7v7h-7z M3 14h7v7h-7z M14 14h3v3 M21 14v7h-4 M14 21v-3",
        link: "M10 14a4 4 0 0 0 5.7 0l3 -3a4 4 0 0 0 -5.7 -5.7l-1 1 M14 10a4 4 0 0 0 -5.7 0l-3 3a4 4 0 0 0 5.7 5.7l1 -1",
        eye: "M2 12s3.5 -7 10 -7s10 7 10 7s-3.5 7 -10 7s-10 -7 -10 -7z M9 12a3 3 0 1 0 6 0a3 3 0 1 0 -6 0",
        check: "M5 12.5l4.5 4.5l9.5 -10",
        chevron: "M9 6l6 6l-6 6",
        chevronDown: "M6 9l6 6l6 -6",
        chevronUp: "M6 15l6 -6l6 6",
        back: "M15 18l-6 -6l6 -6",
        lines: "M4 7h16 M4 12h16 M4 17h10",
        calendar: "M5.5 4.5h13a2.5 2.5 0 0 1 2.5 2.5v11a2.5 2.5 0 0 1 -2.5 2.5h-13a2.5 2.5 0 0 1 -2.5 -2.5v-11a2.5 2.5 0 0 1 2.5 -2.5z M3 9.5h18 M8 2.5v4 M16 2.5v4",
        music: "M9 18v-13l11 -2v13 M3 18a3 3 0 1 0 6 0a3 3 0 1 0 -6 0 M14 16a3 3 0 1 0 6 0a3 3 0 1 0 -6 0",
        question: "M3 12a9 9 0 1 0 18 0a9 9 0 1 0 -18 0 M9.5 9.5a2.5 2.5 0 1 1 3.5 2.3c-.6 .3 -1 .8 -1 1.5v.7 M12 17v.01",
        send: "M4 12l16 -8l-6 16l-3 -7z M11 13l9 -9",
        trash: "M4 7h16 M10 11v6 M14 11v6 M6 7l1 14h10l1 -14 M9 7v-3h6v3",
        close: "M6 6l12 12 M18 6l-12 12",
        star: "M12 3l2.7 5.5l6 0.9l-4.4 4.3l1 6l-5.3 -2.9l-5.3 2.9l1 -6l-4.4 -4.3l6 -0.9z",
        warning: "M12 4l9 16h-18z M12 10v4.5 M12 17.5v0.01",
        plus: "M12 5v14 M5 12h14",
        cloudOff: "M3 3l18 18 M8 8a5 5 0 0 0 -1.6 9h10.1 M10.5 5.4a6 6 0 0 1 7.5 4.1a4 4 0 0 1 2.6 6.4"
    })

    Shape {
        anchors.fill: parent
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
