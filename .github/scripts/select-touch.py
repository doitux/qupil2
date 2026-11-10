#!/usr/bin/env python3
"""Reads what Qupil's "select" phase logs (UiTest::traceSelection) for the touch tests.

    select-touch.py <log> <scale> word <Word> start|end|middle|below-end
    select-touch.py <log> <scale> button <Name>           centre of a format button
    select-touch.py <log> <scale> field-end               end of the last line (inside the field)
    select-touch.py <log> -       sel                     what is selected now ("" = nothing)
    select-touch.py <log> -       markup                  the note as Qupil stores it

<scale> turns Qupil's logical pixels into the unit of the tool that taps: 1 for idb (points),
"dpr" for adb (the screen's pixels, the factor Qupil logged). Coordinates are printed as
"x y" (integers). The last logged value counts. Exit code 1 if nothing was logged."""
import re
import sys

LINE = re.compile(r'select: (\w+) ?(.*?)\s*$')


def latest(path):
    values = {}
    with open(path, encoding='utf-8', errors='replace') as f:
        for line in f:
            m = LINE.search(line)
            if not m:
                continue
            kind, rest = m.group(1), m.group(2)
            if kind in ('word', 'button'):
                name, _, coords = rest.partition(' ')
                values[(kind, name)] = coords
            else:
                values[(kind, '')] = rest
    return values


def main():
    path, scale, what = sys.argv[1], sys.argv[2], sys.argv[3]
    values = latest(path)
    if scale == 'dpr':
        factor = float(values.get(('dpr', ''), '1') or 1)
    elif scale == '-':
        factor = 1.0
    else:
        factor = float(scale)

    def out(x, y):
        print(int(round(x * factor)), int(round(y * factor)))
        return 0

    if what == 'sel':
        rest = values.get(('sel', ''))
        if rest is None:
            return 1
        parts = rest.split(' ', 2)
        print(parts[2] if len(parts) > 2 and parts[0] != parts[1] else '')
        return 0
    if what == 'markup':
        rest = values.get(('markup', ''))
        if rest is None:
            return 1
        print(rest)
        return 0
    if what == 'word':
        coords = values.get(('word', sys.argv[4]))
        if not coords:
            return 1
        x1, y1, x2, y2 = map(float, coords.split())
        where = sys.argv[5]
        if where == 'start':
            return out(x1 + 2, (y1 + y2) / 2)
        if where == 'end':
            return out(x2 - 2, (y1 + y2) / 2)
        if where == 'below-end':     # where the end handle of a selection hangs (Android, iOS)
            return out(x2, y2 + 12)
        return out((x1 + x2) / 2, (y1 + y2) / 2)
    if what == 'button':
        coords = values.get(('button', sys.argv[4]))
        if not coords:
            return 1
        x, y = map(float, coords.split())
        return out(x, y)
    if what == 'field-end':
        coords = values.get(('word', 'Mittelteil'))
        if not coords:
            return 1
        x1, y1, x2, y2 = map(float, coords.split())
        return out(x2 + 4, (y1 + y2) / 2)
    return 1


if __name__ == '__main__':
    sys.exit(main())
