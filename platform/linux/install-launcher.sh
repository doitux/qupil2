#!/bin/sh
# SPDX-FileCopyrightText: 2006-2026 Felix Hammer
# SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#
# Adds Qupil from this folder to the application menu and puts a launcher with
# the Qupil icon on the desktop (KDE, GNOME and others: freedesktop.org desktop
# entries). Only for the current user, no root needed. Run it again after moving
# the folder; a newer nightly unpacked over this folder needs nothing.
#
#   ./install-launcher.sh            menu entry and desktop launcher
#   ./install-launcher.sh --remove   takes both away again

set -e

dir=$(dirname "$(readlink -f "$0")")
data="${XDG_DATA_HOME:-$HOME/.local/share}"
apps="$data/applications"
entry="de.qupil.app.desktop"
desktop=$(xdg-user-dir DESKTOP 2>/dev/null || true)
[ -n "$desktop" ] && [ "$desktop" != "$HOME" ] || desktop="$HOME/Desktop"

case "${LC_ALL:-${LC_MESSAGES:-$LANG}}" in
    de*) german=1 ;;
    *) german= ;;
esac
say() { if [ -n "$german" ]; then echo "$1"; else echo "$2"; fi; }

refresh() {
    update-desktop-database -q "$apps" 2>/dev/null || true
    kbuildsycoca6 >/dev/null 2>&1 || kbuildsycoca5 >/dev/null 2>&1 || true
}

if [ "$1" = "--remove" ]; then
    rm -f "$apps/$entry" "$desktop/$entry"
    rm -f "$data"/icons/hicolor/*/apps/de.qupil.app.png "$data"/icons/hicolor/*/apps/de.qupil.app.svg
    refresh
    say "Qupil ist nicht mehr im Startmenü und auf dem Schreibtisch." \
        "Qupil was removed from the application menu and the desktop."
    exit 0
fi

if [ ! -x "$dir/bin/qupil" ] || [ ! -f "$dir/share/applications/$entry" ]; then
    say "Bitte im entpackten Qupil-Ordner ausführen (dort, wo qupil.sh liegt)." \
        "Please run this in the unpacked Qupil folder (next to qupil.sh)."
    exit 1
fi
chmod +x "$dir/qupil.sh" "$dir/bin/qupil" 2>/dev/null || true

# Icons into the user's icon theme: menus, the launcher and the window (Wayland) find "de.qupil.app"
mkdir -p "$data/icons/hicolor" "$apps"
cp -R "$dir/share/icons/hicolor/." "$data/icons/hicolor/"

# The desktop entry of the package, started through qupil.sh in this folder
# (quoted as the desktop entry spec wants it: \ " ` $ escaped, % doubled)
exec_path=$(printf '%s' "$dir/qupil.sh" | sed -e 's/\\/\\\\\\\\/g' -e 's/["`$]/\\\\&/g' -e 's/%/%%/g')
tmp="$apps/.$entry.new"
while IFS= read -r line || [ -n "$line" ]; do
    case "$line" in
        Exec=*) printf 'Exec="%s"\n' "$exec_path" ;;
        TryExec=*) ;;
        *) printf '%s\n' "$line" ;;
    esac
done < "$dir/share/applications/$entry" > "$tmp"
chmod 755 "$tmp"
mv -f "$tmp" "$apps/$entry"

# The same as a launcher on the desktop; executable = trusted (KDE), GNOME asks for "trusted"
placed=
if [ -d "$desktop" ]; then
    placed="$desktop"
    cp -f "$apps/$entry" "$desktop/$entry"
    chmod 755 "$desktop/$entry"
    gio set "$desktop/$entry" metadata::trusted true 2>/dev/null || true
fi
refresh

say "Qupil ist jetzt im Startmenü${placed:+ und auf dem Schreibtisch ($placed)}." \
    "Qupil is now in the application menu${placed:+ and on the desktop ($placed)}."
