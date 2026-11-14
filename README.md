# Desktop UI test run 38 (d30ff1f)

## linux

### phone (390x844), first
- platform: Ubuntu 24.04.5 LTS (xcb), Qt 6.8.3
- window: 390 × 844

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.4 s |  |
| ✅ | pages – every page opens | 6.9 s |  |
| ✅ | today – today with the running lesson, switching days | 2.4 s |  |
| ✅ | note-save – write and save a note for the running lesson | 3.2 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 2.9 s |  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 7.0 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 11.0 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 5.5 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 5.2 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 12.6 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 6.6 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 12.2 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 10.2 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 6.7 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 5.3 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 8.2 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 6.1 s |  |
| ✅ | student-search – search a student, open and close the student page | 3.5 s |  |
| ✅ | student-new – add a student | 3.5 s |  |
| ✅ | event-open – open an event and go back | 2.1 s |  |
| ✅ | metronome – start and stop the metronome | 2.5 s | no sound: Konnte die Soundkarte nicht initialisieren. |
| ✅ | appearance – switch to dark and back to light | 2.9 s |  |
| ✅ | language – switch the language to English and back | 4.1 s |  |
| ✅ | reminders – open the reminders | 2.3 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 4.9 s |  |
| ✅ | plan-save – plan the next lesson of a student | 4.1 s |  |
| ✅ | piece-new – add a piece for a student | 5.0 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 4.3 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 3.2 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 6.0 s |  |
| ✅ | together – two individual lessons of a student kept together | 7.6 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 5.7 s |  |
| ✅ | reminder-new – add a reminder | 3.5 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 12.7 s |  |
| ✅ | loan-return – return sheet music from the student's page | 5.2 s |  |
| ✅ | more-pages – birthdays and data pages open | 2.3 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 2.3 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.0 s |  |

38 of 38 cases passed.

### phone (390x844), restart
- platform: Ubuntu 24.04.5 LTS (xcb), Qt 6.8.3
- window: 390 × 844

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.5 s |  |

1 of 1 cases passed.

### wide (1000x660), first
- platform: Ubuntu 24.04.5 LTS (xcb), Qt 6.8.3
- window: 1000 × 660

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.4 s |  |
| ✅ | pages – every page opens | 6.9 s |  |
| ✅ | today – today with the running lesson, switching days | 2.4 s |  |
| ✅ | note-save – write and save a note for the running lesson | 3.2 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 2.9 s |  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 7.0 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 11.0 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 5.5 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 5.2 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 12.6 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 6.6 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 12.2 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 10.2 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 6.8 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 5.3 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 8.2 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 6.1 s |  |
| ✅ | student-search – search a student, open and close the student page | 3.6 s |  |
| ✅ | student-new – add a student | 3.5 s |  |
| ✅ | event-open – open an event and go back | 2.1 s |  |
| ✅ | metronome – start and stop the metronome | 2.5 s | no sound: Konnte die Soundkarte nicht initialisieren. |
| ✅ | appearance – switch to dark and back to light | 2.9 s |  |
| ✅ | language – switch the language to English and back | 4.0 s |  |
| ✅ | reminders – open the reminders | 2.3 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 5.0 s |  |
| ✅ | plan-save – plan the next lesson of a student | 4.2 s |  |
| ✅ | piece-new – add a piece for a student | 5.0 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 4.3 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 3.2 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 6.0 s |  |
| ✅ | together – two individual lessons of a student kept together | 7.7 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 5.7 s |  |
| ✅ | reminder-new – add a reminder | 3.5 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 12.9 s |  |
| ✅ | loan-return – return sheet music from the student's page | 5.1 s |  |
| ✅ | more-pages – birthdays and data pages open | 2.4 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 2.3 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.0 s |  |

38 of 38 cases passed.

### wide (1000x660), restart
- platform: Ubuntu 24.04.5 LTS (xcb), Qt 6.8.3
- window: 1000 × 660

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.5 s |  |

1 of 1 cases passed.

- screenshot tour: 168 pictures

## macos

### phone (390x844), first
- platform: macOS Sequoia (15.8) (cocoa), Qt 6.8.3
- window: 390 × 653

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.5 s |  |
| ✅ | pages – every page opens | 7.5 s |  |
| ✅ | today – today with the running lesson, switching days | 2.6 s |  |
| ✅ | note-save – write and save a note for the running lesson | 3.5 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 3.1 s |  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 7.6 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 12.0 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 5.9 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 5.6 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 14.0 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 8.6 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 14.1 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 11.9 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 7.9 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 6.0 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 9.7 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 6.7 s |  |
| ✅ | student-search – search a student, open and close the student page | 4.8 s |  |
| ✅ | student-new – add a student | 4.2 s |  |
| ✅ | event-open – open an event and go back | 2.6 s |  |
| ✅ | metronome – start and stop the metronome | 4.2 s |  |
| ✅ | appearance – switch to dark and back to light | 4.1 s |  |
| ✅ | language – switch the language to English and back | 5.9 s |  |
| ✅ | reminders – open the reminders | 3.4 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 6.3 s |  |
| ✅ | plan-save – plan the next lesson of a student | 5.1 s |  |
| ✅ | piece-new – add a piece for a student | 6.1 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 6.0 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 3.7 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 7.2 s |  |
| ✅ | together – two individual lessons of a student kept together | 9.4 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 7.1 s |  |
| ✅ | reminder-new – add a reminder | 4.5 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 16.3 s |  |
| ✅ | loan-return – return sheet music from the student's page | 6.1 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.3 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 3.0 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.4 s |  |

38 of 38 cases passed.

### phone (390x844), restart
- platform: macOS Sequoia (15.8) (cocoa), Qt 6.8.3
- window: 390 × 656

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.8 s |  |

1 of 1 cases passed.

### wide (1000x660), first
- platform: macOS Sequoia (15.8) (cocoa), Qt 6.8.3
- window: 1000 × 656

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.7 s |  |
| ✅ | pages – every page opens | 8.5 s |  |
| ✅ | today – today with the running lesson, switching days | 3.0 s |  |
| ✅ | note-save – write and save a note for the running lesson | 3.7 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 3.8 s |  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 8.1 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 13.0 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 6.4 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 6.4 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 14.8 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 8.0 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 14.6 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 12.1 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 7.9 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 6.4 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 9.9 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 7.4 s |  |
| ✅ | student-search – search a student, open and close the student page | 5.0 s |  |
| ✅ | student-new – add a student | 4.8 s |  |
| ✅ | event-open – open an event and go back | 2.9 s |  |
| ✅ | metronome – start and stop the metronome | 4.1 s |  |
| ✅ | appearance – switch to dark and back to light | 3.7 s |  |
| ✅ | language – switch the language to English and back | 5.9 s |  |
| ✅ | reminders – open the reminders | 3.4 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 6.5 s |  |
| ✅ | plan-save – plan the next lesson of a student | 5.8 s |  |
| ✅ | piece-new – add a piece for a student | 6.7 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 6.2 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.1 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 8.2 s |  |
| ✅ | together – two individual lessons of a student kept together | 10.4 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 7.4 s |  |
| ✅ | reminder-new – add a reminder | 5.0 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 17.5 s |  |
| ✅ | loan-return – return sheet music from the student's page | 6.8 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.9 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 3.2 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.4 s |  |

38 of 38 cases passed.

### wide (1000x660), restart
- platform: macOS Sequoia (15.8) (cocoa), Qt 6.8.3
- window: 1000 × 656

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 4.0 s |  |

1 of 1 cases passed.


## windows

### phone (390x844), first
- platform: Windows Server 2022 Version 21H2 (windows), Qt 6.8.3
- window: 390 × 749

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.4 s |  |
| ✅ | pages – every page opens | 10.1 s |  |
| ✅ | today – today with the running lesson, switching days | 2.9 s |  |
| ✅ | note-save – write and save a note for the running lesson | 4.7 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 3.4 s |  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 8.4 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 12.1 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 7.0 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 5.7 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 13.2 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 8.1 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 12.9 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 10.8 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 8.9 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 5.9 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 8.9 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 6.6 s |  |
| ✅ | student-search – search a student, open and close the student page | 6.2 s |  |
| ✅ | student-new – add a student | 5.0 s |  |
| ✅ | event-open – open an event and go back | 4.3 s |  |
| ✅ | metronome – start and stop the metronome | 3.2 s | no sound: Konnte die Soundkarte nicht initialisieren. |
| ✅ | appearance – switch to dark and back to light | 4.4 s |  |
| ✅ | language – switch the language to English and back | 6.3 s |  |
| ✅ | reminders – open the reminders | 4.7 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 6.9 s |  |
| ✅ | plan-save – plan the next lesson of a student | 6.0 s |  |
| ✅ | piece-new – add a piece for a student | 6.0 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 6.3 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.7 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 8.1 s |  |
| ✅ | together – two individual lessons of a student kept together | 10.7 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 7.5 s |  |
| ✅ | reminder-new – add a reminder | 4.2 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 19.3 s |  |
| ✅ | loan-return – return sheet music from the student's page | 7.5 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.8 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 4.5 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.7 s |  |

38 of 38 cases passed.

### phone (390x844), restart
- platform: Windows Server 2022 Version 21H2 (windows), Qt 6.8.3
- window: 390 × 749

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.4 s |  |

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
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 8.4 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 12.1 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 7.0 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 5.8 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 13.3 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 8.1 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 12.9 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 10.8 s |  |
| ✅ | text-menu-narrow – finger: the menu fits the window, wiping the bar shows the rest | 8.9 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 5.9 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 9.0 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 6.6 s |  |
| ✅ | student-search – search a student, open and close the student page | 6.2 s |  |
| ✅ | student-new – add a student | 5.0 s |  |
| ✅ | event-open – open an event and go back | 4.3 s |  |
| ✅ | metronome – start and stop the metronome | 3.1 s | no sound: Konnte die Soundkarte nicht initialisieren. |
| ✅ | appearance – switch to dark and back to light | 4.5 s |  |
| ✅ | language – switch the language to English and back | 6.3 s |  |
| ✅ | reminders – open the reminders | 4.8 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 6.9 s |  |
| ✅ | plan-save – plan the next lesson of a student | 6.0 s |  |
| ✅ | piece-new – add a piece for a student | 6.0 s |  |
| ✅ | date-calendar – the calendar of a date field is on the screen as a whole | 6.3 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.6 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 8.1 s |  |
| ✅ | together – two individual lessons of a student kept together | 10.8 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 7.4 s |  |
| ✅ | reminder-new – add a reminder | 4.3 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 19.4 s |  |
| ✅ | loan-return – return sheet music from the student's page | 7.4 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.9 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 4.5 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 2.7 s |  |

38 of 38 cases passed.

### wide (1000x660), restart
- platform: Windows Server 2022 Version 21H2 (windows), Qt 6.8.3
- window: 1000 × 660

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.4 s |  |

1 of 1 cases passed.


## Screens changed since the run before

- unchanged: 449, changed: 19, new: 168

- `desktop/linux/phone/first/case-more-pages.png` – 0.5 % (`visual-diff/desktop_linux_phone_first_case-more-pages.png`)
- `desktop/linux/phone/first/date-calendar.png` – 0.7 % (`visual-diff/desktop_linux_phone_first_date-calendar.png`)
- `desktop/linux/wide/first/case-more-pages.png` – 0.3 % (`visual-diff/desktop_linux_wide_first_case-more-pages.png`)
- `desktop/linux/wide/first/date-calendar.png` – 0.4 % (`visual-diff/desktop_linux_wide_first_date-calendar.png`)
- `desktop/macos/phone/first/case-more-pages.png` – 0.8 % (`visual-diff/desktop_macos_phone_first_case-more-pages.png`)
- `desktop/macos/phone/first/case-reminders.png` – 0.4 % (`visual-diff/desktop_macos_phone_first_case-reminders.png`)
- `desktop/macos/phone/first/date-calendar.png` – 0.9 % (`visual-diff/desktop_macos_phone_first_date-calendar.png`)
- `desktop/macos/phone/first/page-Mehr.png` – 0.4 % (`visual-diff/desktop_macos_phone_first_page-Mehr.png`)
- `desktop/macos/phone/restart/case-restart-draft.png` – 100.0 % (`visual-diff/desktop_macos_phone_restart_case-restart-draft.png`)
- `desktop/macos/phone/restart/restart-draft.png` – 100.0 % (`visual-diff/desktop_macos_phone_restart_restart-draft.png`)
- `desktop/macos/wide/first/case-more-pages.png` – 0.4 % (`visual-diff/desktop_macos_wide_first_case-more-pages.png`)
- `desktop/macos/wide/first/date-calendar.png` – 0.5 % (`visual-diff/desktop_macos_wide_first_date-calendar.png`)
- `desktop/windows/phone/first/case-more-pages.png` – 0.6 % (`visual-diff/desktop_windows_phone_first_case-more-pages.png`)
- `desktop/windows/phone/first/case-reminders.png` – 0.3 % (`visual-diff/desktop_windows_phone_first_case-reminders.png`)
- `desktop/windows/phone/first/date-calendar.png` – 0.8 % (`visual-diff/desktop_windows_phone_first_date-calendar.png`)
- `desktop/windows/phone/first/page-Mehr.png` – 0.3 % (`visual-diff/desktop_windows_phone_first_page-Mehr.png`)
- `desktop/windows/wide/first/case-more-pages.png` – 0.4 % (`visual-diff/desktop_windows_wide_first_case-more-pages.png`)
- `desktop/windows/wide/first/case-text-field.png` – 5.1 % (`visual-diff/desktop_windows_wide_first_case-text-field.png`)
- `desktop/windows/wide/first/date-calendar.png` – 0.5 % (`visual-diff/desktop_windows_wide_first_date-calendar.png`)
