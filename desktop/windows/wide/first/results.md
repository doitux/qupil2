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
| ✅ | student-search – search a student, open and close the student page | 5.8 s |  |
| ✅ | student-new – add a student | 5.0 s |  |
| ✅ | event-open – open an event and go back | 4.3 s |  |
| ✅ | metronome – start and stop the metronome | 2.8 s | no sound: Konnte die Soundkarte nicht initialisieren. |
| ✅ | appearance – switch to dark and back to light | 4.4 s |  |
| ✅ | language – switch the language to English and back | 6.2 s |  |
| ✅ | reminders – open the reminders | 4.7 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 6.9 s |  |
| ✅ | plan-save – plan the next lesson of a student | 6.0 s |  |
| ✅ | piece-new – add a piece for a student | 5.9 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.8 s |  |
| ✅ | together – two individual lessons of a student kept together | 10.5 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 7.2 s |  |
| ✅ | reminder-new – add a reminder | 4.1 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.9 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.6 s |  |

21 of 21 cases passed.
