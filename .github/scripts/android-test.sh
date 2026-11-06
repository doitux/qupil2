#!/bin/bash
# Scenarios for the Android test run (android-test.yml) on a running emulator:
#   android-test.sh <apk> <output folder>
# Each step leaves a screenshot and the lines Qupil logged meanwhile (tag "Qupil", see
# main.cpp and QupilActivity.java) in summary.md. Sample data only.
set -u
APK=$1
OUT=$2
PKG=de.qupil.app
ACT=$PKG/.QupilActivity
mkdir -p "$OUT/shots" "$OUT/ui"
SUMMARY=$OUT/summary.md
LOG=$OUT/logcat-full.txt
HERE=$(cd "$(dirname "$0")" && pwd)
FAILED=0
N=0

say() { echo "$*" | tee -a "$SUMMARY"; }
mark() { MARK=$(wc -l < "$LOG"); }
since() { tail -n +$((MARK + 1)) "$LOG"; }
# wait_for <text> <seconds>: until Qupil logged the text after the last mark
wait_for() {
    local i
    for ((i = 0; i < $2; i++)); do
        if since | grep -q -- "$1"; then
            return 0
        fi
        sleep 1
    done
    say "- **waited ${2}s in vain for:** \`$1\`"
    FAILED=1
    return 1
}
# shot <name>: screenshot (and what Android's UI dump sees of Qupil)
shot() {
    N=$((N + 1))
    local name
    name=$(printf '%02d-%s' "$N" "$1")
    adb exec-out screencap -p > "$OUT/shots/$name.png"
    if adb shell uiautomator dump /sdcard/ui.xml > /dev/null 2>&1; then
        adb pull /sdcard/ui.xml "$OUT/ui/$name.xml" > /dev/null 2>&1
    fi
    say "- screenshot \`shots/$name.png\`"
}
# dump: Android's view of the screen (Qt hands over its items and their accessible names)
dump() {
    adb shell uiautomator dump /sdcard/ui.xml > /dev/null 2>&1 && adb pull /sdcard/ui.xml "$OUT/ui/now.xml" > /dev/null 2>&1
}
# tap <pattern>: taps the first element whose name matches (regular expression)
tap() {
    local xy
    dump
    if xy=$(python3 -I "$HERE/android-ui.py" "$OUT/ui/now.xml" "$1"); then
        adb shell input tap $xy
        say "- tapped \`$1\` at $xy"
        return 0
    fi
    say "- **not on the screen:** \`$1\`"
    return 1
}
# seen <pattern>: whether an element with that name is on the screen
seen() {
    dump
    python3 -I "$HERE/android-ui.py" "$OUT/ui/now.xml" "$1" > /dev/null
}
# notes: process, top activity and Qupil's log lines since the mark
notes() {
    say "- process: \`$(adb shell pidof $PKG | tr -d '\r')\`"
    say "- top: \`$(adb shell dumpsys activity activities | grep -m1 -E 'topResumedActivity|mResumedActivity' | sed 's/^ *//' | tr -d '\r')\`"
    say ''
    say '```'
    since | grep -E ' Qupil *:|FATAL|F DEBUG|F libc|ANR in' | cut -c 7- | tail -n 40 | tee -a "$SUMMARY" > /dev/null
    say '```'
    say ''
}
step() { say ''; say "## $*"; say ''; mark; }
home() { adb shell input keyevent KEYCODE_HOME; }
# Like tapping the icon: brings the running app (or its task) back to the front
launcher() { adb shell monkey -p $PKG -c android.intent.category.LAUNCHER 1 > /dev/null 2>&1; }

: > "$SUMMARY"
say "# Android test run ${GITHUB_RUN_NUMBER:-local} (${GITHUB_SHA:0:7})"
say ''

adb wait-for-device
adb root > /dev/null 2>&1
sleep 3
adb wait-for-device

# A fixed day: the last Tuesday, 14:20 German time (the first lesson of the sample week runs)
# No "isn't responding" dialogs of the emulator's own apps (slow software graphics)
adb shell settings put global hide_error_dialogs 1
sleep 20
adb shell am broadcast -a android.intent.action.CLOSE_SYSTEM_DIALOGS > /dev/null 2>&1
adb shell settings put global auto_time 0
adb shell settings put global auto_time_zone 0
adb shell service call alarm 3 s16 Europe/Berlin > /dev/null 2>&1
days=$(( ($(date +%u) + 5) % 7 ))
set_date=$(TZ=Europe/Berlin date -d "$days days ago" +%m%d1420%Y.00)
adb shell date "$set_date" > /dev/null
say "- device: $(adb shell getprop ro.product.model | tr -d '\r'), Android $(adb shell getprop ro.build.version.release | tr -d '\r'), $(adb shell wm size | tr -d '\r' | sed 's/Physical size: //'), $(adb shell wm density | tr -d '\r' | sed 's/Physical density: //') dpi"
say "- date on the device: $(adb shell date | tr -d '\r')"

adb install -r -g "$APK" > /dev/null || { say "- **install failed**"; exit 1; }
adb shell appops set $PKG SCHEDULE_EXACT_ALARM allow > /dev/null 2>&1
adb shell settings put global always_finish_activities 0

adb logcat -c
adb logcat -v threadtime > "$LOG" 2>&1 &
LOGCAT=$!
sleep 1

step "Start with sample data"
adb shell "am start -W -n $ACT --es applicationArguments '--load-demo --lang de'" > /dev/null
wait_for "main: first frame" 120
wait_for "main: sample data loaded" 120
sleep 5
shot start-today
notes

step "To the background and back (process stays)"
home
sleep 3
shot home-screen
launcher
sleep 4
shot back
notes

step "Android ends Qupil in the background, then back"
home
sleep 3
pid=$(adb shell pidof $PKG | tr -d '\r')
adb shell am kill $PKG
sleep 2
if [ -n "$(adb shell pidof $PKG | tr -d '\r')" ]; then
    adb shell kill -9 "$pid"
    sleep 2
fi
say "- process after ending: \`$(adb shell pidof $PKG | tr -d '\r')\` (was $pid)"
launcher
wait_for "main: first frame" 90
sleep 4
shot after-process-end
notes

step "Draft: typing in the note editor, Android ends Qupil, back"
tap '^OK$' && sleep 2
tap '^Notiz$'
sleep 3
adb shell input text 'Entwurf%saus%sdem%sEmulator'
sleep 2
shot editor-typing
home
sleep 3
adb shell am kill $PKG
sleep 2
pid=$(adb shell pidof $PKG | tr -d '\r')
[ -n "$pid" ] && adb shell kill -9 "$pid" && sleep 2
launcher
wait_for "main: first frame" 90
sleep 6
shot draft-back
if seen 'Entwurf aus dem Emulator'; then
    say "- the draft is back in the editor"
else
    say "- **the draft is not on the screen**"
    FAILED=1
fi
notes
# Leave the editor without saving (the draft goes away with it)
adb shell input keyevent KEYCODE_BACK
sleep 2
adb shell input keyevent KEYCODE_BACK
sleep 2
shot editor-left
notes

step "Dark mode switched on while Qupil runs"
adb shell cmd uimode night yes > /dev/null
sleep 5
shot dark-runtime
home
sleep 2
launcher
sleep 4
shot dark-back
notes

step "Fresh start in dark mode"
adb shell am force-stop $PKG
sleep 1
launcher
wait_for "main: first frame" 90
sleep 4
shot dark-start
adb shell cmd uimode night no > /dev/null
sleep 5
shot light-again
notes

step "Landscape"
adb shell settings put system accelerometer_rotation 0
adb shell settings put system user_rotation 1
sleep 5
shot landscape
adb shell settings put system user_rotation 0
sleep 4
shot portrait-again
notes

# Android reads the developer option at the start of the system only: restart it
step "Developer option \"Don't keep activities\": background and back"
adb shell settings put global always_finish_activities 1
adb shell stop
sleep 2
adb shell start
for ((i = 0; i < 180; i++)); do
    [ "$(adb shell getprop init.svc.bootanim | tr -d '\r')" = stopped ] && adb shell pm path android > /dev/null 2>&1 && break
    sleep 1
done
sleep 25
adb shell am broadcast -a android.intent.action.CLOSE_SYSTEM_DIALOGS > /dev/null 2>&1
mark
launcher
wait_for "main: first frame" 120
sleep 5
tap '^OK$' && sleep 1
tap '^Schüler$'
sleep 3
shot dka-students
home
sleep 4
launcher
wait_for "activity: resume" 60
sleep 6
shot dka-back
if since | grep -q "activity: destroy, finishing false"; then
    say "- Android closed the window in the background (as wanted for this step)"
else
    say "- **Android kept the window: the option did not take effect**"
fi
notes
adb shell settings put global always_finish_activities 0

if [ "${QUPIL_TOUR:-0}" = 1 ]; then
    step "Screenshot tour on the emulator"
    adb shell am force-stop $PKG
    tour=/data/data/$PKG/files/tour
    adb shell "am start -W -n $ACT --es applicationArguments '--screenshots $tour --lang de'" > /dev/null
    wait_for "main: end" 1500
    adb pull "$tour" "$OUT/tour" > /dev/null 2>&1
    say "- tour screenshots: $(ls "$OUT/tour" 2>/dev/null | grep -c png)"
    notes
fi

step "Crashes and hangs during the whole run"
MARK=0
if grep -E "FATAL EXCEPTION|Fatal signal|ANR in $PKG" "$LOG" > "$OUT/crashes.txt"; then
    say "- **crash or hang found** (crashes.txt)"
    FAILED=1
else
    say "- none"
    rm -f "$OUT/crashes.txt"
fi

kill $LOGCAT 2> /dev/null
grep -E ' Qupil *:| AndroidRuntime| DEBUG +:|F libc|ANR in|de\.qupil' "$LOG" > "$OUT/logcat-qupil.txt"
say ''
say "Result: $([ $FAILED = 0 ] && echo 'all steps ran' || echo 'something failed, see above')"
exit $FAILED
