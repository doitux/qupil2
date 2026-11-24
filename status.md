# TestFlight-Sammellauf 2026-11-24 13:04 (von Hand, now)

**Build 59 gestartet**

- Build 59 (Commit 00c7c0c): queued, Upload: -, Testtext: -
- https://github.com/doitux/qupil2/actions/runs/45390001853
- Seit Build 58 (Commit 2f3a0d4)

Commits:
- b8db77f Coupling page qupil.de/u: "No app yet?" first, with TestFlight for the test phase
- 28491d5 Coupling page qupil.de/u: on Android a button to the test version
- 41a111a Practice book: the teacher sees how many devices have the booklet
- dd1fb56 Student app, iOS simulator test: Safari's hint is closed before the page's button is looked for
- cd0a729 Merge fix/student-web-testflight: the coupling page starts with how to get the app (TestFlight, Android test version)
- c71091d Merge feature/practice-devices: the teacher sees how many devices have the booklet; the app says when it takes and when it removes one
- 5222e11 Student app: "this device has the booklet" is not sent twice while the first one is on its way
- 4d67061 Student app, iOS simulator test: Safari's hint is closed as often as it takes
- 490f929 TestFlight text: new since build 53 (the build the external testers have)
- 00c7c0c Merge fix/student-hello-once: "this device has the booklet" once; the simulator test closes Safari's hint

Für iOS relevante Dateien:
- .github/scripts/publish-nightly.sh
- .github/scripts/student-ios-test.sh
- .github/workflows/student.yml
- i18n/qupil_de.ts
- i18n/qupil_en.ts
- i18n/qupil_es.ts
- i18n/qupil_fr.ts
- qml/editors/BookletSheet.qml
- src/app/BookletService.cpp
- src/app/BookletService.h
- src/app/DataService.h
- src/app/DataServiceTasks.cpp
- src/app/UiTest.cpp
- src/shared/BookletInbox.cpp
- src/shared/BookletInbox.h
- student/i18n/strings.json
- student/i18n/strings.py
- student/src/StudentApp.cpp
- student/src/StudentApp.h
- student/tests/FakeNextcloud.cpp
- student/tests/FakeNextcloud.h
- student/tests/tst_studentapp.cpp
- student/tests/tst_teacherside.cpp
- student/web/u/index.html
- tests/tst_tasks.cpp
- tools/i18n/catalog.py
