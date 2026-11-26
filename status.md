# TestFlight-Sammellauf 2026-11-25 22:05

**Build 62 hochgeladen**

- Build 62 (Commit ea506c5): success, Upload: success, Testtext: success
- https://github.com/doitux/qupil2/actions/runs/45749204795
- Seit Build 61 (Commit 8afbfc1)

Commits:
- 1c9a667 Student app: collected TestFlight builds with the testers' text set by the workflow
- a1cb3d2 Merge feature/student-collect: the student app joins the collected TestFlight builds; its testers' text is set by the workflow
- 4ba0e7c Trial: the arrow of a task unfolds it in the window of a lesson
- 76aa6c3 Merge fix/idea-fold-arrow: the arrow of a task unfolds it in the trial's lesson window
- fe3bb04 Dev helpers: local build and UI test in one go, one-call CI start and status [skip ci]
- ee8b304 Dev helpers: the student app's local tests in one go, its device tests in the CI status [skip ci]
- db7e249 Merge fix/student-dev-helpers: local test script and CI status for the student app [skip ci]
- 49d0d86 Trial of the new idea: a paused task taken up while preparing stays taken up
- 7bf6101 What's new: taking up a paused task while preparing
- b929370 What's new: back under 4000 characters
- ea506c5 Merge fix/idea-prepare-resume: a paused task taken up while preparing stays taken up

Für iOS relevante Dateien:
- .github/scripts/android-test.sh
- .github/scripts/asc-whats-new.py
- qml/idea/IdeaModel.qml
- qml/idea/IdeaTaskEdit.qml
- src/app/UiTestIdea.cpp
- tools/dev/ci-start.sh
- tools/dev/ci-status.sh
- tools/dev/local-test.sh
- tools/dev/qt64build.sh
- tools/dev/student-test.sh

Schüler-App (Qupil Üben): **Kein Build nötig**
- Seit Build 50 (Commit 8ea0845)

Nächtliche Tests:
- Android: gestartet (seit 8afbfc1, letzter Lauf: success)
- Desktop: gestartet (seit 8afbfc1, letzter Lauf: success)
- iOS: gestartet (seit 8afbfc1, letzter Lauf: success)
