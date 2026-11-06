#!/bin/bash
# Compares the screenshots of a test run with the run before (same path, same system):
#   visual-diff.sh <previous folder> <new folder>
# Writes <new>/visual-diff.md and, for each screen that changed noticeably, a picture
# "before | after | difference" into <new>/visual-diff/. Tiny changes (the build number
# in the corner, anti-aliasing) stay below the threshold.
set -u
PREV=$1
NEW=$2
THRESHOLD=0.003   # share of pixels that differ
LIMIT=40          # pictures written at most
OUT=$NEW/visual-diff
REPORT=$NEW/visual-diff.md

{
    echo "## Screens changed since the run before"
    echo
} > "$REPORT"
if [ ! -d "$PREV/desktop" ]; then
    echo "No run before to compare with." >> "$REPORT"
    exit 0
fi
mkdir -p "$OUT"
changed=0 same=0 added=0 written=0
list=""
while IFS= read -r -d '' file; do
    rel=${file#"$NEW"/}
    case "$rel" in
        visual-diff/*) continue ;;
        */fail-*) continue ;;
    esac
    old=$PREV/$rel
    if [ ! -f "$old" ]; then
        added=$((added + 1))
        continue
    fi
    size_new=$(identify -format '%wx%h' "$file" 2> /dev/null)
    size_old=$(identify -format '%wx%h' "$old" 2> /dev/null)
    if [ "$size_new" != "$size_old" ]; then
        share=1
    else
        diff=$(compare -metric AE -fuzz 3% "$old" "$file" null: 2>&1 | awk '{print int($1)}')
        total=$(identify -format '%[fx:w*h]' "$file")
        share=$(awk -v d="${diff:-0}" -v t="$total" 'BEGIN { printf "%.4f", (t > 0 ? d / t : 0) }')
    fi
    if awk -v s="$share" -v t=$THRESHOLD 'BEGIN { exit !(s > t) }'; then
        changed=$((changed + 1))
        percent=$(awk -v s="$share" 'BEGIN { printf "%.1f", s * 100 }')
        name=$(echo "$rel" | tr '/' '_')
        if [ $written -lt $LIMIT ]; then
            if [ "$size_new" = "$size_old" ]; then
                compare -fuzz 3% "$old" "$file" "$OUT/.d.png" 2> /dev/null
                montage "$old" "$file" "$OUT/.d.png" -tile 3x1 -geometry 360x+4+4 "$OUT/$name"
            else
                montage "$old" "$file" -tile 2x1 -geometry 360x+4+4 "$OUT/$name"
            fi
            written=$((written + 1))
            list="$list- \`$rel\` – $percent % (\`visual-diff/$name\`)\n"
        else
            list="$list- \`$rel\` – $percent %\n"
        fi
    else
        same=$((same + 1))
    fi
done < <(find "$NEW" -name '*.png' -print0 | sort -z)
rm -f "$OUT/.d.png"
{
    echo "- unchanged: $same, changed: $changed, new: $added"
    echo
    printf "%b" "$list"
} >> "$REPORT"
