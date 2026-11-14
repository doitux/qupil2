# TestFlight-Sammellauf 2026-11-13 22:05

**Build 48 gestartet**

- Build 48 (Commit d30ff1f): in_progress, Upload: -, Testtext: -
- https://github.com/doitux/qupil2/actions/runs/43540352894
- Seit Build 47 (Commit ca6fcee)

Commits:
- 4f2ce0b UI test: no taps on what an editor covers, none while a popup moves
- 7ef1a3c iOS test: wait until Qupil is drawn before the idb and dark mode checks
- d740dbe UI test together-new: open the weekday list until it is open, check Monday
- ecde403 iOS test: idb waits for the reminders, finds "Notiz" also at night; theme setting noted
- 6f4f46e Merge fix/draft-tap-under-editor: UI test taps nothing an editor covers (restart-draft on the iPhone)
- a16b049 Tests keep the real settings untouched on macOS and iOS too
- 0120815 Merge fix/apple-test-settings: tests keep the real settings untouched on macOS and iOS
- d30ff1f What's new since build 46 (46 distributed)

Für iOS relevante Dateien:
- .github/scripts/ios-test.sh
- src/app/UiTest.cpp
- src/app/UiTest.h
- src/main.cpp

Nächtliche Tests:
- Android: gestartet (seit ca6fcee, letzter Lauf: success)
- Desktop: gestartet (seit ca6fcee, letzter Lauf: success)
- iOS: gestartet (seit ca6fcee, letzter Lauf: failure)
