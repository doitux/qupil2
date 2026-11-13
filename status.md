# TestFlight-Sammellauf 2026-11-12 22:05

**Build 47 gestartet**

- Build 47 (Commit ca6fcee): in_progress, Upload: -, Testtext: -
- https://github.com/doitux/qupil2/actions/runs/43347275755
- Seit Build 46 (Commit 6c53974)

Commits:
- 94763ed What's new: system text selection as in build 42 (43 never reached TestFlight)
- 4e723d3 What's new: metronome in the background checked on the iPad
- f47c594 What's new: metronome with the screen locked checked on the iPad; silent mode explained
- caabb1a What's new: metronome in silent mode checked on the iPad
- 8e47bf0 iOS keyboard in an editor: room made with the keyboard, the view no longer jumps
- b8b8230 UI test keyboard-smooth: tap into the note first, note the keyboard's state
- efe0709 iOS test: the simulator with its on-screen keyboard (per-device setting, Simulator app)
- 0106d44 UI test keyboard-smooth: no on-screen keyboard in the simulator is noted, not failed
- 5778b71 Android test: keyboard-smooth in the first part
- 8a81eb6 What's new: the keyboard in the note editor (check on the iPad)
- 6ea3668 Merge fix/ios-keyboard-smooth: the editor makes room with the iOS keyboard, the view no longer jumps
- ca6fcee Docs: no on-screen keyboard in the test simulators

Für iOS relevante Dateien:
- .github/scripts/android-test.sh
- qml/Main.qml
- qml/components/Sheet.qml
- src/app/KeyboardHeight.h
- src/app/KeyboardHeight_ios.mm
- src/app/UiTest.cpp
- src/main.cpp

Nächtliche Tests:
- Android: gestartet (seit 6c53974, letzter Lauf: success)
- Desktop: gestartet (seit 6c53974, letzter Lauf: success)
- iOS: gestartet (seit 6c53974, letzter Lauf: failure)
