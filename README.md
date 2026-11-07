# Desktop UI test run 3 (08413b7)

## linux

### phone (390x844), first
- platform: Ubuntu 24.04.5 LTS (xcb), Qt 6.8.3
- window: 390 × 844

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.4 s |  |
| ✅ | pages – every page opens | 6.8 s |  |
| ✅ | today – today with the running lesson, switching days | 2.3 s |  |
| ✅ | note-save – write and save a note for the running lesson | 3.2 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 2.9 s |  |
| ✅ | student-search – search a student, open and close the student page | 3.2 s |  |
| ✅ | student-new – add a student | 3.5 s |  |
| ✅ | event-open – open an event and go back | 2.1 s |  |
| ✅ | metronome – start and stop the metronome | 2.3 s | no sound: Konnte die Soundkarte nicht initialisieren. |
| ✅ | appearance – switch to dark and back to light | 2.8 s |  |
| ✅ | language – switch the language to English and back | 4.1 s |  |
| ✅ | reminders – open the reminders | 2.3 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 4.9 s |  |
| ✅ | plan-save – plan the next lesson of a student | 4.1 s |  |
| ✅ | piece-new – add a piece for a student | 5.0 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 3.1 s |  |
| ✅ | together – two individual lessons of a student kept together | 7.5 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 5.5 s |  |
| ✅ | reminder-new – add a reminder | 3.4 s |  |
| ✅ | more-pages – birthdays and data pages open | 2.3 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 1.6 s |  |

21 of 21 cases passed.

### phone (390x844), restart
- platform: Ubuntu 24.04.5 LTS (xcb), Qt 6.8.3
- window: 390 × 844

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 2.7 s |  |

1 of 1 cases passed.

### wide (1000x660), first
- platform: Ubuntu 24.04.5 LTS (xcb), Qt 6.8.3
- window: 1000 × 660

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.4 s |  |
| ✅ | pages – every page opens | 6.8 s |  |
| ✅ | today – today with the running lesson, switching days | 2.3 s |  |
| ✅ | note-save – write and save a note for the running lesson | 3.2 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 2.9 s |  |
| ✅ | student-search – search a student, open and close the student page | 3.3 s |  |
| ✅ | student-new – add a student | 3.5 s |  |
| ✅ | event-open – open an event and go back | 2.1 s |  |
| ✅ | metronome – start and stop the metronome | 2.3 s | no sound: Konnte die Soundkarte nicht initialisieren. |
| ✅ | appearance – switch to dark and back to light | 2.8 s |  |
| ✅ | language – switch the language to English and back | 4.0 s |  |
| ✅ | reminders – open the reminders | 2.3 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 5.0 s |  |
| ✅ | plan-save – plan the next lesson of a student | 4.1 s |  |
| ✅ | piece-new – add a piece for a student | 5.0 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 3.2 s |  |
| ✅ | together – two individual lessons of a student kept together | 7.5 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 5.5 s |  |
| ✅ | reminder-new – add a reminder | 3.4 s |  |
| ✅ | more-pages – birthdays and data pages open | 2.4 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 1.6 s |  |

21 of 21 cases passed.

### wide (1000x660), restart
- platform: Ubuntu 24.04.5 LTS (xcb), Qt 6.8.3
- window: 1000 × 660

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 2.7 s |  |

1 of 1 cases passed.


## macos

### phone (390x844), first
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

### phone (390x844), restart
- platform: macOS Sequoia (15.8) (cocoa), Qt 6.8.3
- window: 390 × 656

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 2.9 s |  |

1 of 1 cases passed.

### wide (1000x660), first
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

### wide (1000x660), restart
- platform: macOS Sequoia (15.8) (cocoa), Qt 6.8.3
- window: 1000 × 656

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 2.8 s |  |

1 of 1 cases passed.


## windows

### phone (390x844), first
- platform: Windows Server 2022 Version 21H2 (windows), Qt 6.8.3
- window: 390 × 749

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.3 s |  |
| ✅ | pages – every page opens | 10.1 s |  |
| ✅ | today – today with the running lesson, switching days | 2.9 s |  |
| ✅ | note-save – write and save a note for the running lesson | 4.6 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 3.4 s |  |
| ✅ | student-search – search a student, open and close the student page | 5.9 s |  |
| ✅ | student-new – add a student | 5.0 s |  |
| ✅ | event-open – open an event and go back | 4.3 s |  |
| ✅ | metronome – start and stop the metronome | 3.0 s | no sound: Konnte die Soundkarte nicht initialisieren. |
| ✅ | appearance – switch to dark and back to light | 4.4 s |  |
| ✅ | language – switch the language to English and back | 6.3 s |  |
| ✅ | reminders – open the reminders | 4.7 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 7.0 s |  |
| ✅ | plan-save – plan the next lesson of a student | 5.9 s |  |
| ✅ | piece-new – add a piece for a student | 5.9 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.9 s |  |
| ✅ | together – two individual lessons of a student kept together | 10.5 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 7.2 s |  |
| ✅ | reminder-new – add a reminder | 4.2 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.7 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.6 s |  |

21 of 21 cases passed.

### phone (390x844), restart
- platform: Windows Server 2022 Version 21H2 (windows), Qt 6.8.3
- window: 390 × 749

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 2.6 s |  |

1 of 1 cases passed.

### wide (1000x660), first
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

### wide (1000x660), restart
- platform: Windows Server 2022 Version 21H2 (windows), Qt 6.8.3
- window: 1000 × 660

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 2.6 s |  |

1 of 1 cases passed.


## Screens changed since the run before

- unchanged: 190, changed: 20, new: 42

- `desktop/linux/phone/first/case-more-pages.png` – 0.5 % (`visual-diff/desktop_linux_phone_first_case-more-pages.png`)
- `desktop/linux/phone/first/case-student-search.png` – 0.7 % (`visual-diff/desktop_linux_phone_first_case-student-search.png`)
- `desktop/linux/phone/first/page-Schüler.png` – 4.1 % (`visual-diff/desktop_linux_phone_first_page-Schüler.png`)
- `desktop/linux/phone/first/student-search.png` – 0.7 % (`visual-diff/desktop_linux_phone_first_student-search.png`)
- `desktop/linux/wide/first/case-student-search.png` – 0.4 % (`visual-diff/desktop_linux_wide_first_case-student-search.png`)
- `desktop/linux/wide/first/page-Schüler.png` – 1.6 % (`visual-diff/desktop_linux_wide_first_page-Schüler.png`)
- `desktop/linux/wide/first/student-search.png` – 0.4 % (`visual-diff/desktop_linux_wide_first_student-search.png`)
- `desktop/macos/phone/first/case-more-pages.png` – 0.6 % (`visual-diff/desktop_macos_phone_first_case-more-pages.png`)
- `desktop/macos/phone/first/case-reminders.png` – 0.3 % (`visual-diff/desktop_macos_phone_first_case-reminders.png`)
- `desktop/macos/phone/first/case-student-search.png` – 0.9 % (`visual-diff/desktop_macos_phone_first_case-student-search.png`)
- `desktop/macos/phone/first/page-Mehr.png` – 0.3 % (`visual-diff/desktop_macos_phone_first_page-Mehr.png`)
- `desktop/macos/phone/first/page-Schüler.png` – 3.5 % (`visual-diff/desktop_macos_phone_first_page-Schüler.png`)
- `desktop/macos/phone/first/student-search.png` – 0.9 % (`visual-diff/desktop_macos_phone_first_student-search.png`)
- `desktop/macos/wide/first/case-more-pages.png` – 0.7 % (`visual-diff/desktop_macos_wide_first_case-more-pages.png`)
- `desktop/macos/wide/first/case-student-search.png` – 0.5 % (`visual-diff/desktop_macos_wide_first_case-student-search.png`)
- `desktop/macos/wide/first/page-Schüler.png` – 1.7 % (`visual-diff/desktop_macos_wide_first_page-Schüler.png`)
- `desktop/macos/wide/first/student-search.png` – 0.5 % (`visual-diff/desktop_macos_wide_first_student-search.png`)
- `desktop/windows/phone/first/case-more-pages.png` – 0.4 % (`visual-diff/desktop_windows_phone_first_case-more-pages.png`)
- `desktop/windows/phone/first/student-page.png` – 22.1 % (`visual-diff/desktop_windows_phone_first_student-page.png`)
- `desktop/windows/wide/first/student-page.png` – 12.7 % (`visual-diff/desktop_windows_wide_first_student-page.png`)
