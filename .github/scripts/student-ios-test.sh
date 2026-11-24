#!/bin/bash
# The student app in the iOS simulator (student.yml):
#   student-ios-test.sh <output folder>
# - the app starts (welcome page), also with the hook for the start's links in place
# - qupil-student:… starts the app and adds the teacher; the same while the app runs
# - the button on the page qupil.de/u (student/web, in Safari) hands its link to the app;
#   without the app Safari says "the address is invalid" (checked too, as the control)
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
# tap_label <pattern> <seconds> [optional]: taps what the system's accessibility shows under
# that label; "optional": fine if it does not come
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
    if [ "${3:-}" = optional ]; then
        say "- no \`$1\` to tap"
        return 0
    fi
    say "- **not on the screen:** \`$1\`"
    FAILED=1
    return 1
}
# wait_for_answering <pattern> <seconds>: as wait_for, for a link that starts the app. On a
# slow runner the system's question comes late or the first tap gets lost: "Open" is tapped
# again whenever it is (still) on the screen.
wait_for_answering() {
    local i xy
    for ((i = 0; i < $2; i += 5)); do
        if since | grep -q -- "$1"; then
            say "- logged: \`$1\`"
            return 0
        fi
        idb ui describe-all --udid "$UDID" > "$OUT/now.json" 2> /dev/null
        if xy=$(python3 "$HERE/ios-ui.py" "$OUT/now.json" '^(Open|Öffnen)$' 2> /dev/null); then
            idb ui tap --udid "$UDID" $xy
            say "- the question is (still) there: tapped at $xy"
        fi
        sleep 5
    done
    say "- **waited ${2}s in vain for:** \`$1\`"
    FAILED=1
    return 1
}
# open_link <url>: as a link tapped elsewhere. The system asks "Open in …?" when the link
# starts the app; for the running app it hands the link over at once.
open_link() {
    xcrun simctl openurl "$UDID" "$1"
    tap_label '^(Open|Öffnen)$' 12 optional
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
    # Under Rosetta this start took more than three minutes once (the link arrived then)
    STARTED_BY_LINK=0
    wait_for_answering "link received, teacher added" 300 && STARTED_BY_LINK=1
    sleep 10
    shot link-at-start

    say ""
    say "## A link while the app is running"
    if [ "$STARTED_BY_LINK" = 1 ]; then
        mark
        open_link "qupil-student:$(link TestTokenBBBB2222)"
        wait_for "link received, teacher added" 60
        sleep 2
    else
        # A second link on top of an unanswered start says nothing
        say "- not checked: the app did not start with the first link"
    fi

    say ""
    say "## The button on the page qupil.de/u (Safari hands the link to the app)"
    # What the camera does with the QR code as long as the https link does not open the app
    # itself: Safari shows the page, its button carries qupil-student:… (23.11.2026 on an iPad:
    # "Safari cannot open the page because the address is invalid"). The page comes from this
    # repository (student/web), served here; the link behind # never leaves the simulator.
    python3 -m http.server 8765 --bind 127.0.0.1 --directory student/web > /dev/null 2>&1 &
    SERVER=$!
    sleep 2
    mark
    xcrun simctl openurl "$UDID" "http://127.0.0.1:8765/u/#$(link TestTokenDDDD4444)"
    sleep 15
    tap_label '^(Continue|Weiter|Not Now|Jetzt nicht)$' 6 optional    # Safari's first start
    # Safari's hint about its menu (first start) lies over the lower part of the page, where
    # the button is since "No app yet?" comes first: a tap on the page's heading closes it
    idb ui tap --udid "$UDID" 195 200
    sleep 3
    PAGE_BUTTON='(Open in Qupil Practice|In Qupil Üben öffnen)'
    # idb does not see into Safari's page nor its alerts: both are found in screenshots
    python3 -c 'import PIL' 2> /dev/null || python3 -m pip install --quiet --break-system-packages pillow > /dev/null 2>&1 \
        || python3 -m pip install --quiet --user pillow > /dev/null 2>&1
    xy=""
    for ((i = 0; i < 60; i += 4)); do
        idb ui describe-all --udid "$UDID" > "$OUT/now.json" 2> /dev/null
        xy=$(python3 "$HERE/ios-ui.py" "$OUT/now.json" "$PAGE_BUTTON" 2> /dev/null) && break
        xy=""
        sleep 4
    done
    shot page
    if [ -z "$xy" ]; then
        # idb does not always see into a web page: the button is the only wide bar in the accent colour
        xcrun simctl io "$UDID" screenshot "$OUT/page.png" > /dev/null 2>&1
        xy=$(python3 "$HERE/ios-find-colour.py" "$OUT/page.png" "$OUT/now.json" 6E3C5A 2> /dev/null) \
            && say "- the button found by its colour (idb does not see into the page)"
    fi
    if [ -z "$xy" ]; then
        say "- **the page's button is not on the screen**"
        python3 "$HERE/ios-ui.py" "$OUT/now.json" --list 2> /dev/null | head -n 40
        FAILED=1
    else
        # Safari's own hints (first start) take the first tap: tap until Safari reacts
        HANDED=0
        for attempt in 1 2 3 4; do
            idb ui tap --udid "$UDID" $xy
            say "- tapped the page's button at $xy"
            # Safari asks "Open in “Qupil Practice”?"
            for ((i = 0; i < 30 && HANDED == 0; i += 3)); do
                sleep 3
                if since | grep -q -- "link received, teacher added"; then
                    say "- logged: \`link received, teacher added\`"
                    HANDED=1
                    break
                fi
                # Safari's alert is not in idb's view of the screen: found in the picture
                xcrun simctl io "$UDID" screenshot "$OUT/page.png" > /dev/null 2>&1
                alert=$(python3 "$HERE/ios-find-alert.py" "$OUT/page.png" "$OUT/now.json" 2> /dev/null) || continue
                shot safari-alert
                if [ "${alert%% *}" = question ]; then
                    idb ui tap --udid "$UDID" ${alert#* }
                    say "- Safari asked \"Open in …?\", tapped \`Open\` at ${alert#* }"
                    # From here the app is in front: no more taps at the page's button
                    if wait_for "link received, teacher added" 90; then HANDED=1; else HANDED=3; fi
                else
                    say "- **Safari shows a message instead of its question (the address is invalid: no app answers to qupil-student:)**"
                    HANDED=2
                fi
            done
            [ "$HANDED" != 0 ] && break
        done
        if [ "$HANDED" != 1 ]; then
            [ "$HANDED" = 0 ] && say "- **the button did nothing: no question from Safari, no link in the app**"
            python3 "$HERE/ios-ui.py" "$OUT/now.json" --list 2> /dev/null | head -n 40
            FAILED=1
        fi
    fi
    sleep 3
    shot page-button

    if [ -n "$xy" ]; then
        say ""
        say "## The same button without the app: Safari's message (the check of the check)"
        # What the iPad showed on 23.11.2026. The page says what to do then (copy the link).
        xcrun simctl terminate "$UDID" $BUNDLE > /dev/null 2>&1
        xcrun simctl uninstall "$UDID" $BUNDLE > /dev/null 2>&1
        xcrun simctl openurl "$UDID" "http://127.0.0.1:8765/u/#$(link TestTokenEEEE5555)"
        sleep 12
        SEEN=""
        for attempt in 1 2 3; do
            idb ui tap --udid "$UDID" $xy
            for ((i = 0; i < 15; i += 3)); do
                sleep 3
                xcrun simctl io "$UDID" screenshot "$OUT/page.png" > /dev/null 2>&1
                SEEN=$(python3 "$HERE/ios-find-alert.py" "$OUT/page.png" "$OUT/now.json" 2> /dev/null) && break
                SEEN=""
            done
            [ -n "$SEEN" ] && break
        done
        shot page-button-without-app
        case "$SEEN" in
            message*) say "- Safari's message with one button, as expected"
                      idb ui tap --udid "$UDID" ${SEEN#* } ;;
            question*) say "- **Safari asks \"Open in …?\" although the app is gone**"; FAILED=1 ;;
            *) say "- **no message from Safari seen**"; FAILED=1 ;;
        esac
    fi
    kill $SERVER 2> /dev/null
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
for report in ~/Library/Logs/DiagnosticReports/*QupilStudent*; do
    [ -f "$report" ] || continue
    say "- **a crash report was written:** \`crash/$(basename "$report")\`"
    mkdir -p "$OUT/crash"
    cp "$report" "$OUT/crash/"
    python3 "$HERE/ios-crash-summary.py" "$report" | tee "$OUT/crash/$(basename "$report").txt"
    FAILED=1
done
say ""
if [ "$FAILED" = 0 ]; then say "**All fine.**"; else say "**Something failed.**"; fi
[ -n "${GITHUB_STEP_SUMMARY:-}" ] && cat "$SUMMARY" >> "$GITHUB_STEP_SUMMARY"
exit $FAILED
