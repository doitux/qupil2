# TestFlight-Sammellauf 2026-11-11 23:38 (von Hand, now)

**Build 46 gestartet**

- Build 46 (Commit 6c53974): in_progress, Upload: -, Testtext: -
- https://github.com/doitux/qupil2/actions/runs/43113447383
- Seit Build 44 (Commit f52dbef)

Commits:
- 4ca38b2 WIP: one touch/mouse rule set for text fields (TextTouch)
- 3cd0baa Touch tests from outside follow the new text rules and fail the run
- 595af8f UI test: student search starts from an empty field
- 224e71a Touch tests from outside: read the menu from a fresh log, allow Einsetzen
- f576685 Docs: text by finger (rules, implementation, tests); What's new
- 942f7ec Text touch: format buttons commit the composed word; iOS/iPad test fixes
- 7b60c8c Text touch: a tap right behind the selection ends it
- 9b4963d Touch tests from outside: the wipe comes last
- e55de2d Merge feature/text-touch: one set of touch rules for text on iOS and Android
- 5a8a307 iOS: metronome keeps playing in the background and on silent
- 1a5b951 What's new: metronome in the background fixed after build 43
- 10b3a3f What's new: say what was tested where, not more
- 33597c8 What's new: new since build 42, and only what was checked
- 3100511 Metronome: the beat timer runs on the audio thread; test log connected
- a793920 UI test select phase: log what the keyboard sends to the note
- 71e8e51 Text touch: commit the keyboard only when it is composing a word
- 564c6d3 Date field: the calendar opens above the field when there is no room below
- 880f394 What's new: the date calendar above the field
- c2ccc9a Metronome background: the simulator does not show the error; say so
- d6d3ee8 Merge fix/date-popup-ipad into fix/ios-metronome-background (one set of platform runs)
- 8b1de39 iOS: system text selection again for now; calendar test uses "Start"
- 134f5bd UI test: date-calendar not on iOS yet
- c5124e9 Merge fix/ios-metronome-background: metronome on iOS, date calendar, iOS text selection
- 467647c What's new: say exactly where the calendar was tested; metronome line tidied
- db6667a Text menu: the bar scrolls sideways when it is wider than the window
- 0b6f0c8 What's new: new since build 44 (given to the external testers this morning)
- ddafb9d Merge branch 'main' into fix/text-menu-scroll
- 6c53974 Merge fix/text-menu-scroll: the text menu bar scrolls sideways on narrow screens

Für iOS relevante Dateien:
- .github/scripts/android-test.sh
- .github/scripts/ios-check-archive.sh
- .github/scripts/ios-test.sh
- .github/scripts/select-scenarios.sh
- .github/scripts/select-touch.py
- CMakeLists.txt
- i18n/qupil_de.ts
- i18n/qupil_en.ts
- i18n/qupil_es.ts
- i18n/qupil_fr.ts
- qml/components/DateField.qml
- qml/components/InputField.qml
- qml/components/TextTouch.qml
- qml/editors/NoteEditor.qml
- qml/pages/ArchiveEntryPage.qml
- qml/pages/LicensePage.qml
- src/app/AudioSession_ios.mm
- src/app/Metronome.cpp
- src/app/TextFormatter.cpp
- src/app/TextFormatter.h
- src/app/TextFormatting.cpp
- src/app/TextFormatting.h
- src/app/UiTest.cpp
- src/app/UiTest.h
- src/main.cpp
- tests/tst_notes.cpp
- tools/i18n/catalog.py
