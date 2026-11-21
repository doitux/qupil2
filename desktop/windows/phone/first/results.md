# Qupil UI test

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
