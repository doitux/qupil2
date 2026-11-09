# TestFlight-Sammellauf 2026-11-09 21:43 (von Hand, now)

**Build 42 gestartet**

- Build 42 (Commit 7a9836f): in_progress, Upload: -, Testtext: -
- https://github.com/doitux/qupil2/actions/runs/42695741266
- Seit Build 41 (Commit 5c6dffa)

Commits:
- c6c761c Reminders for one lesson
- a1bcc2e Lists of choices: readable entry under the finger; UI test checks contrast
- e741dc5 UI test: open a list of choices until it is open (iOS)
- b6d38d1 UI test readability: only texts mostly on the screen, at their visible middle
- 7a9836f Add a searchable lesson picker and return loans from the student page

Für iOS relevante Dateien:
- .github/scripts/android-test.sh
- CMakeLists.txt
- i18n/qupil_de.ts
- i18n/qupil_en.ts
- i18n/qupil_es.ts
- i18n/qupil_fr.ts
- qml/Labels.qml
- qml/Main.qml
- qml/Nav.qml
- qml/components/ChoiceField.qml
- qml/components/LessonBadge.qml
- qml/components/LessonPicker.qml
- qml/editors/ReminderEditor.qml
- qml/pages/LessonDetail.qml
- qml/pages/RemindersPage.qml
- qml/pages/ScheduleOverview.qml
- qml/pages/StudentDetail.qml
- src/app/DataService.cpp
- src/app/DataService.h
- src/app/ReminderAlert.cpp
- src/app/UiTest.cpp
- src/app/UiTest.h
- src/core/Schema.cpp
- src/core/Schema.h
- tests/tst_core.cpp
- tools/i18n/catalog.py
