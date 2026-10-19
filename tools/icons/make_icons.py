# SPDX-FileCopyrightText: 2006-2026 Felix Hammer
# SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
"""Renders every app icon of every package from the SVG sources in assets/icons/.

    pip install resvg-py pillow
    python3 -I tools/icons/make_icons.py          (from the repository root)

Sources (the logo of 19.10.2026: plum tile #6E3C5A, lime and white figures):
  qupil.svg                     tile with round corners: window icon, Linux, About page,
                                Windows .ico, Android legacy icon
  qupil-square.svg              without corners: iOS (iOS rounds it itself)
  qupil-macos.svg               macOS grid (tile 824 of 1024, radius 185): .icns
  qupil-android-foreground.svg  adaptive icon foreground (108 dp, background colour
                                #6E3C5A in res/values/ic_launcher_background.xml)
  qupil-android-monochrome.svg  themed icon (Android 13+), white with the border cut out
"""

import io
import struct
import sys
from pathlib import Path

try:
    import resvg_py
    from PIL import Image
except ImportError as error:
    sys.exit(f"{error}: pip install resvg-py pillow")

ROOT = Path(__file__).resolve().parents[2]
SRC = ROOT / "assets" / "icons"


def render(svg: str, size: int) -> Image.Image:
    data = resvg_py.svg_to_bytes(svg_path=str(SRC / svg), width=size, height=size)
    return Image.open(io.BytesIO(bytes(data))).convert("RGBA")


def png_bytes(image: Image.Image) -> bytes:
    out = io.BytesIO()
    image.save(out, "PNG", optimize=True)
    return out.getvalue()


def write(path: Path, data: bytes) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(data)
    print(f"{path.relative_to(ROOT)}  ({len(data)} bytes)")


def dib(image: Image.Image) -> bytes:
    """32-bit BMP entry of an .ico: header, BGRA rows bottom-up, 1-bit AND mask."""
    w, h = image.size
    r, g, b, a = image.split()
    pixels = Image.merge("RGBA", (b, g, r, a)).transpose(Image.Transpose.FLIP_TOP_BOTTOM).tobytes()
    row = ((w + 31) // 32) * 4
    mask = bytearray()
    alpha = a.transpose(Image.Transpose.FLIP_TOP_BOTTOM).tobytes()
    for y in range(h):
        bits = bytearray(row)
        for x in range(w):
            if alpha[y * w + x] == 0:
                bits[x // 8] |= 0x80 >> (x % 8)
        mask += bits
    header = struct.pack("<IiiHHIIiiII", 40, w, h * 2, 1, 32, 0, len(pixels) + len(mask), 0, 0, 0, 0)
    return header + pixels + bytes(mask)


def ico(svg: str, sizes: list[int]) -> bytes:
    """Windows icon: BMP entries for the small sizes, PNG for 256 px (as Windows' own tools do)."""
    entries = [png_bytes(render(svg, s)) if s >= 256 else dib(render(svg, s)) for s in sizes]
    out = struct.pack("<HHH", 0, 1, len(sizes))
    offset = 6 + 16 * len(sizes)
    for size, data in zip(sizes, entries):
        dim = 0 if size >= 256 else size
        out += struct.pack("<BBBBHHII", dim, dim, 0, 0, 1, 32, len(data), offset)
        offset += len(data)
    return out + b"".join(entries)


def icns(svg: str) -> bytes:
    """macOS icon: PNG entries for 16 to 512 pt, each at 1x and 2x."""
    types = [("icp4", 16), ("ic11", 32), ("icp5", 32), ("ic12", 64), ("ic07", 128), ("ic13", 256),
             ("ic08", 256), ("ic14", 512), ("ic09", 512), ("ic10", 1024)]
    cache: dict[int, bytes] = {}
    body = b""
    for kind, size in types:
        if size not in cache:
            cache[size] = png_bytes(render(svg, size))
        body += kind.encode() + struct.pack(">I", 8 + len(cache[size])) + cache[size]
    return b"icns" + struct.pack(">I", 8 + len(body)) + body


def main() -> None:
    # Window icon, About page, Linux (hicolor)
    for size in (16, 24, 32, 48, 64, 128, 256, 512):
        write(SRC / f"qupil-{size}.png", png_bytes(render("qupil.svg", size)))

    write(ROOT / "platform" / "windows" / "qupil.ico",
          ico("qupil.svg", [16, 20, 24, 32, 40, 48, 64, 96, 128, 256]))
    write(SRC / "qupil.icns", icns("qupil-macos.svg"))

    # iOS: one 1024 px image without transparency, Xcode makes the other sizes
    ios = render("qupil-square.svg", 1024)
    flat = Image.new("RGB", ios.size, "#6E3C5A")
    flat.paste(ios, mask=ios.getchannel("A"))
    write(ROOT / "platform" / "ios" / "Assets.xcassets" / "AppIcon.appiconset" / "AppIcon-1024.png", png_bytes(flat))

    # Android: adaptive (foreground + monochrome, 108 dp) and legacy (48 dp) per density
    res = ROOT / "platform" / "android" / "res"
    for density, scale in (("mdpi", 1), ("hdpi", 1.5), ("xhdpi", 2), ("xxhdpi", 3), ("xxxhdpi", 4)):
        folder = res / f"mipmap-{density}"
        write(folder / "ic_launcher_foreground.png", png_bytes(render("qupil-android-foreground.svg", round(108 * scale))))
        write(folder / "ic_launcher_monochrome.png", png_bytes(render("qupil-android-monochrome.svg", round(108 * scale))))
        write(folder / "ic_launcher.png", png_bytes(render("qupil.svg", round(48 * scale))))


if __name__ == "__main__":
    main()
