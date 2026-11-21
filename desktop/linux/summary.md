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

