# Qupil UI test

- platform: macOS Sequoia (15.8) (cocoa), Qt 6.8.3
- window: 390 × 653

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.5 s |  |
| ✅ | pages – every page opens | 7.5 s |  |
| ✅ | today – today with the running lesson, switching days | 2.5 s |  |
| ✅ | note-save – write and save a note for the running lesson | 3.8 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 3.1 s |  |
| ✅ | student-search – search a student, open and close the student page | 4.1 s |  |
| ✅ | student-new – add a student | 4.0 s |  |
| ✅ | event-open – open an event and go back | 2.5 s |  |
| ✅ | metronome – start and stop the metronome | 2.9 s |  |
| ✅ | appearance – switch to dark and back to light | 3.6 s |  |
| ✅ | language – switch the language to English and back | 5.0 s |  |
| ✅ | reminders – open the reminders | 2.9 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 7.0 s |  |
| ✅ | plan-save – plan the next lesson of a student | 5.2 s |  |
| ✅ | piece-new – add a piece for a student | 6.6 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.3 s |  |
| ✅ | together – two individual lessons of a student kept together | 9.3 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 6.9 s |  |
| ✅ | reminder-new – add a reminder | 4.2 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.9 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.0 s |  |

21 of 21 cases passed.
