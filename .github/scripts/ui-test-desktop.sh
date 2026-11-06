#!/bin/bash
# Desktop UI test (ui-test.yml) on a real screen: Qupil's UI test in the phone and the
# wide layout, each followed by its restart half, and optionally the screenshot tour.
#   ui-test-desktop.sh <system> <output folder>
set -u
SYSTEM=$1
OUT=$2
mkdir -p "$OUT"
SUMMARY=$OUT/summary.md
FAILED=0

case "$SYSTEM" in
    macos) APP=build/Qupil.app/Contents/MacOS/Qupil ;;
    windows) APP=build/Qupil.exe ;;
    *) APP=build/qupil ;;
esac
if [ "$SYSTEM" = linux ]; then
    # An X server of its own (the runners have no screen): Qupil as on a Linux desktop
    Xvfb :99 -screen 0 1920x1080x24 > /dev/null 2>&1 &
    export DISPLAY=:99
    export QT_QPA_PLATFORM=xcb
    sleep 2
fi

echo "## $SYSTEM" > "$SUMMARY"
echo >> "$SUMMARY"

# run <name> <size>: both halves on one data folder
run() {
    local name=$1 size=$2 data
    data=$(mktemp -d)
    for phase in first restart; do
        local args=(--ui-test "$OUT/$name/$phase" --lang de --ui-test-size "$size" --data-dir "$data")
        [ "$phase" = restart ] && args+=(--ui-test-phase restart)
        "$APP" "${args[@]}" > "$OUT/$name-$phase.log" 2>&1
        local code=$?
        local result=$OUT/$name/$phase/results.md
        if [ -f "$result" ]; then
            echo "### $name ($size), $phase" >> "$SUMMARY"
            sed -e '1,/^$/d' "$result" >> "$SUMMARY"
            echo >> "$SUMMARY"
        else
            echo "### $name ($size), $phase: **no result** (exit $code), see $name-$phase.log" >> "$SUMMARY"
        fi
        if [ $code -ne 0 ]; then
            FAILED=1
            grep '❌' "$result" 2>/dev/null | sed "s/^/::error title=UI test $SYSTEM $name $phase::/"
        fi
    done
    rm -rf "$data"
}

run phone 390x844
# The runners' screens are about 1024 × 768: a wide window that still fits
run wide 1000x660

if [ "${QUPIL_TOUR:-0}" = 1 ]; then
    "$APP" --screenshots "$OUT/tour" --lang de > "$OUT/tour.log" 2>&1 \
        || { echo "- **screenshot tour failed**, see tour.log" >> "$SUMMARY"; FAILED=1; }
    echo "- screenshot tour: $(ls "$OUT/tour" 2>/dev/null | grep -c png) pictures" >> "$SUMMARY"
fi
exit $FAILED
