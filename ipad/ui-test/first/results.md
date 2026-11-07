# Qupil UI test

- platform: iOS 26.2 (ios), Qt 6.8.3
- window: 1024 × 1309

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.8 s |  |
| ✅ | pages – every page opens | 9.0 s |  |
| ✅ | today – today with the running lesson, switching days | 2.9 s |  |
| ✅ | note-save – write and save a note for the running lesson | 8.0 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 6.3 s |  |
| ✅ | student-search – search a student, open and close the student page | 10.1 s | 2 taps needed for "^Lena Muster$".  |
| ✅ | student-new – add a student | 10.6 s |  |
| ✅ | event-open – open an event and go back | 3.1 s |  |
| ✅ | metronome – start and stop the metronome | 6.3 s |  |
| ✅ | appearance – switch to dark and back to light | 4.0 s |  |
| ✅ | language – switch the language to English and back | 5.3 s |  |
| ✅ | reminders – open the reminders | 3.4 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 6.6 s |  |
| ✅ | plan-save – plan the next lesson of a student | 13.7 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | piece-new – add a piece for a student | 17.6 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | lesson-new – add a lesson from the schedule | 4.6 s |  |
| ✅ | together – two individual lessons of a student kept together | 16.1 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | together-new – new lesson for a student who has one: kept together | 12.2 s | 2 taps needed for "^Muster, Lena".  |
| ✅ | reminder-new – add a reminder | 7.3 s |  |
| ✅ | more-pages – birthdays and data pages open | 2.9 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 5.5 s |  |

21 of 21 cases passed.
