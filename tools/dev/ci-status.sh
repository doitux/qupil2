#!/bin/bash
# One look at a branch: its runs on GitHub and the results of the device tests. Meant to be called
# once at the time the runs should be done, not in a loop.
#
#   tools/dev/ci-status.sh <branch>
#
# A macOS or iOS job that was cancelled after 15–30 minutes without a runner is not a failure:
# `gh run rerun <id> --failed`. The device tests write their results to the branches ios-test and
# android-test (GitHub's logs are not readable from everywhere); check that the commit in their
# last message is the one of the branch.
b=${1:?usage: ci-status.sh <branch>}
cd "$(git -C "$(dirname "$0")" rev-parse --show-toplevel)" || exit 1
gh() { ( unset GH_TOKEN; command gh "$@" ); }     # a token set in the environment may be stale

git fetch -q origin main ios-test android-test "$b" || exit 1
echo "branch $(git rev-parse --short "origin/$b"), main $(git rev-parse --short origin/main)," \
     "commits of main missing in the branch: $(git rev-list --count "origin/$b..origin/main")"
gh api "repos/doitux/qupil2/actions/runs?branch=$b&per_page=12" \
    --jq '.workflow_runs[] | "\(.id) \(.name) \(.status) \(.conclusion) \(.created_at) \(.head_sha[0:7])"'
echo "--- iOS test: $(git log -1 --format='%h %ci %s' origin/ios-test)"
for d in iphone ipad; do
    git show "origin/ios-test:$d/ui-test/first/results.md" 2>/dev/null \
        | awk -v d="$d" '/✅/{ok++} /❌/{bad++; print d": "$0} END{print d" ok="ok+0" failed="bad+0}'
done
echo "--- Android test: $(git log -1 --format='%h %ci %s' origin/android-test)"
echo "android failed lines: $(git show origin/android-test:summary.md 2>/dev/null | grep -c '❌')"
