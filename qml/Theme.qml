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

    // ---------------------------------------------------------------- your two colours
    // Chosen under Appearance, per device. Empty: Qupil's plum and lime (from the
    // timetable colours of Qupil 1.x), with the hand-tuned shades below.
    property string accentChoice: ""
    property string highlightChoice: ""
    readonly property color defaultAccent: "#6E3C5A"
    readonly property color defaultHighlight: "#C9F36B"
    readonly property bool customAccent: accentChoice !== "" && !Qt.colorEqual(accentChoice, defaultAccent)
    readonly property color accentBase: customAccent ? accentChoice : defaultAccent
    readonly property color highlightBase: highlightChoice !== "" ? highlightChoice : defaultHighlight

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
    // Every shade of the main colour stays readable: white text on filled areas,
    // coloured text on the page (see _readable)
    readonly property color accent:         !customAccent ? (dark ? "#A35C88" : "#6E3C5A")  // filled buttons, selection
                                            : dark ? _readable(_shade(accentBase, 1, 0.5), "#FFFFFF", 4.5, false)
                                                   : _readable(accentBase, "#FFFFFF", 4.5, false)
    readonly property color textOnAccent:       "#FFFFFF"
    readonly property color accentText:     !customAccent ? (dark ? "#E6B9D7" : "#6E3C5A")  // links, active labels, dates
                                            : dark ? _readable(_shade(accentBase, 1.6, 0.81), surface, 7, true)
                                                   : _readable(accentBase, "#FFFFFF", 5, false)
    readonly property color accentSoft:     !customAccent ? (dark ? "#3A2534" : "#F1E7EE")
                                            : dark ? _shade(accentBase, 0.75, 0.185) : _shade(accentBase, 0.95, 0.93)
    readonly property color hero:           !customAccent ? (dark ? "#4B2742" : "#6E3C5A")  // running lesson card
                                            : dark ? _readable(_shade(accentBase, 1.07, 0.225), "#FFFFFF", 7, false)
                                                   : _readable(accentBase, "#FFFFFF", 4.5, false)
    readonly property color heroText:       "#FFFFFF"
    readonly property color heroMuted:      !customAccent ? (dark ? "#DCC4D4" : "#EADDE6")
                                            : dark ? _shade(accentBase, 0.85, 0.82) : _shade(accentBase, 0.8, 0.89)
    readonly property color heroButton:     !customAccent ? (dark ? "#6B3A5D" : "#835071")
                                            : dark ? _shade(hero, 1, Math.min(0.42, hero.hslLightness + 0.1))
                                                   : _shade(hero, 0.82, Math.min(0.55, hero.hslLightness + 0.08))
    readonly property color lime:           highlightBase                 // "now", primary quick action
    readonly property color textOnLime:     textOn(highlightBase)
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

    // Marked text in lesson notes (==…==): red unless another colour is chosen
    // under Appearance; always readable on the page
    property string markChoice: ""
    readonly property color defaultMark: "#B3261E"
    readonly property bool customMark: markChoice !== "" && !Qt.colorEqual(markChoice, defaultMark)
    readonly property color noteMark:       !customMark ? (dark ? "#FF8F85" : "#B3261E")
                                            : dark ? _readable(_shade(markChoice, 1, 0.75), surface, 4.5, true)
                                                   : _readable(markChoice, "#FFFFFF", 4.5, false)

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
        plum:   { bg: accentSoft, fg: accentText },   // the main colour
        blue:   { bg: dark ? "#172A48" : "#E3EDFF", fg: dark ? "#A9C7FF" : "#1F4E8C" },
        green:  { bg: dark ? "#173020" : "#DDF1DF", fg: dark ? "#8FD69B" : "#2F6B3A" },
        yellow: { bg: dark ? "#3B3000" : "#FFF0B3", fg: dark ? "#F0D45E" : "#6B5300" },
        gray:   { bg: dark ? "#2B2630" : "#ECEAF0", fg: dark ? "#C9C3CE" : "#4A4452" }
    })

    // Avatar circles cycle through these
    readonly property var avatar: dark ? [accent, "#3D6BB0", "#3E8A50"]
                                       : [accent, "#1F4E8C", "#2F6B3A"]

    // ---------------------------------------------------------------- type & shape
    readonly property string fontFamily: "Figtree"
    readonly property int radiusSmall: 10
    readonly property int radius: 16
    readonly property int radiusLarge: 22
    readonly property int touchTarget: 44
    //! Hints while a mouse rests on a button (Tip): not on phones and tablets
    readonly property bool hoverHints: Qt.platform.os !== "ios" && Qt.platform.os !== "android"

    // ---------------------------------------------------------------- colour helpers
    //! Dark or white text, whichever is easier to read on c
    function textOn(c) {
        return _contrast(c, "#1D1A22") >= _contrast(c, "#FFFFFF") ? "#1D1A22" : "#FFFFFF"
    }
    // Same hue, saturation × satFactor, the given lightness
    function _shade(c, satFactor, lightness) {
        const col = Qt.darker(c, 1.0)
        return Qt.hsla(Math.max(0, col.hslHue), Math.min(1, col.hslSaturation * satFactor),
                       Math.max(0, Math.min(1, lightness)), 1)
    }
    // WCAG contrast ratio of two colours (1 … 21)
    function _contrast(a, b) {
        const lum = function(c) {
            const col = Qt.darker(c, 1.0)
            const ch = v => v <= 0.03928 ? v / 12.92 : Math.pow((v + 0.055) / 1.055, 2.4)
            return 0.2126 * ch(col.r) + 0.7152 * ch(col.g) + 0.0722 * ch(col.b)
        }
        const la = lum(a)
        const lb = lum(b)
        return (Math.max(la, lb) + 0.05) / (Math.min(la, lb) + 0.05)
    }
    // Lighter or darker in small steps until the contrast against bg is reached
    function _readable(c, bg, ratio, lighter) {
        let col = Qt.darker(c, 1.0)
        let l = col.hslLightness
        for (let i = 0; i < 40 && _contrast(col, bg) < ratio; ++i) {
            l = lighter ? Math.min(1, l + 0.025) : Math.max(0, l - 0.025)
            col = _shade(c, 1, l)
        }
        return col
    }

    // ---------------------------------------------------------------- persistence
    property Settings _store: Settings {
        category: "appearance"
        property alias mode: theme.mode
        property alias accentColor: theme.accentChoice
        property alias highlightColor: theme.highlightChoice
        property alias markColor: theme.markChoice
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
