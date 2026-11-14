# Qupil UI test

- platform: iOS 26.2 (ios), Qt 6.8.3
- window: 1024 × 1309

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 11.0 s |  |
| ✅ | pages – every page opens | 13.9 s |  |
| ✅ | today – today with the running lesson, switching days | 3.3 s |  |
| ✅ | note-save – write and save a note for the running lesson | 7.8 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 6.5 s |  |
| ✅ | keyboard-smooth – the keyboard comes in an open editor: the editor makes room smoothly | 16.5 s | No on-screen keyboard came: not measured.  |
| ✅ | student-search – search a student, open and close the student page | 10.6 s | 2 taps needed for "^Lena Muster$".  |
| ✅ | student-new – add a student | 10.9 s |  |
| ✅ | event-open – open an event and go back | 3.4 s |  |
| ✅ | metronome – start and stop the metronome | 9.6 s |  |
| ✅ | appearance – switch to dark and back to light | 5.0 s |  |
| ✅ | language – switch the language to English and back | 5.9 s |  |
| ✅ | reminders – open the reminders | 3.4 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 6.9 s |  |
| ✅ | plan-save – plan the next lesson of a student | 14.3 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | piece-new – add a piece for a student | 17.9 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | lesson-new – add a lesson from the schedule | 4.5 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 8.2 s |  |
| ✅ | together – two individual lessons of a student kept together | 18.2 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | together-new – new lesson for a student who has one: kept together | 14.3 s | 2 taps needed for "combobox:^Wochentag$". 2 taps needed for "^Muster, Lena".  |
| ✅ | reminder-new – add a reminder | 7.4 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 35.1 s | 2 taps needed for "^Unterricht$". 2 taps needed for "^Streicherensemble · Di 17:00$". 2 taps needed for "^Lena\ Muster$".  |
| ✅ | loan-return – return sheet music from the student's page | 12.0 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | more-pages – birthdays and data pages open | 2.7 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 3.1 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 5.4 s |  |

26 of 26 cases passed.
