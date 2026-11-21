# Desktop UI test run 47 (9c740d7)

## linux

### phone (390x844), first
- platform: Ubuntu 24.04.5 LTS (xcb), Qt 6.8.3
- window: 390 × 844

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.4 s |  |
| ✅ | pages – every page opens | 6.9 s |  |
| ✅ | today – today with the running lesson, switching days | 2.4 s |  |
| ✅ | note-save – write and save a note for the running lesson | 3.2 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 2.9 s |  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 7.0 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 11.0 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 5.5 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 5.2 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 12.6 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 6.6 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 12.2 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 10.2 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 6.7 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 5.3 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 8.2 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 6.1 s |  |
| ❌ | dialogs-all – every editor sheet is in the keyboard test | 0.9 s | step 1: no editor left out |
| ✅ | dialogs-prepare – sample data for the editors | 1.1 s |  |
| ✅ | dialogs-note – NoteEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 62.5 s |  |
| ✅ | dialogs-plan – NoteEditor (editPlan) with the keyboard: Save and the cursor stay in view, no jumping | 62.5 s |  |
| ✅ | dialogs-tasks – TaskEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 62.5 s |  |
| ✅ | dialogs-plan-finish – PlanFinish (start) with the keyboard: Save and the cursor stay in view, no jumping | 62.4 s |  |
| ✅ | dialogs-plan-from-note – PlanFinish (startFromNote) with the keyboard: Save and the cursor stay in view, no jumping | 62.4 s |  |
| ✅ | dialogs-booklet – BookletSheet (show) with the keyboard: Save and the cursor stay in view, no jumping | 62.5 s |  |
| ✅ | dialogs-piece – PieceEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 62.5 s |  |
| ✅ | dialogs-student – NewStudent (start) with the keyboard: Save and the cursor stay in view, no jumping | 62.5 s |  |
| ✅ | dialogs-lesson – LessonEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 62.5 s |  |
| ✅ | dialogs-activity – ActivityEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 62.5 s |  |
| ✅ | dialogs-reminder – ReminderEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 62.5 s |  |
| ✅ | dialogs-event – EventEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 62.5 s |  |
| ✅ | dialogs-external – ExternalEditor (start) with the keyboard: Save and the cursor stay in view, no jumping | 62.5 s |  |
| ✅ | dialogs-ready-pieces – ReadyPiecesPicker (start) with the keyboard: Save and the cursor stay in view, no jumping | 62.5 s |  |
| ✅ | dialogs-library – LibraryItemEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 62.5 s |  |
| ✅ | dialogs-pairing – PairingSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 62.8 s |  |
| ✅ | dialogs-day-overview – DayOverviewSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 62.4 s |  |
| ✅ | dialogs-csv-import – CsvImportSheet (picked) with the keyboard: Save and the cursor stay in view, no jumping | 62.5 s |  |
| ✅ | dialogs-cleanup – the editors' sample data goes again | 1.2 s |  |
| ✅ | student-search – search a student, open and close the student page | 3.6 s |  |
| ✅ | student-new – add a student | 3.6 s |  |
| ✅ | event-open – open an event and go back | 2.1 s |  |
| ✅ | metronome – start and stop the metronome | 2.6 s | no sound: Konnte die Soundkarte nicht initialisieren. |
| ✅ | appearance – switch to dark and back to light | 3.0 s |  |
| ✅ | language – switch the language to English and back | 4.3 s |  |
| ✅ | reminders – open the reminders | 2.3 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 5.1 s |  |
| ✅ | plan-save – plan the next lesson of a student | 4.2 s |  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 7.6 s |  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 5.3 s |  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 4.7 s |  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 8.2 s |  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 6.1 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 5.5 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 2.8 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 3.3 s |  |
| ✅ | piece-new – add a piece for a student | 5.2 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 4.4 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 3.4 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 6.1 s |  |
| ✅ | together – two individual lessons of a student kept together | 8.0 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 5.8 s |  |
| ✅ | reminder-new – add a reminder | 3.5 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 6.5 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 13.0 s |  |
| ✅ | loan-return – return sheet music from the student's page | 5.3 s |  |
| ✅ | more-pages – birthdays and data pages open | 2.4 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 2.4 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.0 s |  |

67 of 68 cases passed.

### phone (390x844), restart
- platform: Ubuntu 24.04.5 LTS (xcb), Qt 6.8.3
- window: 390 × 844

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.4 s |  |

1 of 1 cases passed.

### wide (1000x660), first
- platform: Ubuntu 24.04.5 LTS (xcb), Qt 6.8.3
- window: 1000 × 660

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.4 s |  |
| ✅ | pages – every page opens | 6.9 s |  |
| ✅ | today – today with the running lesson, switching days | 2.4 s |  |
| ✅ | note-save – write and save a note for the running lesson | 3.3 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 2.9 s |  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 7.0 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 11.0 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 5.5 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 5.2 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 12.6 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 6.7 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 12.2 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 10.2 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 6.8 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 5.3 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 8.2 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 6.1 s |  |
| ❌ | dialogs-all – every editor sheet is in the keyboard test | 1.0 s | step 1: no editor left out |
| ✅ | dialogs-prepare – sample data for the editors | 1.1 s |  |
| ✅ | dialogs-note – NoteEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-plan – NoteEditor (editPlan) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-tasks – TaskEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-plan-finish – PlanFinish (start) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-plan-from-note – PlanFinish (startFromNote) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-booklet – BookletSheet (show) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-piece – PieceEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-student – NewStudent (start) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-lesson – LessonEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-activity – ActivityEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-reminder – ReminderEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-event – EventEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-external – ExternalEditor (start) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-ready-pieces – ReadyPiecesPicker (start) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-library – LibraryItemEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-pairing – PairingSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 16.3 s |  |
| ✅ | dialogs-day-overview – DayOverviewSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-csv-import – CsvImportSheet (picked) with the keyboard: Save and the cursor stay in view, no jumping | 16.2 s |  |
| ✅ | dialogs-cleanup – the editors' sample data goes again | 1.2 s |  |
| ✅ | student-search – search a student, open and close the student page | 3.6 s |  |
| ✅ | student-new – add a student | 3.6 s |  |
| ✅ | event-open – open an event and go back | 2.2 s |  |
| ✅ | metronome – start and stop the metronome | 2.6 s | no sound: Konnte die Soundkarte nicht initialisieren. |
| ✅ | appearance – switch to dark and back to light | 3.0 s |  |
| ✅ | language – switch the language to English and back | 4.2 s |  |
| ✅ | reminders – open the reminders | 2.4 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 5.2 s |  |
| ✅ | plan-save – plan the next lesson of a student | 4.3 s |  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 7.7 s |  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 5.3 s |  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 4.7 s |  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 8.3 s |  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 6.2 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 5.6 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 2.8 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 3.3 s |  |
| ✅ | piece-new – add a piece for a student | 5.2 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 4.4 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 3.4 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 6.2 s |  |
| ✅ | together – two individual lessons of a student kept together | 8.0 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 5.9 s |  |
| ✅ | reminder-new – add a reminder | 3.5 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 6.6 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 13.1 s |  |
| ✅ | loan-return – return sheet music from the student's page | 5.3 s |  |
| ✅ | more-pages – birthdays and data pages open | 2.5 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 2.4 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.0 s |  |

67 of 68 cases passed.

### wide (1000x660), restart
- platform: Ubuntu 24.04.5 LTS (xcb), Qt 6.8.3
- window: 1000 × 660

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.4 s |  |

1 of 1 cases passed.


## macos

### phone (390x844), first
- platform: macOS Sequoia (15.8) (cocoa), Qt 6.8.3
- window: 390 × 653

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.7 s |  |
| ✅ | pages – every page opens | 8.1 s |  |
| ✅ | today – today with the running lesson, switching days | 2.7 s |  |
| ✅ | note-save – write and save a note for the running lesson | 3.9 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 3.4 s |  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 8.4 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 12.9 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 6.3 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 6.4 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 14.6 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 7.8 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 15.0 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 12.4 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 8.7 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 6.5 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 10.0 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 7.2 s |  |
| ❌ | dialogs-all – every editor sheet is in the keyboard test | 1.0 s | step 1: no editor left out |
| ✅ | dialogs-prepare – sample data for the editors | 1.3 s |  |
| ❌ | dialogs-note – NoteEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 5.0 s | step 4: window has the size (375×667) |
| ❌ | dialogs-plan – NoteEditor (editPlan) with the keyboard: Save and the cursor stay in view, no jumping | 5.2 s | step 4: window has the size (375×667) |
| ❌ | dialogs-tasks – TaskEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 5.2 s | step 4: window has the size (375×667) |
| ❌ | dialogs-plan-finish – PlanFinish (start) with the keyboard: Save and the cursor stay in view, no jumping | 5.5 s | step 4: window has the size (375×667) |
| ❌ | dialogs-plan-from-note – PlanFinish (startFromNote) with the keyboard: Save and the cursor stay in view, no jumping | 5.2 s | step 4: window has the size (375×667) |
| ❌ | dialogs-booklet – BookletSheet (show) with the keyboard: Save and the cursor stay in view, no jumping | 5.3 s | step 4: window has the size (375×667) |
| ❌ | dialogs-piece – PieceEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 5.2 s | step 4: window has the size (375×667) |
| ❌ | dialogs-student – NewStudent (start) with the keyboard: Save and the cursor stay in view, no jumping | 5.2 s | step 4: window has the size (375×667) |
| ❌ | dialogs-lesson – LessonEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 5.4 s | step 4: window has the size (375×667) |
| ❌ | dialogs-activity – ActivityEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 5.2 s | step 4: window has the size (375×667) |
| ❌ | dialogs-reminder – ReminderEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 5.2 s | step 4: window has the size (375×667) |
| ❌ | dialogs-event – EventEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 5.3 s | step 4: window has the size (375×667) |
| ❌ | dialogs-external – ExternalEditor (start) with the keyboard: Save and the cursor stay in view, no jumping | 5.4 s | step 4: window has the size (375×667) |
| ❌ | dialogs-ready-pieces – ReadyPiecesPicker (start) with the keyboard: Save and the cursor stay in view, no jumping | 5.3 s | step 4: window has the size (375×667) |
| ❌ | dialogs-library – LibraryItemEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 5.2 s | step 4: window has the size (375×667) |
| ❌ | dialogs-pairing – PairingSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 5.2 s | step 4: window has the size (375×667) |
| ❌ | dialogs-day-overview – DayOverviewSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 5.5 s | step 4: window has the size (375×667) |
| ❌ | dialogs-csv-import – CsvImportSheet (picked) with the keyboard: Save and the cursor stay in view, no jumping | 5.5 s | step 4: window has the size (375×667) |
| ✅ | dialogs-cleanup – the editors' sample data goes again | 1.2 s |  |
| ✅ | student-search – search a student, open and close the student page | 4.7 s |  |
| ✅ | student-new – add a student | 4.7 s |  |
| ✅ | event-open – open an event and go back | 2.9 s |  |
| ✅ | metronome – start and stop the metronome | 4.6 s |  |
| ✅ | appearance – switch to dark and back to light | 3.8 s |  |
| ✅ | language – switch the language to English and back | 5.7 s |  |
| ✅ | reminders – open the reminders | 3.7 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 6.9 s |  |
| ✅ | plan-save – plan the next lesson of a student | 5.6 s |  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 10.4 s |  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 6.6 s |  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 6.3 s |  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 10.2 s |  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 8.1 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 7.0 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 3.3 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 3.9 s |  |
| ✅ | piece-new – add a piece for a student | 6.8 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 6.2 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.2 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 7.9 s |  |
| ✅ | together – two individual lessons of a student kept together | 10.2 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 7.6 s |  |
| ✅ | reminder-new – add a reminder | 4.8 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 8.3 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 17.0 s |  |
| ✅ | loan-return – return sheet music from the student's page | 7.0 s |  |
| ✅ | more-pages – birthdays and data pages open | 4.1 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 3.4 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.7 s |  |

49 of 68 cases passed.

### phone (390x844), restart
- platform: macOS Sequoia (15.8) (cocoa), Qt 6.8.3
- window: 390 × 656

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.6 s |  |

1 of 1 cases passed.

### wide (1000x660), first
- platform: macOS Sequoia (15.8) (cocoa), Qt 6.8.3
- window: 1000 × 656

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.5 s |  |
| ✅ | pages – every page opens | 8.9 s |  |
| ✅ | today – today with the running lesson, switching days | 2.9 s |  |
| ✅ | note-save – write and save a note for the running lesson | 4.2 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 3.9 s |  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 8.9 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 13.6 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 7.0 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 6.9 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 15.6 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 8.7 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 15.0 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 12.7 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 8.5 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 6.6 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 10.4 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 7.9 s |  |
| ❌ | dialogs-all – every editor sheet is in the keyboard test | 1.1 s | step 1: no editor left out |
| ✅ | dialogs-prepare – sample data for the editors | 1.4 s |  |
| ✅ | dialogs-note – NoteEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 19.4 s |  |
| ✅ | dialogs-plan – NoteEditor (editPlan) with the keyboard: Save and the cursor stay in view, no jumping | 19.5 s |  |
| ✅ | dialogs-tasks – TaskEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 19.2 s |  |
| ✅ | dialogs-plan-finish – PlanFinish (start) with the keyboard: Save and the cursor stay in view, no jumping | 19.5 s |  |
| ✅ | dialogs-plan-from-note – PlanFinish (startFromNote) with the keyboard: Save and the cursor stay in view, no jumping | 19.6 s |  |
| ✅ | dialogs-booklet – BookletSheet (show) with the keyboard: Save and the cursor stay in view, no jumping | 19.4 s |  |
| ✅ | dialogs-piece – PieceEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 19.6 s |  |
| ✅ | dialogs-student – NewStudent (start) with the keyboard: Save and the cursor stay in view, no jumping | 18.9 s |  |
| ✅ | dialogs-lesson – LessonEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 19.8 s |  |
| ✅ | dialogs-activity – ActivityEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 19.4 s |  |
| ✅ | dialogs-reminder – ReminderEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 19.6 s |  |
| ✅ | dialogs-event – EventEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 19.4 s |  |
| ✅ | dialogs-external – ExternalEditor (start) with the keyboard: Save and the cursor stay in view, no jumping | 19.7 s |  |
| ✅ | dialogs-ready-pieces – ReadyPiecesPicker (start) with the keyboard: Save and the cursor stay in view, no jumping | 20.3 s |  |
| ✅ | dialogs-library – LibraryItemEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 18.7 s |  |
| ✅ | dialogs-pairing – PairingSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 20.9 s |  |
| ✅ | dialogs-day-overview – DayOverviewSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 19.7 s |  |
| ✅ | dialogs-csv-import – CsvImportSheet (picked) with the keyboard: Save and the cursor stay in view, no jumping | 19.8 s |  |
| ✅ | dialogs-cleanup – the editors' sample data goes again | 1.4 s |  |
| ✅ | student-search – search a student, open and close the student page | 5.4 s |  |
| ✅ | student-new – add a student | 5.1 s |  |
| ✅ | event-open – open an event and go back | 3.1 s |  |
| ✅ | metronome – start and stop the metronome | 4.4 s |  |
| ✅ | appearance – switch to dark and back to light | 3.8 s |  |
| ✅ | language – switch the language to English and back | 5.5 s |  |
| ✅ | reminders – open the reminders | 3.7 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 7.1 s |  |
| ✅ | plan-save – plan the next lesson of a student | 5.6 s |  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 9.7 s |  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 7.0 s |  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 6.4 s |  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 11.5 s |  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 8.3 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 6.9 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 3.6 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 4.1 s |  |
| ✅ | piece-new – add a piece for a student | 7.2 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 5.8 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.7 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 8.2 s |  |
| ✅ | together – two individual lessons of a student kept together | 10.7 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 7.2 s |  |
| ✅ | reminder-new – add a reminder | 5.1 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 8.7 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 17.4 s |  |
| ✅ | loan-return – return sheet music from the student's page | 7.0 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.7 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 3.6 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.6 s |  |

67 of 68 cases passed.

### wide (1000x660), restart
- platform: macOS Sequoia (15.8) (cocoa), Qt 6.8.3
- window: 1000 × 656

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.8 s |  |

1 of 1 cases passed.


## windows

### phone (390x844), first
- platform: Windows Server 2022 Version 21H2 (windows), Qt 6.8.3
- window: 390 × 749

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.4 s |  |
| ✅ | pages – every page opens | 10.1 s |  |
| ✅ | today – today with the running lesson, switching days | 2.9 s |  |
| ✅ | note-save – write and save a note for the running lesson | 4.7 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 3.4 s |  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 8.4 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 12.1 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 7.0 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 5.7 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 13.2 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 8.1 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 12.9 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 10.8 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 8.9 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 5.9 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 8.9 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 6.6 s |  |
| ❌ | dialogs-all – every editor sheet is in the keyboard test | 1.4 s | step 1: no editor left out |
| ✅ | dialogs-prepare – sample data for the editors | 1.6 s |  |
| ❌ | dialogs-note – NoteEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 22.7 s | step 41: window has the size (390×844) |
| ❌ | dialogs-plan – NoteEditor (editPlan) with the keyboard: Save and the cursor stay in view, no jumping | 22.6 s | step 41: window has the size (390×844) |
| ❌ | dialogs-tasks – TaskEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 22.7 s | step 41: window has the size (390×844) |
| ❌ | dialogs-plan-finish – PlanFinish (start) with the keyboard: Save and the cursor stay in view, no jumping | 22.5 s | step 41: window has the size (390×844) |
| ❌ | dialogs-plan-from-note – PlanFinish (startFromNote) with the keyboard: Save and the cursor stay in view, no jumping | 22.5 s | step 41: window has the size (390×844) |
| ❌ | dialogs-booklet – BookletSheet (show) with the keyboard: Save and the cursor stay in view, no jumping | 22.6 s | step 41: window has the size (390×844) |
| ❌ | dialogs-piece – PieceEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 22.7 s | step 41: window has the size (390×844) |
| ❌ | dialogs-student – NewStudent (start) with the keyboard: Save and the cursor stay in view, no jumping | 22.7 s | step 41: window has the size (390×844) |
| ❌ | dialogs-lesson – LessonEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 22.7 s | step 41: window has the size (390×844) |
| ❌ | dialogs-activity – ActivityEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 22.7 s | step 41: window has the size (390×844) |
| ❌ | dialogs-reminder – ReminderEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 22.7 s | step 41: window has the size (390×844) |
| ❌ | dialogs-event – EventEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 22.7 s | step 41: window has the size (390×844) |
| ❌ | dialogs-external – ExternalEditor (start) with the keyboard: Save and the cursor stay in view, no jumping | 22.7 s | step 41: window has the size (390×844) |
| ❌ | dialogs-ready-pieces – ReadyPiecesPicker (start) with the keyboard: Save and the cursor stay in view, no jumping | 22.6 s | step 41: window has the size (390×844) |
| ❌ | dialogs-library – LibraryItemEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 22.7 s | step 41: window has the size (390×844) |
| ❌ | dialogs-pairing – PairingSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 20.9 s | step 41: window has the size (390×844) |
| ❌ | dialogs-day-overview – DayOverviewSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 22.5 s | step 41: window has the size (390×844) |
| ❌ | dialogs-csv-import – CsvImportSheet (picked) with the keyboard: Save and the cursor stay in view, no jumping | 22.5 s | step 41: window has the size (390×844) |
| ✅ | dialogs-cleanup – the editors' sample data goes again | 1.9 s |  |
| ✅ | student-search – search a student, open and close the student page | 6.1 s |  |
| ✅ | student-new – add a student | 5.0 s |  |
| ✅ | event-open – open an event and go back | 4.3 s |  |
| ✅ | metronome – start and stop the metronome | 3.2 s | no sound: Konnte die Soundkarte nicht initialisieren. |
| ✅ | appearance – switch to dark and back to light | 4.5 s |  |
| ✅ | language – switch the language to English and back | 6.4 s |  |
| ✅ | reminders – open the reminders | 4.7 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 7.0 s |  |
| ✅ | plan-save – plan the next lesson of a student | 6.0 s |  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 9.1 s |  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 6.7 s |  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 6.0 s |  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 10.4 s |  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 7.6 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 7.9 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 3.3 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 4.5 s |  |
| ✅ | piece-new – add a piece for a student | 5.7 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 6.3 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.7 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 8.1 s |  |
| ✅ | together – two individual lessons of a student kept together | 10.7 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 7.5 s |  |
| ✅ | reminder-new – add a reminder | 4.2 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 9.9 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 19.2 s |  |
| ✅ | loan-return – return sheet music from the student's page | 7.5 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.7 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 4.5 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.7 s |  |

49 of 68 cases passed.

### phone (390x844), restart
- platform: Windows Server 2022 Version 21H2 (windows), Qt 6.8.3
- window: 390 × 749

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.4 s |  |

1 of 1 cases passed.

### wide (1000x660), first
- platform: Windows Server 2022 Version 21H2 (windows), Qt 6.8.3
- window: 1000 × 660

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.4 s |  |
| ✅ | pages – every page opens | 10.1 s |  |
| ✅ | today – today with the running lesson, switching days | 2.9 s |  |
| ✅ | note-save – write and save a note for the running lesson | 4.7 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 3.4 s |  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 8.4 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 12.1 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 7.0 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 5.7 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 13.2 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 8.1 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 12.9 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 10.8 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 8.9 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 5.9 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 8.9 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 6.6 s |  |
| ❌ | dialogs-all – every editor sheet is in the keyboard test | 1.5 s | step 1: no editor left out |
| ✅ | dialogs-prepare – sample data for the editors | 1.6 s |  |
| ✅ | dialogs-note – NoteEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 19.1 s |  |
| ✅ | dialogs-plan – NoteEditor (editPlan) with the keyboard: Save and the cursor stay in view, no jumping | 18.9 s |  |
| ✅ | dialogs-tasks – TaskEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 19.1 s |  |
| ✅ | dialogs-plan-finish – PlanFinish (start) with the keyboard: Save and the cursor stay in view, no jumping | 19.0 s |  |
| ✅ | dialogs-plan-from-note – PlanFinish (startFromNote) with the keyboard: Save and the cursor stay in view, no jumping | 19.0 s |  |
| ✅ | dialogs-booklet – BookletSheet (show) with the keyboard: Save and the cursor stay in view, no jumping | 19.1 s |  |
| ✅ | dialogs-piece – PieceEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 19.1 s |  |
| ✅ | dialogs-student – NewStudent (start) with the keyboard: Save and the cursor stay in view, no jumping | 19.1 s |  |
| ✅ | dialogs-lesson – LessonEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 19.1 s |  |
| ✅ | dialogs-activity – ActivityEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 19.1 s |  |
| ✅ | dialogs-reminder – ReminderEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 19.1 s |  |
| ✅ | dialogs-event – EventEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 19.1 s |  |
| ✅ | dialogs-external – ExternalEditor (start) with the keyboard: Save and the cursor stay in view, no jumping | 19.2 s |  |
| ✅ | dialogs-ready-pieces – ReadyPiecesPicker (start) with the keyboard: Save and the cursor stay in view, no jumping | 19.1 s |  |
| ✅ | dialogs-library – LibraryItemEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 19.1 s |  |
| ✅ | dialogs-pairing – PairingSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 17.4 s |  |
| ✅ | dialogs-day-overview – DayOverviewSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 19.0 s |  |
| ✅ | dialogs-csv-import – CsvImportSheet (picked) with the keyboard: Save and the cursor stay in view, no jumping | 19.0 s |  |
| ✅ | dialogs-cleanup – the editors' sample data goes again | 1.9 s |  |
| ✅ | student-search – search a student, open and close the student page | 6.2 s |  |
| ✅ | student-new – add a student | 5.0 s |  |
| ✅ | event-open – open an event and go back | 4.3 s |  |
| ✅ | metronome – start and stop the metronome | 3.1 s | no sound: Konnte die Soundkarte nicht initialisieren. |
| ✅ | appearance – switch to dark and back to light | 4.5 s |  |
| ✅ | language – switch the language to English and back | 6.4 s |  |
| ✅ | reminders – open the reminders | 4.7 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 7.0 s |  |
| ✅ | plan-save – plan the next lesson of a student | 6.0 s |  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 9.1 s |  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 6.8 s |  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 6.0 s |  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 10.5 s |  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 7.6 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 7.9 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 3.3 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 4.6 s |  |
| ✅ | piece-new – add a piece for a student | 5.8 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 6.3 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.6 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 8.1 s |  |
| ✅ | together – two individual lessons of a student kept together | 10.8 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 7.4 s |  |
| ✅ | reminder-new – add a reminder | 4.2 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 10.0 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 19.3 s |  |
| ✅ | loan-return – return sheet music from the student's page | 7.4 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.8 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 4.5 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.8 s |  |

67 of 68 cases passed.

### wide (1000x660), restart
- platform: Windows Server 2022 Version 21H2 (windows), Qt 6.8.3
- window: 1000 × 660

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.4 s |  |

1 of 1 cases passed.


## Screens changed since the run before

No run before to compare with.
