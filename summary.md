# iOS test run 89 (ea506c5)

## iphone: iPhone 16

### UI test (inside Qupil)

- platform: iOS 26.2 (ios), Qt 6.8.3
- window: 393 × 759

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.7 s |  |
| ✅ | pages – every page opens | 8.4 s |  |
| ✅ | today – today with the running lesson, switching days | 3.0 s |  |
| ✅ | note-save – write and save a note for the running lesson | 7.7 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 6.3 s |  |
| ✅ | keyboard-smooth – the keyboard comes in an open editor: the editor makes room smoothly | 16.2 s | No on-screen keyboard came: not measured.  |
| ✅ | dialogs-all – every editor sheet is in the keyboard test | 1.1 s |  |
| ✅ | dialogs-prepare – sample data for the editors | 1.9 s |  |
| ✅ | dialogs-note – NoteEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 7.9 s |  |
| ✅ | dialogs-plan – NoteEditor (editPlan) with the keyboard: Save and the cursor stay in view, no jumping | 8.1 s |  |
| ✅ | dialogs-tasks – TaskEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.0 s |  |
| ✅ | dialogs-plan-finish – PlanFinish (start) with the keyboard: Save and the cursor stay in view, no jumping | 6.7 s |  |
| ✅ | dialogs-plan-from-note – PlanFinish (startFromNote) with the keyboard: Save and the cursor stay in view, no jumping | 6.6 s |  |
| ✅ | dialogs-booklet – BookletSheet (show) with the keyboard: Save and the cursor stay in view, no jumping | 8.3 s |  |
| ✅ | dialogs-piece – PieceEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.0 s |  |
| ✅ | dialogs-student – NewStudent (start) with the keyboard: Save and the cursor stay in view, no jumping | 8.8 s |  |
| ✅ | dialogs-lesson – LessonEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.0 s |  |
| ✅ | dialogs-activity – ActivityEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 8.0 s |  |
| ✅ | dialogs-reminder – ReminderEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 8.0 s |  |
| ✅ | dialogs-event – EventEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 8.9 s |  |
| ✅ | dialogs-external – ExternalEditor (start) with the keyboard: Save and the cursor stay in view, no jumping | 8.7 s |  |
| ✅ | dialogs-ready-pieces – ReadyPiecesPicker (start) with the keyboard: Save and the cursor stay in view, no jumping | 6.9 s |  |
| ✅ | dialogs-library – LibraryItemEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.0 s |  |
| ✅ | dialogs-pairing – PairingSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 7.1 s |  |
| ✅ | dialogs-day-overview – DayOverviewSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 6.7 s |  |
| ✅ | dialogs-csv-import – CsvImportSheet (picked) with the keyboard: Save and the cursor stay in view, no jumping | 6.6 s |  |
| ✅ | dialogs-cleanup – the editors' sample data goes again | 1.9 s |  |
| ✅ | note-menu – a note: a tap only shows a block, the menu has new note, plan, edit, share, delete | 20.5 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | tasks-menu – tasks of a coupled student: a tap shows the hints, the menu plans, edits and deletes | 18.7 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | idea-demo – The trial of the new idea: a lesson, the history, preparing, pausing; nothing is stored | 71.4 s |  |
| ✅ | idea-point-wrap – trial: a long point wraps in its field while it is written | 11.5 s |  |
| ✅ | idea-fold-arrow – trial: the arrow of a task unfolds it in the window of a lesson | 5.3 s |  |
| ✅ | student-search – search a student, open and close the student page | 9.6 s | 2 taps needed for "^Lena Muster$".  |
| ✅ | student-new – add a student | 11.0 s |  |
| ✅ | event-open – open an event and go back | 2.8 s |  |
| ✅ | metronome – start and stop the metronome | 13.6 s |  |
| ✅ | appearance – switch to dark and back to light | 5.3 s |  |
| ✅ | language – switch the language to English and back | 6.8 s |  |
| ✅ | reminders – open the reminders | 2.9 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 7.1 s |  |
| ✅ | plan-save – plan the next lesson of a student | 13.5 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 14.3 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 12.0 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 11.1 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 27.2 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | tasks-long-texts – tasks: a long hint wraps in its field, a long title is wiped sideways and shows its start | 32.0 s |  |
| ✅ | card-long-texts – student page: a long private note and a long block heading wrap instead of being cut | 12.1 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 7.4 s |  |
| ✅ | booklet-questions – practice book: the student's questions on the lesson's card, "Gelesen" takes them away; link copied is said | 18.8 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | booklet-devices – practice book sheet: how many devices have the booklet, where it was removed, the last feedback | 8.9 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 11.0 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 4.4 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 4.9 s |  |
| ✅ | piece-new – add a piece for a student | 18.1 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | lesson-new – add a lesson from the schedule | 5.6 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 7.9 s |  |
| ✅ | together – two individual lessons of a student kept together | 16.9 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | together-new – new lesson for a student who has one: kept together | 14.0 s | 2 taps needed for "combobox:^Wochentag$". 2 taps needed for "^Muster, Lena".  |
| ✅ | reminder-new – add a reminder | 7.6 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 12.3 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 36.1 s | 2 taps needed for "^Unterricht$". 2 taps needed for "^Streicherensemble · Di 17:00$". 2 taps needed for "^Lena\ Muster$".  |
| ✅ | loan-return – return sheet music from the student's page | 12.4 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | more-pages – birthdays and data pages open | 3.0 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 3.2 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 5.4 s |  |

65 of 65 cases passed.

- platform: iOS 26.2 (ios), Qt 6.8.3
- window: 393 × 759

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 6.7 s |  |

1 of 1 cases passed.

- platform: iOS 26.2 (ios), Qt 6.8.3
- window: 393 × 759

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.8 s |  |
| ✅ | reset – reset Qupil on this device, then restore the data from before | 8.8 s |  |
| ✅ | update-hint – changes from a newer Qupil: More asks for an update | 2.8 s |  |

3 of 3 cases passed.

### Background and ended by the system

- screenshot `iphone/shots/01-editor-typing.png`
- screenshot `iphone/shots/02-background.png`
- the editor came back with the unsaved note

### Metronome in the background

- screenshot `iphone/shots/03-metronome-playing.png`
- screenshot `iphone/shots/04-metronome-background.png`
| | Check | Got |
|---|---|---|
| ✅ | plays in front | 8 beats |
| ✅ | plays on in the background (8 s) | 6 beats |
| ✅ | audio session | Playback |

### idb

- Qupil drawn after about 14 s
- screenshot `iphone/shots/05-start.png`
- **not on the screen:** `^OK$`
- tapped `^[0-9][0-9]:[0-9][0-9] EU-` at 196 1056
- **not on the screen:** `^Notiz$`

### Selecting text by touch (note editor)

- window offset: 0, 0 ("Fett" at 42 708 on the screen, Qupil logged 42 708)
- screenshot `iphone/shots/06-select-start.png`
- screenshot `iphone/shots/07-select-1-double-tap.png`
- screenshot `iphone/shots/08-select-3-bold.png`
- screenshot `iphone/shots/09-select-5-long-press.png`
- screenshot `iphone/shots/10-select-8-word.png`
- screenshot `iphone/shots/11-select-9-phrase.png`
- screenshot `iphone/shots/12-select-13-colour.png`
- screenshot `iphone/shots/13-select-17-paste-menu.png`
- screenshot `iphone/shots/14-select-18-pasted.png`
- screenshot `iphone/shots/15-select-19-wipe.png`
| | Scenario | Expected | Got |
|---|---|---|---|
| ⏭ | 0 first long press of the tool | `Bogeneinteilung` | `"": came as a tap (noted only)` |
| ⏭ | 1 double tap on a word, line 1 | `Tonleitern` | `the tool cannot double tap: long press` |
| ℹ️ | 1 double tap on a word, line 1 (long press instead) | `Tonleitern` | `` |
| ℹ️ | 2 menu after the double tap | `Ausschneiden, Kopieren, Alles auswählen` | `` |
| ℹ️ | 2 menu after the double tap: no system menu | `` | `Paste, Select, Select All` |
| ℹ️ | 3 "B" with the first tap | `\*\*Tonleitern\*\*` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilun` |
| ℹ️ ✓ | 4 a tap behind the text ends the selection | `` | `` |
| ℹ️ ✓ | 4 menu closed | `` | `` |
| ℹ️ ✓ | 4 menu closed: no system menu | `` | `` |
| ✅ | 4 the text itself unchanged | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |
| ℹ️ | 5 long press on a word, line 2 | `Metronom` | `` |
| ℹ️ | 6 menu after the long press | `Ausschneiden, Kopieren, Alles auswählen` | `` |
| ℹ️ | 6 menu after the long press: no system menu | `` | `Paste, Select, Select All` |
| ℹ️ | 7 "I" on it | `\*Metronom\*` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilun` |
| ⏭ | 8 double tap on "G-Dur" | `G-Dur` | `the tool cannot double tap: long press` |
| ℹ️ | 8 double tap on "G-Dur" (long press instead) | `G-Dur` | `` |
| ℹ️ | 9 end handle dragged to "D-Dur" | `G-Dur und D-Dur` | `no end handle` |
| ℹ️ | 10 menu again after the handle | `Ausschneiden, Kopieren, Alles auswählen` | `` |
| ℹ️ | 10 menu again after the handle: no system menu | `` | `Paste, Select, Select All` |
| ℹ️ | 11 "U" on the phrase | `\+\+G-Dur und D-Dur\+\+` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilun` |
| ⏭ | 12 double tap on a word, line 3 | `Mittelteil` | `the tool cannot double tap: long press` |
| ℹ️ | 12 double tap on a word, line 3 (long press instead) | `Mittelteil` | `` |
| ℹ️ | 13 colour on it | `==Mittelteil==` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilun` |
| ✅ | 13 the text itself unchanged | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |
| ℹ️ | 15 long press on "Etüde" | `Etüde` | `` |
| ℹ️ | 16 "Kopieren" in the menu | `Kopieren` | `` |
| ℹ️ | 17 a tap on the cursor: menu | `Auswählen, Einsetzen, Alles auswählen` | `` |
| ℹ️ ✓ | 17 a tap on the cursor: menu: no system menu | `` | `` |
| ℹ️ | 18 "Einsetzen" | `Einsetzen` | `` |
| ✅ | 18 the text: what was pasted at the end, nothing else changed | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |
| ℹ️ ✓ | 19 a quick wipe over the text selects nothing | `` | `` |
| ✅ | 19 the text after the wipe unchanged | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |

4 of 4 checks that the text stays intact as expected (Qupil's touch rules are off here: the other rows show what the system does).

Markup at the end: `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil`
- Qupil drawn after about 16 s

### Dark mode

- setting in `de.qupil.Qupil.plist`: `"appearance.mode" => "system"`
- setting in `de.qupil.uitest.Qupil.plist`: `"appearance.mode" => "light"`
- screenshot `iphone/shots/16-dark-running.png`
- Qupil drawn after about 14 s
- screenshot `iphone/shots/17-dark-start.png`
- screenshot `iphone/shots/18-light-again.png`

## ipad: iPad Air 13-inch

### UI test (inside Qupil)

- platform: iOS 26.2 (ios), Qt 6.8.3
- window: 1024 × 1309

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.9 s |  |
| ✅ | pages – every page opens | 10.4 s |  |
| ✅ | today – today with the running lesson, switching days | 3.4 s |  |
| ✅ | note-save – write and save a note for the running lesson | 7.8 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 6.4 s |  |
| ✅ | keyboard-smooth – the keyboard comes in an open editor: the editor makes room smoothly | 16.7 s | No on-screen keyboard came: not measured.  |
| ✅ | dialogs-all – every editor sheet is in the keyboard test | 1.3 s |  |
| ✅ | dialogs-prepare – sample data for the editors | 2.1 s |  |
| ✅ | dialogs-note – NoteEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 8.5 s |  |
| ✅ | dialogs-plan – NoteEditor (editPlan) with the keyboard: Save and the cursor stay in view, no jumping | 8.6 s |  |
| ✅ | dialogs-tasks – TaskEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.3 s |  |
| ✅ | dialogs-plan-finish – PlanFinish (start) with the keyboard: Save and the cursor stay in view, no jumping | 7.3 s |  |
| ✅ | dialogs-plan-from-note – PlanFinish (startFromNote) with the keyboard: Save and the cursor stay in view, no jumping | 6.8 s |  |
| ✅ | dialogs-booklet – BookletSheet (show) with the keyboard: Save and the cursor stay in view, no jumping | 8.7 s |  |
| ✅ | dialogs-piece – PieceEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.7 s |  |
| ✅ | dialogs-student – NewStudent (start) with the keyboard: Save and the cursor stay in view, no jumping | 9.5 s |  |
| ✅ | dialogs-lesson – LessonEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.3 s |  |
| ✅ | dialogs-activity – ActivityEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 8.2 s |  |
| ✅ | dialogs-reminder – ReminderEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 8.2 s |  |
| ✅ | dialogs-event – EventEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.5 s |  |
| ✅ | dialogs-external – ExternalEditor (start) with the keyboard: Save and the cursor stay in view, no jumping | 9.4 s |  |
| ✅ | dialogs-ready-pieces – ReadyPiecesPicker (start) with the keyboard: Save and the cursor stay in view, no jumping | 7.4 s |  |
| ✅ | dialogs-library – LibraryItemEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.3 s |  |
| ✅ | dialogs-pairing – PairingSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 7.1 s |  |
| ✅ | dialogs-day-overview – DayOverviewSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 6.8 s |  |
| ✅ | dialogs-csv-import – CsvImportSheet (picked) with the keyboard: Save and the cursor stay in view, no jumping | 6.8 s |  |
| ✅ | dialogs-cleanup – the editors' sample data goes again | 1.9 s |  |
| ✅ | note-menu – a note: a tap only shows a block, the menu has new note, plan, edit, share, delete | 21.5 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | tasks-menu – tasks of a coupled student: a tap shows the hints, the menu plans, edits and deletes | 19.1 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | idea-demo – The trial of the new idea: a lesson, the history, preparing, pausing; nothing is stored | 68.3 s |  |
| ✅ | idea-point-wrap – trial: a long point wraps in its field while it is written | 11.7 s |  |
| ✅ | idea-fold-arrow – trial: the arrow of a task unfolds it in the window of a lesson | 5.4 s |  |
| ✅ | student-search – search a student, open and close the student page | 10.0 s | 2 taps needed for "^Lena Muster$".  |
| ✅ | student-new – add a student | 10.6 s |  |
| ✅ | event-open – open an event and go back | 3.0 s |  |
| ✅ | metronome – start and stop the metronome | 10.7 s |  |
| ✅ | appearance – switch to dark and back to light | 4.5 s |  |
| ✅ | language – switch the language to English and back | 6.4 s |  |
| ✅ | reminders – open the reminders | 3.4 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 14.8 s |  |
| ✅ | plan-save – plan the next lesson of a student | 16.1 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 14.8 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 11.9 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 12.1 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 28.6 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | tasks-long-texts – tasks: a long hint wraps in its field, a long title is wiped sideways and shows its start | 32.2 s |  |
| ✅ | card-long-texts – student page: a long private note and a long block heading wrap instead of being cut | 11.5 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 7.5 s |  |
| ✅ | booklet-questions – practice book: the student's questions on the lesson's card, "Gelesen" takes them away; link copied is said | 19.6 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | booklet-devices – practice book sheet: how many devices have the booklet, where it was removed, the last feedback | 10.5 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 11.5 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 4.2 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 5.3 s |  |
| ✅ | piece-new – add a piece for a student | 18.1 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | lesson-new – add a lesson from the schedule | 5.1 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 8.2 s |  |
| ✅ | together – two individual lessons of a student kept together | 17.0 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | together-new – new lesson for a student who has one: kept together | 14.2 s | 2 taps needed for "combobox:^Wochentag$". 2 taps needed for "^Muster, Lena".  |
| ✅ | reminder-new – add a reminder | 7.7 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 12.6 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 35.8 s | 2 taps needed for "^Unterricht$". 2 taps needed for "^Streicherensemble · Di 17:00$". 2 taps needed for "^Lena\ Muster$".  |
| ✅ | loan-return – return sheet music from the student's page | 12.1 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | more-pages – birthdays and data pages open | 2.8 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 3.3 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 5.3 s |  |

65 of 65 cases passed.

- platform: iOS 26.2 (ios), Qt 6.8.3
- window: 1024 × 1309

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 7.0 s |  |

1 of 1 cases passed.

- platform: iOS 26.2 (ios), Qt 6.8.3
- window: 1024 × 1309

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.8 s |  |
| ✅ | reset – reset Qupil on this device, then restore the data from before | 8.6 s |  |
| ✅ | update-hint – changes from a newer Qupil: More asks for an update | 3.0 s |  |

3 of 3 cases passed.

### Background and ended by the system

- screenshot `ipad/shots/01-editor-typing.png`
- screenshot `ipad/shots/02-background.png`
- the editor came back with the unsaved note

### Metronome in the background

- screenshot `ipad/shots/03-metronome-playing.png`
- screenshot `ipad/shots/04-metronome-background.png`
| | Check | Got |
|---|---|---|
| ✅ | plays in front | 6 beats |
| ✅ | plays on in the background (8 s) | 10 beats |
| ✅ | audio session | Playback |

### idb

- Qupil drawn after about 10 s
- screenshot `ipad/shots/05-start.png`
- **not on the screen:** `^OK$`
- tapped `^[0-9][0-9]:[0-9][0-9] EU-` at 620 1029
- tapped `^Notiz$` at 949 1293
- screenshot `ipad/shots/06-idb-typing.png`
- **not on the screen:** `^Abbrechen$`

### Selecting text by touch (note editor)

- window offset: 0, 0 ("Fett" at 244 918 on the screen, Qupil logged 244 918)
- screenshot `ipad/shots/07-select-start.png`
- screenshot `ipad/shots/08-select-1-double-tap.png`
- screenshot `ipad/shots/09-select-3-bold.png`
- screenshot `ipad/shots/10-select-5-long-press.png`
- screenshot `ipad/shots/11-select-8-word.png`
- screenshot `ipad/shots/12-select-9-phrase.png`
- screenshot `ipad/shots/13-select-13-colour.png`
- screenshot `ipad/shots/14-select-17-paste-menu.png`
- screenshot `ipad/shots/15-select-18-pasted.png`
- screenshot `ipad/shots/16-select-19-wipe.png`
| | Scenario | Expected | Got |
|---|---|---|---|
| ⏭ | 0 first long press of the tool | `Bogeneinteilung` | `"": came as a tap (noted only)` |
| ⏭ | 1 double tap on a word, line 1 | `Tonleitern` | `the tool cannot double tap: long press` |
| ℹ️ | 1 double tap on a word, line 1 (long press instead) | `Tonleitern` | `` |
| ℹ️ | 2 menu after the double tap | `Ausschneiden, Kopieren, Alles auswählen` | `` |
| ℹ️ | 2 menu after the double tap: no system menu | `` | `Paste, Select, Select All` |
| ℹ️ | 3 "B" with the first tap | `\*\*Tonleitern\*\*` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilun` |
| ℹ️ ✓ | 4 a tap behind the text ends the selection | `` | `` |
| ℹ️ ✓ | 4 menu closed | `` | `` |
| ℹ️ ✓ | 4 menu closed: no system menu | `` | `` |
| ✅ | 4 the text itself unchanged | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |
| ℹ️ | 5 long press on a word, line 2 | `Metronom` | `` |
| ℹ️ | 6 menu after the long press | `Ausschneiden, Kopieren, Alles auswählen` | `` |
| ℹ️ | 6 menu after the long press: no system menu | `` | `Paste, Select, Select All` |
| ℹ️ | 7 "I" on it | `\*Metronom\*` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilun` |
| ⏭ | 8 double tap on "G-Dur" | `G-Dur` | `the tool cannot double tap: long press` |
| ℹ️ | 8 double tap on "G-Dur" (long press instead) | `G-Dur` | `` |
| ℹ️ | 9 end handle dragged to "D-Dur" | `G-Dur und D-Dur` | `no end handle` |
| ℹ️ | 10 menu again after the handle | `Ausschneiden, Kopieren, Alles auswählen` | `` |
| ℹ️ | 10 menu again after the handle: no system menu | `` | `Paste, Select, Select All` |
| ℹ️ | 11 "U" on the phrase | `\+\+G-Dur und D-Dur\+\+` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilun` |
| ⏭ | 12 double tap on a word, line 3 | `Mittelteil` | `the tool cannot double tap: long press` |
| ℹ️ | 12 double tap on a word, line 3 (long press instead) | `Mittelteil` | `` |
| ℹ️ | 13 colour on it | `==Mittelteil==` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilun` |
| ✅ | 13 the text itself unchanged | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |
| ℹ️ | 15 long press on "Etüde" | `Etüde` | `` |
| ℹ️ | 16 "Kopieren" in the menu | `Kopieren` | `` |
| ℹ️ | 17 a tap on the cursor: menu | `Auswählen, Einsetzen, Alles auswählen` | `` |
| ℹ️ ✓ | 17 a tap on the cursor: menu: no system menu | `` | `` |
| ℹ️ | 18 "Einsetzen" | `Einsetzen` | `` |
| ✅ | 18 the text: what was pasted at the end, nothing else changed | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |
| ℹ️ ✓ | 19 a quick wipe over the text selects nothing | `` | `` |
| ✅ | 19 the text after the wipe unchanged | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |

4 of 4 checks that the text stays intact as expected (Qupil's touch rules are off here: the other rows show what the system does).

Markup at the end: `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil`
- Qupil drawn after about 10 s

### Dark mode

- setting in `de.qupil.Qupil.plist`: `"appearance.mode" => "system"`
- setting in `de.qupil.uitest.Qupil.plist`: `"appearance.mode" => "light"`
- screenshot `ipad/shots/17-dark-running.png`
- Qupil drawn after about 0 s
- screenshot `ipad/shots/18-dark-start.png`
- screenshot `ipad/shots/19-light-again.png`

Result: all steps ran
