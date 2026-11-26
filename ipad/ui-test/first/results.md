# Qupil UI test

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
