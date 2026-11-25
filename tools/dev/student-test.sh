#!/bin/bash
# The student app (student/) locally in one go: builds it with the system's Qt (6.4 is enough, the
# student app needs no adapting) and runs its three tests without a screen. Prints only what
# failed. Takes about two minutes.
#
#   tools/dev/student-test.sh [--no-build] [test function …]
#
#   tools/dev/student-test.sh                                  # build, all three tests
#   tools/dev/student-test.sh uiKeepsClearOfTheSystemBars      # build, one case of tst_studentapp
#
# Build tree: $QUPIL_WORK/student-build, logs beside it. The exit code is the tests'; a failed
# build exits with 1 before any test runs – an old binary passing the tests must never look like
# a check of the new code. What only a device shows (keyboard, links, scanner, the screen's
# edges) runs on GitHub: the emulator and simulator jobs of the workflow "Student app" on every
# push; see tools/dev/ci-status.sh and docs/student-app.md.
set -u
ROOT="$(git -C "$(dirname "$0")" rev-parse --show-toplevel)" || exit 1
S=${QUPIL_WORK:-/home/claude/work}
B=$S/student-build
build=1
[ "${1:-}" = "--no-build" ] && { build=0; shift; }
mkdir -p "$S"

if [ $build = 1 ]; then
    if ! { cmake -S "$ROOT/student" -B "$B" -G Ninja && cmake --build "$B"; } > "$S/student-build.log" 2>&1; then
        grep -E "error|Error|FAILED" "$S/student-build.log" | head -20
        echo "BUILD FAILED – no test run ($S/student-build.log)"
        exit 1
    fi
    echo "build exit 0"
fi

failed=0
run() {     # run <test> [functions …]
    local t=$1 rc; shift
    [ -x "$B/$t" ] || { echo "$t: not built"; failed=1; return; }
    for try in 1 2 3; do
        ( cd "$B" && QT_QPA_PLATFORM=offscreen timeout "${QUPIL_TEST_TIMEOUT:-500}" "./$t" "$@" ) > "$S/$t.log" 2>&1
        rc=$?
        [ $rc -ne 139 ] && break        # 139: Qt 6.4 crashes at random without a screen – run again
    done
    echo "$t: rc=$rc, $(grep -E '^Totals' "$S/$t.log" || echo 'no totals') ($S/$t.log)"
    grep -E -A6 '^(FAIL!|XPASS)' "$S/$t.log" | cut -c1-300 | head -40
    [ $rc -eq 0 ] || failed=1
}

if [ $# -gt 0 ]; then
    run tst_studentapp "$@"             # single cases are those of the app's own test
else
    run tst_booklet
    run tst_teacherside
    run tst_studentapp
fi
exit $failed
