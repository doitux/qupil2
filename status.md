# TestFlight-Sammellauf 2026-11-25 00:55 (von Hand, now)

**Build 61 gestartet**

- Build 61 (Commit 8afbfc1): queued, Upload: -, Testtext: -
- https://github.com/doitux/qupil2/actions/runs/45513939348
- Seit Build 60 (Commit 8e04cd1)

Commits:
- 2647a7e No look into the clipboard when a text field is made; the tick at the end of a student's question
- 414e8ca Merge main into fix/clipboard-ask
- 21ffee4 Student app CI: no false alarm about Associated Domains after an upload
- f007fa2 Merge main into fix/clipboard-ask
- b4eb04f Merge fix/clipboard-ask: no look into the clipboard when a text field is made (no iOS paste prompt); the tick at the end of a student's question
- d3e451d Long hints wrap in their field; the points of the trial too
- 5effe2f Student app, iOS: the page's colour to the screen's edges, no black strips
- 407c213 Student page: the private note of a lesson with tasks and a long block heading wrap instead of being cut
- 5f599bc Student app: the commit of the build beside the version, as in Qupil
- b608137 Student app, iOS simulator test: the edge check has its picture library from the start
- 8ea0845 Merge fix/student-ios-edges: the student app paints to the screen's edges on iOS (no black strips); the commit of the build beside the version
- 2ef171f Test card-long-texts: two lines of the private note are 30 px high on Windows, not more
- e29da17 Merge main into feature/hint-wrap
- 8afbfc1 Merge feature/hint-wrap: long hints wrap in their field; the private note and long block headings on a student's cards are shown whole

Für iOS relevante Dateien:
- .github/scripts/android-test.sh
- .github/scripts/ios-edges.py
- .github/scripts/student-ios-test.sh
- .github/workflows/student.yml
- qml/components/InputField.qml
- qml/components/TaskLessonCard.qml
- qml/components/TextTouch.qml
- qml/editors/TaskEditor.qml
- qml/idea/IdeaEntrySheet.qml
- qml/idea/IdeaLessonSheet.qml
- qml/idea/IdeaTaskEdit.qml
- qml/idea/IdeaTaskSheet.qml
- qml/pages/StudentDetail.qml
- src/app/DataService.cpp
- src/app/DataService.h
- src/app/DataServiceTasks.cpp
- src/app/UiTest.cpp
- src/app/UiTestIdea.cpp
- student/CMakeLists.txt
- student/i18n/strings.json
- student/i18n/strings.py
- student/qml/Main.qml
- student/qml/SettingsPage.qml
- student/src/SafeArea.h
- student/src/SafeArea_ios.mm
- student/src/SafeArea_none.cpp
- student/src/StudentApp.cpp
- student/src/StudentApp.h
- student/src/main.cpp
- student/tests/tst_studentapp.cpp
