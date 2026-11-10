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
# tap_until <pattern> <expected>: taps until the expected element is there (at most 3 times;
# a reminder dialog that comes up late takes the first tap)
tap_until() {
    local i
    for ((i = 0; i < 3; i++)); do
        tap "$1" > /dev/null || true
        sleep 2
        seen "$2" && { say "- tapped \`$1\`, \`$2\` is there"; return 0; }
    done
    say "- **\`$2\` did not come after tapping \`$1\`**"
    return 1
}
# dismiss: closes every reminder dialog shown at the start (there can be several in a row)
dismiss() {
    local i
    for ((i = 0; i < 4; i++)); do
        seen '^OK$' || return 0
        tap '^OK$' > /dev/null
        sleep 2
    done
}
# notes: process, top activity and Qupil's log lines since the mark
notes() {
    say "- process: \`$(adb shell pidof $PKG | tr -d '\r')\`"
    say "- top: \`$(adb shell dumpsys activity activities | grep -m1 -E 'topResumedActivity|mResumedActivity' | sed 's/^ *//' | tr -d '\r')\`"
    say ''
    say '```'
    since | grep -E ' (Qupil|qml|js|default) *:|FATAL|F DEBUG|F libc|ANR in' | cut -c 7- | tail -n 40 | tee -a "$SUMMARY" > /dev/null
    say '```'
    say ''
}
step() { say ''; say "## $*"; say ''; mark; }
# catch <name>: Qupil has not shown its first frame: memory, stacks of all threads, then
# end it (a process growing without end takes the whole emulator down)
catch() {
    local pid
    pid=$(adb shell pidof $PKG | tr -d '\r')
    say "- **no first frame** – memory: $(adb shell dumpsys meminfo $PKG 2> /dev/null | grep -m1 'TOTAL' | tr -s ' ')"
    if [ -n "$pid" ]; then
        adb shell debuggerd -b "$pid" > "$OUT/hang-$1-native.txt" 2>&1
        adb shell debuggerd -j "$pid" > "$OUT/hang-$1-java.txt" 2>&1
        say "- stacks: \`hang-$1-native.txt\`, \`hang-$1-java.txt\`"
        if grep -q "qemu_pipe_write" "$OUT/hang-$1-native.txt"; then
            say "- the render thread waits for the emulator's graphics (qemu pipe): a stall of the emulator, not of Qupil"
        fi
        adb shell kill -9 "$pid"
    fi
    FAILED=1
}
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
# Right after an install Android still sends package events; one of them changed the
# resources and made Android recreate the window in the middle of the UI test (a
# configuration change Qupil cannot declare, Qupil then starts afresh). Let them pass.
sleep 40

adb logcat -c
adb logcat -v threadtime > "$LOG" 2>&1 &
LOGCAT=$!
sleep 1

# The UI test first, on a fresh emulator: after many starts the emulator's graphics
# (the GL pipe to the host) can stall, and later starts hang in shader setup
step "Qupil's own UI test (inside the app) and its restart half"
# In three starts of six cases each, then the restart half: a long run in one start
# wore out the emulator's graphics (a stall in its GL pipe)
adb shell am force-stop $PKG
uidir=/data/data/$PKG/files/ui-test
adb shell rm -rf "$uidir"
PARTS="1:pages,today,note-save,note-cancel,note-touch-select,student-search,student-new
2:event-open,metronome,appearance,language,reminders,test-mode
3:plan-save,piece-new,lesson-new,together,together-new,reminder-new,lesson-reminder,loan-return,lesson-times,more-pages,alert-volume,restart-prepare
restart:"
while IFS= read -r part <&3; do
    name=${part%%:*}
    cases=${part#*:}
    if [ "$name" = restart ]; then phase=restart; else phase="only:$cases"; fi
    mark
    adb shell "am start -W -n $ACT --es applicationArguments '--ui-test $uidir/$name --lang de --data-dir $uidir/data --ui-test-phase $phase'" > /dev/null
    if ! wait_for "ui test: [a-z]" 30; then
        catch "ui-test-$name"
    fi
    [ -n "$(adb shell pidof $PKG | tr -d '\r')" ] && wait_for "ui test: [0-9]* of [0-9]* cases passed" 600
    sleep 3
    adb shell am force-stop $PKG
    sleep 2
done 3<<< "$PARTS"   # (on fd 3: adb reads stdin and would eat the list)
adb pull "$uidir" "$OUT/ui-test" > /dev/null 2>&1
# (the data folder stays: sample data only, to start the restart half elsewhere)
for name in 1 2 3 restart; do
    if [ -f "$OUT/ui-test/$name/results.md" ]; then
        sed -e '1,/^$/d' "$OUT/ui-test/$name/results.md" >> "$SUMMARY"
        say ''
        grep -q '❌' "$OUT/ui-test/$name/results.md" && FAILED=1
    else
        say "- **part $name: no result**"
        FAILED=1
    fi
done

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

# Also stands for "Don't keep activities": Qt ends the whole process when Android closes
# its window (QtActivityBase.onDestroy), so both come back as a new start. (Android only
# reads that developer option when its system starts, which a test cannot switch.)
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
# Qt does not hand the text of the field to Android: the editor is open and Qupil says it
# opened it again with the draft (Main.qml), the screenshot shows the text
if seen '^Notiz speichern$' && since | grep -q "Draft: note editor opened again"; then
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

step "Light chosen in Qupil (not the system's), then a new start"
adb shell am force-stop $PKG
launcher
wait_for "main: first frame" 90
sleep 4
dismiss
tap_until '^Mehr$' '^Einstellungen$'
tap_until '^Einstellungen$' '^Darstellung'
tap_until '^Darstellung' '^Hell$'
tap '^Hell$' && sleep 2
shot light-chosen
adb shell am force-stop $PKG
mark
launcher
if wait_for "main: first frame" 25; then
    say "- Qupil starts with its own light mode"
else
    catch light-start
fi
sleep 4
shot light-start
notes
dismiss
tap_until '^Mehr$' '^Einstellungen$'
tap_until '^Einstellungen$' '^Darstellung'
tap_until '^Darstellung' '^System$'
tap '^System$' && sleep 2
adb shell am force-stop $PKG

step "Selecting words by touch in the note editor"
# Touches through the system (as a finger): a double tap needs two touches within Android's
# double-tap time, so it goes straight to the touch screen device (sendevent), not through
# "input tap" (which starts a program for every tap)
touchdev=$(adb shell getevent -pl 2> /dev/null | tr -d '\r' | awk '/add device/ { dev = $4 } /ABS_MT_POSITION_X/ { print dev; exit }')
maxx=$(adb shell getevent -pl "$touchdev" 2> /dev/null | tr -d '\r' | awk '/ABS_MT_POSITION_X/ { for (i = 1; i <= NF; i++) if ($i == "max") { gsub(",", "", $(i + 1)); print $(i + 1) } }' | head -1)
maxy=$(adb shell getevent -pl "$touchdev" 2> /dev/null | tr -d '\r' | awk '/ABS_MT_POSITION_Y/ { for (i = 1; i <= NF; i++) if ($i == "max") { gsub(",", "", $(i + 1)); print $(i + 1) } }' | head -1)
read -r scrw scrh < <(adb shell wm size | tr -d '\r' | sed -n 's/.*: \([0-9]*\)x\([0-9]*\).*/\1 \2/p' | tail -1)
say "- touch screen: \`${touchdev:-?}\` (${maxx:-?} x ${maxy:-?}), screen ${scrw:-?} x ${scrh:-?}"
t_tap() { adb shell input tap "$1" "$2"; }
t_double() {
    if [ -n "$touchdev" ] && [ -n "$maxx" ] && [ -n "$scrw" ]; then
        local x=$(($1 * (maxx + 1) / scrw)) y=$(($2 * (maxy + 1) / scrh)) one
        one="sendevent $touchdev 3 47 0; sendevent $touchdev 3 57 7; sendevent $touchdev 3 53 $x; sendevent $touchdev 3 54 $y; sendevent $touchdev 1 330 1; sendevent $touchdev 0 0 0; sendevent $touchdev 3 57 -1; sendevent $touchdev 1 330 0; sendevent $touchdev 0 0 0"
        adb shell "$one; sleep 0.09; $one"
    else
        adb shell "input tap $1 $2; input tap $1 $2"
    fi
}
t_long() { adb shell input swipe "$1" "$2" "$1" "$2" 1000; }
t_drag() { adb shell input swipe "$1" "$2" "$3" "$4" "$([ "${5:-}" = fast ] && echo 150 || echo 900)"; }
t_shot() { shot "$1"; }
t_find() { dump; python3 -I "$HERE/android-ui.py" "$OUT/ui/now.xml" "$1"; }
t_menu() {
    dump
    python3 -I "$HERE/android-ui.py" "$OUT/ui/now.xml" --list | sed 's/^ *[0-9]* *[0-9]*  //' \
        | grep -E -i '^(Ausschneiden|Kopieren|Einfügen|Einsetzen|Alle auswählen|Alles auswählen|Auswählen|Teilen|Cut|Copy|Paste|Select all|Share)$' | sort -u
}
adb shell am force-stop $PKG
mark
adb shell "am start -W -n $ACT --es applicationArguments '--ui-test $uidir/select --lang de --data-dir $uidir/select-data --ui-test-phase select'" > /dev/null
if wait_for "ui test: [0-9]* of [0-9]* cases passed" 240; then
    sleep 3
    SEL_LOG=$OUT/select-log.txt
    since > "$SEL_LOG"
    SEL_SCALE=dpr
    # The positions change when the keyboard comes up: read them again before each touch
    sel_refresh() { since > "$SEL_LOG"; }
    . "$HERE/select-scenarios.sh"
    sel_xy() { sel_refresh; python3 -I "$HERE/select-touch.py" "$SEL_LOG" "$SEL_SCALE" "$@"; }
    sel_now() { sel_refresh; python3 -I "$HERE/select-touch.py" "$SEL_LOG" - sel; }
    sel_markup() { sel_refresh; python3 -I "$HERE/select-touch.py" "$SEL_LOG" - markup; }
    select_scenarios
fi
notes
adb shell am force-stop $PKG

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
grep -E ' (Qupil|qml|js|default) *:|FATAL|F DEBUG|F libc|ANR in|de\.qupil' "$LOG" > "$OUT/logcat-qupil.txt"
say ''
say "Result: $([ $FAILED = 0 ] && echo 'all steps ran' || echo 'something failed, see above')"
exit $FAILED
