#!/bin/bash
# Local UI test in one go: builds with Qt 6.4 (and stops if the build fails), runs the given
# UI test cases without a screen and prints only what matters.
#
#   tools/dev/local-test.sh [--no-build] <case,case,…> [WxH] [name]
#
# Results: $QUPIL_WORK/tr/<name>/s (results.md, fail-<case>.png/.txt), log in …/<name>/log.
# The exit code is the test's; a failed build exits with 1 before any test runs – an old binary
# passing the tests must never look like a check of the new code.
set -u
HERE="$(cd "$(dirname "$0")" && pwd)"
S=${QUPIL_WORK:-/home/claude/work}
build=1
[ "${1:-}" = "--no-build" ] && { build=0; shift; }
cases=${1:?usage: local-test.sh [--no-build] <cases> [WxH] [name]}
size=${2:-390x844}
name=${3:-run}

if [ $build = 1 ]; then
    "$HERE/qt64build.sh" || { echo "BUILD FAILED – no test run"; exit 1; }
fi

out=$S/tr/$name
for try in 1 2 3 4 5 6; do
    rm -rf "$out"; mkdir -p "$out/d" "$out/s"
    QT_QPA_PLATFORM=offscreen QT_MESSAGE_PATTERN='%{message}' timeout "${QUPIL_TEST_TIMEOUT:-520}" \
        "$S/qt64test/build/qupil" --ui-test "$out/s" --lang de --ui-test-size "$size" \
        --data-dir "$out/d" --ui-test-phase "only:$cases" > "$out/log" 2>&1
    rc=$?
    [ $rc -ne 139 ] && break        # 139: Qt 6.4 crashes at random without a screen – run again
done
echo "rc=$rc ($out)"
grep -E "qml|QML|Error|FAILED|cases passed|unreadable|Binding" "$out/log" \
    | grep -v 'stale\|StandardPaths\|Cyclic' | head -"${QUPIL_TEST_LINES:-10}"
grep -B2 -A8 "❌" "$out/s/results.md" 2>/dev/null | cut -c1-400 | head -24
exit $rc
