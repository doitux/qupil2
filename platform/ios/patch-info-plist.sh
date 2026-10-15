#!/bin/sh
# SPDX-FileCopyrightText: 2006-2026 Felix Hammer
# SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#
# Adds Qupil's keys to the Info.plist Qt generates (runs after the build, before signing).
# Usage: patch-info-plist.sh [path to Qupil.app/Info.plist]
# Without an argument it uses Xcode's build settings (TARGET_BUILD_DIR, INFOPLIST_PATH).
set -e
plist="${1:-$TARGET_BUILD_DIR/$INFOPLIST_PATH}"
echo "Adding Qupil keys to $plist"

# Backups and Qupil 1.x files show up in Files › On My iPad › Qupil
plutil -replace UIFileSharingEnabled -bool YES "$plist"
plutil -replace LSSupportsOpeningDocumentsInPlace -bool YES "$plist"

plutil -replace CFBundleDisplayName -string Qupil "$plist"
plutil -replace CFBundleDevelopmentRegion -string en "$plist"
plutil -replace CFBundleLocalizations -json '["en","de","fr","es"]' "$plist"
# Per-app language in iOS Settings › Qupil › Language
plutil -replace UIPrefersShowingLanguageSettings -bool YES "$plist"

# Wi-Fi sync (English fallback; translations in <lang>.lproj/InfoPlist.strings)
plutil -replace NSBonjourServices -json '["_qupil-sync._tcp"]' "$plist"
plutil -replace NSLocalNetworkUsageDescription \
    -string "Qupil uses your local network to sync with your other devices over Wi-Fi." "$plist"
