# Qupil UI test

- platform: iOS 26.2 (ios), Qt 6.8.3
- window: 393 × 759

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.7 s |  |
| ✅ | pages – every page opens | 8.0 s |  |
| ✅ | today – today with the running lesson, switching days | 2.9 s |  |
| ✅ | note-save – write and save a note for the running lesson | 7.4 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 6.1 s |  |
| ✅ | keyboard-smooth – the keyboard comes in an open editor: the editor makes room smoothly | 16.2 s | No on-screen keyboard came: not measured.  |
| ✅ | student-search – search a student, open and close the student page | 9.9 s | 2 taps needed for "^Lena Muster$".  |
| ✅ | student-new – add a student | 10.4 s |  |
| ✅ | event-open – open an event and go back | 2.7 s |  |
| ✅ | metronome – start and stop the metronome | 10.5 s |  |
| ✅ | appearance – switch to dark and back to light | 3.9 s |  |
| ✅ | language – switch the language to English and back | 5.3 s |  |
| ✅ | reminders – open the reminders | 2.8 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 6.4 s |  |
| ✅ | plan-save – plan the next lesson of a student | 13.5 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 14.2 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 11.8 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 10.8 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 27.2 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 8.0 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 10.6 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 3.8 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 4.4 s |  |
| ✅ | piece-new – add a piece for a student | 17.5 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | lesson-new – add a lesson from the schedule | 4.6 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 7.4 s |  |
| ✅ | together – two individual lessons of a student kept together | 15.6 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | together-new – new lesson for a student who has one: kept together | 13.4 s | 2 taps needed for "combobox:^Wochentag$". 2 taps needed for "^Muster, Lena".  |
| ✅ | reminder-new – add a reminder | 7.0 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 11.0 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 33.2 s | 2 taps needed for "^Unterricht$". 2 taps needed for "^Streicherensemble · Di 17:00$". 2 taps needed for "^Lena\ Muster$".  |
| ✅ | loan-return – return sheet music from the student's page | 11.7 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | more-pages – birthdays and data pages open | 3.0 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 2.7 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 5.3 s |  |

35 of 35 cases passed.
