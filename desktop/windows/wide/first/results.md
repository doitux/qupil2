# Qupil UI test

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
