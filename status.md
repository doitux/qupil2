# TestFlight-Sammellauf 2026-11-20 22:05

**Build 54 hochgeladen**

- Build 54 (Commit dda1b7e): success, Upload: success, Testtext: success
- https://github.com/doitux/qupil2/actions/runs/44853951733
- Seit Build 53 (Commit 37eb400)

Commits:
- d00ea69 Student app: booklet format, app skeleton, interface and tests
- 3812bb2 Student app: icons, Android and iOS packaging, own workflow
- 9526dd4 Practice book, teacher's side: links in Nextcloud and the inbox (not wired in yet)
- 928b75e Student app: old Nextcloud through the old public WebDAV (both ways); docs/student-app.md
- 940fe03 Practice book: QR code for the coupling link (Nayuki qrcodegen, MIT)
- 233be8c Student app: qupil.de files for the coupling link (/u page, app and asset links)
- 4ffff2a Homework as a message: share a note through the system's share menu
- fa714b1 Practice book data: tasks and coupling (schema 8)
- b5ed62a Merge branch 'feature/homework-share' into feature/uebeheft
- f3b3e6e Schedule: the clipboard in a fixed column on the right, never hidden by a birthday
- 043d511 Practice book: tasks editor, lesson cards, booklets in the Nextcloud
- 958cc1a What's new: the clipboard column
- 7652655 Merge fix/plan-icon-with-birthday: the clipboard in a fixed column, never hidden by a birthday
- 55471cf Practice book: coupling sheet, answers at lesson start, plan to tasks
- 7c4883e Practice book: "Tasks" at the running lesson and on the lesson page; docs
- 9e30341 Merge remote-tracking branch 'origin/main' into feature/uebeheft
- b488066 Practice book: plans of coupled students as tasks, private blocks, edit before saving
- 35d9c4a What did you cover?: the lock centred on the tick box, the lock glyph centred in its box
- 76e50fd What did you cover?: open lock (goes to the student), closed lock (private)
- e044c0f What did you cover?: a point without lines sits in the middle of its row
- 85f9c3b What's new: share a note as a message; practice book (preparation)
- 2df98ae Merge feature/uebeheft: practice book (teacher's side and student app), share a note as a message
- 0193d34 Lesson view: "Plan" ("Plan for everybody" for groups and ensembles)
- 2b1ffae What's new: plan from the lesson view
- cf9ee20 Android test: the weekday in Berlin time, the draft step closes every reminder dialog first
- dda1b7e Merge feature/lesson-plan-button: "Planung" in the lesson view, "Planung für alle" for groups and ensembles

Für iOS relevante Dateien:
- .github/scripts/android-test.sh
- .github/workflows/student.yml
- 3rdparty/qrcodegen/qrcodegen.cpp
- 3rdparty/qrcodegen/qrcodegen.hpp
- CMakeLists.txt
- i18n/qupil_de.ts
- i18n/qupil_en.ts
- i18n/qupil_es.ts
- i18n/qupil_fr.ts
- qml/Icon.qml
- qml/Labels.qml
- qml/Main.qml
- qml/Nav.qml
- qml/components/TaskLessonCard.qml
- qml/editors/BookletSheet.qml
- qml/editors/PlanFinish.qml
- qml/editors/TaskEditor.qml
- qml/pages/LessonDetail.qml
- qml/pages/ScheduleOverview.qml
- qml/pages/StudentDetail.qml
- src/app/AppController.cpp
- src/app/AppController.h
- src/app/BookletService.cpp
- src/app/BookletService.h
- src/app/DataService.cpp
- src/app/DataService.h
- src/app/DataServiceTasks.cpp
- src/app/ReminderAlert.cpp
- src/app/ReminderAlert.h
- src/app/SyncController.h
- src/app/TextShare.h
- src/app/TextShare_ios.mm
- src/app/TextShare_none.cpp
- src/app/UiTest.cpp
- src/app/UiTest.h
- src/core/Schema.cpp
- src/core/Schema.h
- src/main.cpp
- src/shared/Booklet.cpp
- src/shared/Booklet.h
- src/shared/BookletInbox.cpp
- src/shared/BookletInbox.h
- src/shared/NextcloudShares.cpp
- src/shared/NextcloudShares.h
- src/shared/QrCode.cpp
- src/shared/QrCode.h
- student/CMakeLists.txt
- student/assets/icons/student-256.png
- student/assets/icons/student-android-foreground.svg
- student/assets/icons/student-android-monochrome.svg

Nächtliche Tests:
- Android: gestartet (seit 2df98ae, letzter Lauf: success)
- Desktop: gestartet (seit e55d6c2, letzter Lauf: success)
- iOS: gestartet (seit e55d6c2, letzter Lauf: success)
