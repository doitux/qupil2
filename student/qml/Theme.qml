// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Colours of the student app: the teacher's Qupil (plum and lime), light and dark
// following the system.

pragma Singleton
import QtQuick

QtObject {
    // Qt.styleHints.colorScheme: Qt 6.5 and later (2 = dark); older Qt stays light
    readonly property bool dark: Qt.styleHints.colorScheme === 2

    readonly property color bg:            dark ? "#121014" : "#F4F3F6"
    readonly property color surface:       dark ? "#1D1A21" : "#FFFFFF"
    readonly property color surfaceAlt:    dark ? "#28232D" : "#F4F3F6"
    readonly property color border:        dark ? "#36303C" : "#E4E1E8"
    readonly property color divider:       dark ? "#2B2630" : "#ECE9EF"
    readonly property color segment:       dark ? "#28232D" : "#E9E6ED"
    readonly property color segmentActive: dark ? "#3A3340" : "#FFFFFF"

    readonly property color text:          dark ? "#F3EFF6" : "#1D1A22"
    readonly property color textSecondary: dark ? "#D9D2DE" : "#3B3542"
    readonly property color textMuted:     dark ? "#ADA5B4" : "#5E5866"
    readonly property color icon:          dark ? "#8F8797" : "#6A6472"

    readonly property color accent:        dark ? "#A35C88" : "#6E3C5A"
    readonly property color accentText:    dark ? "#E6B9D7" : "#6E3C5A"
    readonly property color accentSoft:    dark ? "#3A2534" : "#F1E7EE"
    readonly property color hero:          dark ? "#4B2742" : "#6E3C5A"
    readonly property color heroMuted:     dark ? "#DCC4D4" : "#EADDE6"
    readonly property color lime:          "#C9F36B"
    readonly property color textOnLime:    "#1D1A22"
    readonly property color inverse:       dark ? "#F3EFF6" : "#1D1A22"
    readonly property color textOnInverse: dark ? "#1D1A22" : "#FFFFFF"

    readonly property color success:       dark ? "#8FD69B" : "#2F6B3A"
    readonly property color successSoft:   dark ? "#173020" : "#DDF1DF"
    readonly property color warningSoft:   dark ? "#3B3000" : "#FFF0B3"
    readonly property color warningText:   dark ? "#F0D45E" : "#6B5300"
    readonly property color danger:        dark ? "#FF8F85" : "#B3261E"
    readonly property color overlay:       dark ? Qt.rgba(0, 0, 0, 0.62) : Qt.rgba(0.114, 0.102, 0.133, 0.55)

    readonly property string fontFamily: "Figtree"
    readonly property int radiusSmall: 10
    readonly property int radius: 16
    readonly property int radiusLarge: 22
    readonly property int touchTarget: 44
    readonly property int gutter: 20
}
