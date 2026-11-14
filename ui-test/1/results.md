# Qupil UI test

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.6 s |  |
| ✅ | pages – every page opens | 7.3 s |  |
| ✅ | today – today with the running lesson, switching days | 2.5 s |  |
| ✅ | note-save – write and save a note for the running lesson | 6.6 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 6.2 s |  |
| ✅ | keyboard-smooth – the keyboard comes in an open editor: the editor makes room smoothly | 15.8 s | No on-screen keyboard came: not measured.  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 9.7 s |  |
| ✅ | student-search – search a student, open and close the student page | 7.9 s |  |
| ✅ | student-new – add a student | 10.0 s |  |

9 of 9 cases passed.
