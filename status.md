# TestFlight-Sammellauf 2026-11-24 02:57 (von Hand, now)

**Build 57 hochgeladen**

- Build 57 (Commit 2cb393a): success, Upload: success, Testtext: success
- https://github.com/doitux/qupil2/actions/runs/45308838843
- Seit Build 56 (Commit 9aaaaf9)

Commits:
- 203f85b Notes and tasks: one menu on every card, a tap only unfolds
- 4c37c1e UI test booklet-questions: scroll "Gelesen" clear of the floating buttons first
- 18be7b7 UI test note-menu: close the menu by a tap beside it after press and hold
- 3078ab2 UI test note-menu: press and hold with a finger, as on a phone
- b918398 UI test note-menu: on iOS the menu after a right click is closed by a tap
- 93b09b6 Merge feature/note-menu: one menu on every note and tasks card, a tap only unfolds
- 0552c05 Student app, iOS simulator test: the button on the page qupil.de/u hands its link to the app
- 6da79c0 Student app, iOS simulator test: tap the page's button until Safari reacts
- 387df92 Student app, iOS simulator test: Safari's alert is found in the picture
- 628f004 Student app, iOS simulator test: a crash report goes to the results, with its essentials
- 89e7b67 Student app docs: the iOS simulator test and its check of the page's button
- cbef512 Merge fix/student-page-button-test: the iOS simulator test taps the button on the page qupil.de/u in Safari
- d2c0398 Settings: "Try the new idea", a trial of tasks, lesson and history for the test team
- 8e05c2d Trial of the new idea: ticks and small buttons in the lesson's window keep the keyboard
- b2c89a8 Trial of the new idea: the note of a lesson gets a field of its own each time
- 2cb393a Merge feature/idea-demo: Settings › "Try the new idea", a trial with invented students

Für iOS relevante Dateien:
- .github/scripts/android-test.sh
- .github/scripts/ios-crash-summary.py
- .github/scripts/ios-find-alert.py
- .github/scripts/ios-find-colour.py
- .github/scripts/student-ios-test.sh
- CMakeLists.txt
- i18n/qupil_de.ts
- i18n/qupil_en.ts
- i18n/qupil_es.ts
- i18n/qupil_fr.ts
- qml/Icon.qml
- qml/Main.qml
- qml/Nav.qml
- qml/components/TaskLessonCard.qml
- qml/editors/NoteEditor.qml
- qml/editors/PlanFinish.qml
- qml/idea/IdeaEntry.qml
- qml/idea/IdeaFold.qml
- qml/idea/IdeaLessonSheet.qml
- qml/idea/IdeaLink.qml
- qml/idea/IdeaModel.qml
- qml/idea/IdeaPage.qml
- qml/idea/IdeaPoint.qml
- qml/idea/IdeaPupilSheet.qml
- qml/idea/IdeaStudentPage.qml
- qml/idea/IdeaTask.qml
- qml/idea/IdeaTaskEdit.qml
- qml/pages/MorePage.qml
- qml/pages/SettingsPage.qml
- qml/pages/StudentDetail.qml
- src/app/UiTest.cpp
- src/app/UiTest.h
- src/app/UiTestIdea.cpp
- src/app/UiTestNoteMenu.cpp
- tools/i18n/catalog.py
