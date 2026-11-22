#!/bin/bash
# The student app in the iOS simulator (student.yml):
#   student-ios-test.sh <output folder>
# - the app starts (welcome page), also with the hook for the start's links in place
# - qupil-student:… starts the app and adds the teacher; the same while the app runs
# - "Open camera" (--scan) shows the scanner's sheet (the simulator has no camera: black,
#   with the hint and Cancel), and the app goes on working behind it
# The system asks "Open in “Qupil Practice”?" for a link from outside: idb taps "Open"
# (without idb the links are not checked, and the summary says so).
# Universal links (https://qupil.de/u#…) need the signed app and Apple's copy of
# qupil.de's association file: on a device only. Invented links only.
set -u
OUT=$1
BUNDLE=de.qupil.student
mkdir -p "$OUT/shots"
SUMMARY=$OUT/summary.md
LOG=$OUT/app.log
FAILED=0
N=0
: > "$LOG"

say() { echo "$*" | tee -a "$SUMMARY"; }
mark() { MARK=$(wc -l < "$LOG"); }
since() { tail -n +$((MARK + 1)) "$LOG"; }
wait_for() {
    local i
    for ((i = 0; i < $2; i++)); do
        if since | grep -q -- "$1"; then
            say "- logged: \`$1\`"
            return 0
        fi
        sleep 1
    done
    say "- **waited ${2}s in vain for:** \`$1\`"
    FAILED=1
    return 1
}
shot() {
    N=$((N + 1))
    local name
    name=$(printf '%02d-%s' "$N" "$1")
    xcrun simctl io "$UDID" screenshot "$OUT/shots/$name.png" > /dev/null 2>&1
    say "- screenshot \`shots/$name.png\`"
}
link() {
    python3 -c "
import base64, json, sys
k = base64.urlsafe_b64encode(bytes(range(32))).decode().rstrip('=')
j = json.dumps({'v': 1, 's': 'https://cloud.example.org', 't': sys.argv[1], 'p': 'Test-Passwort-1', 'k': k}, separators=(',', ':'))
print(base64.urlsafe_b64encode(j.encode()).decode().rstrip('='))" "$1"
}
running() { xcrun simctl spawn "$UDID" launchctl list 2> /dev/null | grep -q "$BUNDLE"; }
HERE=$(cd "$(dirname "$0")" && pwd)
HAVE_IDB=0
command -v idb > /dev/null 2>&1 && HAVE_IDB=1
# tap_label <pattern> <seconds>: taps what the system's accessibility shows under that label
tap_label() {
    local i xy
    for ((i = 0; i < $2; i += 2)); do
        idb ui describe-all --udid "$UDID" > "$OUT/now.json" 2> /dev/null
        if xy=$(python3 "$HERE/ios-ui.py" "$OUT/now.json" "$1" 2> /dev/null); then
            idb ui tap --udid "$UDID" $xy
            say "- tapped \`$1\` at $xy"
            return 0
        fi
        sleep 2
    done
    say "- **not on the screen:** \`$1\`"
    FAILED=1
    return 1
}
# open_link <url>: as a link tapped elsewhere; the system asks first
open_link() {
    xcrun simctl openurl "$UDID" "$1"
    tap_label '^(Open|Öffnen)$' 30
}

APP=$(find build -type d -name 'QupilStudent.app' -path '*iphonesimulator*' | head -n 1)
[ -n "$APP" ] || { say "**no simulator build of the app**"; exit 1; }

# The newest plain iPhone the runner offers
UDID=$(xcrun simctl list devices available -j | python3 -c '
import json, sys
devices = json.load(sys.stdin)["devices"]
phone = ""
for runtime in sorted(devices, key=lambda r: [int(x) if x.isdigit() else 0 for x in r.replace("-", ".").split(".")]):
    if "iOS" not in runtime:
        continue
    for d in devices[runtime]:
        name = d["name"]
        if name.startswith("iPhone") and "Pro" not in name and "Plus" not in name and "SE" not in name:
            phone = d["udid"]
print(phone)')
[ -n "$UDID" ] || { say "**no iPhone simulator**"; exit 1; }

say "# Student app in the iOS simulator"
say "- $(xcrun simctl list devices | grep "$UDID" | sed 's/ (.*//; s/^ *//')"
xcrun simctl boot "$UDID" 2> /dev/null
xcrun simctl bootstatus "$UDID" -b > /dev/null
xcrun simctl ui "$UDID" appearance light
xcrun simctl install "$UDID" "$APP" || { say "- **install failed**"; exit 1; }
xcrun simctl privacy "$UDID" grant camera $BUNDLE > /dev/null 2>&1

# What the app says (Qt logs to the system's log when no terminal is attached)
xcrun simctl spawn "$UDID" log stream --level info --style compact \
    --predicate 'process == "QupilStudent"' >> "$LOG" 2>&1 &
STREAM=$!
sleep 3

say ""
say "## The app starts"
mark
xcrun simctl launch "$UDID" $BUNDLE > /dev/null
# Under Rosetta the first picture takes its time
wait_for "Qupil student: started" 90
sleep 15
if running; then say "- the app is running"; else say "- **the app is not running (crashed at the start?)**"; FAILED=1; fi
shot welcome
[ "$HAVE_IDB" = 1 ] && idb connect "$UDID" > /dev/null 2>&1

if [ "$HAVE_IDB" = 1 ]; then
    say ""
    say "## A link starts the app (qupil-student:)"
    xcrun simctl terminate "$UDID" $BUNDLE > /dev/null 2>&1
    sleep 2
    mark
    open_link "qupil-student:$(link TestTokenAAAA1111)"
    wait_for "link received, teacher added" 90
    sleep 10
    shot link-at-start

    say ""
    say "## A link while the app is running"
    mark
    open_link "qupil-student:$(link TestTokenBBBB2222)"
    wait_for "link received, teacher added" 30
    sleep 2
else
    say ""
    say "## Links: not checked (no idb on this runner to answer the system's question)"
fi

say ""
say "## The scanner's sheet"
xcrun simctl terminate "$UDID" $BUNDLE > /dev/null 2>&1
xcrun simctl uninstall "$UDID" $BUNDLE > /dev/null 2>&1
xcrun simctl install "$UDID" "$APP"
xcrun simctl privacy "$UDID" grant camera $BUNDLE > /dev/null 2>&1
mark
xcrun simctl launch "$UDID" $BUNDLE --scan > /dev/null
wait_for "Qupil student: started" 90
sleep 15
shot scanner
if running; then say "- the app is running with the scanner open"; else say "- **the app is not running (crashed with the scanner?)**"; FAILED=1; fi
if [ "$HAVE_IDB" = 1 ]; then
    # The sheet's own button closes it, and the app says so
    mark
    tap_label '^(Cancel|Abbrechen)$' 20 && wait_for "scanner closed, no code" 20
    sleep 3
    shot after-scanner
fi

kill $STREAM 2> /dev/null
xcrun simctl shutdown "$UDID" > /dev/null 2>&1
echo "---- the app's lines in the log"
grep -E 'Qupil student' "$LOG" | cut -c1-240
echo "----"
# Crash reports of this run, if any
ls ~/Library/Logs/DiagnosticReports/ 2> /dev/null | grep -i QupilStudent && { say "- **a crash report was written**"; FAILED=1; }
say ""
if [ "$FAILED" = 0 ]; then say "**All fine.**"; else say "**Something failed.**"; fi
[ -n "${GITHUB_STEP_SUMMARY:-}" ] && cat "$SUMMARY" >> "$GITHUB_STEP_SUMMARY"
exit $FAILED
