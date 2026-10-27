#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2006-2026 Felix Hammer
# SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#
# Checks an Xcode archive of Qupil for what App Store Connect requires before
# it is signed and uploaded, and writes a summary for the run page.
#
# Usage: ios-check-archive.sh <Qupil.xcarchive> <expected build number>
set -u
archive=$1
build=$2
app="$archive/Products/Applications/Qupil.app"
plist="$app/Info.plist"
errors=0

fail() { echo "::error::$1"; errors=$((errors + 1)); }
get() { /usr/libexec/PlistBuddy -c "Print :$1" "$plist" 2>/dev/null; }

if [ ! -d "$app" ]; then
    echo "::error::No Qupil.app in the archive (would be a 'Generic Xcode Archive')"
    find "$archive" -maxdepth 4 | head -n 40
    exit 1
fi

[ "$(get CFBundleIdentifier)" = "de.qupil.app" ] || fail "CFBundleIdentifier is '$(get CFBundleIdentifier)', expected de.qupil.app"
[ "$(get CFBundleVersion)" = "$build" ] || fail "CFBundleVersion is '$(get CFBundleVersion)', expected $build"
[[ "$(get CFBundleShortVersionString)" =~ ^[0-9]+(\.[0-9]+){0,2}$ ]] || fail "CFBundleShortVersionString '$(get CFBundleShortVersionString)' is not x.y.z"

# Apple requires the iOS 26 SDK (Xcode 26) for uploads since 28 April 2026
sdk=$(get DTPlatformVersion)
[ "${sdk%%.*}" -ge 26 ] 2>/dev/null || fail "Built with iOS SDK '$sdk', App Store Connect needs iOS 26 or later"

# Launch screen (otherwise iOS shows the app letterboxed and App Store Connect rejects it)
storyboard=$(get UILaunchStoryboardName)
[ -n "$storyboard" ] && [ -d "$app/$storyboard.storyboardc" ] || fail "Launch screen '$storyboard' missing"

# Home screen icon compiled from the asset catalog
get CFBundleIcons:CFBundlePrimaryIcon:CFBundleIconName > /dev/null || fail "CFBundleIcons missing (asset catalog not compiled into Info.plist)"
[ -f "$app/Assets.car" ] || fail "Assets.car missing"
ls "$app"/AppIcon*.png > /dev/null 2>&1 || echo "::warning::No AppIcon*.png in the bundle (only Assets.car)"

# iPad multitasking needs all four orientations
for o in Portrait PortraitUpsideDown LandscapeLeft LandscapeRight; do
    /usr/libexec/PlistBuddy -c "Print :UISupportedInterfaceOrientations" "$plist" | grep -q "UIInterfaceOrientation$o\$" \
        || fail "Orientation $o missing"
done

# Qupil's own keys (patch-info-plist.sh) and the privacy manifest
[ "$(get UIFileSharingEnabled)" = "true" ] || fail "UIFileSharingEnabled missing"
[ -n "$(get NSLocalNetworkUsageDescription)" ] || fail "NSLocalNetworkUsageDescription missing"
# Required by App Store Connect because Qt Multimedia refers to these APIs (ITMS-90683)
[ -n "$(get NSCameraUsageDescription)" ] || fail "NSCameraUsageDescription missing"
[ -n "$(get NSMicrophoneUsageDescription)" ] || fail "NSMicrophoneUsageDescription missing"
plutil -lint "$app/PrivacyInfo.xcprivacy" || fail "PrivacyInfo.xcprivacy missing or invalid"
for l in de en es fr; do
    [ -f "$app/$l.lproj/InfoPlist.strings" ] \
        || fail "$l.lproj/InfoPlist.strings missing; found: $(cd "$app" && find . -name '*.strings' -o -name '*.lproj' | tr '\n' ' ')"
done

# Only the app may be in the archive
others=$(find "$archive/Products" -mindepth 1 -maxdepth 2 ! -path "*/Applications" ! -path "*/Applications/Qupil.app" | head -n 5)
[ -z "$others" ] || fail "Archive contains more than the app: $others"

{
    echo "### iOS archive"
    echo
    echo "| | |"
    echo "|---|---|"
    echo "| Bundle ID | \`$(get CFBundleIdentifier)\` |"
    echo "| Version | $(get CFBundleShortVersionString) (build $(get CFBundleVersion)) |"
    echo "| SDK / Xcode | iOS $(get DTPlatformVersion), Xcode $(get DTXcode) ($(get DTXcodeBuild)) |"
    echo "| Minimum iOS | $(get MinimumOSVersion) |"
    echo "| App size | $(du -sh "$app" | cut -f1) |"
    echo "| Checks | $([ $errors -eq 0 ] && echo passed || echo "$errors failed") |"
} >> "${GITHUB_STEP_SUMMARY:-/dev/stdout}"

echo "Info.plist:"
plutil -p "$plist"
exit $((errors > 0))
