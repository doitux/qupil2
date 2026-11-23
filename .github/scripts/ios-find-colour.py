#!/usr/bin/env python3
"""Finds a wide bar of one colour in a simulator screenshot: a web page's button that idb's
accessibility view does not show.

    ios-find-colour.py <screenshot.png> <describe.json> <RRGGBB>     prints "x y" in points

The scale (pixels per point) comes from the widest frame idb reports for the screen."""
import json
import sys

from PIL import Image


def main():
    shot, describe, colour = sys.argv[1], sys.argv[2], sys.argv[3]
    target = tuple(int(colour[i:i + 2], 16) for i in (0, 2, 4))
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
    rows = []
    for y in range(0, height, 2):
        xs = [x for x in range(0, width, 2)
              if all(abs(pixels[x, y][c] - target[c]) <= 10 for c in range(3))]
        if len(xs) * 2 > width * 0.3:
            rows.append((y, sum(xs) / len(xs)))
    if len(rows) < 8:
        return 1
    y, x = rows[len(rows) // 2]
    print(int(x / scale), int(y / scale))
    return 0


if __name__ == '__main__':
    sys.exit(main())
