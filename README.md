# Qupil 2

[![Desktop](https://github.com/doitux/qupil2/actions/workflows/desktop.yml/badge.svg)](https://github.com/doitux/qupil2/actions/workflows/desktop.yml)
[![Mobile](https://github.com/doitux/qupil2/actions/workflows/mobile.yml/badge.svg)](https://github.com/doitux/qupil2/actions/workflows/mobile.yml)
[![Translations](https://github.com/doitux/qupil2/actions/workflows/translations.yml/badge.svg)](https://github.com/doitux/qupil2/actions/workflows/translations.yml)

Lesson management for music teachers: students, lessons, notes, pieces, recitals, metronome and tuning tone.

Qupil 2 is the complete rewrite of [Qupil](https://qupil.de) 1.x – from a Qt Widgets desktop program to Qt 6 / QML that runs on **Windows, macOS, Linux, iPhone, iPad and Android** from one code base.

> **Deutsch:** Qupil ist die Unterrichtsverwaltung für Instrumentalpädagoginnen und -pädagogen. Qupil 2 läuft auf Desktop, Tablet und Handy und gleicht die Geräte per WLAN oder Nextcloud ab.

**Status: early development.** The app shell with adaptive navigation, dark mode and four languages is in place; the features of 1.4.1 follow step by step. For daily work use Qupil 1.4.1 from [qupil.de](https://qupil.de).

## What Qupil 2 will do

- Everything Qupil 1.4.1 does: students and lessons, timetable, lesson notes (also for group lessons), pieces with status, activities, instruments and rentals, sheet music library, recitals with programme and timing, recital and ensemble candidates, birthdays and reminders, metronome and tuning tone, PDF documents, CSV import
- One layout for every screen: tab bar on phones, sidebar + list + detail on tablets and desktops
- Sync between your devices **over local Wi-Fi or Nextcloud/WebDAV**, end-to-end encrypted, without a Qupil account
- Backups as a single database file, restorable on any device
- English, German, French and Spanish; light and dark mode that follow the system

## Building

Requirements: **Qt 6.5 or later** (6.8 LTS recommended) with Qt Quick, Qt Quick Controls and the Qt Linguist tools, **CMake 3.21+**, a C++17 compiler.

```sh
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=/path/to/Qt/6.8.3/gcc_64
cmake --build build
ctest --test-dir build --output-on-failure
./build/qupil
```

A self-contained copy with the Qt runtime: `cmake --install build --prefix dist`.

For Android and iOS, configure with the `qt-cmake` of the mobile Qt kit and point `QT_HOST_PATH` to the desktop Qt of the same version. `.github/workflows/mobile.yml` shows the exact commands.

## Continuous integration

Every push and pull request is built on GitHub Actions:

| Workflow | What it does | Download |
|---|---|---|
| **Desktop** | Linux, Windows, macOS (universal): build, unit tests, QML smoke test (loads the UI, switches all languages and colour schemes, fails on any QML warning), package with Qt runtime | `.tar.gz` / `.zip` per platform |
| **Mobile** | Android arm64 APK signed with a throwaway test key; iOS device build without signing | test APK |
| **Translations** | TS files match the catalog, placeholders and plurals consistent, every string translated | – |

Downloads are on the run's page under *Artifacts* and are kept for 30 days. The desktop builds are not code-signed yet, so Windows SmartScreen and macOS Gatekeeper will warn.

## Project layout

```
qml/            user interface (Main.qml, Theme.qml, pages/)
src/            C++ (main.cpp, core/ with LanguageManager)
i18n/           qupil_en/de/fr/es.ts – generated, see docs/i18n.md
tools/i18n/     string catalog (single source of truth) and generators
assets/         font (Figtree) and app icons
platform/       platform files (Windows icon, iOS/Android localisation)
tests/          unit tests
docs/           developer documentation
```

## Translations

English is the source language. German keeps the wording of Qupil 1.4.1; French and Spanish are new. All texts live in `tools/i18n/catalog.py`; the TS files are generated from it. Details and how to add a language: [docs/i18n.md](docs/i18n.md).

## Licence

Copyright © 2006–2026 Felix Hammer.

Qupil is free software under the [GNU General Public License v3.0 or later](LICENSE) with an [additional permission for app stores](LICENSES/LicenseRef-Qupil-AppStore-exception.txt) (GPLv3 section 7): anyone may distribute Qupil through app stores as long as the complete source code stays freely available.

The Figtree font is © The Figtree Project Authors, [SIL Open Font License 1.1](LICENSES/OFL-1.1.txt). Qt is used under the LGPLv3.

## Contributing

Contributions are welcome – see [CONTRIBUTING.md](CONTRIBUTING.md).
