# -*- coding: utf-8 -*-
"""Builds the Qupil 2 translation files from catalog.py + the 1.4.1 German strings.

Usage: python3 -I build.py <raw.json from 1.4.1> <output dir>
"""
import html
import json
import os
import re
import sys
from xml.sax.saxutils import escape

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from catalog import CATALOG, DROPPED  # noqa: E402

RAW, OUT = sys.argv[1], sys.argv[2]
os.makedirs(OUT, exist_ok=True)

rows = [r for r in json.load(open(RAW, encoding="utf-8")) if r["type"] not in ("obsolete", "vanished")]
lookup = {}
for r in rows:
    lookup.setdefault(r["src"], [])
    if r["de"] not in lookup[r["src"]]:
        lookup[r["src"]].append(r["de"])


def plain(s):
    s = re.sub(r"<style.*?</style>", "", s, flags=re.S)
    s = re.sub(r"<br\s*/?>", "\n", s)
    s = re.sub(r"</p>", "\n", s)
    s = re.sub(r"<[^>]+>", "", s)
    return html.unescape(s)


# The Designer HTML strings are addressed by placeholder keys in the catalog.
HTML_KEYS = {}
for src in lookup:
    if src.startswith("<!DOCTYPE"):
        p = plain(src)
        if "Management of pupils" in p:
            HTML_KEYS["__HTML_FEATURES__"] = src
        elif "Project website" in p:
            HTML_KEYS["__HTML_PROJECT__"] = src
        elif "Please contact the Author" in p:
            HTML_KEYS["__HTML_TRANSLATION__"] = src
        elif "Format:" in p:
            HTML_KEYS["__HTML_FORMAT__"] = src
        else:
            HTML_KEYS["__HTML_URL__"] = src
DROPPED[HTML_KEYS["__HTML_URL__"]] = "URL"

# Pure spelling / orthography fixes in the 1.4.1 German (wording unchanged).
SUBSTRING_FIXES = [
    (" wir die ", " wird die "),
    ("Programm-Author", "Programm-Autor"),
    ("Komponist/Author", "Komponist/Autor"),
    ("\"%1\"nicht", "\"%1\" nicht"),
    ("Unterrrichtsnotizen", "Unterrichtsnotizen"),
    ("imporieren", "importieren"),
    ("Regelmäig", "Regelmäßig"),
    ("berücksichtig:", "berücksichtigt:"),
    ("Folgender Schüler sind", "Folgende Schüler sind"),
    ("(CVS)", "(CSV)"),
]
FULL_FIXES = {
    "Archivieren und Löschen": "Archivieren und löschen",
    "Externen Beitrag Hinzufügen ...": "Externen Beitrag hinzufügen ...",
    "Archiveintrag Löschen": "Archiveintrag löschen",
    "Veranstaltung Löschen": "Veranstaltung löschen",
    "Schüler Löschen": "Schüler löschen",
    "Schüler Archivieren": "Schüler archivieren",
    "Schüler Archivieren und Löschen": "Schüler archivieren und löschen",
    "Neue Notiz Hinzufügen": "Neue Notiz hinzufügen",
    "Notiz Löschen": "Notiz löschen",
    "Datei Exportieren": "Datei exportieren",
    "PDF Datei (*.pdf)": "PDF-Datei (*.pdf)",
    "Qupil Backup Datei (*.db)": "Qupil-Backup-Datei (*.db)",
    "Adress CSV (*.csv)": "Adress-CSV (*.csv)",
    "Dokumenten Ansicht": "Dokumentenansicht",
    "City:": "Ort:",
    "CSV-Datei konnte nicht geöffnet werden CSV-Import wird abgebrochen!":
        "CSV-Datei konnte nicht geöffnet werden.\nCSV-Import wird abgebrochen!",
}


def join_lines(s):
    lines = [l.strip() for l in s.split("\n")]
    out, prev_blank = "", False
    for l in lines:
        if not l:
            prev_blank = bool(out)
            continue
        if not out:
            out = l
        elif prev_blank:
            out += "\n\n" + l
        elif out[-1] in ".!?:":
            out += "\n" + l
        else:
            out += " " + l
        prev_blank = False
    return re.sub(r"[ \t]{2,}", " ", out)


def normalize_de(s):
    changes = []
    t = s
    t2 = re.sub(r"&(?=[^\s&])", "", t)
    if t2 != t:
        changes.append("Tastatur-Kürzel „&“ entfernt")
    t = t2
    if "<" in t:
        t2 = plain(t)
        if t2 != t:
            changes.append("HTML entfernt")
        t = t2
    t2 = re.sub(r"\s*-\s*Qupil\s*$", "", t)
    if t2 != t:
        changes.append("Titelzusatz „ - Qupil“ entfernt")
    t = t2
    t2 = join_lines(t)
    if t2 != t.strip():
        changes.append("Zeilenumbrüche für automatischen Umbruch bereinigt")
    elif t2 != t:
        changes.append("Leerzeichen am Rand entfernt")
    t = t2
    for a, b in SUBSTRING_FIXES:
        if a in t:
            t = t.replace(a, b)
            changes.append(f"Rechtschreibung: „{a.strip()}“ → „{b.strip()}“")
    if t in FULL_FIXES:
        changes.append(f"Rechtschreibung: „{t}“ → „{FULL_FIXES[t]}“")
        t = FULL_FIXES[t]
    return t, changes


def fr_typo(s):
    s = s.replace(" :", " :").replace(" ?", " ?").replace(" !", " !").replace(" ;", " ;")
    s = s.replace("« ", "« ").replace(" »", " »")
    return s


def placeholders(s):
    return sorted(set(re.findall(r"%(?:n|\d+)", s)))


errors, warnings, covered = [], [], set()
ids = set()
entries = []
for e in CATALOG:
    if e["id"] in ids:
        errors.append(f"doppelte ID {e['id']}")
    ids.add(e["id"])
    old = HTML_KEYS.get(e["old"], e["old"])
    de141, de, changes = None, None, []
    if old is not None:
        cands = lookup.get(old)
        if not cands:
            errors.append(f"{e['id']}: 1.4.1-Quelle nicht gefunden: {old!r}")
            cands = [""]
        if len(cands) > 1:
            if not e["pick"]:
                errors.append(f"{e['id']}: mehrdeutig {cands}")
            cands = [c for c in cands if c == e["pick"] or normalize_de(c)[0] == e["pick"]] or cands
        de141 = cands[0]
        covered.add((old, de141))
        for m in e["merged"]:
            if m.startswith("(code)"):
                continue
            for d in lookup.get(m, []):
                covered.add((m, d))
                if e["de"] is None and not e["num"]:
                    nd = normalize_de(d)[0]
                    if nd != normalize_de(de141)[0] and not (m == "irregular" and nd.lower() == "unregelmäßig"):
                        warnings.append(f"{e['id']}: zusammengeführt „{m}“ hat anderes Deutsch: {nd!r}")
            if m not in lookup:
                errors.append(f"{e['id']}: zusammengeführte Quelle fehlt: {m!r}")
        if e["pick"] and old in lookup:
            for d in lookup[old]:
                pass
    if e["de"] is not None:
        de = e["de"]
        if old is not None:
            changes.append(e["note"] or "angepasst")
        elif e["code"]:
            changes.append(e["note"] or "aus dem Code übernommen")
        else:
            changes.append("neu in Qupil 2")
    else:
        if de141 is None:
            errors.append(f"{e['id']}: kein Deutsch")
            de = ""
        else:
            de, changes = normalize_de(de141)
            if e["email"]:
                de = de.replace("fhammer@qupil.de", "%1")
                if not e["note"]:
                    changes.append("E-Mail-Adresse als Platzhalter %1")
            if e["note"]:
                changes.append(e["note"])
    fr = [fr_typo(x) for x in e["fr"]] if e["num"] else fr_typo(e["fr"])
    item = dict(id=e["id"], en=e["en"], de=de, fr=fr, es=e["es"], num=e["num"],
                src=e["src"] or (e["en"] if not e["num"] else None), comment=e["comment"],
                old=old if old is None or not old.startswith("<!") else "(HTML-Text)",
                de141=de141 if de141 is None or not de141.startswith("<!") else plain(de141).strip(),
                changes=changes, new=(old is None and not e["code"]), code=e["code"])
    # checks
    if e["num"]:
        if not item["src"]:
            errors.append(f"{e['id']}: Plural ohne src")
        for lang in ("en", "de", "fr", "es"):
            forms = item[lang]
            if not isinstance(forms, list) or len(forms) != 2:
                errors.append(f"{e['id']}: {lang} braucht 2 Pluralformen")
            elif "%n" not in forms[-1]:
                errors.append(f"{e['id']}: {lang} Pluralform ohne %n")
    else:
        ph = placeholders(item["en"])
        for lang in ("de", "fr", "es"):
            if placeholders(item[lang]) != ph:
                errors.append(f"{e['id']}: Platzhalter {lang} {placeholders(item[lang])} ≠ en {ph}")
        for lang in ("en", "de", "fr", "es"):
            if not item[lang]:
                errors.append(f"{e['id']}: {lang} leer")
    entries.append(item)

uncovered = [(s, d) for (s, d) in {(r["src"], r["de"]) for r in rows}
             if (s, d) not in covered and s not in DROPPED]
for s, d in sorted(uncovered):
    errors.append(f"1.4.1-String nicht übernommen: {s[:70]!r} => {d[:50]!r}")

print(f"{len(entries)} Einträge, {sum(1 for x in entries if x['new'])} neu, "
      f"{sum(1 for x in entries if x['num'])} mit Plural")
for w in warnings:
    print("WARN", w)
for er in errors:
    print("ERR ", er)

# ------------------------------------------------------------------ write TS
LANGS = {"en": "en_US", "de": "de_DE", "fr": "fr_FR", "es": "es_ES"}


def esc(s):
    return escape(s, {'"': "&quot;", "'": "&apos;"})


for lang, code in LANGS.items():
    out = ['<?xml version="1.0" encoding="utf-8"?>', "<!DOCTYPE TS>",
           f'<TS version="2.1" language="{code}" sourcelanguage="en_US">', "<context>", "    <name></name>"]
    for it in entries:
        num = ' numerus="yes"' if it["num"] else ""
        out.append(f'    <message id="{esc(it["id"])}"{num}>')
        out.append(f"        <source>{esc(it['src'])}</source>")
        if it["comment"]:
            out.append(f"        <extracomment>{esc(it['comment'])}</extracomment>")
        if it["num"]:
            out.append("        <translation>")
            for f in it[lang]:
                out.append(f"            <numerusform>{esc(f)}</numerusform>")
            out.append("        </translation>")
        else:
            out.append(f"        <translation>{esc(it[lang])}</translation>")
        out.append("    </message>")
    out += ["</context>", "</TS>", ""]
    with open(os.path.join(OUT, f"qupil_{lang}.ts"), "w", encoding="utf-8") as f:
        f.write("\n".join(out))

json.dump(entries, open(os.path.join(OUT, "entries.json"), "w", encoding="utf-8"), ensure_ascii=False, indent=1)
sys.exit(1 if errors else 0)
