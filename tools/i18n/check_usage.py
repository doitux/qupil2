# -*- coding: utf-8 -*-
"""Checks that every qsTrId()/qtTrId() in qml/ and src/ has an entry in catalog.py.

A missing ID would show up as the raw ID in the UI. Usage: python3 -I tools/i18n/check_usage.py
"""
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
sys.path.insert(0, os.path.join(ROOT, "tools", "i18n"))
from catalog import CATALOG  # noqa: E402

known = {e["id"]: e for e in CATALOG}
pattern = re.compile(r'\bq[st]TrId\(\s*"([^"]+)"\s*(,)?')
missing, plural_mismatch, used = [], [], set()
for folder in ("qml", "src"):
    for dirpath, _, files in os.walk(os.path.join(ROOT, folder)):
        for name in files:
            if not name.endswith((".qml", ".cpp", ".h", ".js")):
                continue
            path = os.path.join(dirpath, name)
            with open(path, encoding="utf-8") as f:
                for lineno, line in enumerate(f, 1):
                    if line.lstrip().startswith(("//", "*", "#")):
                        continue   # comments and examples
                    for m in pattern.finditer(line):
                        rel = os.path.relpath(path, ROOT)
                        used.add(m.group(1))
                        e = known.get(m.group(1))
                        if e is None:
                            missing.append(f"{rel}:{lineno}: unknown ID {m.group(1)!r}")
                        elif e["num"] and not m.group(2):
                            plural_mismatch.append(f"{rel}:{lineno}: {m.group(1)!r} is a plural, pass the count")

for problem in missing + plural_mismatch:
    print(f"::error::{problem}" if os.environ.get("GITHUB_ACTIONS") else problem)
print(f"{len(used)} IDs used in code, {len(known)} in the catalog")
sys.exit(1 if missing or plural_mismatch else 0)
