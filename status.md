# TestFlight-Sammellauf 2026-11-22 22:05

**Build 56 gestartet**

- Build 56 (Commit 9aaaaf9): in_progress, Upload: -, Testtext: -
- https://github.com/doitux/qupil2/actions/runs/45095137738
- Seit Build 55 (Commit 81d205f)

Commits:
- 33852f4 Student app: QR code scanner in the app, links while the app runs, texts, logo, German name under the icon
- 6acc1ed Student app, Android: links come through the app's own activity
- 3dda8b3 Practice book: the student's questions on the lesson's card with date and "Gelesen"; link copied is said
- d240339 Docs: the student app's scanner, incoming links, keyboard, questions
- 364c1a9 Student app, iOS: the link the app is started with; simulator job
- fa8c7ff Student app, simulator test: wait for the start, answer "Open in …?" with idb
- d85c99a Student app: a teacher not loaded yet has a name in the switcher
- b50c161 Student app, simulator test: the system's question comes only when the link starts the app
- 95fd650 Merge feature/practice-scan-questions: QR code scanner and links in the student app, the student's questions on the lesson's card
- c01fb9d Student app CI: the upload's summary also in the job's log
- 9d22feb Nightly: moving the tag is tried again, and a failure shows its reason
- d22ffea Student app, iOS simulator test: patient with a slow start by a link
- 9aaaaf9 Merge fix/student-ios-test-wait: the iOS simulator test waits for a slow start by a link

Für iOS relevante Dateien:
- .github/scripts/publish-nightly.sh
- .github/scripts/student-android-test.sh
- .github/scripts/student-ios-test.sh
- .github/workflows/student.yml
- i18n/qupil_de.ts
- i18n/qupil_en.ts
- i18n/qupil_es.ts
- i18n/qupil_fr.ts
- qml/Main.qml
- qml/components/TaskLessonCard.qml
- qml/editors/BookletSheet.qml
- qml/pages/StudentDetail.qml
- src/app/DataService.h
- src/app/DataServiceTasks.cpp
- src/app/UiTest.cpp
- student/CMakeLists.txt
- student/i18n/strings.json
- student/i18n/strings.py
- student/platform/android/AndroidManifest.xml
- student/platform/android/build.gradle
- student/platform/android/src/de/qupil/student/StudentActivity.java
- student/platform/ios/de.lproj/InfoPlist.strings
- student/platform/ios/en.lproj/InfoPlist.strings
- student/platform/ios/es.lproj/InfoPlist.strings
- student/platform/ios/fr.lproj/InfoPlist.strings
- student/platform/ios/patch-info-plist.sh
- student/qml/HomePage.qml
- student/qml/Icon.qml
- student/qml/SettingsPage.qml
- student/qml/WelcomePage.qml
- student/qml/components/AddTeacher.qml
- student/qml/components/Logo.qml
- student/qml/components/PageFlickable.qml
- student/src/IncomingLinks.h
- student/src/IncomingLinks_ios.mm
- student/src/IncomingLinks_qt.cpp
- student/src/QrScanner.cpp
- student/src/QrScanner.h
- student/src/QrScanner_ios.mm
- student/src/QrScanner_none.cpp
- student/src/StudentApp.cpp
- student/src/StudentApp.h
- student/src/main.cpp
- student/tests/tst_studentapp.cpp
- tests/tst_tasks.cpp
- tools/i18n/catalog.py

Nächtliche Tests:
- Android: gestartet (seit 81d205f, letzter Lauf: success)
- Desktop: gestartet (seit 81d205f, letzter Lauf: success)
- iOS: gestartet (seit 81d205f, letzter Lauf: success)
