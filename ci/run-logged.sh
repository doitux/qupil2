#!/usr/bin/env bash
# Runs a step script, prints its output, and on failure repeats the last
# lines as an error annotation (readable via the GitHub API without logs).
# Usage: run-logged.sh <title> <script> [args...]
title=$1
shift
log=$(mktemp)
bash -eo pipefail "$@" > "$log" 2>&1
rc=$?
cat "$log"
if [ $rc -ne 0 ]; then
  msg=$(tail -n 60 "$log" | sed -e 's/%/%25/g' -e 's/\r/%0D/g' | awk '{printf "%s%%0A", $0}')
  echo "::error title=$title::exit $rc%0A$msg"
fi
exit $rc
