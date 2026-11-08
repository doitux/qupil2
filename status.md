# TestFlight-Sammellauf 2026-11-07 22:05

**Build 41 gestartet**

- Build 41 (Commit 5c6dffa): in_progress, Upload: -, Testtext: -
- https://github.com/doitux/qupil2/actions/runs/42429624644
- Seit Build 40 (Commit 6b0a159)

Commits:
- 827d074 TestFlight collect: no midday build Monday to Thursday
- d7cc7a0 Start the nightly platform tests from the TestFlight collect run, only on changes
- 08413b7 Keep a student's individual lessons on several days together
- 6d89e15 UI test together-new: tap the student again when the first tap only closed the keyboard
- 5822cb9 Delete fix/ and feature/ branches once they are in main
- 5c6dffa TestFlight text: new since build 39

Für iOS relevante Dateien:
- .github/scripts/android-test.sh
- .github/workflows/android-test.yml
- .github/workflows/cleanup-branches.yml
- .github/workflows/ios-test.yml
- .github/workflows/ui-test.yml
- i18n/qupil_de.ts
- i18n/qupil_en.ts
- i18n/qupil_es.ts
- i18n/qupil_fr.ts
- qml/Icon.qml
- qml/components/AppMenuItem.qml
- qml/editors/LessonEditor.qml
- qml/pages/StudentDetail.qml
- src/app/DataService.cpp
- src/app/DataService.h
- src/app/DocumentService.cpp
- src/app/UiTest.cpp
- src/app/UiTest.h
- src/core/Database.cpp
- src/core/Schema.cpp
- src/core/Schema.h
- src/core/Store.cpp
- src/core/Store.h
- tests/tst_core.cpp
- tools/i18n/catalog.py

Nächtliche Tests:
- Android: gestartet (seit 81de258, letzter Lauf: failure)
- Desktop: gestartet (seit a1af107, letzter Lauf: success)
- iOS: gestartet (seit 81de258, letzter Lauf: failure)
