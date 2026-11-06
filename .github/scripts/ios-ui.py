#!/usr/bin/env python3
"""Finds an element in what idb sees on the simulator's screen (idb ui describe-all).

    ios-ui.py <describe.json> <pattern>     prints "x y" (centre, in points) of the first match
    ios-ui.py <describe.json> --list        prints every label with its centre

The pattern is a regular expression matched against the accessibility label (and the
value). The last match is taken: editors and dialogs come after the page."""
import json
import re
import sys


def elements(path):
    with open(path, encoding='utf-8') as f:
        text = f.read().strip()
    try:
        data = json.loads(text)
    except json.JSONDecodeError:
        data = [json.loads(line) for line in text.splitlines() if line.strip().startswith('{')]
    if isinstance(data, dict):
        data = [data]
    for e in data:
        label = (e.get('AXLabel') or e.get('AXValue') or '').strip()
        frame = e.get('frame') or {}
        if not label or not frame:
            continue
        x, y = frame.get('x', 0), frame.get('y', 0)
        w, h = frame.get('width', 0), frame.get('height', 0)
        if w <= 0 or h <= 0:
            continue
        yield label, e.get('type', ''), int(x + w / 2), int(y + h / 2)


def main():
    path, pattern = sys.argv[1], sys.argv[2]
    found = list(elements(path))
    if pattern == '--list':
        for label, kind, x, y in found:
            print(f'{x:5d} {y:5d}  {kind:12s} {label}')
        return 0
    regex = re.compile(pattern)
    for label, _, x, y in reversed(found):
        if regex.search(label):
            print(x, y)
            return 0
    return 1


if __name__ == '__main__':
    sys.exit(main())
