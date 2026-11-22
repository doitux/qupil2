#!/bin/sh
# SPDX-FileCopyrightText: 2006-2026 Felix Hammer
# SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#
# Adds the student app's keys to the Info.plist Qt generates (after the build, before signing).
# Usage: patch-info-plist.sh [path to Info.plist]; without one, Xcode's build settings.
set -e
plist="${1:-$TARGET_BUILD_DIR/$INFOPLIST_PATH}"
echo "Adding Qupil student keys to $plist"

# Name under the icon (translations in <lang>.lproj/InfoPlist.strings)
plutil -replace CFBundleDisplayName -string "Qupil Practice" "$plist"
plutil -replace CFBundleDevelopmentRegion -string en "$plist"
plutil -replace CFBundleLocalizations -json '["en","de","fr","es"]' "$plist"
plutil -replace UIPrefersShowingLanguageSettings -bool YES "$plist"

# qupil-student:… opens the app (fallback until universal links for qupil.de/u are set up)
plutil -replace CFBundleURLTypes -json '[{"CFBundleURLName":"de.qupil.student","CFBundleURLSchemes":["qupil-student"]}]' "$plist"

# The QR code scanner (translations in <lang>.lproj/InfoPlist.strings)
plutil -replace NSCameraUsageDescription -string "To scan the QR code your teacher shows you." "$plist"

# Portrait on iPhone; iPad all orientations (required for iPad multitasking)
plutil -replace UISupportedInterfaceOrientations -json '["UIInterfaceOrientationPortrait"]' "$plist"
plutil -replace UISupportedInterfaceOrientations~ipad -json '["UIInterfaceOrientationPortrait","UIInterfaceOrientationPortraitUpsideDown","UIInterfaceOrientationLandscapeLeft","UIInterfaceOrientationLandscapeRight"]' "$plist"

# Only standard encryption (Monocypher: XChaCha20-Poly1305) besides HTTPS, as in Qupil
plutil -replace ITSAppUsesNonExemptEncryption -bool NO "$plist"
