#!/bin/bash
# Scenarios for the iOS test run (ios-test.yml) on an iPhone and an iPad simulator:
#   ios-test.sh <output folder>
# 1. Qupil's own UI test (--ui-test) and its restart half, inside the app.
# 2. From outside with idb: start with sample data, type a note, send Qupil to the
#    background, end it, start it again (the note has to come back), dark mode.
# Each step leaves a screenshot and what idb saw of the screen; summary.md says how it went.
set -u
OUT=$1
BUNDLE=de.qupil.app
HERE=$(cd "$(dirname "$0")" && pwd)
mkdir -p "$OUT"
SUMMARY=$OUT/summary.md
FAILED=0
say() { echo "$*" | tee -a "$SUMMARY"; }
# idb: the command pip installed, wherever it landed; else the Python module itself
IDB_NOTE=''
if ! command -v idb > /dev/null 2>&1; then
    for d in "$(python3 -c 'import sysconfig; print(sysconfig.get_path("scripts"))' 2> /dev/null)" \
             "$(python3 -m site --user-base 2> /dev/null)/bin" /opt/homebrew/bin /usr/local/bin \
             "$HOME"/Library/Python/*/bin /Library/Frameworks/Python.framework/Versions/*/bin; do
        if [ -x "$d/idb" ]; then
            PATH="$d:$PATH"
            IDB_NOTE="idb found in $d"
            break
        fi
    done
fi
if ! command -v idb > /dev/null 2>&1; then
    if python3 -c 'import idb.cli.main' 2> /dev/null; then
        idb() { python3 -c 'import sys; from idb.cli.main import main; sys.argv[0] = "idb"; sys.exit(main())' "$@"; }
        IDB_NOTE="idb as a Python module (no command on the PATH)"
    else
        IDB_NOTE="idb missing: $(python3 -c 'import idb' 2>&1 | tail -1); python $(python3 --version 2>&1); companion: $(command -v idb_companion || echo none)"
    fi
fi

APP=$(find build -type d -name 'Qupil.app' -path '*iphonesimulator*' | head -n 1)
[ -n "$APP" ] || { say "**no simulator app built**"; exit 1; }

# run_for <seconds> <command…>: ends the command when it takes longer
run_for() {
    local limit=$1
    shift
    "$@" &
    local pid=$! waited=0
    while kill -0 $pid 2> /dev/null; do
        if [ $waited -ge "$limit" ]; then
            kill $pid 2> /dev/null
            say "- **ended after ${limit}s:** \`$*\`"
            return 124
        fi
        sleep 2
        waited=$((waited + 2))
    done
    wait $pid
}

# The newest iPhone and iPad the runner offers
read -r IPHONE IPAD < <(xcrun simctl list devices available -j | python3 -c '
import json, sys
devices = json.load(sys.stdin)["devices"]
phone = pad = ""
for runtime in sorted(devices, key=lambda r: [int(x) if x.isdigit() else 0 for x in r.replace("-", ".").split(".")]):
    if "iOS" not in runtime:
        continue
    for d in devices[runtime]:
        name = d["name"]
        if name.startswith("iPhone") and "Pro" not in name and "Plus" not in name and "SE" not in name:
            phone = d["udid"]
        if name.startswith("iPad Air"):
            pad = d["udid"]
print(phone or "-", pad or "-")')

# On-screen keyboard instead of the Mac's keyboard (as on a device without one)
defaults write com.apple.iphonesimulator ConnectHardwareKeyboard -bool false

say "# iOS test run ${GITHUB_RUN_NUMBER:-local} (${GITHUB_SHA:0:7})"
say ''

device_run() {
    local kind=$1 udid=$2
    local dir=$OUT/$kind
    mkdir -p "$dir/shots" "$dir/screen"
    local n=0
    if [ "$udid" = - ]; then
        say "## $kind: no simulator available"
        FAILED=1
        return
    fi
    say "## $kind: $(xcrun simctl list devices | grep "$udid" | sed 's/ (.*//; s/^ *//')"
    say ''
    xcrun simctl boot "$udid" 2> /dev/null
    xcrun simctl bootstatus "$udid" -b > /dev/null
    xcrun simctl status_bar "$udid" override --time 14:20 > /dev/null 2>&1
    xcrun simctl ui "$udid" appearance light
    xcrun simctl install "$udid" "$APP" || { say "- **install failed**"; FAILED=1; return; }
    idb connect "$udid" > /dev/null 2>&1
    local data
    data=$(xcrun simctl get_app_container "$udid" $BUNDLE data)

    shot() {
        n=$((n + 1))
        local name
        name=$(printf '%02d-%s' $n "$1")
        xcrun simctl io "$udid" screenshot "$dir/shots/$name.png" > /dev/null 2>&1
        idb ui describe-all --udid "$udid" > "$dir/screen/$name.json" 2> /dev/null
        say "- screenshot \`$kind/shots/$name.png\`"
    }
    describe() { idb ui describe-all --udid "$udid" > "$dir/screen/now.json" 2> /dev/null; }
    tap() {
        local xy
        describe
        if xy=$(python3 -I "$HERE/ios-ui.py" "$dir/screen/now.json" "$1"); then
            idb ui tap --udid "$udid" $xy
            say "- tapped \`$1\` at $xy"
            return 0
        fi
        say "- **not on the screen:** \`$1\`"
        return 1
    }
    seen() {
        describe
        python3 -I "$HERE/ios-ui.py" "$dir/screen/now.json" "$1" > /dev/null
    }
    # launch [arguments…]: starts Qupil, its output goes to the log (until it ends)
    launch() {
        xcrun simctl launch --console-pty --terminate-running-process "$udid" $BUNDLE "$@" >> "$dir/qupil.log" 2>&1 &
        sleep 8
    }

    # --- 1. Qupil's own UI test, then its restart half on the same data
    say '### UI test (inside Qupil)'
    say ''
    local layout
    for phase in first restart; do
        local args=(--ui-test "$data/Documents/ui-test/$phase" --lang de --data-dir "$data/Library/ui-test-data")
        [ $phase = restart ] && args+=(--ui-test-phase restart)
        run_for 900 xcrun simctl launch --console-pty --terminate-running-process "$udid" $BUNDLE "${args[@]}" \
            > "$dir/ui-test-$phase.log" 2>&1
        if [ -f "$data/Documents/ui-test/$phase/results.md" ]; then
            sed -e '1,/^$/d' "$data/Documents/ui-test/$phase/results.md" >> "$SUMMARY"
            say ''
            grep -q '❌' "$data/Documents/ui-test/$phase/results.md" && FAILED=1
        else
            say "- **$phase: no result** (ui-test-$phase.log)"
            FAILED=1
        fi
    done
    mkdir -p "$dir/ui-test"
    cp -R "$data/Documents/ui-test/." "$dir/ui-test/" 2> /dev/null

    # --- 2. From outside: a note typed (by Qupil's draft phase), Settings in front,
    #        Qupil ended by the system, started again – the note has to be back
    say '### Background and ended by the system'
    say ''
    xcrun simctl terminate "$udid" $BUNDLE > /dev/null 2>&1
    local ddir=$data/Library/draft-data
    rm -rf "$data/Documents/draft"
    xcrun simctl launch --console-pty --terminate-running-process "$udid" $BUNDLE \
        --ui-test "$data/Documents/draft/prepare" --ui-test-phase draft --lang de --data-dir "$ddir" \
        >> "$dir/qupil.log" 2>&1 &
    for ((i = 0; i < 120; i++)); do
        [ -f "$data/Documents/draft/prepare/results.md" ] && break
        sleep 2
    done
    sleep 2
    shot editor-typing
    xcrun simctl launch "$udid" com.apple.Preferences > /dev/null
    sleep 5
    shot background
    xcrun simctl terminate "$udid" $BUNDLE > /dev/null 2>&1
    sleep 2
    run_for 300 xcrun simctl launch --console-pty --terminate-running-process "$udid" $BUNDLE \
        --ui-test "$data/Documents/draft/back" --ui-test-phase restart --lang de --data-dir "$ddir" \
        > "$dir/draft-back.log" 2>&1
    if grep -q '✅' "$data/Documents/draft/back/results.md" 2> /dev/null; then
        say "- the editor came back with the unsaved note"
    else
        say "- **the note did not come back** (draft-back.log)"
        FAILED=1
    fi
    mkdir -p "$dir/draft"
    cp -R "$data/Documents/draft/." "$dir/draft/" 2> /dev/null

    # --- idb (when it installed): what the system's accessibility sees, a tap from outside
    [ -n "$IDB_NOTE" ] && say "- $IDB_NOTE"
    if command -v idb > /dev/null 2>&1 || declare -F idb > /dev/null; then
        say ''
        say '### idb'
        say ''
        launch --load-demo --lang de
        sleep 6
        shot start
        tap '^OK$' && sleep 2
        if tap '^Notiz$'; then
            sleep 3
            idb ui text --udid "$udid" "Mit idb getippt"
            sleep 2
            shot idb-typing
            tap '^Abbrechen$' && sleep 2
        fi
        xcrun simctl terminate "$udid" $BUNDLE > /dev/null 2>&1

        # --- Selecting words by touch in the note editor (select-scenarios.sh)
        rm -rf "$data/Documents/select"
        SEL_LOG=$dir/select.log
        : > "$SEL_LOG"
        xcrun simctl launch --console-pty --terminate-running-process "$udid" $BUNDLE \
            --ui-test "$data/Documents/select" --ui-test-phase select --lang de --data-dir "$data/Library/select-data" \
            >> "$SEL_LOG" 2>&1 &
        for ((i = 0; i < 120; i++)); do
            [ -f "$data/Documents/select/results.md" ] && break
            sleep 2
        done
        sleep 3
        SEL_SCALE=1
        t_tap() { idb ui tap --udid "$udid" "$1" "$2"; }
        # Two taps right after each other (idb has no double tap); the time between them is logged
        t_double() {
            local s e
            s=$(python3 -c 'import time; print(time.time())')
            idb ui tap --udid "$udid" "$1" "$2"
            idb ui tap --udid "$udid" "$1" "$2"
            e=$(python3 -c 'import time; print(time.time())')
            echo "double tap took $(python3 -c "print(round(($e - $s) * 1000))") ms" >> "$dir/select-taps.txt"
            # Roughly the time between the touches: one call of idb each
            SEL_DOUBLE_MS=$(python3 -c "print(round(($e - $s) * 500))")
        }
        t_long() { idb ui tap --udid "$udid" --duration 1.0 "$1" "$2"; }
        t_drag() { idb ui swipe --udid "$udid" --duration "$([ "${5:-}" = fast ] && echo 0.15 || echo 1.0)" "$1" "$2" "$3" "$4"; }
        t_shot() { shot "$1"; }
        t_find() { describe; python3 -I "$HERE/ios-ui.py" "$dir/screen/now.json" "$1"; }
        t_menu() {
            describe
            python3 -I "$HERE/ios-ui.py" "$dir/screen/now.json" --list | sed 's/^ *[0-9]* *[0-9]*  [A-Za-z]* *//' \
                | grep -E -i '^(Ausschneiden|Kopieren|Einsetzen|Alles auswählen|Auswählen|Teilen|Nachschlagen|Übersetzen|Cut|Copy|Paste|Select All|Select)$' | sort -u
        }
        # idb's touches are 0.6–1.8 s apart: no double tap (Qupil's UI test above has it)
        SEL_NO_DOUBLE_TAP=1
        . "$HERE/select-scenarios.sh"
        select_scenarios
        [ -f "$dir/select-taps.txt" ] && say "- $(sort -u "$dir/select-taps.txt" | head -3 | tr '\n' ';')"
        [ "$SEL_FAILED" = 0 ] || FAILED=1
        xcrun simctl terminate "$udid" $BUNDLE > /dev/null 2>&1
    else
        say '- (idb not installed: no taps from outside)'
    fi
    launch --load-demo --lang de
    sleep 6

    # --- 3. Dark mode while running and at the start
    say ''
    say '### Dark mode'
    say ''
    xcrun simctl ui "$udid" appearance dark
    sleep 4
    shot dark-running
    xcrun simctl terminate "$udid" $BUNDLE > /dev/null 2>&1
    launch
    sleep 4
    shot dark-start
    xcrun simctl ui "$udid" appearance light
    sleep 4
    shot light-again
    xcrun simctl terminate "$udid" $BUNDLE > /dev/null 2>&1

    if [ "${QUPIL_TOUR:-0}" = 1 ]; then
        say ''
        say '### Screenshot tour'
        run_for 1800 xcrun simctl launch --console-pty --terminate-running-process "$udid" $BUNDLE \
            --screenshots "$data/Documents/tour" --lang de > "$dir/tour.log" 2>&1
        mkdir -p "$dir/tour"
        cp -R "$data/Documents/tour/." "$dir/tour/" 2> /dev/null
        say "- $(ls "$dir/tour" 2> /dev/null | grep -c png) pictures"
    fi

    grep -E "Draft|QML|qml:|Warning|warning" "$dir/qupil.log" 2> /dev/null | tail -n 30 > "$dir/qupil-excerpt.txt"
    xcrun simctl shutdown "$udid" > /dev/null 2>&1
    say ''
}

device_run iphone "$IPHONE"
device_run ipad "$IPAD"

say "Result: $([ $FAILED = 0 ] && echo 'all steps ran' || echo 'something failed, see above')"
exit $FAILED
