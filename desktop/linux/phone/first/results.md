# Qupil UI test

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
