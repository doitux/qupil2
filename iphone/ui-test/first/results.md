# Qupil UI test

- platform: iOS 26.2 (ios), Qt 6.8.3
- window: 393 × 759

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.8 s |  |
| ✅ | pages – every page opens | 8.1 s |  |
| ✅ | today – today with the running lesson, switching days | 3.0 s |  |
| ✅ | note-save – write and save a note for the running lesson | 7.3 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 6.1 s |  |
| ✅ | student-search – search a student, open and close the student page | 9.3 s | 2 taps needed for "^Lena Muster$".  |
| ✅ | student-new – add a student | 10.3 s |  |
| ✅ | event-open – open an event and go back | 2.5 s |  |
| ✅ | metronome – start and stop the metronome | 7.4 s |  |
| ✅ | appearance – switch to dark and back to light | 3.6 s |  |
| ✅ | language – switch the language to English and back | 5.2 s |  |
| ✅ | reminders – open the reminders | 2.9 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 6.5 s |  |
| ✅ | plan-save – plan the next lesson of a student | 13.2 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | piece-new – add a piece for a student | 17.1 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | lesson-new – add a lesson from the schedule | 4.3 s |  |
| ✅ | together – two individual lessons of a student kept together | 15.5 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | together-new – new lesson for a student who has one: kept together | 11.5 s | 2 taps needed for "^Muster, Lena".  |
| ✅ | reminder-new – add a reminder | 6.8 s |  |
| ✅ | more-pages – birthdays and data pages open | 2.6 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 4.9 s |  |

21 of 21 cases passed.
