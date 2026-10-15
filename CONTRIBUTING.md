# Contributing to Qupil

Thank you for helping! A few rules keep the project healthy and its licence clean.

## Licence of contributions (inbound = outbound)

Qupil is licensed under `GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception`. By contributing you agree that your contribution is licensed under exactly these terms, including the additional permission for app stores.

Sign off every commit to confirm this ([Developer Certificate of Origin](https://developercertificate.org/)):

```sh
git commit -s -m "Add lesson duration picker"
```

New source files start with:

```cpp
// SPDX-FileCopyrightText: 2026 Your Name
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
```

## Code

- C++17, Qt 6.5+ API. QML for all user interface, C++ for data, sync and platform code.
- Colours only from `Theme` tokens, never hex values in components – this keeps dark mode working.
- Every visible text through `qsTrId("area.thing")` / `qtTrId(...)`, never a literal string. Add new IDs to `tools/i18n/catalog.py` in all four languages and regenerate (see [docs/i18n.md](docs/i18n.md)).
- Touch targets at least 44 px; every control reachable by keyboard and labelled for screen readers.
- Before a pull request: `cmake --build build && ctest --test-dir build --output-on-failure`. The CI runs the same on Linux, Windows and macOS and builds Android and iOS.

## Translations

Corrections and new languages are very welcome. Please change `tools/i18n/catalog.py`, not the generated `i18n/*.ts` files. If you are not a programmer, open an issue with the string ID and your suggestion.
