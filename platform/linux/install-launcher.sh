#!/bin/sh
# SPDX-FileCopyrightText: 2006-2026 Felix Hammer
# SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#
# Puts Qupil with its icon into the application menu and a launcher on the
# desktop (freedesktop.org desktop entries: KDE, GNOME and others). Only for the
# current user, no root needed.
#
# The launcher starts the newest Qupil unpacked next to this one (the folders
# Qupil-nightly-<commit> in the same parent folder): a newer nightly unpacked
# there needs nothing else. Unpacked somewhere else: run this script there.
#
#   ./install-launcher.sh            menu entry and desktop launcher
#   ./install-launcher.sh --remove   takes both away again

set -e

dir=$(dirname "$(readlink -f "$0")")
parent=$(dirname "$dir")
data="${XDG_DATA_HOME:-$HOME/.local/share}"
apps="$data/applications"
entry="de.qupil.app.desktop"            # the name the app reports (Wayland finds its icon by it)
icon="$data/icons/hicolor/256x256/apps/de.qupil.app.png"
launcher="$HOME/.local/bin/qupil"
marker="# Qupil launcher (install-launcher.sh)"

case "${LC_ALL:-${LC_MESSAGES:-$LANG}}" in
    de*) german=1 ;;
    *) german= ;;
esac
say() { if [ -n "$german" ]; then echo "$1"; else echo "$2"; fi; }

# The desktop folder (~/Schreibtisch on a German system)
desktop=$(xdg-user-dir DESKTOP 2>/dev/null || true)
if [ -z "$desktop" ] || [ "$desktop" = "$HOME" ]; then
    conf="${XDG_CONFIG_HOME:-$HOME/.config}/user-dirs.dirs"
    desktop=
    if [ -f "$conf" ]; then
        desktop=$(sed -n 's/^XDG_DESKTOP_DIR="\(.*\)"$/\1/p' "$conf" | sed "s|^\$HOME|$HOME|")
    fi
fi
if [ -z "$desktop" ] || [ "$desktop" = "$HOME" ] || [ ! -d "$desktop" ]; then
    desktop=
    for d in "$HOME/Schreibtisch" "$HOME/Desktop"; do
        if [ -d "$d" ]; then desktop=$d; break; fi
    done
fi

refresh() {
    update-desktop-database -q "$apps" 2>/dev/null || true
    kbuildsycoca6 >/dev/null 2>&1 || kbuildsycoca5 >/dev/null 2>&1 || true
}

if [ "$1" = "--remove" ]; then
    rm -f "$apps/$entry"
    [ -z "$desktop" ] || rm -f "$desktop/Qupil.desktop" "$desktop/$entry"
    rm -f "$data"/icons/hicolor/*/apps/de.qupil.app.png "$data"/icons/hicolor/*/apps/de.qupil.app.svg
    if [ -f "$launcher" ] && grep -qF "$marker" "$launcher"; then rm -f "$launcher"; fi
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

# Icons in the user's icon theme, and one of them under a fixed path for the
# entries: Plasma only sees icon folders that existed when it started
mkdir -p "$data/icons/hicolor" "$apps" "$(dirname "$launcher")"
cp -R "$dir/share/icons/hicolor/." "$data/icons/hicolor/"

# Launcher: the newest Qupil in the parent folder (by the build time of the program)
quote() { printf "'%s'" "$(printf '%s' "$1" | sed "s/'/'\\\\''/g")"; }
missing=$(say "Qupil ist nicht mehr in $parent. Bitte install-launcher.sh im entpackten Qupil-Ordner ausführen." \
              "Qupil is no longer in $parent. Please run install-launcher.sh in the unpacked Qupil folder.")
cat > "$launcher.new" <<EOF
#!/bin/sh
$marker: starts the newest Qupil unpacked in this folder
folder=$(quote "$parent")
fallback=$(quote "$dir")
newest=
for d in "\$folder"/Qupil*/; do
    [ -x "\${d}qupil.sh" ] && [ -x "\${d}bin/qupil" ] || continue
    # shellcheck disable=SC3013
    if [ -z "\$newest" ] || [ "\${d}bin/qupil" -nt "\${newest}bin/qupil" ]; then newest=\$d; fi
done
[ -n "\$newest" ] || newest="\$fallback/"
if [ ! -x "\${newest}qupil.sh" ]; then
    message=$(quote "$missing")
    kdialog --error "\$message" 2>/dev/null || notify-send Qupil "\$message" 2>/dev/null || echo "\$message" >&2
    exit 1
fi
exec "\${newest}qupil.sh" "\$@"
EOF
chmod 755 "$launcher.new"
mv -f "$launcher.new" "$launcher"

# The package's desktop entry with that launcher and icon (quoted as the desktop
# entry spec wants it: \ " ` $ escaped, % doubled)
spec() { printf '%s' "$1" | sed -e 's/\\/\\\\\\\\/g' -e 's/["`$]/\\\\&/g' -e 's/%/%%/g'; }
tmp="$apps/.$entry.new"
while IFS= read -r line || [ -n "$line" ]; do
    case "$line" in
        Exec=*) printf 'Exec="%s"\n' "$(spec "$launcher")" ;;
        Icon=*) printf 'Icon=%s\n' "$icon" ;;
        TryExec=*) ;;
        *) printf '%s\n' "$line" ;;
    esac
done < "$dir/share/applications/$entry" > "$tmp"
chmod 755 "$tmp"
mv -f "$tmp" "$apps/$entry"

# The same as a launcher on the desktop; executable = trusted (KDE), GNOME asks for "trusted"
if [ -n "$desktop" ]; then
    rm -f "$desktop/$entry" "$desktop/Qupil.desktop"   # also the one of an earlier version
    cp "$apps/$entry" "$desktop/Qupil.desktop"
    chmod 755 "$desktop/Qupil.desktop"
    gio set "$desktop/Qupil.desktop" metadata::trusted true 2>/dev/null || true
fi
refresh

say "Qupil ist jetzt im Startmenü${desktop:+ und auf dem Schreibtisch ($desktop)}." \
    "Qupil is now in the application menu${desktop:+ and on the desktop ($desktop)}."
say "Neue Versionen einfach ebenfalls nach $parent entpacken: Der Starter nimmt immer die neueste." \
    "Unpack newer versions into $parent as well: the launcher always starts the newest."
