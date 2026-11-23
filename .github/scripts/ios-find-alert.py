#!/usr/bin/env python3
"""Finds Safari's own alert in a simulator screenshot by the blue of its buttons; idb's
accessibility view does not show it.

    ios-find-alert.py <screenshot.png> <describe.json>

The simulator must be in the light appearance. Prints "question x y" for two buttons (Cancel, Open: x y is the right one, in points) or
"message x y" for one button (OK: "the address is invalid"); exit code 1 without an alert."""
import json
import sys

from PIL import Image


def main():
    shot, describe = sys.argv[1], sys.argv[2]
    image = Image.open(shot).convert('RGB')
    width, height = image.size
    points = 0
    try:
        with open(describe, encoding='utf-8') as f:
            data = json.load(f)
        for e in data if isinstance(data, list) else [data]:
            points = max(points, (e.get('frame') or {}).get('width', 0))
    except (OSError, ValueError):
        pass
    scale = width / points if points else 3.0
    pixels = image.load()
    # Safari dims the page behind its alert (light appearance: the page's margin is nearly
    # white otherwise); its hints at the first start have blue in them too, but do not dim
    if min(pixels[int(width * 0.03), int(height * 0.3)]) > 225:
        return 1
    blue = []
    # Not the status bar, not Safari's bar at the bottom
    for y in range(int(height * 0.12), int(height * 0.85), 2):
        for x in range(0, width, 2):
            r, g, b = pixels[x, y]
            if r < 70 and 90 < g < 175 and b > 200:
                blue.append((x, y))
    if len(blue) < 40:
        return 1
    columns = sorted({x for x, _ in blue})
    words = [[columns[0], columns[0]]]
    for x in columns[1:]:
        if x - words[-1][1] > 12 * scale:
            words.append([x, x])
        else:
            words[-1][1] = x
    left, right = words[-1]
    ys = sorted(y for x, y in blue if left <= x <= right)
    x, y = (left + right) / 2, ys[len(ys) // 2]
    print('question' if len(words) > 1 else 'message', int(x / scale), int(y / scale))
    return 0


if __name__ == '__main__':
    sys.exit(main())
