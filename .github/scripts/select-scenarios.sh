# Touch tests for selecting words in the note editor, from outside (sourced by
# android-test.sh and ios-test.sh). Qupil runs with --ui-test-phase select: a note with
# three lines is in the editor, Qupil logs where words and buttons are and what is
# selected (lines "select: …", select-touch.py reads them).
#
# The calling script defines:
#   t_tap x y, t_double x y, t_long x y, t_drag x1 y1 x2 y2   touches through the system
#   t_shot name                                                screenshot (+ what the system sees)
#   t_menu                                                     prints the system's edit menu items on the screen
#   t_find pattern                                             "x y" of an element the system's accessibility sees
#   say text, SEL_LOG (Qupil's log), SEL_SCALE (1 or dpr), HERE
# Sets SEL_PASSED and SEL_TOTAL. Informational for now: what is hard on a touch screen.

sel_xy() { python3 -I "$HERE/select-touch.py" "$SEL_LOG" "$SEL_SCALE" "$@"; }
sel_now() { python3 -I "$HERE/select-touch.py" "$SEL_LOG" - sel; }
sel_markup() { python3 -I "$HERE/select-touch.py" "$SEL_LOG" - markup; }

SEL_PASSED=0
SEL_TOTAL=0
SEL_ROWS=''

# sel_result <scenario> <expected> <got> <ok 0/1>
sel_result() {
    SEL_TOTAL=$((SEL_TOTAL + 1))
    local mark='❌'
    if [ "$4" = 1 ]; then
        SEL_PASSED=$((SEL_PASSED + 1))
        mark='✅'
    fi
    SEL_ROWS="$SEL_ROWS| $mark | $1 | \`$2\` | \`$3\` |"$'\n'
}
# expect_sel <scenario> <text>: the selection is exactly that text. Qupil's log lines come
# with a delay (adb logcat, the console), so it waits up to 5 s for the expected value
expect_sel() {
    local got i
    sleep 1.5
    for ((i = 0; i < 10; i++)); do
        got=$(sel_now)
        [ "$got" = "$2" ] && break
        sleep 0.5
    done
    [ "$got" = "$2" ] && sel_result "$1" "$2" "$got" 1 || sel_result "$1" "$2" "$got" 0
}
# expect_markup <scenario> <fragment>: the stored note contains it (also waits up to 5 s)
expect_markup() {
    local got i
    sleep 1
    for ((i = 0; i < 10; i++)); do
        got=$(sel_markup)
        case "$got" in *"$2"*) break ;; esac
        sleep 0.5
    done
    case "$got" in
    *"$2"*) sel_result "$1" "$2" "$(echo "$got" | cut -c1-90)" 1 ;;
    *) sel_result "$1" "$2" "$(echo "$got" | cut -c1-90)" 0 ;;
    esac
}
# clear_sel: a tap behind the last word ends the selection (the cursor goes there)
clear_sel() {
    local xy
    xy=$(sel_xy field-end) && t_tap $xy
    sleep 1.2
}
# menu_note <scenario>: which items of the system's edit menu are on the screen
menu_note() {
    local items
    items=$(t_menu | tr '\n' ',' | sed 's/,$//; s/,/, /g')
    sel_result "$1" "(recorded)" "${items:-no menu}" 1
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
        return
    fi
    sel_calibrate
    t_shot select-start

    # 1. A word at the start of the first line: the system's menu comes up over the format buttons
    xy=$(sel_xy word Tonleitern middle)
    t_double $xy
    expect_sel '1 double tap on a word, line 1' 'Tonleitern'
    t_shot select-1-double-tap
    menu_note '1 system menu after the double tap'
    xy=$(sel_xy button Fett)
    t_tap $xy
    expect_markup '2 "B" while the system menu is open' '**Tonleitern**'
    t_shot select-2-bold
    # Does only the first tap get lost (the system closing its handles or menu)?
    case "$(sel_markup)" in
    *'**Tonleitern**'*) ;;
    *)
        xy=$(sel_xy button Fett)
        t_tap $xy
        expect_markup '2b "B" a second time' '**Tonleitern**'
        ;;
    esac
    clear_sel
    expect_sel '3 a tap behind the text ends the selection' ''

    # 2. Long press on a word in line 2
    xy=$(sel_xy word Metronom middle)
    t_long $xy
    expect_sel '4 long press on a word, line 2' 'Metronom'
    t_shot select-4-long-press
    menu_note '4 system menu after the long press'
    xy=$(sel_xy button Kursiv)
    t_tap $xy
    expect_markup '5 "I" on it' '*Metronom*'
    clear_sel

    # 3. A phrase: double tap on the first word, then the end handle to the last one
    xy=$(sel_xy word G-Dur middle)
    t_double $xy
    expect_sel '6 double tap on "G-Dur"' 'G-Dur'
    t_shot select-6-word
    xy=$(sel_xy word G-Dur below-end)
    xy2=$(sel_xy word D-Dur below-end)
    t_drag $xy $xy2
    expect_sel '7 end handle dragged to "D-Dur"' 'G-Dur und D-Dur'
    t_shot select-7-phrase
    xy=$(sel_xy button Unterstrichen)
    t_tap $xy
    expect_markup '8 "U" on the phrase' '++G-Dur und D-Dur++'
    clear_sel

    # 4. A word in the last line, then the colour
    xy=$(sel_xy word Mittelteil middle)
    t_double $xy
    expect_sel '9 double tap on a word, line 3' 'Mittelteil'
    xy=$(sel_xy button Textfarbe)
    t_tap $xy
    expect_markup '10 colour on it' '==Mittelteil=='
    t_shot select-10-colour
    clear_sel

    # 5. Wiping over the text (to scroll) must not select anything
    xy=$(sel_xy word Etüde start)
    xy2=$(sel_xy word Bogeneinteilung end)
    t_drag $xy $xy2 fast
    expect_sel '11 a quick wipe over the text selects nothing' ''
    t_shot select-11-wipe
    clear_sel

    say "| | Scenario | Expected | Got |"
    say "|---|---|---|---|"
    printf '%s' "$SEL_ROWS" | tee -a "$SUMMARY" > /dev/null
    say ''
    say "$SEL_PASSED of $SEL_TOTAL touch checks as expected (informational, does not fail the run)."
    say ''
    say "Markup at the end: \`$(sel_markup)\`"
}
