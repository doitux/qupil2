# Qupil UI test

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 4.7 s |  |
| ✅ | note-menu – a note: a tap only shows a block, the menu has new note, plan, edit, share, delete | 17.5 s |  |
| ✅ | tasks-menu – tasks of a coupled student: a tap shows the hints, the menu plans, edits and deletes | 14.1 s |  |
| ✅ | idea-demo – The trial of the new idea: a lesson, the history, preparing, pausing; nothing is stored | 64.4 s |  |
| ✅ | idea-point-wrap – trial: a long point wraps in its field while it is written | 11.8 s |  |
| ✅ | idea-fold-arrow – trial: the arrow of a task unfolds it in the window of a lesson | 6.5 s |  |
| ✅ | plan-save – plan the next lesson of a student | 10.8 s |  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 11.0 s |  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 8.7 s |  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 8.1 s |  |
| ✅ | tasks-long-texts – tasks: a long hint wraps in its field, a long title is wiped sideways and shows its start | 31.0 s |  |
| ✅ | card-long-texts – student page: a long private note and a long block heading wrap instead of being cut | 7.7 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 3.4 s |  |
| ✅ | piece-new – add a piece for a student | 14.7 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 3.6 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 6.4 s |  |
| ✅ | together – two individual lessons of a student kept together | 11.3 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 9.1 s |  |
| ✅ | reminder-new – add a reminder | 7.0 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 9.9 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 26.2 s |  |
| ✅ | loan-return – return sheet music from the student's page | 8.8 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.1 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 2.8 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 5.2 s |  |

25 of 25 cases passed.
