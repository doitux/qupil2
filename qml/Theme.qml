// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Qupil 2 colour theme with light and dark mode.
//
// mode "system" follows the operating system (Qt.styleHints.colorScheme, Qt >= 6.5)
// and switches live when the user changes it in the OS settings.
// mode "light" / "dark" force a scheme; with Qt >= 6.8 the native parts
// (window title bar, file dialogs) are switched as well.
//
// Register as singleton in CMake:
//   set_source_files_properties(qml/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
// Usage:  color: Theme.surface   /   Theme.mode = "dark"

pragma Singleton
import QtQuick
import QtCore

QtObject {
    id: theme

    // "system" | "light" | "dark" – stored per device, not synced
    property string mode: "system"

    readonly property bool systemDark: Qt.styleHints.colorScheme === Qt.ColorScheme.Dark
    readonly property bool dark: mode === "dark" || (mode === "system" && systemDark)

    // ---------------------------------------------------------------- surfaces
    readonly property color bg:            dark ? "#121014" : "#F4F3F6"
    readonly property color surface:       dark ? "#1D1A21" : "#FFFFFF"
    readonly property color surfaceAlt:    dark ? "#28232D" : "#F4F3F6"   // inputs, chips inside cards
    readonly property color surfaceSunken: dark ? "#18151B" : "#F8F7FA"   // list rows inside cards
    readonly property color border:        dark ? "#36303C" : "#E4E1E8"
    readonly property color divider:       dark ? "#2B2630" : "#ECE9EF"
    readonly property color segment:       dark ? "#28232D" : "#E9E6ED"   // segmented control track
    readonly property color segmentActive: dark ? "#3A3340" : "#FFFFFF"

    // ---------------------------------------------------------------- text
    readonly property color text:          dark ? "#F3EFF6" : "#1D1A22"
    readonly property color textSecondary: dark ? "#D9D2DE" : "#3B3542"
    readonly property color textMuted:     dark ? "#ADA5B4" : "#5E5866"
    readonly property color icon:          dark ? "#8F8797" : "#6A6472"

    // ---------------------------------------------------------------- brand
    readonly property color accent:         dark ? "#A35C88" : "#6E3C5A"  // filled buttons, selection
    readonly property color textOnAccent:       "#FFFFFF"
    readonly property color accentText:     dark ? "#E6B9D7" : "#6E3C5A"  // links, active labels, dates
    readonly property color accentSoft:     dark ? "#3A2534" : "#F1E7EE"
    readonly property color hero:           dark ? "#4B2742" : "#6E3C5A"  // running lesson card
    readonly property color heroText:       "#FFFFFF"
    readonly property color heroMuted:      dark ? "#DCC4D4" : "#EADDE6"
    readonly property color heroButton:     dark ? "#6B3A5D" : "#835071"
    readonly property color lime:           "#C9F36B"                     // "now", primary quick action
    readonly property color textOnLime:         "#1D1A22"
    readonly property color inverse:        dark ? "#F3EFF6" : "#1D1A22"  // active chip, pressed toggle
    readonly property color textOnInverse:      dark ? "#1D1A22" : "#FFFFFF"
    readonly property color inverseMuted:   dark ? "#5E5866" : "#CFC9D4"
    readonly property color player:         dark ? "#3A3340" : "#1D1A22"  // floating mini metronome
    readonly property color textOnPlayer:       dark ? "#F3EFF6" : "#FFFFFF"
    readonly property color playerMuted:    dark ? "#ADA5B4" : "#CFC9D4"

    // ---------------------------------------------------------------- feedback
    readonly property color danger:         dark ? "#FF8F85" : "#B3261E"  // text
    readonly property color dangerFill:     dark ? "#C42B22" : "#B3261E"  // buttons, badges
    readonly property color textOnDanger:       "#FFFFFF"
    readonly property color neutralFill:    dark ? "#4A4350" : "#5E5866"  // swipe action "edit"
    readonly property color success:        dark ? "#8FD69B" : "#2F6B3A"
    readonly property color successSoft:    dark ? "#173020" : "#DDF1DF"
    readonly property color switchOn:       dark ? "#3E9150" : "#2F6B3A"
    readonly property color switchOff:      dark ? "#4A4350" : "#B9B4BF"
    readonly property color overlay:        dark ? Qt.rgba(0, 0, 0, 0.62) : Qt.rgba(0.114, 0.102, 0.133, 0.55)
    readonly property color shadow:         dark ? Qt.rgba(0, 0, 0, 0.45) : Qt.rgba(0.114, 0.102, 0.133, 0.18)

    // Colours the user chose in rich-text notes are lightened in dark mode,
    // see noteColor(). Red is the most common one and used as reference.
    readonly property color noteRed:        dark ? "#FF8F85" : "#B3261E"

    // ---------------------------------------------------------------- piece status
    // index = piece state in the database (0 planned … 4 finished)
    readonly property var statusBg: dark
        ? ["#172A48", "#1E3A1A", "#3B3000", "#4A1D1A", "#2E2A33"]
        : ["#E3EDFF", "#D9F2C7", "#FFF0B3", "#FFD9D4", "#E5E4E7"]
    readonly property var statusFg: dark
        ? ["#A9C7FF", "#A8E08E", "#F0D45E", "#FFB4AB", "#C9C3CE"]
        : ["#1F4E8C", "#22561A", "#6B5300", "#8A1F17", "#4A4452"]

    // ---------------------------------------------------------------- icon tiles
    readonly property var tile: ({
        orange: { bg: dark ? "#3B2413" : "#FFE9D6", fg: dark ? "#FFB77A" : "#9A4A0B" },
        plum:   { bg: dark ? "#3A2534" : "#F1E7EE", fg: dark ? "#E6B9D7" : "#6E3C5A" },
        blue:   { bg: dark ? "#172A48" : "#E3EDFF", fg: dark ? "#A9C7FF" : "#1F4E8C" },
        green:  { bg: dark ? "#173020" : "#DDF1DF", fg: dark ? "#8FD69B" : "#2F6B3A" },
        yellow: { bg: dark ? "#3B3000" : "#FFF0B3", fg: dark ? "#F0D45E" : "#6B5300" },
        gray:   { bg: dark ? "#2B2630" : "#ECEAF0", fg: dark ? "#C9C3CE" : "#4A4452" }
    })

    // Avatar circles cycle through these
    readonly property var avatar: dark ? ["#A35C88", "#3D6BB0", "#3E8A50"]
                                       : ["#6E3C5A", "#1F4E8C", "#2F6B3A"]

    // ---------------------------------------------------------------- type & shape
    readonly property string fontFamily: "Figtree"
    readonly property int radiusSmall: 10
    readonly property int radius: 16
    readonly property int radiusLarge: 22
    readonly property int touchTarget: 44

    // Keeps a colour from a rich-text note readable on the current background.
    function noteColor(c) {
        const col = Qt.darker(c, 1.0)          // accepts strings and colours, returns a colour
        if (!dark || col.hslLightness >= 0.6)
            return col
        return Qt.hsla(col.hslHue, col.hslSaturation, 0.75, 1)
    }

    // ---------------------------------------------------------------- persistence
    property Settings _store: Settings {
        category: "appearance"
        property alias mode: theme.mode
    }

    // Qt >= 6.8: also switch native window decorations and dialogs.
    function _applyNativeScheme() {
        if (!("colorScheme" in Qt.styleHints))
            return
        try {
            if (mode === "dark")
                Qt.styleHints.colorScheme = Qt.ColorScheme.Dark
            else if (mode === "light")
                Qt.styleHints.colorScheme = Qt.ColorScheme.Light
            else
                Qt.styleHints.colorScheme = undefined   // reset: follow the OS again
        } catch (e) {
            // Qt 6.5 – 6.7: colorScheme is read-only; the app itself still switches.
        }
    }
    onModeChanged: _applyNativeScheme()
    Component.onCompleted: _applyNativeScheme()
}
