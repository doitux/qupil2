# SPDX-FileCopyrightText: 2006-2026 Felix Hammer
# SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
"""Icons of the student app: Qupil's logo with the colours turned round
(lime tile #C9F36B, front figure plum #6E3C5A, back figure light plum #A35C88),
so teacher and student app are told apart at a glance on one phone.

    pip install resvg-py pillow
    python3 -I student/tools/make_icons.py        (from the repository root)

Writes the SVG sources (student/assets/icons) and every raster size for iOS and Android.
"""

import io
import sys
from pathlib import Path

try:
    import resvg_py
    from PIL import Image
except ImportError as error:
    sys.exit(f"{error}: pip install resvg-py pillow")

STUDENT = Path(__file__).resolve().parents[1]
ICONS = STUDENT / "assets" / "icons"
TILE, BACK, FRONT = "#C9F36B", "#A35C88", "#6E3C5A"

FIGURES = (
    f'<circle cx="44" cy="18" r="8" fill="{BACK}"/>'
    f'<path d="M34 43.5c0-7 4.5-12 10-12s10 5 10 12" fill="none" stroke="{BACK}" stroke-width="4" stroke-linecap="round"/>'
    f'<path d="M11 53.75c0-8 5.8-14 13-14s13 6 13 14" fill="none" stroke="{TILE}" stroke-width="10" stroke-linecap="round"/>'
    f'<circle cx="24" cy="25" r="11.6" fill="{TILE}"/>'
    f'<circle cx="24" cy="25" r="9" fill="{FRONT}"/>'
    f'<path d="M11 53.75c0-8 5.8-14 13-14s13 6 13 14" fill="none" stroke="{FRONT}" stroke-width="4.5" stroke-linecap="round"/>'
)
ADAPTIVE = 'transform="translate(54 54) scale(1.05) translate(-32.4 -33)"'
SOURCES = {
    "student.svg": f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="1024" height="1024">'
                   f'<rect width="64" height="64" rx="16" fill="{TILE}"/>{FIGURES}</svg>',
    "student-square.svg": f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64" width="1024" height="1024">'
                          f'<rect width="64" height="64" fill="{TILE}"/>{FIGURES}</svg>',
    # The border of the front figure is the tile colour: on the adaptive icon's lime
    # background it disappears the same way
    "student-android-foreground.svg": f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 108 108" width="432" height="432">'
                                      f'<g {ADAPTIVE}>{FIGURES}</g></svg>',
    "student-android-monochrome.svg": (
        '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 108 108" width="432" height="432"><defs>'
        '<mask id="m" maskUnits="userSpaceOnUse" x="0" y="0" width="108" height="108"><rect width="108" height="108" fill="#fff"/>'
        f'<g {ADAPTIVE}><path d="M11 53.75c0-8 5.8-14 13-14s13 6 13 14" fill="none" stroke="#000" stroke-width="10" stroke-linecap="round"/>'
        '<circle cx="24" cy="25" r="11.6" fill="#000"/></g></mask></defs>'
        f'<g mask="url(#m)"><g {ADAPTIVE}><circle cx="44" cy="18" r="8" fill="#FFFFFF"/>'
        '<path d="M34 43.5c0-7 4.5-12 10-12s10 5 10 12" fill="none" stroke="#FFFFFF" stroke-width="4" stroke-linecap="round"/></g></g>'
        f'<g {ADAPTIVE}><circle cx="24" cy="25" r="9" fill="#FFFFFF"/>'
        '<path d="M11 53.75c0-8 5.8-14 13-14s13 6 13 14" fill="none" stroke="#FFFFFF" stroke-width="4.5" stroke-linecap="round"/></g></svg>'),
}


def render(name: str, size: int) -> Image.Image:
    data = resvg_py.svg_to_bytes(svg_path=str(ICONS / name), width=size, height=size)
    return Image.open(io.BytesIO(bytes(data))).convert("RGBA")


def save(image: Image.Image, path: Path) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    image.save(path, optimize=True)
    print(path.relative_to(STUDENT))


def main() -> None:
    ICONS.mkdir(parents=True, exist_ok=True)
    for name, svg in SOURCES.items():
        (ICONS / name).write_text(svg + "\n", encoding="utf-8")

    # iOS: one 1024 px image without transparency
    ios = render("student-square.svg", 1024)
    flat = Image.new("RGB", ios.size, TILE)
    flat.paste(ios, mask=ios.split()[3])
    save(flat, STUDENT / "platform/ios/Assets.xcassets/AppIcon.appiconset/AppIcon-1024.png")

    # Android: adaptive (8+), themed (13+), legacy
    res = STUDENT / "platform/android/res"
    for density, scale in (("mdpi", 1), ("hdpi", 1.5), ("xhdpi", 2), ("xxhdpi", 3), ("xxxhdpi", 4)):
        save(render("student-android-foreground.svg", int(108 * scale)), res / f"mipmap-{density}/ic_launcher_foreground.png")
        save(render("student-android-monochrome.svg", int(108 * scale)), res / f"mipmap-{density}/ic_launcher_monochrome.png")
        save(render("student.svg", int(48 * scale)), res / f"mipmap-{density}/ic_launcher.png")

    # Window icon on the desktop (trying it out)
    save(render("student.svg", 256), STUDENT / "assets/icons/student-256.png")


if __name__ == "__main__":
    main()
