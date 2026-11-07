# Qupil UI test

- platform: macOS Sequoia (15.8) (cocoa), Qt 6.8.3
- window: 1000 × 656

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.6 s |  |
| ✅ | pages – every page opens | 7.9 s |  |
| ✅ | today – today with the running lesson, switching days | 2.8 s |  |
| ✅ | note-save – write and save a note for the running lesson | 3.7 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 3.4 s |  |
| ✅ | student-search – search a student, open and close the student page | 4.1 s |  |
| ✅ | student-new – add a student | 4.4 s |  |
| ✅ | event-open – open an event and go back | 2.6 s |  |
| ✅ | metronome – start and stop the metronome | 2.9 s |  |
| ✅ | appearance – switch to dark and back to light | 3.8 s |  |
| ✅ | language – switch the language to English and back | 5.5 s |  |
| ✅ | reminders – open the reminders | 3.5 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 6.7 s |  |
| ✅ | plan-save – plan the next lesson of a student | 5.3 s |  |
| ✅ | piece-new – add a piece for a student | 6.5 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.0 s |  |
| ✅ | together – two individual lessons of a student kept together | 9.3 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 6.2 s |  |
| ✅ | reminder-new – add a reminder | 4.5 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.7 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 1.9 s |  |

21 of 21 cases passed.
