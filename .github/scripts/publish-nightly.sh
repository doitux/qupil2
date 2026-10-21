#!/usr/bin/env bash
# Puts test builds on the "nightly" pre-release, so they can be downloaded
# directly (also on an iPad or phone) without logging in to GitHub.
# The tag "nightly" is moved to the commit that was built.
#
# Usage: bash .github/scripts/publish-nightly.sh 'path/glob*.ipa=Asset-Name.ipa' ...
#   An asset name with * keeps the file's own name (e.g. with the build number)
#   and replaces the older assets matching it.
# Needs GH_TOKEN with contents: write, GITHUB_REPOSITORY and GITHUB_SHA.
set -euo pipefail

repo="$GITHUB_REPOSITORY"
tag="nightly"

if ! gh release view "$tag" --repo "$repo" > /dev/null 2>&1; then
    # Desktop and Mobile may both try this on the very first run
    gh release create "$tag" --repo "$repo" --prerelease --target "$GITHUB_SHA" \
        --title "Nightly test builds" --notes "Test builds of the latest commit on main." || true
fi
log="$(dirname "$0")/run-logged.sh"
bash "$log" gh api -X PATCH "repos/$repo/git/refs/tags/$tag" -f sha="$GITHUB_SHA" -F force=true > /dev/null

staging="$(mktemp -d)"
for pair in "$@"; do
    pattern="${pair%%=*}"
    name="${pair##*=}"
    file="$(compgen -G "$pattern" | head -n 1 || true)"
    if [ -z "$file" ]; then
        echo "::error::nothing matches $pattern"
        exit 1
    fi
    replaces=""
    if [[ "$name" == *"*"* ]]; then
        replaces="$name"
        name="$(basename "$file")"
        if [[ "$name" != $replaces ]]; then
            echo "::error::$name does not match $replaces"
            exit 1
        fi
    fi
    cp "$file" "$staging/$name"
    # Uploads fail now and then (GitHub 5xx): try again before giving up
    for attempt in 1 2 3; do
        if bash "$log" gh release upload "$tag" "$staging/$name" --repo "$repo" --clobber; then
            break
        fi
        if [ "$attempt" -eq 3 ]; then exit 1; fi
        sleep 10
    done
    echo "Published $name"
    # Older builds of the same file (another build number) go away
    if [ -n "$replaces" ]; then
        gh release view "$tag" --repo "$repo" --json assets -q '.assets[].name' | while read -r asset; do
            if [[ "$asset" == $replaces && "$asset" != "$name" ]]; then
                gh release delete-asset "$tag" "$asset" --repo "$repo" -y && echo "Removed $asset"
            fi
        done
    fi
done

gh release edit "$tag" --repo "$repo" --prerelease --title "Nightly test builds" --notes "$(cat <<EOF
Automatic test builds of the newest commit on \`main\` (last update: ${GITHUB_SHA::7}, $(date -u '+%Y-%m-%d %H:%M UTC')).
They are **not signed** and only meant for testing.

| File | How to install |
|---|---|
| \`Qupil-iOS-unsigned.ipa\` | iPad / iPhone: sideload with AltStore, SideStore or Sideloadly, which sign it with your Apple ID. Needs iOS 16 or later. |
| \`Qupil-Android-arm64-test-<build>-<commit>.apk\` | Android: open the file and allow installing from this source. Play Protect warns about an unknown developer: tap *More details* › *Install anyway* (the big *OK* cancels the installation). Signed with a public test key, so a newer nightly installs as an update and keeps the data (builds before Oct 15, 2026 used a different key: uninstall those once). The commit in the file name is shown on the first screen and under More › About; Android's app info shows the build number. |
| \`Qupil-macOS-universal.zip\` | macOS 12+: unzip, then right-click Qupil → Open the first time. |
| \`Qupil-Windows-x64.zip\` | Windows 10/11: unzip and start \`bin\\Qupil.exe\`; SmartScreen → More info → Run anyway. |
| \`Qupil-Linux-x86_64-nightly-<commit>.tar.gz\` | Linux: unpacks into a folder \`Qupil\`; start \`Qupil/qupil.sh\` (works through a link, too). Run \`Qupil/install-launcher.sh\` once to get Qupil with its icon in the application menu and on the desktop (\`--remove\` takes both away); a newer nightly unpacked over the same folder keeps them working. Needs \`libxcb-cursor0\` (\`sudo apt install libxcb-cursor0\`). |
| \`Qupil-Screenshots.zip\` | Every screen of this build with sample data (German and English, light and dark, desktop and phone size). |

Built by the [Desktop](${GITHUB_SERVER_URL}/${repo}/actions/workflows/desktop.yml) and [Mobile](${GITHUB_SERVER_URL}/${repo}/actions/workflows/mobile.yml) workflows.
EOF
)"
