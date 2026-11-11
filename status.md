# TestFlight-Sammellauf 2026-11-10 22:05

**Build 44 gestartet**

- Build 44 (Commit f52dbef): in_progress, Upload: -, Testtext: -
- https://github.com/doitux/qupil2/actions/runs/42937138949
- Seit Build 42 (Commit 7a9836f)

Commits:
- dc07d26 Fix typed lesson times not saved; instruments per performer; PDF table spacing
- dd6683d UI test: typeTime puts the cursor at the start directly (macOS: Home goes elsewhere)
- 62793fd iPad/iPhone: alert sounds at the system's alert volume, no own slider
- 1c16e0d AlertSound: include SystemAlerts.h on every platform (iOS build)
- c966fa0 UI test: no readability check while a page slides in
- f24eb99 Lesson times: Tab/Return from start to end; an end before the start is refused
- c2c20d5 Denser "Ähem" lesson-end sound: about 6 dB louder, no clipping
- 8f0b4d5 "Ähem" without the noise: DC, rumble and hiss removed before making it denser
- b8d867c "Ähem": natural fade at the end instead of a rattle
- a2d7512 Metronome: audio on its own thread, bigger buffer, exact beats, iOS background
- c06bbbf Touch tests for selecting words in the note editor (Android emulator, iOS simulator)
- d48fbf1 Touch tests: measure the window's offset on the screen before touching
- 99b9440 Note editor: a double tap selects a word (touch); touch tests more robust
- fff1ae9 Touch: long press on Android selects when the finger lifts; better double taps in the test
- d7211ee Touch: a double tap selects hyphenated words whole ("G-Dur"); find idb on iOS runs
- 82c81f7 Android long press: select the word at once and again after the finger lifts
- a5477ff iOS test: really install idb (Xcode 26 selected for Homebrew, Python 3.12 for fb-idb)
- 3158978 Touch tests: when "B" does nothing, tap it a second time and record that
- 2a1e19a whats-new: double tap selects a word in notes
- f52dbef Note editor: format buttons in a bar above the keyboard (proposal B)

Für iOS relevante Dateien:
- .github/scripts/android-test.sh
- .github/scripts/ios-test.sh
- .github/scripts/select-scenarios.sh
- .github/scripts/select-touch.py
- .github/workflows/ios-test.yml
- CMakeLists.txt
- assets/sounds/lesson-end.wav
- i18n/qupil_de.ts
- i18n/qupil_en.ts
- i18n/qupil_es.ts
- i18n/qupil_fr.ts
- platform/ios/patch-info-plist.sh
- qml/components/Sheet.qml
- qml/components/SoundChooser.qml
- qml/components/TimeField.qml
- qml/editors/LessonEditor.qml
- qml/editors/NoteEditor.qml
- src/app/AlertSound.cpp
- src/app/AudioSession_ios.mm
- src/app/ClickGenerator.cpp
- src/app/ClickGenerator.h
- src/app/DataService.cpp
- src/app/DocumentService.cpp
- src/app/Metronome.cpp
- src/app/Metronome.h
- src/app/SystemAlerts.h
- src/app/SystemAlerts_ios.mm
- src/app/SystemAlerts_none.cpp
- src/app/UiTest.cpp
- src/app/UiTest.h
- src/core/Performers.cpp
- src/core/Performers.h
- tests/tst_core.cpp
- tests/tst_metronome.cpp
- tools/i18n/catalog.py
- tools/sounds/clean_ahem.py

Nächtliche Tests:
- Android: gestartet (seit 7a9836f, letzter Lauf: success)
- Desktop: gestartet (seit 7a9836f, letzter Lauf: success)
- iOS: gestartet (seit 7a9836f, letzter Lauf: success)
