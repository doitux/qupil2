#!/usr/bin/env bash
# Runs a command and, if it fails, repeats the last lines of its output as an
# error annotation, so the reason shows on the run's summary page and in pull
# requests without opening the full log.
#
# Usage: bash .github/scripts/run-logged.sh <command> [args...]

log="$(mktemp)"
"$@" 2>&1 | tee "$log"
status=${PIPESTATUS[0]}

if [ "$status" -ne 0 ]; then
    # Annotations are short: lines that look like errors first, then the end of the output
    {
        grep -iE "error|fail|cannot|could not|unable|not found|undefined" "$log" | grep -v "^-- Installing" | tail -n 25
        echo "----- last lines -----"
        tail -n 12 "$log"
    } > "$log.summary"
    message=$(tail -c 3500 "$log.summary" | sed -e 's/%/%25/g' -e 's/\r//g' | awk 'BEGIN { ORS = "%0A" } { print }')
    echo "::error title=$(basename "$1") $2 failed (exit $status)::${message}"
fi

rm -f "$log" "$log.summary"
exit "$status"
