# TestFlight-Sammellauf 2026-11-06 14:47 (von Hand, now)

**Build 38 gestartet**

- Build 38 (Commit a75f8e0): in_progress, Upload: -, Testtext: -
- https://github.com/doitux/qupil2/actions/runs/42228296968
- Seit Build 37 (Commit 0d57e5e)

Commits:
- ee70b62 Android: status and navigation bar in Qupil's colours
- 0c37910 Android: log the steps of the start and of the activity (black window)
- 19079e6 Android: always start fresh, never from a saved state (black window)
- 9871496 Android: restart afresh when brought back with a saved state (black window)
- f4ec077 CI: unsigned iOS build on macos-15 (macos-14 no longer accepted)
- 75b3899 CI: publish the Android test build without waiting for the iOS job
- 5570480 Note editor: keep unsaved text as a draft, open it again at the start
- 7ae2ea1 Note editor: no "Clear" next to the line of a restored draft
- 6434c2f CI: collected TestFlight builds by GitHub itself (13:05 and 22:05)
- be1cd52 CI: collect builds only what is not on TestFlight yet
- b98c689 Nightly files named alike: Qupil-<platform>-nightly-<commit>
- 3d3ee13 CI: macOS desktop build on macos-15 (macos-14 no longer accepted)
- 1e09dcb Nightly: no unsigned iOS file any more (iPad and iPhone via TestFlight)
- eb87940 CI: Mobile publish downloads the Android artifact into its folder
- 827b1b0 CI: Android test run on an emulator
- d7e41fd Today: "Later today" only when no lesson of the list was earlier
- 797f3b8 Android test: drafts, "Don't keep activities" for real, cleaner screens
- c94acc6 Note draft: write it to disk at once (Android emulator: draft was lost)
- e08dbfc UI test: go through the important flows by tapping and typing (--ui-test)
- ac1f258 CI: UI test runs on real desktops, in the iOS simulator and on Android
- a1af107 UI test: smaller windows, layouts in between, Android restart half, iOS simulator build
- d4aba78 docs: how Qupil is tested (unit tests, UI test, platform runs)
- 81de258 Test runs: stacks of a hanging Android start, idb tap trusted
- b868345 CI: platform tests every night, screens compared with the night before, fix branches
- 43a9524 Android test: catch the restart half while it hangs, keep its data
- 1f892a5 iOS test: background and ended by the system without idb; idb optional
- 0517b33 Tests: no notification permission dialog over the window; idb with Xcode 26
- cb5e360 docs: nightly platform runs, visual diff, fixes on fix/ branches
- a75f8e0 Reminders follow the app's clock; UI test taps again when a tap did nothing

Für iOS relevante Dateien:
- .github/scripts/android-test.sh
- .github/scripts/android-ui.py
- .github/scripts/ios-test.sh
- .github/scripts/ios-ui.py
- .github/scripts/publish-nightly.sh
- .github/scripts/ui-test-desktop.sh
- .github/scripts/visual-diff.sh
- .github/workflows/android-test.yml
- .github/workflows/ios-test.yml
- .github/workflows/ui-test.yml
- CMakeLists.txt
- i18n/qupil_de.ts
- i18n/qupil_en.ts
- i18n/qupil_es.ts
- i18n/qupil_fr.ts
- qml/AppearanceSettings.qml
- qml/Main.qml
- qml/Nav.qml
- qml/components/InputField.qml
- qml/editors/NoteEditor.qml
- qml/pages/ScheduleOverview.qml
- src/app/AppController.cpp
- src/app/AppController.h
- src/app/LessonEndAlert.cpp
- src/app/LessonEndAlert.h
- src/app/ReminderAlert.cpp
- src/app/ScreenshotTour.cpp
- src/app/SystemBars.h
- src/app/UiTest.cpp
- src/app/UiTest.h
- src/main.cpp
- tests/ui-test.cmake
- tools/i18n/catalog.py
