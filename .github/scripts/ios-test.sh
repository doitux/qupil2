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

    # --- 2. From outside: start, a note, background, ended, back
    say '### Background and restart (from outside, idb)'
    say ''
    xcrun simctl terminate "$udid" $BUNDLE > /dev/null 2>&1
    launch --load-demo --lang de
    sleep 6
    shot start
    tap '^OK$' && sleep 2
    if tap '^Notiz$'; then
        sleep 3
        idb ui text --udid "$udid" "Entwurf aus dem Simulator"
        sleep 2
        shot editor-typing
    fi
    xcrun simctl launch "$udid" com.apple.Preferences > /dev/null
    sleep 4
    shot background
    xcrun simctl terminate "$udid" $BUNDLE > /dev/null 2>&1
    sleep 2
    launch
    sleep 4
    shot draft-back
    if seen '^Notiz speichern$'; then
        say "- the editor is open again with the draft (see the screenshot)"
    else
        say "- **the editor did not come back**"
        FAILED=1
    fi
    tap '^Abbrechen$' && sleep 2

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
