#!/bin/bash
# Starts the UI test runs for a branch on GitHub: desktop (Linux, Windows, macOS), the Android
# emulator and the iOS simulator. One call instead of three.
#
#   tools/dev/ci-start.sh <branch>
#
# The iOS test is NOT started while another one is running or waiting: its workflow cancels a
# running iOS test as soon as a new one starts (about 70 minutes lost). Collect the branches into
# one and run it once instead. Exit code 2 says the iOS test was left out.
b=${1:?usage: ci-start.sh <branch>}
R=repos/doitux/qupil2
gh() { ( unset GH_TOKEN; command gh "$@" ); }     # a token set in the environment may be stale

busy=$(gh api "$R/actions/workflows/ios-test.yml/runs?per_page=10" \
    --jq '[.workflow_runs[] | select(.status != "completed")] | length')
for w in ui-test.yml android-test.yml; do
    gh api -X POST "$R/actions/workflows/$w/dispatches" -f ref="$b" && echo "started $w on $b"
done
if [ "$busy" != "0" ]; then
    echo "iOS test NOT started: $busy run(s) still going"
    exit 2
fi
gh api -X POST "$R/actions/workflows/ios-test.yml/dispatches" -f ref="$b" && echo "started ios-test.yml on $b"
