# TestFlight-Sammellauf 2026-11-06 23:48 (von Hand, dry-run)

**Probelauf: würde bauen**

6 Dateien für iOS geändert; im Probelauf wird nichts gestartet.

- Seit Build 39 (Commit af8bae2)

Commits:
- 500fc50 Android test: end a start that hangs before it takes the emulator down
- f408d90 Merge branch 'main' into fix/android-ui-restart
- 65f56ca Android test: UI test first; emulator graphics stalls named as such
- 9487581 Android emulator test build: draw in software
- 6d5f731 UI test: type only into a focused field; Android test closes all start dialogs
- 69919fc UI test: typing waits for the on-screen keyboard and checks the field
- 260fae0 Android test: let the package events after the install pass first
- 8f20841 Android test: back to GL drawing, SwiftShader instead of ANGLE
- 859d6c6 UI test: run a chosen set of cases; Android emulator runs it in four starts
- 8ee1185 Android test: time limit for the Qt downloads
- 589d911 Students list: rows without Layouts; Android test on an ATD image
- f820a32 Android emulator: no shader binary cache (the GL pipe stalls); UI test parts fixed
- c974e63 UI test: cases independent of each other and of the start reminders
- 7f136e2 Android test: the light-mode step taps until each page is there
- 6b0a159 TestFlight collect: a late evening run may build until 06:30, midday only on time

Für iOS relevante Dateien:
- .github/scripts/android-test.sh
- .github/workflows/android-test.yml
- qml/pages/StudentsPage.qml
- src/app/UiTest.cpp
- src/app/UiTest.h
- src/main.cpp
