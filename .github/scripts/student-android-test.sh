#!/bin/bash
# The student app on a running Android emulator (student.yml):
#   student-android-test.sh <apk> <output folder>
# - a coupling link opens the app and adds the teacher: at the app's start, and while the
#   app is running (that one opened the app and did nothing), https and qupil-student:
# - "Open camera" opens the QR code scanner, "back" closes it
# - the on-screen keyboard: the app measures it (the window keeps its size) and the field
#   with the cursor is above it
# Invented links only (a server that does not exist).
set -u
APK=$1
OUT=$2
PKG=de.qupil.student
ACT=$PKG/.StudentActivity
mkdir -p "$OUT/shots" "$OUT/ui"
SUMMARY=$OUT/summary.md
LOG=$OUT/logcat-full.txt
HERE=$(cd "$(dirname "$0")" && pwd)
FAILED=0
N=0

say() { echo "$*" | tee -a "$SUMMARY"; }
mark() { MARK=$(wc -l < "$LOG"); }
since() { tail -n +$((MARK + 1)) "$LOG"; }
# wait_for <text> <seconds>: until the app logged the text after the last mark
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
    adb exec-out screencap -p > "$OUT/shots/$name.png"
    say "- screenshot \`shots/$name.png\`"
}
# tap <pattern>: taps the first element whose accessible name matches
tap() {
    local xy
    adb shell uiautomator dump /sdcard/ui.xml > /dev/null 2>&1 && adb pull /sdcard/ui.xml "$OUT/ui/now.xml" > /dev/null 2>&1
    if xy=$(python3 -I "$HERE/android-ui.py" "$OUT/ui/now.xml" "$1"); then
        adb shell input tap $xy
        say "- tapped \`$1\` at $xy"
        return 0
    fi
    say "- **not on the screen:** \`$1\`"
    python3 -I "$HERE/android-ui.py" "$OUT/ui/now.xml" --list >> "$SUMMARY" 2>&1
    FAILED=1
    return 1
}
# link <token>: the part after "#" of a coupling link to a server that does not exist
link() {
    python3 -I -c "
import base64, json, sys
k = base64.urlsafe_b64encode(bytes(range(32))).decode().rstrip('=')
j = json.dumps({'v': 1, 's': 'https://cloud.example.org', 't': sys.argv[1], 'p': 'Test-Passwort-1', 'k': k}, separators=(',', ':'))
print(base64.urlsafe_b64encode(j.encode()).decode().rstrip('='))" "$1"
}
# open_link <url>: as the camera or the browser hands it to the app
open_link() {
    adb shell "am start -W -a android.intent.action.VIEW -c android.intent.category.BROWSABLE -d '$1' -n $ACT" > /dev/null
}
top_activity() {
    adb shell dumpsys activity activities | grep -E 'topResumedActivity|mResumedActivity' | head -n 1 | tr -d '\r'
}

say "# Student app on the emulator"
say "- device: $(adb shell getprop ro.product.model | tr -d '\r'), Android $(adb shell getprop ro.build.version.release | tr -d '\r')"
adb install -r -g "$APK" > /dev/null || { say "- **install failed**"; exit 1; }
sleep 20   # Android's package events after an install
adb logcat -c
adb logcat -v threadtime > "$LOG" 2>&1 &
LOGCAT=$!
sleep 1

say ""
say "## A link starts the app"
adb shell am force-stop $PKG
mark
open_link "https://qupil.de/u#$(link TestTokenAAAA1111)"
wait_for "kept until the app is ready" 30
wait_for "link received, teacher added" 60
sleep 3
shot link-at-start

say ""
say "## A link while the app is running"
mark
open_link "https://qupil.de/u#$(link TestTokenBBBB2222)"
wait_for "link received, teacher added" 30
mark
open_link "qupil-student:$(link TestTokenCCCC3333)"
wait_for "link received, teacher added" 30
mark
adb shell input keyevent KEYCODE_HOME
sleep 2
open_link "https://qupil.de/u#$(link TestTokenDDDD4444)"
wait_for "link received, teacher added" 30
sleep 2
shot link-while-running

say ""
say "## The QR code scanner"
adb shell am force-stop $PKG
adb shell pm clear $PKG > /dev/null
adb shell pm grant $PKG android.permission.CAMERA
mark
adb shell "am start -W -n $ACT" > /dev/null
sleep 8
shot welcome
if tap '^(Open camera|Kamera öffnen)$'; then
    wait_for "scanner opened" 20
    sleep 4
    top=$(top_activity)
    say "- on top: \`$top\`"
    case $top in
        *CaptureActivity*) ;;
        *) say "- **the scanner is not on top**"; FAILED=1 ;;
    esac
    shot scanner
    adb shell input keyevent KEYCODE_BACK
    wait_for "scanner closed, no code" 20
    sleep 2
    shot after-scanner
fi

say ""
say "## The on-screen keyboard"
# The window keeps its size (adjustNothing): StudentActivity measures the keyboard and the
# page makes room. The emulator shows the keyboard only when told to.
adb shell settings put secure show_ime_with_hard_keyboard 1 > /dev/null
LINK_FIELD='qupil\.de/u#'
mark
if tap "$LINK_FIELD"; then
    if wait_for "keyboard: [1-9][0-9]* px" 40; then
        sleep 3
        shot keyboard
        px=$(since | grep -o 'keyboard: [0-9]* px' | tail -n 1 | grep -o '[0-9]*')
        height=$(adb shell wm size | tr -d '\r' | tail -n 1 | sed 's/.*x//')
        adb shell uiautomator dump /sdcard/ui.xml > /dev/null 2>&1 && adb pull /sdcard/ui.xml "$OUT/ui/keyboard.xml" > /dev/null 2>&1
        if xy=$(python3 -I "$HERE/android-ui.py" "$OUT/ui/keyboard.xml" "$LINK_FIELD"); then
            y=${xy#* }
            if [ "$y" -lt $((height - px)) ]; then
                say "- the field (y $y) is above the keyboard ($px px of $height)"
            else
                say "- **the field (y $y) is under the keyboard ($px px of $height)**"
                FAILED=1
            fi
        else
            say "- **the field is not on the screen with the keyboard open**"
            FAILED=1
        fi
        mark
        adb shell input keyevent KEYCODE_BACK
        wait_for "keyboard: 0 px" 20
        sleep 2
        shot keyboard-gone
    else
        # Did the emulator show a keyboard at all?
        shot no-keyboard
        say "- input method: \`$(adb shell dumpsys input_method | grep -E 'mInputShown|mShowRequested|mCurMethodId|mImeWindowVis' | tr -d '\r' | tr -s ' ' | tr '\n' ';' | cut -c1-300)\`"
        say "- focus: \`$(adb shell dumpsys window | grep -E 'mCurrentFocus|mInputMethodTarget|imeLayeringTarget' | tr -d '\r' | tr -s ' ' | tr '\n' ';' | cut -c1-300)\`"
    fi
fi

kill $LOGCAT 2> /dev/null
# What the app said (the artifacts are not always at hand)
echo "---- the app's lines in the device log"
grep -E 'Qupil student|QupilStudent|Incoming links|QR scanner' "$LOG" | cut -c1-240
echo "----"
grep -E 'Qupil|QupilStudent|FATAL|F DEBUG|F libc|ANR in|de\.qupil' "$LOG" > "$OUT/logcat-qupil.txt"
if grep -qE 'FATAL EXCEPTION|F DEBUG|ANR in de\.qupil' "$LOG"; then
    say "- **a crash or ANR in the log**"
    FAILED=1
fi
say ""
if [ "$FAILED" = 0 ]; then say "**All fine.**"; else say "**Something failed.**"; fi
[ -n "${GITHUB_STEP_SUMMARY:-}" ] && cat "$SUMMARY" >> "$GITHUB_STEP_SUMMARY"
exit $FAILED
