# TestFlight-Sammellauf 2026-11-21 22:05

**Build 55 hochgeladen**

- Build 55 (Commit 81d205f): success, Upload: success, Testtext: success
- https://github.com/doitux/qupil2/actions/runs/44970033800
- Seit Build 54 (Commit dda1b7e)

Commits:
- b710a1b Names of the apps: "Qupil Üben" / "Qupil Practice" instead of "Übeheft"
- 6a3d18f TestFlight text: shorter (3957 of 4000 characters), length checked on every push
- 03f3530 Merge feature/app-names: "Qupil Üben" / "Qupil Practice" in both apps, shorter TestFlight text with a length check
- cd48790 UI test: every editor sheet with the keyboard; sheets in a low window keep the cursor in view
- 9c740d7 UI test dialogs: invokeMethod with Q_ARG per number of arguments (Qt 6.5+)
- fc3dfb8 Student app: Android test build on the nightly page
- d43bf0b Merge fix/student-apk-nightly: the student app's Android test build on the nightly page
- ba729fa UI test dialogs: editor list from CMake, faster (one opening per size, fewer waits)
- 593182f UI test dialogs: a size the desktop's window cannot take is left out with a note
- 81d205f Merge fix/sheet-keyboard-test: UI test of every editor sheet with the keyboard; phone held sideways keeps the cursor in view

Für iOS relevante Dateien:
- .github/scripts/android-test.sh
- .github/workflows/student.yml
- CMakeLists.txt
- i18n/qupil_de.ts
- i18n/qupil_en.ts
- i18n/qupil_es.ts
- i18n/qupil_fr.ts
- qml/components/Sheet.qml
- src/app/UiTest.cpp
- src/app/UiTest.h
- src/app/UiTestDialogs.cpp
- student/CMakeLists.txt
- student/i18n/strings.json
- student/i18n/strings.py
- student/platform/android/res/values-de/strings.xml
- student/platform/android/res/values-es/strings.xml
- student/platform/android/res/values-fr/strings.xml
- student/platform/android/res/values/strings.xml
- student/platform/ios/de.lproj/InfoPlist.strings
- student/platform/ios/en.lproj/InfoPlist.strings
- student/platform/ios/es.lproj/InfoPlist.strings
- student/platform/ios/fr.lproj/InfoPlist.strings
- student/platform/ios/patch-info-plist.sh
- student/web/u/index.html
- tools/i18n/catalog.py

Nächtliche Tests:
- Android: gestartet (seit dda1b7e, letzter Lauf: success)
- Desktop: gestartet (seit dda1b7e, letzter Lauf: success)
- iOS: gestartet (seit dda1b7e, letzter Lauf: success)
