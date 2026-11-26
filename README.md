# Desktop UI test run 68 (ea506c5)

## linux

### phone (390x844), first
- platform: Ubuntu 24.04.5 LTS (xcb), Qt 6.8.3
- window: 390 × 844

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.4 s |  |
| ✅ | pages – every page opens | 6.8 s |  |
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
| ✅ | text-menu-paste-unasked – finger: Einsetzen is offered without reading the clipboard; empty, it changes nothing | 7.0 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 6.7 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 5.3 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 8.2 s |  |
| ✅ | text-field-wipe – finger: wiping sideways moves a long text in a one-line field; without focus it shows its start | 18.1 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 6.1 s |  |
| ✅ | dialogs-all – every editor sheet is in the keyboard test | 0.8 s |  |
| ✅ | dialogs-prepare – sample data for the editors | 1.1 s |  |
| ✅ | dialogs-note – NoteEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 20.6 s |  |
| ✅ | dialogs-plan – NoteEditor (editPlan) with the keyboard: Save and the cursor stay in view, no jumping | 20.6 s |  |
| ✅ | dialogs-tasks – TaskEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 23.2 s |  |
| ✅ | dialogs-plan-finish – PlanFinish (start) with the keyboard: Save and the cursor stay in view, no jumping | 17.8 s |  |
| ✅ | dialogs-plan-from-note – PlanFinish (startFromNote) with the keyboard: Save and the cursor stay in view, no jumping | 17.8 s |  |
| ✅ | dialogs-booklet – BookletSheet (show) with the keyboard: Save and the cursor stay in view, no jumping | 20.5 s |  |
| ✅ | dialogs-piece – PieceEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 23.2 s |  |
| ✅ | dialogs-student – NewStudent (start) with the keyboard: Save and the cursor stay in view, no jumping | 23.2 s |  |
| ✅ | dialogs-lesson – LessonEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 23.2 s |  |
| ✅ | dialogs-activity – ActivityEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 20.5 s |  |
| ✅ | dialogs-reminder – ReminderEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 20.5 s |  |
| ✅ | dialogs-event – EventEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 23.2 s |  |
| ✅ | dialogs-external – ExternalEditor (start) with the keyboard: Save and the cursor stay in view, no jumping | 23.2 s |  |
| ✅ | dialogs-ready-pieces – ReadyPiecesPicker (start) with the keyboard: Save and the cursor stay in view, no jumping | 17.8 s |  |
| ✅ | dialogs-library – LibraryItemEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 23.2 s |  |
| ✅ | dialogs-pairing – PairingSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 17.9 s |  |
| ✅ | dialogs-day-overview – DayOverviewSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 17.8 s |  |
| ✅ | dialogs-csv-import – CsvImportSheet (picked) with the keyboard: Save and the cursor stay in view, no jumping | 17.8 s |  |
| ✅ | dialogs-cleanup – the editors' sample data goes again | 1.1 s |  |
| ✅ | note-menu – a note: a tap only shows a block, the menu has new note, plan, edit, share, delete | 14.0 s |  |
| ✅ | tasks-menu – tasks of a coupled student: a tap shows the hints, the menu plans, edits and deletes | 10.5 s |  |
| ✅ | idea-demo – The trial of the new idea: a lesson, the history, preparing, pausing; nothing is stored | 33.6 s |  |
| ✅ | idea-point-wrap – trial: a long point wraps in its field while it is written | 7.0 s |  |
| ✅ | idea-fold-arrow – trial: the arrow of a task unfolds it in the window of a lesson | 4.8 s |  |
| ✅ | student-search – search a student, open and close the student page | 3.6 s |  |
| ✅ | student-new – add a student | 3.6 s |  |
| ✅ | event-open – open an event and go back | 2.1 s |  |
| ✅ | metronome – start and stop the metronome | 2.6 s | no sound: Konnte die Soundkarte nicht initialisieren. |
| ✅ | appearance – switch to dark and back to light | 3.0 s |  |
| ✅ | language – switch the language to English and back | 4.2 s |  |
| ✅ | reminders – open the reminders | 2.3 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 5.1 s |  |
| ✅ | plan-save – plan the next lesson of a student | 4.2 s |  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 7.5 s |  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 5.2 s |  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 4.7 s |  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 8.1 s |  |
| ✅ | tasks-long-texts – tasks: a long hint wraps in its field, a long title is wiped sideways and shows its start | 21.2 s |  |
| ✅ | card-long-texts – student page: a long private note and a long block heading wrap instead of being cut | 4.4 s |  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 6.2 s |  |
| ✅ | booklet-questions – practice book: the student's questions on the lesson's card, "Gelesen" takes them away; link copied is said | 11.2 s |  |
| ✅ | booklet-devices – practice book sheet: how many devices have the booklet, where it was removed, the last feedback | 5.7 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 5.4 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 2.8 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 3.2 s |  |
| ✅ | piece-new – add a piece for a student | 5.1 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 4.4 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 3.3 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 6.1 s |  |
| ✅ | together – two individual lessons of a student kept together | 7.8 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 5.8 s |  |
| ✅ | reminder-new – add a reminder | 3.5 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 6.4 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 12.8 s |  |
| ✅ | loan-return – return sheet music from the student's page | 5.3 s |  |
| ✅ | more-pages – birthdays and data pages open | 2.4 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 2.5 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.0 s |  |

79 of 79 cases passed.

### phone (390x844), restart
- platform: Ubuntu 24.04.5 LTS (xcb), Qt 6.8.3
- window: 390 × 844

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.5 s |  |

1 of 1 cases passed.

### wide (1000x660), first
- platform: Ubuntu 24.04.5 LTS (xcb), Qt 6.8.3
- window: 1000 × 660

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
| ✅ | text-menu-paste-unasked – finger: Einsetzen is offered without reading the clipboard; empty, it changes nothing | 7.0 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 6.8 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 5.3 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 8.2 s |  |
| ✅ | text-field-wipe – finger: wiping sideways moves a long text in a one-line field; without focus it shows its start | 18.1 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 6.1 s |  |
| ✅ | dialogs-all – every editor sheet is in the keyboard test | 0.8 s |  |
| ✅ | dialogs-prepare – sample data for the editors | 1.1 s |  |
| ✅ | dialogs-note – NoteEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 7.2 s |  |
| ✅ | dialogs-plan – NoteEditor (editPlan) with the keyboard: Save and the cursor stay in view, no jumping | 7.2 s |  |
| ✅ | dialogs-tasks – TaskEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 8.1 s |  |
| ✅ | dialogs-plan-finish – PlanFinish (start) with the keyboard: Save and the cursor stay in view, no jumping | 6.3 s |  |
| ✅ | dialogs-plan-from-note – PlanFinish (startFromNote) with the keyboard: Save and the cursor stay in view, no jumping | 6.3 s |  |
| ✅ | dialogs-booklet – BookletSheet (show) with the keyboard: Save and the cursor stay in view, no jumping | 7.2 s |  |
| ✅ | dialogs-piece – PieceEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 8.1 s |  |
| ✅ | dialogs-student – NewStudent (start) with the keyboard: Save and the cursor stay in view, no jumping | 8.1 s |  |
| ✅ | dialogs-lesson – LessonEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 8.1 s |  |
| ✅ | dialogs-activity – ActivityEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 7.2 s |  |
| ✅ | dialogs-reminder – ReminderEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 7.2 s |  |
| ✅ | dialogs-event – EventEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 8.1 s |  |
| ✅ | dialogs-external – ExternalEditor (start) with the keyboard: Save and the cursor stay in view, no jumping | 8.1 s |  |
| ✅ | dialogs-ready-pieces – ReadyPiecesPicker (start) with the keyboard: Save and the cursor stay in view, no jumping | 6.3 s |  |
| ✅ | dialogs-library – LibraryItemEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 8.1 s |  |
| ✅ | dialogs-pairing – PairingSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 6.3 s |  |
| ✅ | dialogs-day-overview – DayOverviewSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 6.3 s |  |
| ✅ | dialogs-csv-import – CsvImportSheet (picked) with the keyboard: Save and the cursor stay in view, no jumping | 6.3 s |  |
| ✅ | dialogs-cleanup – the editors' sample data goes again | 1.1 s |  |
| ✅ | note-menu – a note: a tap only shows a block, the menu has new note, plan, edit, share, delete | 14.0 s |  |
| ✅ | tasks-menu – tasks of a coupled student: a tap shows the hints, the menu plans, edits and deletes | 10.5 s |  |
| ✅ | idea-demo – The trial of the new idea: a lesson, the history, preparing, pausing; nothing is stored | 34.2 s |  |
| ✅ | idea-point-wrap – trial: a long point wraps in its field while it is written | 7.0 s |  |
| ✅ | idea-fold-arrow – trial: the arrow of a task unfolds it in the window of a lesson | 4.8 s |  |
| ✅ | student-search – search a student, open and close the student page | 3.6 s |  |
| ✅ | student-new – add a student | 3.6 s |  |
| ✅ | event-open – open an event and go back | 2.1 s |  |
| ✅ | metronome – start and stop the metronome | 2.6 s | no sound: Konnte die Soundkarte nicht initialisieren. |
| ✅ | appearance – switch to dark and back to light | 3.0 s |  |
| ✅ | language – switch the language to English and back | 4.1 s |  |
| ✅ | reminders – open the reminders | 2.3 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 5.1 s |  |
| ✅ | plan-save – plan the next lesson of a student | 4.2 s |  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 7.6 s |  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 5.3 s |  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 4.7 s |  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 8.2 s |  |
| ✅ | tasks-long-texts – tasks: a long hint wraps in its field, a long title is wiped sideways and shows its start | 21.2 s |  |
| ✅ | card-long-texts – student page: a long private note and a long block heading wrap instead of being cut | 4.4 s |  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 6.2 s |  |
| ✅ | booklet-questions – practice book: the student's questions on the lesson's card, "Gelesen" takes them away; link copied is said | 11.3 s |  |
| ✅ | booklet-devices – practice book sheet: how many devices have the booklet, where it was removed, the last feedback | 5.7 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 5.5 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 2.8 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 3.3 s |  |
| ✅ | piece-new – add a piece for a student | 5.1 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 4.4 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 3.3 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 6.1 s |  |
| ✅ | together – two individual lessons of a student kept together | 7.9 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 5.8 s |  |
| ✅ | reminder-new – add a reminder | 3.5 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 6.5 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 12.9 s |  |
| ✅ | loan-return – return sheet music from the student's page | 5.2 s |  |
| ✅ | more-pages – birthdays and data pages open | 2.5 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 2.4 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.0 s |  |

79 of 79 cases passed.

### wide (1000x660), restart
- platform: Ubuntu 24.04.5 LTS (xcb), Qt 6.8.3
- window: 1000 × 660

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.5 s |  |

1 of 1 cases passed.

- screenshot tour: 172 pictures

## macos

### phone (390x844), first
- platform: macOS Sequoia (15.8) (cocoa), Qt 6.8.3
- window: 390 × 653

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.5 s |  |
| ✅ | pages – every page opens | 7.3 s |  |
| ✅ | today – today with the running lesson, switching days | 2.5 s |  |
| ✅ | note-save – write and save a note for the running lesson | 3.7 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 3.3 s |  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 7.6 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 12.1 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 6.2 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 6.0 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 15.2 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 7.9 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 13.6 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 12.3 s |  |
| ✅ | text-menu-paste-unasked – finger: Einsetzen is offered without reading the clipboard; empty, it changes nothing | 8.1 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 8.2 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 6.3 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 10.1 s |  |
| ✅ | text-field-wipe – finger: wiping sideways moves a long text in a one-line field; without focus it shows its start | 22.5 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 7.2 s |  |
| ✅ | dialogs-all – every editor sheet is in the keyboard test | 0.8 s |  |
| ✅ | dialogs-prepare – sample data for the editors | 1.3 s |  |
| ✅ | dialogs-note – NoteEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 25.6 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-plan – NoteEditor (editPlan) with the keyboard: Save and the cursor stay in view, no jumping | 24.8 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-tasks – TaskEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 27.3 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-plan-finish – PlanFinish (start) with the keyboard: Save and the cursor stay in view, no jumping | 25.5 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-plan-from-note – PlanFinish (startFromNote) with the keyboard: Save and the cursor stay in view, no jumping | 23.9 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-booklet – BookletSheet (show) with the keyboard: Save and the cursor stay in view, no jumping | 26.2 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-piece – PieceEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 28.9 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-student – NewStudent (start) with the keyboard: Save and the cursor stay in view, no jumping | 27.4 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-lesson – LessonEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 28.6 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-activity – ActivityEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 26.7 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-reminder – ReminderEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 26.7 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-event – EventEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 28.6 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-external – ExternalEditor (start) with the keyboard: Save and the cursor stay in view, no jumping | 29.3 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-ready-pieces – ReadyPiecesPicker (start) with the keyboard: Save and the cursor stay in view, no jumping | 25.7 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-library – LibraryItemEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 28.9 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-pairing – PairingSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 26.6 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-day-overview – DayOverviewSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 25.0 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-csv-import – CsvImportSheet (picked) with the keyboard: Save and the cursor stay in view, no jumping | 25.0 s | The window cannot be 375×667 here (it stays 375×656): that size left out.  |
| ✅ | dialogs-cleanup – the editors' sample data goes again | 1.2 s |  |
| ✅ | note-menu – a note: a tap only shows a block, the menu has new note, plan, edit, share, delete | 17.7 s |  |
| ✅ | tasks-menu – tasks of a coupled student: a tap shows the hints, the menu plans, edits and deletes | 14.3 s |  |
| ✅ | idea-demo – The trial of the new idea: a lesson, the history, preparing, pausing; nothing is stored | 47.1 s |  |
| ✅ | idea-point-wrap – trial: a long point wraps in its field while it is written | 10.2 s |  |
| ✅ | idea-fold-arrow – trial: the arrow of a task unfolds it in the window of a lesson | 7.6 s |  |
| ✅ | student-search – search a student, open and close the student page | 5.2 s |  |
| ✅ | student-new – add a student | 4.8 s |  |
| ✅ | event-open – open an event and go back | 3.0 s |  |
| ✅ | metronome – start and stop the metronome | 4.7 s |  |
| ✅ | appearance – switch to dark and back to light | 4.1 s |  |
| ✅ | language – switch the language to English and back | 6.0 s |  |
| ✅ | reminders – open the reminders | 3.3 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 7.1 s |  |
| ✅ | plan-save – plan the next lesson of a student | 5.7 s |  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 10.1 s |  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 7.1 s |  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 6.2 s |  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 11.2 s |  |
| ✅ | tasks-long-texts – tasks: a long hint wraps in its field, a long title is wiped sideways and shows its start | 28.5 s |  |
| ✅ | card-long-texts – student page: a long private note and a long block heading wrap instead of being cut | 6.0 s |  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 8.3 s |  |
| ✅ | booklet-questions – practice book: the student's questions on the lesson's card, "Gelesen" takes them away; link copied is said | 14.8 s |  |
| ✅ | booklet-devices – practice book sheet: how many devices have the booklet, where it was removed, the last feedback | 7.4 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 7.0 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 3.4 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 3.9 s |  |
| ✅ | piece-new – add a piece for a student | 6.8 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 6.3 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.0 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 7.5 s |  |
| ✅ | together – two individual lessons of a student kept together | 10.0 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 8.0 s |  |
| ✅ | reminder-new – add a reminder | 4.7 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 8.4 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 17.8 s |  |
| ✅ | loan-return – return sheet music from the student's page | 6.9 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.5 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 3.4 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.6 s |  |

79 of 79 cases passed.

### phone (390x844), restart
- platform: macOS Sequoia (15.8) (cocoa), Qt 6.8.3
- window: 390 × 656

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.8 s |  |

1 of 1 cases passed.

### wide (1000x660), first
- platform: macOS Sequoia (15.8) (cocoa), Qt 6.8.3
- window: 1000 × 656

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.6 s |  |
| ✅ | pages – every page opens | 9.0 s |  |
| ✅ | today – today with the running lesson, switching days | 2.9 s |  |
| ✅ | note-save – write and save a note for the running lesson | 4.1 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 3.9 s |  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 8.8 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 13.1 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 6.9 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 6.8 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 16.1 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 8.6 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 15.6 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 12.9 s |  |
| ✅ | text-menu-paste-unasked – finger: Einsetzen is offered without reading the clipboard; empty, it changes nothing | 9.0 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 8.5 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 6.9 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 11.3 s |  |
| ✅ | text-field-wipe – finger: wiping sideways moves a long text in a one-line field; without focus it shows its start | 24.1 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 8.1 s |  |
| ✅ | dialogs-all – every editor sheet is in the keyboard test | 1.0 s |  |
| ✅ | dialogs-prepare – sample data for the editors | 1.4 s |  |
| ✅ | dialogs-note – NoteEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.1 s |  |
| ✅ | dialogs-plan – NoteEditor (editPlan) with the keyboard: Save and the cursor stay in view, no jumping | 9.0 s |  |
| ✅ | dialogs-tasks – TaskEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 10.0 s |  |
| ✅ | dialogs-plan-finish – PlanFinish (start) with the keyboard: Save and the cursor stay in view, no jumping | 8.6 s |  |
| ✅ | dialogs-plan-from-note – PlanFinish (startFromNote) with the keyboard: Save and the cursor stay in view, no jumping | 8.0 s |  |
| ✅ | dialogs-booklet – BookletSheet (show) with the keyboard: Save and the cursor stay in view, no jumping | 9.1 s |  |
| ✅ | dialogs-piece – PieceEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.9 s |  |
| ✅ | dialogs-student – NewStudent (start) with the keyboard: Save and the cursor stay in view, no jumping | 10.0 s |  |
| ✅ | dialogs-lesson – LessonEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 10.1 s |  |
| ✅ | dialogs-activity – ActivityEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 8.9 s |  |
| ✅ | dialogs-reminder – ReminderEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.0 s |  |
| ✅ | dialogs-event – EventEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 10.2 s |  |
| ✅ | dialogs-external – ExternalEditor (start) with the keyboard: Save and the cursor stay in view, no jumping | 9.9 s |  |
| ✅ | dialogs-ready-pieces – ReadyPiecesPicker (start) with the keyboard: Save and the cursor stay in view, no jumping | 8.2 s |  |
| ✅ | dialogs-library – LibraryItemEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.9 s |  |
| ✅ | dialogs-pairing – PairingSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 8.9 s |  |
| ✅ | dialogs-day-overview – DayOverviewSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 8.0 s |  |
| ✅ | dialogs-csv-import – CsvImportSheet (picked) with the keyboard: Save and the cursor stay in view, no jumping | 8.3 s |  |
| ✅ | dialogs-cleanup – the editors' sample data goes again | 1.3 s |  |
| ✅ | note-menu – a note: a tap only shows a block, the menu has new note, plan, edit, share, delete | 18.8 s |  |
| ✅ | tasks-menu – tasks of a coupled student: a tap shows the hints, the menu plans, edits and deletes | 13.9 s |  |
| ✅ | idea-demo – The trial of the new idea: a lesson, the history, preparing, pausing; nothing is stored | 46.7 s |  |
| ✅ | idea-point-wrap – trial: a long point wraps in its field while it is written | 10.2 s |  |
| ✅ | idea-fold-arrow – trial: the arrow of a task unfolds it in the window of a lesson | 7.3 s |  |
| ✅ | student-search – search a student, open and close the student page | 5.2 s |  |
| ✅ | student-new – add a student | 4.7 s |  |
| ✅ | event-open – open an event and go back | 3.2 s |  |
| ✅ | metronome – start and stop the metronome | 4.3 s |  |
| ✅ | appearance – switch to dark and back to light | 4.1 s |  |
| ✅ | language – switch the language to English and back | 5.5 s |  |
| ✅ | reminders – open the reminders | 3.4 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 7.4 s |  |
| ✅ | plan-save – plan the next lesson of a student | 5.6 s |  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 9.9 s |  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 7.6 s |  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 6.6 s |  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 11.0 s |  |
| ✅ | tasks-long-texts – tasks: a long hint wraps in its field, a long title is wiped sideways and shows its start | 28.3 s |  |
| ✅ | card-long-texts – student page: a long private note and a long block heading wrap instead of being cut | 5.4 s |  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 8.2 s |  |
| ✅ | booklet-questions – practice book: the student's questions on the lesson's card, "Gelesen" takes them away; link copied is said | 15.1 s |  |
| ✅ | booklet-devices – practice book sheet: how many devices have the booklet, where it was removed, the last feedback | 7.0 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 7.4 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 3.7 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 4.5 s |  |
| ✅ | piece-new – add a piece for a student | 6.8 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 6.2 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.3 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 8.0 s |  |
| ✅ | together – two individual lessons of a student kept together | 10.4 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 7.5 s |  |
| ✅ | reminder-new – add a reminder | 4.9 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 8.2 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 18.3 s |  |
| ✅ | loan-return – return sheet music from the student's page | 7.0 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.7 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 3.1 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.7 s |  |

79 of 79 cases passed.

### wide (1000x660), restart
- platform: macOS Sequoia (15.8) (cocoa), Qt 6.8.3
- window: 1000 × 656

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 4.0 s |  |

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
| ✅ | text-menu-paste-unasked – finger: Einsetzen is offered without reading the clipboard; empty, it changes nothing | 7.5 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 8.9 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 5.9 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 9.0 s |  |
| ✅ | text-field-wipe – finger: wiping sideways moves a long text in a one-line field; without focus it shows its start | 20.2 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 6.6 s |  |
| ✅ | dialogs-all – every editor sheet is in the keyboard test | 1.3 s |  |
| ✅ | dialogs-prepare – sample data for the editors | 1.7 s |  |
| ✅ | dialogs-note – NoteEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 24.3 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-plan – NoteEditor (editPlan) with the keyboard: Save and the cursor stay in view, no jumping | 24.3 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-tasks – TaskEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 26.0 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-plan-finish – PlanFinish (start) with the keyboard: Save and the cursor stay in view, no jumping | 20.8 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-plan-from-note – PlanFinish (startFromNote) with the keyboard: Save and the cursor stay in view, no jumping | 20.8 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-booklet – BookletSheet (show) with the keyboard: Save and the cursor stay in view, no jumping | 24.2 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-piece – PieceEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 26.1 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-student – NewStudent (start) with the keyboard: Save and the cursor stay in view, no jumping | 26.0 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-lesson – LessonEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 26.0 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-activity – ActivityEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 24.3 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-reminder – ReminderEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 24.2 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-event – EventEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 25.5 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-external – ExternalEditor (start) with the keyboard: Save and the cursor stay in view, no jumping | 25.5 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-ready-pieces – ReadyPiecesPicker (start) with the keyboard: Save and the cursor stay in view, no jumping | 20.5 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-library – LibraryItemEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 25.5 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-pairing – PairingSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 20.6 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-day-overview – DayOverviewSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 20.4 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-csv-import – CsvImportSheet (picked) with the keyboard: Save and the cursor stay in view, no jumping | 20.4 s | The window cannot be 844×390 here (it stays 844×560): that size left out.  |
| ✅ | dialogs-cleanup – the editors' sample data goes again | 1.7 s |  |
| ✅ | note-menu – a note: a tap only shows a block, the menu has new note, plan, edit, share, delete | 15.5 s |  |
| ✅ | tasks-menu – tasks of a coupled student: a tap shows the hints, the menu plans, edits and deletes | 13.0 s |  |
| ✅ | idea-demo – The trial of the new idea: a lesson, the history, preparing, pausing; nothing is stored | 46.9 s |  |
| ✅ | idea-point-wrap – trial: a long point wraps in its field while it is written | 9.7 s |  |
| ✅ | idea-fold-arrow – trial: the arrow of a task unfolds it in the window of a lesson | 7.0 s |  |
| ✅ | student-search – search a student, open and close the student page | 6.7 s |  |
| ✅ | student-new – add a student | 5.1 s |  |
| ✅ | event-open – open an event and go back | 4.3 s |  |
| ✅ | metronome – start and stop the metronome | 3.2 s | no sound: Konnte die Soundkarte nicht initialisieren. |
| ✅ | appearance – switch to dark and back to light | 4.6 s |  |
| ✅ | language – switch the language to English and back | 6.6 s |  |
| ✅ | reminders – open the reminders | 4.8 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 7.0 s |  |
| ✅ | plan-save – plan the next lesson of a student | 6.1 s |  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 9.2 s |  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 6.9 s |  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 6.1 s |  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 10.6 s |  |
| ✅ | tasks-long-texts – tasks: a long hint wraps in its field, a long title is wiped sideways and shows its start | 22.9 s |  |
| ✅ | card-long-texts – student page: a long private note and a long block heading wrap instead of being cut | 6.1 s |  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 7.7 s |  |
| ✅ | booklet-questions – practice book: the student's questions on the lesson's card, "Gelesen" takes them away; link copied is said | 13.8 s |  |
| ✅ | booklet-devices – practice book sheet: how many devices have the booklet, where it was removed, the last feedback | 10.5 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 8.1 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 3.5 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 4.7 s |  |
| ✅ | piece-new – add a piece for a student | 5.9 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 6.4 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.9 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 8.2 s |  |
| ✅ | together – two individual lessons of a student kept together | 11.1 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 7.6 s |  |
| ✅ | reminder-new – add a reminder | 4.3 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 10.2 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 19.5 s |  |
| ✅ | loan-return – return sheet music from the student's page | 7.7 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.8 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 4.5 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.7 s |  |

79 of 79 cases passed.

### phone (390x844), restart
- platform: Windows Server 2022 Version 21H2 (windows), Qt 6.8.3
- window: 390 × 749

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.3 s |  |

1 of 1 cases passed.

### wide (1000x660), first
- platform: Windows Server 2022 Version 21H2 (windows), Qt 6.8.3
- window: 1000 × 660

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.4 s |  |
| ✅ | pages – every page opens | 10.2 s |  |
| ✅ | today – today with the running lesson, switching days | 2.9 s |  |
| ✅ | note-save – write and save a note for the running lesson | 4.7 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 3.4 s |  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 8.4 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 12.1 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 7.0 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 5.8 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 13.3 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 8.1 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 12.9 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 10.8 s |  |
| ✅ | text-menu-paste-unasked – finger: Einsetzen is offered without reading the clipboard; empty, it changes nothing | 7.6 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 8.9 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 5.9 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 9.0 s |  |
| ✅ | text-field-wipe – finger: wiping sideways moves a long text in a one-line field; without focus it shows its start | 20.1 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 6.6 s |  |
| ✅ | dialogs-all – every editor sheet is in the keyboard test | 1.3 s |  |
| ✅ | dialogs-prepare – sample data for the editors | 1.7 s |  |
| ✅ | dialogs-note – NoteEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.1 s |  |
| ✅ | dialogs-plan – NoteEditor (editPlan) with the keyboard: Save and the cursor stay in view, no jumping | 9.0 s |  |
| ✅ | dialogs-tasks – TaskEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 10.0 s |  |
| ✅ | dialogs-plan-finish – PlanFinish (start) with the keyboard: Save and the cursor stay in view, no jumping | 7.3 s |  |
| ✅ | dialogs-plan-from-note – PlanFinish (startFromNote) with the keyboard: Save and the cursor stay in view, no jumping | 7.3 s |  |
| ✅ | dialogs-booklet – BookletSheet (show) with the keyboard: Save and the cursor stay in view, no jumping | 9.0 s |  |
| ✅ | dialogs-piece – PieceEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 10.0 s |  |
| ✅ | dialogs-student – NewStudent (start) with the keyboard: Save and the cursor stay in view, no jumping | 9.9 s |  |
| ✅ | dialogs-lesson – LessonEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.9 s |  |
| ✅ | dialogs-activity – ActivityEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.0 s |  |
| ✅ | dialogs-reminder – ReminderEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.1 s |  |
| ✅ | dialogs-event – EventEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 9.9 s |  |
| ✅ | dialogs-external – ExternalEditor (start) with the keyboard: Save and the cursor stay in view, no jumping | 10.0 s |  |
| ✅ | dialogs-ready-pieces – ReadyPiecesPicker (start) with the keyboard: Save and the cursor stay in view, no jumping | 7.4 s |  |
| ✅ | dialogs-library – LibraryItemEditor (edit) with the keyboard: Save and the cursor stay in view, no jumping | 10.0 s |  |
| ✅ | dialogs-pairing – PairingSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 7.5 s |  |
| ✅ | dialogs-day-overview – DayOverviewSheet (start) with the keyboard: Save and the cursor stay in view, no jumping | 7.3 s |  |
| ✅ | dialogs-csv-import – CsvImportSheet (picked) with the keyboard: Save and the cursor stay in view, no jumping | 7.3 s |  |
| ✅ | dialogs-cleanup – the editors' sample data goes again | 2.0 s |  |
| ✅ | note-menu – a note: a tap only shows a block, the menu has new note, plan, edit, share, delete | 15.5 s |  |
| ✅ | tasks-menu – tasks of a coupled student: a tap shows the hints, the menu plans, edits and deletes | 13.1 s |  |
| ✅ | idea-demo – The trial of the new idea: a lesson, the history, preparing, pausing; nothing is stored | 47.3 s |  |
| ✅ | idea-point-wrap – trial: a long point wraps in its field while it is written | 9.8 s |  |
| ✅ | idea-fold-arrow – trial: the arrow of a task unfolds it in the window of a lesson | 7.1 s |  |
| ✅ | student-search – search a student, open and close the student page | 6.8 s |  |
| ✅ | student-new – add a student | 5.1 s |  |
| ✅ | event-open – open an event and go back | 4.4 s |  |
| ✅ | metronome – start and stop the metronome | 3.1 s | no sound: Konnte die Soundkarte nicht initialisieren. |
| ✅ | appearance – switch to dark and back to light | 4.8 s |  |
| ✅ | language – switch the language to English and back | 6.5 s |  |
| ✅ | reminders – open the reminders | 4.8 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 7.1 s |  |
| ✅ | plan-save – plan the next lesson of a student | 6.2 s |  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 9.3 s |  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 6.9 s |  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 6.2 s |  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 10.7 s |  |
| ✅ | tasks-long-texts – tasks: a long hint wraps in its field, a long title is wiped sideways and shows its start | 22.9 s |  |
| ✅ | card-long-texts – student page: a long private note and a long block heading wrap instead of being cut | 6.2 s |  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 7.7 s |  |
| ✅ | booklet-questions – practice book: the student's questions on the lesson's card, "Gelesen" takes them away; link copied is said | 13.9 s |  |
| ✅ | booklet-devices – practice book sheet: how many devices have the booklet, where it was removed, the last feedback | 10.7 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 8.1 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 3.5 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 4.7 s |  |
| ✅ | piece-new – add a piece for a student | 5.9 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 6.4 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.7 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 8.3 s |  |
| ✅ | together – two individual lessons of a student kept together | 11.2 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 7.6 s |  |
| ✅ | reminder-new – add a reminder | 4.3 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 10.2 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 19.7 s |  |
| ✅ | loan-return – return sheet music from the student's page | 7.6 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.9 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 4.7 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.7 s |  |

79 of 79 cases passed.

### wide (1000x660), restart
- platform: Windows Server 2022 Version 21H2 (windows), Qt 6.8.3
- window: 1000 × 660

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.3 s |  |

1 of 1 cases passed.


## Screens changed since the run before

- unchanged: 1037, changed: 31, new: 172

- `desktop/linux/phone/first/case-more-pages.png` – 0.5 % (`visual-diff/desktop_linux_phone_first_case-more-pages.png`)
- `desktop/linux/phone/first/date-calendar.png` – 0.7 % (`visual-diff/desktop_linux_phone_first_date-calendar.png`)
- `desktop/linux/phone/first/dialog-note-keyboard-844x390.png` – 1.8 % (`visual-diff/desktop_linux_phone_first_dialog-note-keyboard-844x390.png`)
- `desktop/linux/wide/first/date-calendar.png` – 0.4 % (`visual-diff/desktop_linux_wide_first_date-calendar.png`)
- `desktop/macos/phone/first/case-idea-demo.png` – 0.4 % (`visual-diff/desktop_macos_phone_first_case-idea-demo.png`)
- `desktop/macos/phone/first/case-idea-fold-arrow.png` – 0.4 % (`visual-diff/desktop_macos_phone_first_case-idea-fold-arrow.png`)
- `desktop/macos/phone/first/case-idea-point-wrap.png` – 0.4 % (`visual-diff/desktop_macos_phone_first_case-idea-point-wrap.png`)
- `desktop/macos/phone/first/case-more-pages.png` – 0.8 % (`visual-diff/desktop_macos_phone_first_case-more-pages.png`)
- `desktop/macos/phone/first/case-note-share.png` – 2.9 % (`visual-diff/desktop_macos_phone_first_case-note-share.png`)
- `desktop/macos/phone/first/case-plan-finish-lines.png` – 8.1 % (`visual-diff/desktop_macos_phone_first_case-plan-finish-lines.png`)
- `desktop/macos/phone/first/case-reminders.png` – 0.3 % (`visual-diff/desktop_macos_phone_first_case-reminders.png`)
- `desktop/macos/phone/first/case-test-mode.png` – 0.3 % (`visual-diff/desktop_macos_phone_first_case-test-mode.png`)
- `desktop/macos/phone/first/date-calendar.png` – 0.9 % (`visual-diff/desktop_macos_phone_first_date-calendar.png`)
- `desktop/macos/phone/first/dialog-plan-keyboard-844x390.png` – 1.9 % (`visual-diff/desktop_macos_phone_first_dialog-plan-keyboard-844x390.png`)
- `desktop/macos/phone/first/page-Mehr.png` – 0.3 % (`visual-diff/desktop_macos_phone_first_page-Mehr.png`)
- `desktop/macos/phone/first/test-mode.png` – 0.3 % (`visual-diff/desktop_macos_phone_first_test-mode.png`)
- `desktop/macos/wide/first/case-card-long-texts.png` – 1.9 % (`visual-diff/desktop_macos_wide_first_case-card-long-texts.png`)
- `desktop/macos/wide/first/case-more-pages.png` – 0.3 % (`visual-diff/desktop_macos_wide_first_case-more-pages.png`)
- `desktop/macos/wide/first/case-note-share.png` – 2.7 % (`visual-diff/desktop_macos_wide_first_case-note-share.png`)
- `desktop/macos/wide/first/case-plan-finish-lines.png` – 1.4 % (`visual-diff/desktop_macos_wide_first_case-plan-finish-lines.png`)
- `desktop/macos/wide/first/case-plan-from-note-lines.png` – 1.7 % (`visual-diff/desktop_macos_wide_first_case-plan-from-note-lines.png`)
- `desktop/macos/wide/first/date-calendar.png` – 0.4 % (`visual-diff/desktop_macos_wide_first_date-calendar.png`)
- `desktop/windows/phone/first/case-card-long-texts.png` – 6.8 % (`visual-diff/desktop_windows_phone_first_case-card-long-texts.png`)
- `desktop/windows/phone/first/case-idea-demo.png` – 0.3 % (`visual-diff/desktop_windows_phone_first_case-idea-demo.png`)
- `desktop/windows/phone/first/case-idea-fold-arrow.png` – 0.3 % (`visual-diff/desktop_windows_phone_first_case-idea-fold-arrow.png`)
- `desktop/windows/phone/first/case-idea-point-wrap.png` – 0.3 % (`visual-diff/desktop_windows_phone_first_case-idea-point-wrap.png`)
- `desktop/windows/phone/first/case-more-pages.png` – 0.5 % (`visual-diff/desktop_windows_phone_first_case-more-pages.png`)
- `desktop/windows/phone/first/case-text-field-wipe.png` – 3.6 % (`visual-diff/desktop_windows_phone_first_case-text-field-wipe.png`)
- `desktop/windows/phone/first/date-calendar.png` – 0.8 % (`visual-diff/desktop_windows_phone_first_date-calendar.png`)
- `desktop/windows/wide/first/case-note-share.png` – 5.2 % (`visual-diff/desktop_windows_wide_first_case-note-share.png`)
- `desktop/windows/wide/first/date-calendar.png` – 0.4 % (`visual-diff/desktop_windows_wide_first_date-calendar.png`)
