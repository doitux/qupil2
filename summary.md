# iOS test run 63 (b710a1b)

## iphone: iPhone 16

### UI test (inside Qupil)

- platform: iOS 26.2 (ios), Qt 6.8.3
- window: 393 × 759

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.7 s |  |
| ✅ | pages – every page opens | 8.0 s |  |
| ✅ | today – today with the running lesson, switching days | 2.9 s |  |
| ✅ | note-save – write and save a note for the running lesson | 7.4 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 6.1 s |  |
| ✅ | keyboard-smooth – the keyboard comes in an open editor: the editor makes room smoothly | 16.2 s | No on-screen keyboard came: not measured.  |
| ✅ | student-search – search a student, open and close the student page | 9.9 s | 2 taps needed for "^Lena Muster$".  |
| ✅ | student-new – add a student | 10.4 s |  |
| ✅ | event-open – open an event and go back | 2.7 s |  |
| ✅ | metronome – start and stop the metronome | 10.5 s |  |
| ✅ | appearance – switch to dark and back to light | 3.9 s |  |
| ✅ | language – switch the language to English and back | 5.3 s |  |
| ✅ | reminders – open the reminders | 2.8 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 6.4 s |  |
| ✅ | plan-save – plan the next lesson of a student | 13.5 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 14.2 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 11.8 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 10.8 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 27.2 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 8.0 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 10.6 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 3.8 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 4.4 s |  |
| ✅ | piece-new – add a piece for a student | 17.5 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | lesson-new – add a lesson from the schedule | 4.6 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 7.4 s |  |
| ✅ | together – two individual lessons of a student kept together | 15.6 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | together-new – new lesson for a student who has one: kept together | 13.4 s | 2 taps needed for "combobox:^Wochentag$". 2 taps needed for "^Muster, Lena".  |
| ✅ | reminder-new – add a reminder | 7.0 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 11.0 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 33.2 s | 2 taps needed for "^Unterricht$". 2 taps needed for "^Streicherensemble · Di 17:00$". 2 taps needed for "^Lena\ Muster$".  |
| ✅ | loan-return – return sheet music from the student's page | 11.7 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | more-pages – birthdays and data pages open | 3.0 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 2.7 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 5.3 s |  |

35 of 35 cases passed.

- platform: iOS 26.2 (ios), Qt 6.8.3
- window: 393 × 759

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 6.6 s |  |

1 of 1 cases passed.

- platform: iOS 26.2 (ios), Qt 6.8.3
- window: 393 × 759

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.6 s |  |
| ✅ | reset – reset Qupil on this device, then restore the data from before | 8.2 s |  |
| ✅ | update-hint – changes from a newer Qupil: More asks for an update | 2.5 s |  |

3 of 3 cases passed.

### Background and ended by the system

- screenshot `iphone/shots/01-editor-typing.png`
- screenshot `iphone/shots/02-background.png`
- the editor came back with the unsaved note

### Metronome in the background

- screenshot `iphone/shots/03-metronome-playing.png`
- screenshot `iphone/shots/04-metronome-background.png`
| | Check | Got |
|---|---|---|
| ✅ | plays in front | 8 beats |
| ✅ | plays on in the background (8 s) | 12 beats |
| ✅ | audio session | Playback |

### idb

- Qupil drawn after about 14 s
- screenshot `iphone/shots/05-start.png`
- **not on the screen:** `^OK$`
- **not on the screen:** `^[0-9][0-9]:[0-9][0-9] EU-`
- **not on the screen:** `^Notiz$`

### Selecting text by touch (note editor)

- window offset: 0, 0 ("Fett" at 42 708 on the screen, Qupil logged 42 708)
- screenshot `iphone/shots/06-select-start.png`
- screenshot `iphone/shots/07-select-1-double-tap.png`
- screenshot `iphone/shots/08-select-3-bold.png`
- screenshot `iphone/shots/09-select-5-long-press.png`
- screenshot `iphone/shots/10-select-8-word.png`
- screenshot `iphone/shots/11-select-9-phrase.png`
- screenshot `iphone/shots/12-select-13-colour.png`
- screenshot `iphone/shots/13-select-17-paste-menu.png`
- screenshot `iphone/shots/14-select-18-pasted.png`
- screenshot `iphone/shots/15-select-19-wipe.png`
| | Scenario | Expected | Got |
|---|---|---|---|
| ⏭ | 0 first long press of the tool | `Bogeneinteilung` | `"": came as a tap (noted only)` |
| ⏭ | 1 double tap on a word, line 1 | `Tonleitern` | `the tool cannot double tap: long press` |
| ℹ️ | 1 double tap on a word, line 1 (long press instead) | `Tonleitern` | `` |
| ℹ️ | 2 menu after the double tap | `Ausschneiden, Kopieren, Alles auswählen` | `` |
| ℹ️ | 2 menu after the double tap: no system menu | `` | `Paste, Select, Select All` |
| ℹ️ | 3 "B" with the first tap | `\*\*Tonleitern\*\*` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilun` |
| ℹ️ ✓ | 4 a tap behind the text ends the selection | `` | `` |
| ℹ️ ✓ | 4 menu closed | `` | `` |
| ℹ️ ✓ | 4 menu closed: no system menu | `` | `` |
| ✅ | 4 the text itself unchanged | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |
| ℹ️ | 5 long press on a word, line 2 | `Metronom` | `` |
| ℹ️ | 6 menu after the long press | `Ausschneiden, Kopieren, Alles auswählen` | `` |
| ℹ️ | 6 menu after the long press: no system menu | `` | `Paste, Select, Select All` |
| ℹ️ | 7 "I" on it | `\*Metronom\*` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilun` |
| ⏭ | 8 double tap on "G-Dur" | `G-Dur` | `the tool cannot double tap: long press` |
| ℹ️ | 8 double tap on "G-Dur" (long press instead) | `G-Dur` | `` |
| ℹ️ | 9 end handle dragged to "D-Dur" | `G-Dur und D-Dur` | `no end handle` |
| ℹ️ | 10 menu again after the handle | `Ausschneiden, Kopieren, Alles auswählen` | `` |
| ℹ️ | 10 menu again after the handle: no system menu | `` | `Paste, Select, Select All` |
| ℹ️ | 11 "U" on the phrase | `\+\+G-Dur und D-Dur\+\+` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilun` |
| ⏭ | 12 double tap on a word, line 3 | `Mittelteil` | `the tool cannot double tap: long press` |
| ℹ️ | 12 double tap on a word, line 3 (long press instead) | `Mittelteil` | `` |
| ℹ️ | 13 colour on it | `==Mittelteil==` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilun` |
| ✅ | 13 the text itself unchanged | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |
| ℹ️ | 15 long press on "Etüde" | `Etüde` | `` |
| ℹ️ | 16 "Kopieren" in the menu | `Kopieren` | `` |
| ℹ️ | 17 a tap on the cursor: menu | `Auswählen, Einsetzen, Alles auswählen` | `` |
| ℹ️ ✓ | 17 a tap on the cursor: menu: no system menu | `` | `` |
| ℹ️ | 18 "Einsetzen" | `Einsetzen` | `` |
| ✅ | 18 the text: what was pasted at the end, nothing else changed | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |
| ℹ️ ✓ | 19 a quick wipe over the text selects nothing | `` | `` |
| ✅ | 19 the text after the wipe unchanged | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |

4 of 4 checks that the text stays intact as expected (Qupil's touch rules are off here: the other rows show what the system does).

Markup at the end: `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil`
- Qupil drawn after about 14 s

### Dark mode

- setting in `de.qupil.Qupil.plist`: `"appearance.mode" => "system"`
- setting in `de.qupil.uitest.Qupil.plist`: `"appearance.mode" => "light"`
- screenshot `iphone/shots/16-dark-running.png`
- Qupil drawn after about 10 s
- screenshot `iphone/shots/17-dark-start.png`
- screenshot `iphone/shots/18-light-again.png`

## ipad: iPad Air 13-inch

### UI test (inside Qupil)

- platform: iOS 26.2 (ios), Qt 6.8.3
- window: 1024 × 1309

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 11.2 s |  |
| ✅ | pages – every page opens | 9.1 s |  |
| ✅ | today – today with the running lesson, switching days | 3.2 s |  |
| ✅ | note-save – write and save a note for the running lesson | 8.1 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 6.3 s |  |
| ✅ | keyboard-smooth – the keyboard comes in an open editor: the editor makes room smoothly | 16.5 s | No on-screen keyboard came: not measured.  |
| ✅ | student-search – search a student, open and close the student page | 11.0 s | 2 taps needed for "^Lena Muster$".  |
| ✅ | student-new – add a student | 11.1 s |  |
| ✅ | event-open – open an event and go back | 3.0 s |  |
| ✅ | metronome – start and stop the metronome | 11.1 s |  |
| ✅ | appearance – switch to dark and back to light | 4.8 s |  |
| ✅ | language – switch the language to English and back | 5.6 s |  |
| ✅ | reminders – open the reminders | 3.4 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 7.6 s |  |
| ✅ | plan-save – plan the next lesson of a student | 14.1 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 14.5 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 11.9 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 10.8 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | tasks-enter – tasks of a coupled student with the keyboard: title, hints, next task | 27.4 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | booklet-sheet – practice book sheet: QR code of a coupled student, ending asks first | 8.0 s |  |
| ✅ | plan-tasks – coupled: plan as tasks, then tasks from it with one block kept private | 11.0 s |  |
| ✅ | plan-tasks-edit – coupled: covered blocks into the tasks editor before saving | 4.1 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 4.6 s |  |
| ✅ | piece-new – add a piece for a student | 17.3 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | lesson-new – add a lesson from the schedule | 4.3 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 7.2 s |  |
| ✅ | together – two individual lessons of a student kept together | 15.2 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | together-new – new lesson for a student who has one: kept together | 13.3 s | 2 taps needed for "combobox:^Wochentag$". 2 taps needed for "^Muster, Lena".  |
| ✅ | reminder-new – add a reminder | 7.1 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 11.9 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 34.5 s | 2 taps needed for "^Unterricht$". 2 taps needed for "^Streicherensemble · Di 17:00$". 2 taps needed for "^Lena\ Muster$".  |
| ✅ | loan-return – return sheet music from the student's page | 11.9 s | 2 taps needed for "^Lena\ Muster$".  |
| ✅ | more-pages – birthdays and data pages open | 2.7 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 3.1 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 5.3 s |  |

35 of 35 cases passed.

- platform: iOS 26.2 (ios), Qt 6.8.3
- window: 1024 × 1309

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 6.8 s |  |

1 of 1 cases passed.

- platform: iOS 26.2 (ios), Qt 6.8.3
- window: 1024 × 1309

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 4.7 s |  |
| ✅ | reset – reset Qupil on this device, then restore the data from before | 8.3 s |  |
| ✅ | update-hint – changes from a newer Qupil: More asks for an update | 2.8 s |  |

3 of 3 cases passed.

### Background and ended by the system

- screenshot `ipad/shots/01-editor-typing.png`
- screenshot `ipad/shots/02-background.png`
- the editor came back with the unsaved note

### Metronome in the background

- screenshot `ipad/shots/03-metronome-playing.png`
- screenshot `ipad/shots/04-metronome-background.png`
| | Check | Got |
|---|---|---|
| ✅ | plays in front | 8 beats |
| ✅ | plays on in the background (8 s) | 10 beats |
| ✅ | audio session | Playback |

### idb

- Qupil drawn after about 12 s
- screenshot `ipad/shots/05-start.png`
- **not on the screen:** `^OK$`
- **not on the screen:** `^[0-9][0-9]:[0-9][0-9] EU-`
- **not on the screen:** `^Notiz$`

### Selecting text by touch (note editor)

- window offset: 0, 0 ("Fett" at 244 918 on the screen, Qupil logged 244 918)
- screenshot `ipad/shots/06-select-start.png`
- screenshot `ipad/shots/07-select-1-double-tap.png`
- screenshot `ipad/shots/08-select-3-bold.png`
- screenshot `ipad/shots/09-select-5-long-press.png`
- screenshot `ipad/shots/10-select-8-word.png`
- screenshot `ipad/shots/11-select-9-phrase.png`
- screenshot `ipad/shots/12-select-13-colour.png`
- screenshot `ipad/shots/13-select-17-paste-menu.png`
- screenshot `ipad/shots/14-select-18-pasted.png`
- screenshot `ipad/shots/15-select-19-wipe.png`
| | Scenario | Expected | Got |
|---|---|---|---|
| ⏭ | 0 first long press of the tool | `Bogeneinteilung` | `"": came as a tap (noted only)` |
| ⏭ | 1 double tap on a word, line 1 | `Tonleitern` | `the tool cannot double tap: long press` |
| ℹ️ | 1 double tap on a word, line 1 (long press instead) | `Tonleitern` | `` |
| ℹ️ | 2 menu after the double tap | `Ausschneiden, Kopieren, Alles auswählen` | `` |
| ℹ️ | 2 menu after the double tap: no system menu | `` | `Paste, Select, Select All` |
| ℹ️ | 3 "B" with the first tap | `\*\*Tonleitern\*\*` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilun` |
| ℹ️ ✓ | 4 a tap behind the text ends the selection | `` | `` |
| ℹ️ ✓ | 4 menu closed | `` | `` |
| ℹ️ ✓ | 4 menu closed: no system menu | `` | `` |
| ✅ | 4 the text itself unchanged | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |
| ℹ️ | 5 long press on a word, line 2 | `Metronom` | `` |
| ℹ️ | 6 menu after the long press | `Ausschneiden, Kopieren, Alles auswählen` | `` |
| ℹ️ | 6 menu after the long press: no system menu | `` | `Paste, Select, Select All` |
| ℹ️ | 7 "I" on it | `\*Metronom\*` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilun` |
| ⏭ | 8 double tap on "G-Dur" | `G-Dur` | `the tool cannot double tap: long press` |
| ℹ️ | 8 double tap on "G-Dur" (long press instead) | `G-Dur` | `` |
| ℹ️ | 9 end handle dragged to "D-Dur" | `G-Dur und D-Dur` | `no end handle` |
| ℹ️ | 10 menu again after the handle | `Ausschneiden, Kopieren, Alles auswählen` | `` |
| ℹ️ | 10 menu again after the handle: no system menu | `` | `Paste, Select, Select All` |
| ℹ️ | 11 "U" on the phrase | `\+\+G-Dur und D-Dur\+\+` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilun` |
| ⏭ | 12 double tap on a word, line 3 | `Mittelteil` | `the tool cannot double tap: long press` |
| ℹ️ | 12 double tap on a word, line 3 (long press instead) | `Mittelteil` | `` |
| ℹ️ | 13 colour on it | `==Mittelteil==` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilun` |
| ✅ | 13 the text itself unchanged | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |
| ℹ️ | 15 long press on "Etüde" | `Etüde` | `` |
| ℹ️ | 16 "Kopieren" in the menu | `Kopieren` | `` |
| ℹ️ | 17 a tap on the cursor: menu | `Auswählen, Einsetzen, Alles auswählen` | `` |
| ℹ️ ✓ | 17 a tap on the cursor: menu: no system menu | `` | `` |
| ℹ️ | 18 "Einsetzen" | `Einsetzen` | `` |
| ✅ | 18 the text: what was pasted at the end, nothing else changed | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |
| ℹ️ ✓ | 19 a quick wipe over the text selects nothing | `` | `` |
| ✅ | 19 the text after the wipe unchanged | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |

4 of 4 checks that the text stays intact as expected (Qupil's touch rules are off here: the other rows show what the system does).

Markup at the end: `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil`
- Qupil drawn after about 14 s

### Dark mode

- setting in `de.qupil.Qupil.plist`: `"appearance.mode" => "system"`
- setting in `de.qupil.uitest.Qupil.plist`: `"appearance.mode" => "light"`
- screenshot `ipad/shots/16-dark-running.png`
- Qupil drawn after about 14 s
- screenshot `ipad/shots/17-dark-start.png`
- screenshot `ipad/shots/18-light-again.png`

Result: all steps ran
