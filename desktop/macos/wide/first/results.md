# Qupil UI test

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
