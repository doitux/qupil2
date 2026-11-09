# Testing Qupil

Everything runs with invented sample data (the demo generator), never with real data.

## On every push (CI "Desktop", ctest)

| Test | What it checks |
|---|---|
| `core`, `notes`, `languagemanager` | unit tests of the data layer |
| `qml_smoke` | the whole QML UI loads without a single QML warning |
| `ui_test_phone`, `ui_test_wide` | the UI test below in a phone and a wide window, without a screen |

Run them locally with `QT_QPA_PLATFORM=offscreen ctest --test-dir build`.

## The UI test (`--ui-test`)

`qupil --ui-test <folder> [--ui-test-size 390x844] [--data-dir <folder>]` goes through
the important flows the way a person would: it finds buttons, tabs and fields by the
names Qupil gives them for screen readers, taps them, types, and checks the screen and
the data (`src/app/UiTest.cpp`). "Today" is a Tuesday at 14:20, so a lesson is running.
The run ends with an unsaved note; `--ui-test-phase restart` on the same `--data-dir`
checks that the note editor comes back with it.

The folder gets `results.md` / `results.json`, a screenshot at the end of every case
(`case-*.png`) and, for a failed case, `fail-<case>.png` with `fail-<case>.txt` – the
list of everything that was on the screen (role, position, name).

**New feature → new case.** Add a case to `UiTest::addCases()` for every new flow:

```cpp
Case &c = add(u"piece-new"_s, u"add a piece for a student"_s);
c.steps << tab(u"Schüler"_s) << tap(u"^Lena Muster$"_s) << tap(u"^Werk$"_s)
        << tap(u"edit:^Titel$"_s) << type(u"Menuett"_s)
        << tap(u"^Speichern$"_s) << gone(u"edit:^Titel$"_s)
        << check(u"saved"_s, [this] { return …; });
```

- Patterns are regular expressions on the accessible name; `edit:`, `button:`,
  `combobox:`, `menuitem:` … limit the role, `tab:` means the navigation.
- Give new controls an `Accessible.name` (screen readers need it as well).
- Cases must not depend on the window size: `backIfThere()` taps a back button only
  where the layout has one; elements scrolled out of sight are scrolled to.
- A case that changes data the later cases rely on puts it back (see `lesson-new`,
  `loan-return`).
- A popup that should close after a choice: `tapUntilGone(pattern)` taps until the
  entry is gone (iOS: the first tap may only close the keyboard).
- A toast is found by its text (its `Accessible.name`), its undo button by
  "Rückgängig".

## Readable text

Every screenshot a case takes (`snap`) and the screen at the end of every case are
checked for text that cannot be read: for each visible text the test finds what is
painted behind its middle (a control's background, a card, the window) and computes
the contrast ratio (WCAG). Below 2:1 the case fails with "text not readable: …" and
names the text and both colours. It found an entry in the lists of choices
(ChoiceField) that turned white on an almost white row under the finger.

So a new screen, list, menu or popup gets a case that opens it, puts the highlight
on an entry where it has one (a key press), and takes a `snap` - before it is
committed.

## Platform runs (every evening on main when something changed, and by hand; results on a branch)

| Workflow | Where | Branch |
|---|---|---|
| UI test (desktop) – `ui-test.yml` | Linux (X11), Windows, macOS with a real window; optional screenshot tour | `ui-test-desktop` |
| iOS test – `ios-test.yml` | iPhone and iPad simulator: the UI test inside the app, then with idb from outside: background, ended, started again (draft), dark mode | `ios-test` |
| Android test – `android-test.yml` | Android 13 emulator (ATD image): start with sample data, background, process ended, draft, dark mode, landscape, the UI test | `android-test` |

The TestFlight collect run at 22:05 German time starts them on main
(`.github/scripts/testflight-collect.py`, `start_tests()`), each only when something it
tests changed since the commit of its last run – no run without a change. GitHub's own
schedules started them hours late or not at all, so they have none. The desktop run then
has the input `nightly` (screenshot tour on Linux). The desktop run compares every screenshot with the run before
(`visual-diff.sh`): screens that changed are listed in `visual-diff.md` with a
"before | after | difference" picture.

Each branch holds only the latest run (screenshots, screen dumps, a summary). Start with
`gh workflow run <workflow> -R doitux/qupil2 --ref main`, read with
`git fetch origin <branch>` and the branch's `summary.md` (desktop: `README.md`).

## Fixes

A fix goes to a branch `fix/<topic>` first, with a UI test case that shows the problem.
Pushes to `fix/**` run the Desktop and Mobile CI (unit tests, UI test on three systems,
Android build); the platform runs are started on the branch with
`gh workflow run <workflow> --ref fix/<topic>`. Only when everything is green does the
fix go to main (nightly builds and TestFlight are made from main only). A bigger new
feature goes the same way on `feature/<topic>`, with its UI test cases. After a push to
main, `cleanup-branches.yml` deletes every `fix/…` and `feature/…` branch that is
completely in main.
