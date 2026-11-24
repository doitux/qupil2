#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2006-2026 Felix Hammer
# SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
"""Looks at the top and the bottom edge of a simulator screenshot: does the app's page
reach there (its colour behind the status bar and the home indicator), or are there black
strips?

    ios-edges.py <screenshot.png> <RRGGBB>     prints one line per edge; exit 1 if one is not the page's

The rows looked at are a few pixels from the edge, where neither the clock nor the Dynamic
Island nor the home indicator's bar are."""
import sys

from PIL import Image


def main():
    shot, colour = sys.argv[1], sys.argv[2]
    target = tuple(int(colour[i:i + 2], 16) for i in (0, 2, 4))
    image = Image.open(shot).convert('RGB')
    width, height = image.size
    pixels = image.load()
    failed = 0
    for name, y in (('top', 6), ('bottom', height - 7)):
        row = [pixels[x, y] for x in range(width // 5, width * 4 // 5, 3)]
        same = sum(1 for p in row if all(abs(p[c] - target[c]) <= 8 for c in range(3)))
        middle = row[len(row) // 2]
        found = '%02X%02X%02X' % middle
        if same * 10 >= len(row) * 9:
            print(f'{name} edge: the page\'s colour ({colour})')
        else:
            print(f'{name} edge: NOT the page\'s colour, {found} instead of {colour}')
            failed = 1
    return failed


if __name__ == '__main__':
    sys.exit(main())
