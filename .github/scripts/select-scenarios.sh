# Touch tests for selecting text in the note editor, from outside (sourced by
# android-test.sh and ios-test.sh): touches go through the system like a finger. Qupil runs
# with --ui-test-phase select: a note with three lines is in the editor, Qupil logs where
# words, buttons, its menu items and the selection handles are and what is selected (lines
# "select: …", select-touch.py reads them).
#
# The rules (TextTouch.qml, docs in the project: qupil2-editor-touch-plan.md) are the same on
# Android and iOS: double tap = word, long press = word, handles extend, Qupil's own menu
# (German: Ausschneiden, Kopieren, Einsetzen …) above the selection, never the system's
# (on the English test devices: Cut, Copy, Paste …).
#
# The calling script defines:
#   t_tap x y, t_double x y, t_long x y, t_drag x1 y1 x2 y2 [fast]   touches through the system
#     (t_double sets SEL_DOUBLE_MS: the time between the two touches)
#   t_shot name                                                screenshot (+ what the system sees)
#   t_menu                                                     edit menu items on the screen
#                                                              (Qupil's and the system's)
#   t_find pattern                                             "x y" of an element the system's accessibility sees
#   say text, SEL_LOG (Qupil's log), SEL_SCALE (1 or dpr), HERE
# Sets SEL_PASSED, SEL_TOTAL and SEL_FAILED (number of checks that went wrong).

# sel_refresh (optional, from the caller): brings SEL_LOG up to date before each reading
type sel_refresh > /dev/null 2>&1 || sel_refresh() { :; }
sel_xy() { sel_refresh; python3 -I "$HERE/select-touch.py" "$SEL_LOG" "$SEL_SCALE" "$@"; }
sel_now() { sel_refresh; python3 -I "$HERE/select-touch.py" "$SEL_LOG" - sel; }
sel_markup() { sel_refresh; python3 -I "$HERE/select-touch.py" "$SEL_LOG" - markup; }
sel_menu() { sel_refresh; python3 -I "$HERE/select-touch.py" "$SEL_LOG" - menu; }

SEL_PASSED=0
SEL_TOTAL=0
SEL_FAILED=0
SEL_ROWS=''
# Two touches further apart than this are two taps (Qupil: the system's double click
# interval, 400 ms by default); a test tool that is slower cannot double tap
SEL_DOUBLE_MAX=380

# sel_result <scenario> <expected> <got> <ok 0/1/skip>
sel_result() {
    local mark='❌'
    if [ "$4" = skip ]; then
        mark='⏭'
    else
        SEL_TOTAL=$((SEL_TOTAL + 1))
        if [ "$4" = 1 ]; then
            SEL_PASSED=$((SEL_PASSED + 1))
            mark='✅'
        else
            SEL_FAILED=$((SEL_FAILED + 1))
        fi
    fi
    SEL_ROWS="$SEL_ROWS| $mark | $1 | \`$2\` | \`$3\` |"$'\n'
}
# expect_now <scenario> <expected> <reader>: waits up to 6 s for the reader's output to be
# exactly that (Qupil's log lines come with a delay: adb logcat, the console)
expect_now() {
    local got i
    sleep 1
    for ((i = 0; i < 12; i++)); do
        got=$($3)
        [ "$got" = "$2" ] && break
        sleep 0.5
    done
    [ "$got" = "$2" ] && sel_result "$1" "$2" "$got" 1 || sel_result "$1" "$2" "$got" 0
}
expect_sel() { expect_now "$1" "$2" sel_now; }
# expect_markup <scenario> <regex>: the stored note matches it (also waits up to 6 s)
expect_markup() {
    local got i
    sleep 1
    for ((i = 0; i < 12; i++)); do
        got=$(sel_markup)
        printf '%s' "$got" | grep -q -E -- "$2" && break
        sleep 0.5
    done
    if printf '%s' "$got" | grep -q -E -- "$2"; then
        sel_result "$1" "$2" "$(echo "$got" | cut -c1-110)" 1
    else
        sel_result "$1" "$2" "$(echo "$got" | cut -c1-110)" 0
    fi
}
# expect_menu <scenario> <Qupil's items, "" = closed>: Qupil's menu shows exactly these, and
# no item of the system's own menu is on the screen. "Einsetzen" is there when the clipboard
# holds text (the device's clipboard: whatever a test before copied), so it may come or not
# where the scenario does not depend on it
expect_menu() {
    local system got i want
    want=$(printf '%s' "$2" | sed 's/^Ausschneiden, Kopieren, Alles auswählen$/Ausschneiden, Kopieren, (Einsetzen, )?Alles auswählen/')
    sleep 1
    for ((i = 0; i < 12; i++)); do
        got=$(sel_menu)
        printf '%s\n' "$got" | grep -q -x -E -- "$want" && break
        sleep 0.5
    done
    if printf '%s\n' "$got" | grep -q -x -E -- "$want"; then
        sel_result "$1" "$2" "$got" 1
    else
        sel_result "$1" "$2" "$got" 0
    fi
    system=$(t_menu | grep -v -x -E 'Ausschneiden|Kopieren|Einsetzen|Auswählen|Alles auswählen' \
        | tr '\n' ',' | sed 's/,$//; s/,/, /g')
    [ -z "$system" ] && sel_result "$1: no system menu" "" "" 1 || sel_result "$1: no system menu" "" "$system" 0
}
# clear_sel: a tap behind the last word ends the selection (the cursor goes there)
clear_sel() {
    local xy
    xy=$(sel_xy field-end) && t_tap $xy
    sleep 1.2
}
# select_word <scenario> <word>: a double tap on it. A test tool that cannot double tap
# (SEL_NO_DOUBLE_TAP=1 from the caller – idb: a second between its touches) selects with a
# long press instead; Qupil's own UI test checks the double tap there with real touches.
# A double tap that the tool made too slowly (SEL_DOUBLE_MS) is noted the same way.
select_word() {
    local xy
    xy=$(sel_xy word "$2" middle)
    if [ "${SEL_NO_DOUBLE_TAP:-0}" = 1 ]; then
        sel_result "$1" "$2" "the tool cannot double tap: long press" skip
        t_long $xy
        expect_sel "$1 (long press instead)" "$2"
        return
    fi
    SEL_DOUBLE_MS=0
    t_double $xy
    if [ "${SEL_DOUBLE_MS:-0}" -gt "$SEL_DOUBLE_MAX" ]; then
        sel_result "$1" "$2" "touches ${SEL_DOUBLE_MS} ms apart: no double tap for the tool" skip
        sleep 1.5
        xy=$(sel_xy word "$2" middle)
        t_long $xy
        expect_sel "$1 (long press instead)" "$2"
    else
        expect_sel "$1" "$2"
    fi
}

# sel_calibrate: the offset between Qupil's coordinates and the screen's, from the "B"
# button as the system's accessibility sees it (t_find prints its centre)
sel_calibrate() {
    local seen logged
    SEL_DX=0 SEL_DY=0
    export SEL_DX SEL_DY
    seen=$(t_find '^Fett$') || { say '- (no "Fett" in the accessibility tree: no offset)'; return; }
    logged=$(sel_xy button Fett) || return
    set -- $seen $logged
    SEL_DX=$(($1 - $3))
    SEL_DY=$(($2 - $4))
    export SEL_DX SEL_DY
    say "- window offset: $SEL_DX, $SEL_DY (\"Fett\" at $1 $2 on the screen, Qupil logged $3 $4)"
}

select_scenarios() {
    local xy xy2
    say ''
    say '### Selecting text by touch (note editor)'
    say ''
    if ! sel_xy word Tonleitern middle > /dev/null; then
        say '- **Qupil logged no positions** ("select: word …"): no touch tests'
        SEL_FAILED=1
        return
    fi
    sel_calibrate
    t_shot select-start

    # 1. Double tap on a word at the start of line 1: the word, Qupil's menu, B at once
    select_word '1 double tap on a word, line 1' Tonleitern
    t_shot select-1-double-tap
    expect_menu '2 menu after the double tap' 'Ausschneiden, Kopieren, Alles auswählen'
    xy=$(sel_xy button Fett)
    t_tap $xy
    expect_markup '3 "B" with the first tap' '\*\*Tonleitern\*\*'
    t_shot select-3-bold
    clear_sel
    expect_sel '4 a tap behind the text ends the selection' ''
    expect_menu '4 menu closed' ''

    # 2. Long press on a word in line 2: selected while the finger is down, stays after
    xy=$(sel_xy word Metronom middle)
    t_long $xy
    expect_sel '5 long press on a word, line 2' 'Metronom'
    t_shot select-5-long-press
    expect_menu '6 menu after the long press' 'Ausschneiden, Kopieren, Alles auswählen'
    xy=$(sel_xy button Kursiv)
    t_tap $xy
    expect_markup '7 "I" on it' '\*Metronom\*'
    clear_sel

    # 3. A phrase: the word, then its end handle to the last word
    select_word '8 double tap on "G-Dur"' G-Dur
    t_shot select-8-word
    if xy=$(sel_xy handle end); then
        xy2=$(sel_xy word D-Dur below-end)
        t_drag $xy $xy2
        expect_sel '9 end handle dragged to "D-Dur"' 'G-Dur und D-Dur'
    else
        sel_result '9 end handle dragged to "D-Dur"' 'G-Dur und D-Dur' 'no end handle' 0
    fi
    t_shot select-9-phrase
    expect_menu '10 menu again after the handle' 'Ausschneiden, Kopieren, Alles auswählen'
    xy=$(sel_xy button Unterstrichen)
    t_tap $xy
    expect_markup '11 "U" on the phrase' '\+\+G-Dur und D-Dur\+\+'
    clear_sel

    # 4. A word in the last line, then the colour
    select_word '12 double tap on a word, line 3' Mittelteil
    xy=$(sel_xy button Textfarbe)
    t_tap $xy
    expect_markup '13 colour on it' '==Mittelteil=='
    t_shot select-13-colour
    clear_sel

    # 5. Wiping over the text must not select anything
    xy=$(sel_xy word Etüde start)
    xy2=$(sel_xy word Bogeneinteilung end)
    t_drag $xy $xy2 fast
    expect_sel '14 a quick wipe over the text selects nothing' ''
    t_shot select-14-wipe
    clear_sel

    # 6. Copy and paste with Qupil's menu: "Etüde" to the end of the note
    xy=$(sel_xy word Etüde middle)
    t_long $xy
    expect_sel '15 long press on "Etüde"' 'Etüde'
    if xy=$(sel_xy button Kopieren); then
        t_tap $xy
        expect_menu '16 "Kopieren" closes the menu' ''
    else
        sel_result '16 "Kopieren" in the menu' 'Kopieren' "$(sel_menu)" 0
    fi
    clear_sel
    # A tap on the cursor opens the menu: now with "Einsetzen"
    xy=$(sel_xy field-end) && t_tap $xy
    expect_menu '17 a tap on the cursor: menu' 'Auswählen, Einsetzen, Alles auswählen'
    t_shot select-17-paste-menu
    if xy=$(sel_xy button Einsetzen); then
        t_tap $xy
        expect_markup '18 "Einsetzen" puts it at the end' 'Etüde(==)?$'
    else
        sel_result '18 "Einsetzen"' 'Einsetzen' "$(sel_menu)" 0
    fi
    t_shot select-18-pasted

    say "| | Scenario | Expected | Got |"
    say "|---|---|---|---|"
    printf '%s' "$SEL_ROWS" | tee -a "$SUMMARY" > /dev/null
    say ''
    say "$SEL_PASSED of $SEL_TOTAL touch checks as expected."
    say ''
    say "Markup at the end: \`$(sel_markup)\`"
}
