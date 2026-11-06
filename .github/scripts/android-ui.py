#!/usr/bin/env python3
"""Finds an element in an Android UI dump (uiautomator dump) of the test run.

    android-ui.py <dump.xml> <pattern>      prints "x y" (centre) of the first match
    android-ui.py <dump.xml> --list         prints every text / description with its centre

The pattern is a regular expression matched against the text and the content
description (Qt Quick hands the accessible names of its items to Android)."""
import re
import sys
import xml.etree.ElementTree as ET


def nodes(path):
    for node in ET.parse(path).iter('node'):
        label = node.get('content-desc') or node.get('text') or ''
        match = re.match(r'\[(\d+),(\d+)\]\[(\d+),(\d+)\]', node.get('bounds', ''))
        if not label or not match:
            continue
        x1, y1, x2, y2 = map(int, match.groups())
        if x2 <= x1 or y2 <= y1:
            continue
        yield label, (x1 + x2) // 2, (y1 + y2) // 2


def main():
    path, pattern = sys.argv[1], sys.argv[2]
    if pattern == '--list':
        for label, x, y in nodes(path):
            print(f'{x:5d} {y:5d}  {label}')
        return 0
    regex = re.compile(pattern)
    for label, x, y in nodes(path):
        if regex.search(label):
            print(x, y)
            return 0
    return 1


if __name__ == '__main__':
    sys.exit(main())
