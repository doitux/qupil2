# TestFlight-Sammellauf 2026-11-24 18:26 (von Hand, now)

**Build 60 hochgeladen**

- Build 60 (Commit 8e04cd1): success, Upload: success, Testtext: success
- https://github.com/doitux/qupil2/actions/runs/45448672175
- Seit Build 58 (Commit 2f3a0d4)

Commits:
- b8db77f Coupling page qupil.de/u: "No app yet?" first, with TestFlight for the test phase
- 28491d5 Coupling page qupil.de/u: on Android a button to the test version
- 41a111a Practice book: the teacher sees how many devices have the booklet
- dd1fb56 Student app, iOS simulator test: Safari's hint is closed before the page's button is looked for
- cd0a729 Merge fix/student-web-testflight: the coupling page starts with how to get the app (TestFlight, Android test version)
- c71091d Merge feature/practice-devices: the teacher sees how many devices have the booklet; the app says when it takes and when it removes one
- 63643ff One-line fields: a finger wipes a long text sideways; without the focus it shows its start
- 1b3b89c Test text within 4000 characters; the two wipe tests also run on Android
- 5222e11 Student app: "this device has the booklet" is not sent twice while the first one is on its way
- 4d67061 Student app, iOS simulator test: Safari's hint is closed as often as it takes
- 490f929 TestFlight text: new since build 53 (the build the external testers have)
- 00c7c0c Merge fix/student-hello-once: "this device has the booklet" once; the simulator test closes Safari's hint
- cd3df17 Trial of the new idea: a student without tasks is prepared with a free note, the lesson starts from it
- a8a1119 Merge feature/idea-notes-prepare and main into fix/field-wipe: both changes tested together
- 9147859 Student app, Android: the app makes room for the keyboard itself
- 3a69f22 Student app, emulator test: say what the input method does when no keyboard is measured
- 878c3c4 Merge fix/field-wipe: long one-line texts are wiped sideways and show their start; trial: preparing without tasks is a free note
- 6ea630c Student app, emulator test: an image with an on-screen keyboard
- 8e04cd1 Merge fix/student-android-keyboard: the student app makes room for the Android keyboard itself

Für iOS relevante Dateien:
- .github/scripts/android-test.sh
- .github/scripts/publish-nightly.sh
- .github/scripts/student-android-test.sh
- .github/scripts/student-ios-test.sh
- .github/workflows/student.yml
- CMakeLists.txt
- i18n/qupil_de.ts
- i18n/qupil_en.ts
- i18n/qupil_es.ts
- i18n/qupil_fr.ts
- qml/components/LineScroll.qml
- qml/components/TextTouch.qml
- qml/editors/BookletSheet.qml
- qml/editors/TaskEditor.qml
- qml/idea/IdeaEntry.qml
- qml/idea/IdeaLessonSheet.qml
- qml/idea/IdeaModel.qml
- qml/idea/IdeaStudentPage.qml
- src/app/BookletService.cpp
- src/app/BookletService.h
- src/app/DataService.h
- src/app/DataServiceTasks.cpp
- src/app/UiTest.cpp
- src/app/UiTestIdea.cpp
- src/shared/BookletInbox.cpp
- src/shared/BookletInbox.h
- student/CMakeLists.txt
- student/i18n/strings.json
- student/i18n/strings.py
- student/platform/android/AndroidManifest.xml
- student/platform/android/src/de/qupil/student/StudentActivity.java
- student/qml/Main.qml
- student/qml/components/PageFlickable.qml
- student/src/KeyboardHeight.h
- student/src/KeyboardHeight_none.cpp
- student/src/StudentApp.cpp
- student/src/StudentApp.h
- student/src/main.cpp
- student/tests/FakeNextcloud.cpp
- student/tests/FakeNextcloud.h
- student/tests/tst_studentapp.cpp
- student/tests/tst_teacherside.cpp
- student/web/u/index.html
- tests/tst_tasks.cpp
- tools/i18n/catalog.py
