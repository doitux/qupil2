# Android test run 78 (9c740d7)

- device: Android SDK built for x86_64, Android 13, 1080x2400, 420 dpi
- date on the device: Tue Nov 17 14:20:00 CET 2026

## Qupil's own UI test (inside the app) and its restart half

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.5 s |  |
| ✅ | pages – every page opens | 7.3 s |  |
| ✅ | today – today with the running lesson, switching days | 2.5 s |  |
| ✅ | note-save – write and save a note for the running lesson | 6.4 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 6.0 s |  |
| ✅ | keyboard-smooth – the keyboard comes in an open editor: the editor makes room smoothly | 15.8 s | No on-screen keyboard came: not measured.  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 9.5 s |  |
| ✅ | student-search – search a student, open and close the student page | 7.5 s |  |
| ✅ | student-new – add a student | 9.9 s |  |

9 of 9 cases passed.

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 4.6 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 11.3 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 5.7 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 5.3 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 12.8 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 7.2 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 12.3 s |  |

7 of 7 cases passed.

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 4.6 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 10.4 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 5.7 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 15.0 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 6.2 s |  |

5 of 5 cases passed.

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 4.6 s |  |
| ✅ | event-open – open an event and go back | 2.9 s |  |
| ✅ | metronome – start and stop the metronome | 3.9 s |  |
| ✅ | appearance – switch to dark and back to light | 3.5 s |  |
| ✅ | language – switch the language to English and back | 4.8 s |  |
| ✅ | reminders – open the reminders | 3.3 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 5.9 s |  |

7 of 7 cases passed.

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 4.6 s |  |
| ✅ | plan-save – plan the next lesson of a student | 11.1 s |  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 11.0 s |  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 8.7 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 3.4 s |  |
| ✅ | piece-new – add a piece for a student | 14.7 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 3.5 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 6.4 s |  |
| ✅ | together – two individual lessons of a student kept together | 11.2 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 9.0 s |  |
| ✅ | reminder-new – add a reminder | 7.1 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 9.9 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 26.0 s |  |
| ✅ | loan-return – return sheet music from the student's page | 8.8 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.1 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 2.8 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 5.1 s |  |

17 of 17 cases passed.

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.4 s |  |

1 of 1 cases passed.

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 4.6 s |  |
| ✅ | reset – reset Qupil on this device, then restore the data from before | 8.3 s |  |
| ✅ | update-hint – changes from a newer Qupil: More asks for an update | 2.2 s |  |

3 of 3 cases passed.


## Start with sample data

- screenshot `shots/01-start-today.png`
- process: `2240`
- top: `topResumedActivity=ActivityRecord{5402e92 u0 de.qupil.app/.QupilActivity} t13}`

```
14:32:56.675  2240  2258 I Qupil   : main: window visible 1, exposed 0
14:32:56.728  2240  2263 I Qupil   : main: graphics ready
14:32:56.948  2240  2258 I Qupil   : main: event loop
14:32:56.993  2240  2258 I Qupil   : main: sample data loaded
14:32:57.064  2240  2258 I Qupil   : main: first frame
14:32:57.064  2240  2258 I Qupil   : activity: content ready
14:32:57.085  2240  2240 I Qupil   : activity: focus true
14:32:57.267  2240  2258 I Qupil   : main: application state 4
14:33:02.580  2240  2240 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:02.580  2240  2240 W default : (Parent is QQuickPopupItem(0x70a09893de20), parent's thread is QThread(0x70a088895410), current thread is QThread(0x70a08888ff90)
14:33:02.580  2240  2240 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:02.580  2240  2240 W default : (Parent is ProgressBar_QMLTYPE_213(0x70a1288cd510), parent's thread is QThread(0x70a088895410), current thread is QThread(0x70a08888ff90)
14:33:02.580  2240  2240 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:02.580  2240  2240 W default : (Parent is QQuickPopupItem(0x70a09893d220), parent's thread is QThread(0x70a088895410), current thread is QThread(0x70a08888ff90)
14:33:02.581  2240  2240 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:02.581  2240  2240 W default : (Parent is QQuickPopupItem(0x70a09893e990), parent's thread is QThread(0x70a088895410), current thread is QThread(0x70a08888ff90)
14:33:02.581  2240  2240 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:02.581  2240  2240 W default : (Parent is QQuickPopupItem(0x70a098940d00), parent's thread is QThread(0x70a088895410), current thread is QThread(0x70a08888ff90)
14:33:02.581  2240  2240 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:02.581  2240  2240 W default : (Parent is QQuickPopupItem(0x70a098941b40), parent's thread is QThread(0x70a088895410), current thread is QThread(0x70a08888ff90)
14:33:02.581  2240  2240 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:02.581  2240  2240 W default : (Parent is QQuickPopupItem(0x70a098944840), parent's thread is QThread(0x70a088895410), current thread is QThread(0x70a08888ff90)
14:33:02.581  2240  2240 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:02.581  2240  2240 W default : (Parent is QQuickPopupItem(0x70a098945320), parent's thread is QThread(0x70a088895410), current thread is QThread(0x70a08888ff90)
14:33:02.582  2240  2240 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:02.582  2240  2240 W default : (Parent is QQuickPopupItem(0x70a098982430), parent's thread is QThread(0x70a088895410), current thread is QThread(0x70a08888ff90)
14:33:02.582  2240  2240 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:02.582  2240  2240 W default : (Parent is StackView_QMLTYPE_33(0x70a128944b50), parent's thread is QThread(0x70a088895410), current thread is QThread(0x70a08888ff90)
14:33:02.582  2240  2240 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:02.582  2240  2240 W default : (Parent is QQuickPopupItem(0x70a098984590), parent's thread is QThread(0x70a088895410), current thread is QThread(0x70a08888ff90)
14:33:02.582  2240  2240 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:02.582  2240  2240 W default : (Parent is QQuickPopupItem(0x70a0989847a0), parent's thread is QThread(0x70a088895410), current thread is QThread(0x70a08888ff90)
14:33:02.582  2240  2240 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:02.582  2240  2240 W default : (Parent is ScrollIndicator_QMLTYPE_58(0x70a128947850), parent's thread is QThread(0x70a088895410), current thread is QThread(0x70a08888ff90)
14:33:02.582  2240  2240 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:02.582  2240  2240 W default : (Parent is QQuickPopupItem(0x70a098986420), parent's thread is QThread(0x70a088895410), current thread is QThread(0x70a08888ff90)
14:33:02.583  2240  2240 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:02.583  2240  2240 W default : (Parent is ScrollBar_QMLTYPE_23(0x70a128957330), parent's thread is QThread(0x70a088895410), current thread is QThread(0x70a08888ff90)
14:33:02.583  2240  2240 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:02.583  2240  2240 W default : (Parent is QQuickPopupItem(0x70a098984170), parent's thread is QThread(0x70a088895410), current thread is QThread(0x70a08888ff90)
```


## To the background and back (process stays)

- screenshot `shots/02-home-screen.png`
- screenshot `shots/03-back.png`
- process: `2240`
- top: `topResumedActivity=ActivityRecord{5402e92 u0 de.qupil.app/.QupilActivity} t13}`

```
14:33:04.639  2240  2240 I Qupil   : activity: focus false
14:33:04.646  2240  2258 I Qupil   : main: application state 2
14:33:04.704  2240  2240 I Qupil   : activity: stop
14:33:04.705  2240  2258 I Qupil   : main: application state 0
14:33:09.954  2240  2240 I Qupil   : activity: start
14:33:09.954  2240  2240 I Qupil   : activity: resume, content ready true
14:33:09.984  2240  2240 I Qupil   : activity: focus true
14:33:10.053  2240  2258 I Qupil   : main: application state 4
```


## Android ends Qupil in the background, then back

- process after ending: `` (was 2240)
- screenshot `shots/04-after-process-end.png`
- process: `2418`
- top: `topResumedActivity=ActivityRecord{6807c3c u0 de.qupil.app/.QupilActivity} t13}`

```
14:33:22.466  2418  2418 I Qupil   : activity: focus true
14:33:22.555  2418  2436 I Qupil   : main: application state 4
14:33:26.956  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.956  2418  2418 W default : (Parent is QQuickPopupItem(0x70a0989359c0), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.956  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.956  2418  2418 W default : (Parent is ProgressBar_QMLTYPE_306(0x70a1288cac90), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.956  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.956  2418  2418 W default : (Parent is QQuickPopupItem(0x70a0989358d0), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.956  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.956  2418  2418 W default : (Parent is QQuickPopupItem(0x70a098937730), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.956  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.956  2418  2418 W default : (Parent is QQuickPopupItem(0x70a0989393b0), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.956  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.956  2418  2418 W default : (Parent is QQuickPopupItem(0x70a09893ab50), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.957  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.957  2418  2418 W default : (Parent is QQuickPopupItem(0x70a09893c800), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.957  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.957  2418  2418 W default : (Parent is QQuickPopupItem(0x70a09893ce90), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.957  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.957  2418  2418 W default : (Parent is QQuickPopupItem(0x70a098955e50), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.957  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.957  2418  2418 W default : (Parent is StackView_QMLTYPE_120(0x70a1288f8a10), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.957  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.957  2418  2418 W default : (Parent is QQuickPopupItem(0x70a098958820), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.957  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.957  2418  2418 W default : (Parent is QQuickPopupItem(0x70a098957bf0), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.957  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.957  2418  2418 W default : (Parent is ScrollIndicator_QMLTYPE_28(0x70a1288fd690), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.957  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.957  2418  2418 W default : (Parent is QQuickPopupItem(0x70a09895a500), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.957  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.957  2418  2418 W default : (Parent is ScrollBar_QMLTYPE_7(0x70a128909150), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.957  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.957  2418  2418 W default : (Parent is QQuickPopupItem(0x70a098957f20), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.957  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.957  2418  2418 W default : (Parent is CheckBox_QMLTYPE_80(0x70a12894b750), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.957  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.957  2418  2418 W default : (Parent is CheckBox_QMLTYPE_80(0x70a12897da90), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
14:33:26.957  2418  2418 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:26.957  2418  2418 W default : (Parent is CheckBox_QMLTYPE_80(0x70a128980af0), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ded30)
```


## Draft: typing in the note editor, Android ends Qupil, back

- tapped `^OK$` at 910 1579
- tapped `^Notiz$` at 312 1768
- tapped `^Notiz$`, `^Notiz speichern$` is there
- screenshot `shots/05-editor-typing.png`
- screenshot `shots/06-draft-back.png`
- the draft is back in the editor
- process: `2644`
- top: `topResumedActivity=ActivityRecord{3b51c66 u0 de.qupil.app/.QupilActivity} t13}`

```
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289c80d0), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289c8e50), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289cd770), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289cc210), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289cb910), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289cbb50), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289caef0), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289ca830), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289cd0b0), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289cd890), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289cf390), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289ce610), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289cf6f0), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.596  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.596  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289ce970), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.596  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.596  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289d1d30), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.596  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.596  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289d21b0), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.596  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.596  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289d0470), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.596  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.596  2644  2644 W default : (Parent is CheckBox_QMLTYPE_118(0x70a12894c950), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.596  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.596  2644  2644 W default : (Parent is CheckBox_QMLTYPE_118(0x70a1289a8b10), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.596  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.596  2644  2644 W default : (Parent is CheckBox_QMLTYPE_118(0x70a1289e0610), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
```

- screenshot `shots/07-editor-left.png`
- process: `2644`
- top: `topResumedActivity=ActivityRecord{5591857 u0 com.android.fakesystemapp/.launcher.EmptyHomeActivity} t14}`

```
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289cc210), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289cb910), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289cbb50), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289caef0), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289ca830), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289cd0b0), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289cd890), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289cf390), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289ce610), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.595  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.595  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289cf6f0), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.596  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.596  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289ce970), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.596  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.596  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289d1d30), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.596  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.596  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289d21b0), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.596  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.596  2644  2644 W default : (Parent is AbstractButton_QMLTYPE_9_QML_32(0x70a1289d0470), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.596  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.596  2644  2644 W default : (Parent is CheckBox_QMLTYPE_118(0x70a12894c950), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.596  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.596  2644  2644 W default : (Parent is CheckBox_QMLTYPE_118(0x70a1289a8b10), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:03.596  2644  2644 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:03.596  2644  2644 W default : (Parent is CheckBox_QMLTYPE_118(0x70a1289e0610), parent's thread is QThread(0x70a088895a10), current thread is QThread(0x70a08888d7d0)
14:34:07.799  2644  2663 I Qupil   : main: application state 2
14:34:07.803  2644  2644 I Qupil   : activity: focus false
14:34:07.817  2644  2663 I default : Draft note: kept, 230 characters
14:34:07.868  2644  2644 I Qupil   : activity: stop
14:34:07.869  2644  2663 I Qupil   : main: application state 0
14:34:07.871  2644  2663 I default : Draft note: kept, 230 characters
```


## Dark mode switched on while Qupil runs

- screenshot `shots/08-dark-runtime.png`
- screenshot `shots/09-dark-back.png`
- process: `2644`
- top: `topResumedActivity=ActivityRecord{3b51c66 u0 de.qupil.app/.QupilActivity} t13}`

```
14:34:23.444  2644  2644 I Qupil   : activity: start
14:34:23.444  2644  2644 I Qupil   : activity: resume, content ready true
14:34:23.468  2644  2644 I Qupil   : activity: focus true
14:34:23.478  2644  2667 W default : makeCurrent(): no EGLSurface, likely Surface destroyed by Android.
14:34:23.478  2644  2667 W default : QRhiGles2: Failed to make context current. Expect bad things to happen.
14:34:23.478  2644  2667 W default : Failed to start frame
14:34:23.478  2644  2663 I Qupil   : main: application state 4
```


## Fresh start in dark mode

- screenshot `shots/10-dark-start.png`
- screenshot `shots/11-light-again.png`
- process: `2851`
- top: `topResumedActivity=ActivityRecord{8cac356 u0 de.qupil.app/.QupilActivity} t15}`

```
14:34:36.410  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.410  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289c8430), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.410  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.410  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289c9870), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.410  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.410  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289c8310), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.410  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.410  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289cc9f0), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.410  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.410  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289cdf50), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.410  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.410  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289ce070), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.410  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.410  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289cacb0), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.410  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.410  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289cce70), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.410  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.410  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289cc690), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.410  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.410  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289cbc70), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.411  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.411  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289cdad0), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.411  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.411  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289cfed0), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.411  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.411  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289cebb0), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.411  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.411  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289cecd0), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.411  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.411  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289cedf0), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.411  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.411  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289d0b30), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.411  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.411  2851  2851 W default : (Parent is AbstractButton_QMLTYPE_13_QML_25(0x70a1289d2ab0), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.411  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.411  2851  2851 W default : (Parent is CheckBox_QMLTYPE_26(0x70a1289b4db0), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.411  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.411  2851  2851 W default : (Parent is CheckBox_QMLTYPE_26(0x70a1289a3110), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
14:34:36.411  2851  2851 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:36.411  2851  2851 W default : (Parent is CheckBox_QMLTYPE_26(0x70a1289e1db0), parent's thread is QThread(0x70a088895370), current thread is QThread(0x70a0888ddc90)
```


## Landscape

- screenshot `shots/12-landscape.png`
- screenshot `shots/13-portrait-again.png`
- process: `2851`
- top: `topResumedActivity=ActivityRecord{8cac356 u0 de.qupil.app/.QupilActivity} t15}`

```
```


## Light chosen in Qupil (not the system's), then a new start

- tapped `^OK$` at 910 1579
- tapped `^Mehr$` at 962 2258
- tapped `^Mehr$` at 962 2258
- tapped `^Mehr$`, `^Einstellungen$` is there
- tapped `^Einstellungen$` at 968 290
- tapped `^Einstellungen$`, `^Darstellung` is there
- tapped `^Darstellung` at 539 975
- tapped `^Darstellung`, `^Hell$` is there
- tapped `^Hell$` at 552 700
- screenshot `shots/14-light-chosen.png`
- Qupil starts with its own light mode
- screenshot `shots/15-light-start.png`
- process: `3329`
- top: `topResumedActivity=ActivityRecord{bcd5b50 u0 de.qupil.app/.QupilActivity} t17}`

```
14:35:45.702  3329  3329 I Qupil   : activity: focus true
14:35:45.773  3329  3348 I Qupil   : main: application state 4
14:35:50.327  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.327  3329  3329 W default : (Parent is QQuickPopupItem(0x70a098935930), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.327  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.327  3329  3329 W default : (Parent is ProgressBar_QMLTYPE_157(0x70a1288cbc50), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.328  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.328  3329  3329 W default : (Parent is QQuickPopupItem(0x70a0989357e0), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.328  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.328  3329  3329 W default : (Parent is QQuickPopupItem(0x70a098936c80), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.328  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.328  3329  3329 W default : (Parent is QQuickPopupItem(0x70a098938a50), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.328  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.328  3329  3329 W default : (Parent is QQuickPopupItem(0x70a09893a3a0), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.328  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.328  3329  3329 W default : (Parent is QQuickPopupItem(0x70a09893c710), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.328  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.328  3329  3329 W default : (Parent is QQuickPopupItem(0x70a09893cf50), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.328  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.328  3329  3329 W default : (Parent is QQuickPopupItem(0x70a098956000), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.328  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.328  3329  3329 W default : (Parent is StackView_QMLTYPE_124(0x70a1288fa2d0), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.328  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.328  3329  3329 W default : (Parent is QQuickPopupItem(0x70a098957a40), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.328  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.328  3329  3329 W default : (Parent is QQuickPopupItem(0x70a098958130), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.328  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.328  3329  3329 W default : (Parent is ScrollIndicator_QMLTYPE_71(0x70a1288fc6d0), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.328  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.328  3329  3329 W default : (Parent is QQuickPopupItem(0x70a098959f00), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.328  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.328  3329  3329 W default : (Parent is ScrollBar_QMLTYPE_26(0x70a128908190), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.329  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.329  3329  3329 W default : (Parent is QQuickPopupItem(0x70a098957c50), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.329  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.329  3329  3329 W default : (Parent is CheckBox_QMLTYPE_45(0x70a12893e850), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.329  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.329  3329  3329 W default : (Parent is CheckBox_QMLTYPE_45(0x70a128969e70), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
14:35:50.329  3329  3329 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:50.329  3329  3329 W default : (Parent is CheckBox_QMLTYPE_45(0x70a12897fa10), parent's thread is QThread(0x70a088895930), current thread is QThread(0x70a088895170)
```

- tapped `^OK$` at 910 1579
- tapped `^Mehr$` at 962 2258
- tapped `^Mehr$`, `^Einstellungen$` is there
- tapped `^Einstellungen$` at 968 290
- tapped `^Einstellungen$`, `^Darstellung` is there
- tapped `^Darstellung` at 539 975
- tapped `^Darstellung`, `^System$` is there
- tapped `^System$` at 280 700

## Selecting words by touch in the note editor


### Selecting text by touch (note editor)

- window offset: 0, 128 ("Fett" at 110 2112 on the screen, Qupil logged 110 1984)
- screenshot `shots/16-select-start.png`
- screenshot `shots/17-select-1-double-tap.png`
- screenshot `shots/18-select-3-bold.png`
- screenshot `shots/19-select-5-long-press.png`
- screenshot `shots/20-select-8-word.png`
- screenshot `shots/21-select-9-phrase.png`
- screenshot `shots/22-select-13-colour.png`
- screenshot `shots/23-select-17-paste-menu.png`
- screenshot `shots/24-select-18-pasted.png`
- screenshot `shots/25-select-19-wipe.png`
| | Scenario | Expected | Got |
|---|---|---|---|
| ✅ | 0 first long press of the tool | `Bogeneinteilung` | `Bogeneinteilung` |
| ✅ | 1 double tap on a word, line 1 | `Tonleitern` | `Tonleitern` |
| ✅ | 2 menu after the double tap | `Ausschneiden, Kopieren, Alles auswählen` | `Ausschneiden, Kopieren, Einsetzen, Alles auswählen` |
| ✅ | 2 menu after the double tap: no system menu | `` | `` |
| ✅ | 3 "B" with the first tap | `\*\*Tonleitern\*\*` | `**Tonleitern** G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogenei` |
| ✅ | 4 a tap behind the text ends the selection | `` | `` |
| ✅ | 4 menu closed | `` | `` |
| ✅ | 4 menu closed: no system menu | `` | `` |
| ✅ | 4 the text itself unchanged | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |
| ✅ | 5 long press on a word, line 2 | `Metronom` | `Metronom` |
| ✅ | 6 menu after the long press | `Ausschneiden, Kopieren, Alles auswählen` | `Ausschneiden, Kopieren, Einsetzen, Alles auswählen` |
| ✅ | 6 menu after the long press: no system menu | `` | `` |
| ✅ | 7 "I" on it | `\*Metronom\*` | `**Tonleitern** G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit *Metronom* / Vortragsstück: Bogen` |
| ✅ | 8 double tap on "G-Dur" | `G-Dur` | `G-Dur` |
| ✅ | 9 end handle dragged to "D-Dur" | `G-Dur und D-Dur` | `G-Dur und D-Dur` |
| ✅ | 10 menu again after the handle | `Ausschneiden, Kopieren, Alles auswählen` | `Ausschneiden, Kopieren, Einsetzen, Alles auswählen` |
| ✅ | 10 menu again after the handle: no system menu | `` | `` |
| ✅ | 11 "U" on the phrase | `\+\+G-Dur und D-Dur\+\+` | `**Tonleitern** ++G-Dur und D-Dur++ langsam üben / Etüde Nr. 3 bis Takt 16 mit *Metronom* / Vortragsstück: B` |
| ✅ | 12 double tap on a word, line 3 | `Mittelteil` | `Mittelteil` |
| ✅ | 13 colour on it | `==Mittelteil==` | `**Tonleitern** ++G-Dur und D-Dur++ langsam üben / Etüde Nr. 3 bis Takt 16 mit *Metronom* / Vortragsstück: B` |
| ✅ | 13 the text itself unchanged | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im Mittelteil` |
| ✅ | 15 long press on "Etüde" | `Etüde` | `Etüde` |
| ✅ | 16 "Kopieren" closes the menu | `` | `` |
| ✅ | 16 "Kopieren" closes the menu: no system menu | `` | `` |
| ✅ | 17 a tap on the cursor: menu | `Auswählen, Einsetzen, Alles auswählen` | `Auswählen, Einsetzen, Alles auswählen` |
| ✅ | 17 a tap on the cursor: menu: no system menu | `` | `` |
| ✅ | 18 "Einsetzen" puts it at the end | `Etüde(==)?$` | `**Tonleitern** ++G-Dur und D-Dur++ langsam üben / Etüde Nr. 3 bis Takt 16 mit *Metronom* / Vortragsstück: B` |
| ✅ | 18 the text: what was pasted at the end, nothing else changed | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im MittelteilEtüde` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im MittelteilEtüde` |
| ✅ | 19 a quick wipe over the text selects nothing | `` | `` |
| ✅ | 19 the text after the wipe unchanged | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im MittelteilEtüde` | `Tonleitern G-Dur und D-Dur langsam üben / Etüde Nr. 3 bis Takt 16 mit Metronom / Vortragsstück: Bogeneinteilung im MittelteilEtüde` |

30 of 30 touch checks as expected.

Markup at the end: `**Tonleitern** ++G-Dur und D-Dur++ langsam üben / Etüde Nr. 3 bis Takt 16 mit *Metronom* / Vortragsstück: Bogeneinteilung im ==Mittelteil==Etüde`
- double tap: 172 ms for both;double tap: 174 ms for both;double tap: 181 ms for both;
- process: `3598`
- top: `topResumedActivity=ActivityRecord{35d8e2c u0 de.qupil.app/.QupilActivity} t18}`

```
14:37:30.253  3598  3616 I default : select: sel 115 115 
14:37:30.253  3598  3616 I default : select: cursor 115
14:37:30.453  3598  3616 I default : select: sel 111 121 Mittelteil
14:37:30.453  3598  3616 I default : select: cursor 121
14:37:30.456  3598  3616 I default : select: menu Ausschneiden, Kopieren, Einsetzen, Alles auswählen
14:37:30.457  3598  3616 I default : select: button Ausschneiden 69.1 193.0
14:37:30.457  3598  3616 I default : select: button Kopieren 175.2 193.0
14:37:30.458  3598  3616 I default : select: button Einsetzen 266.8 193.0
14:37:30.460  3598  3616 I default : select: handle start 288.2 232.0
14:37:30.460  3598  3616 I default : select: handle end 351.7 266.0
14:37:31.455  3598  3616 I default : select: menu 
14:37:31.459  3598  3616 I default : select: markup **Tonleitern** ++G-Dur und D-Dur++ langsam üben / Etüde Nr. 3 bis Takt 16 mit *Metronom* / Vortragsstück: Bogeneinteilung im ==Mittelteil==
14:37:34.856  3598  3616 I default : select: sel 121 121 
14:37:34.862  3598  3616 I default : select: handle start -
14:37:34.863  3598  3616 I default : select: handle end -
14:37:37.663  3598  3616 I default : select: sel 40 45 Etüde
14:37:37.663  3598  3616 I default : select: cursor 45
14:37:37.676  3598  3616 I default : select: menu Ausschneiden, Kopieren, Einsetzen, Alles auswählen
14:37:37.678  3598  3616 I default : select: button Ausschneiden 69.1 161.0
14:37:37.679  3598  3616 I default : select: button Kopieren 175.2 161.0
14:37:37.680  3598  3616 I default : select: button Einsetzen 266.8 161.0
14:37:37.685  3598  3616 I default : select: handle start 38.0 200.0
14:37:37.686  3598  3616 I default : select: handle end 80.5 234.0
14:37:39.255  3598  3616 I default : select: menu 
14:37:42.453  3598  3616 I default : select: sel 121 121 
14:37:42.454  3598  3616 I default : select: cursor 121
14:37:42.461  3598  3616 I default : select: handle start -
14:37:42.461  3598  3616 I default : select: handle end -
14:37:43.659  3598  3616 I default : select: menu Auswählen, Einsetzen, Alles auswählen
14:37:43.661  3598  3616 I default : select: button Einsetzen 221.1 207.0
14:37:43.662  3598  3616 I default : select: button Auswählen 122.8 207.0
14:37:49.250  3598  3616 I default : select: word Mittelteil 38.0 259.0 102.5 279.0
14:37:49.255  3598  3616 I default : select: sel 126 126 
14:37:49.255  3598  3616 I default : select: cursor 126
14:37:49.260  3598  3616 I default : select: menu 
14:37:49.265  3598  3616 I default : select: markup **Tonleitern** ++G-Dur und D-Dur++ langsam üben / Etüde Nr. 3 bis Takt 16 mit *Metronom* / Vortragsstück: Bogeneinteilung im ==Mittelteil==Etüde
14:37:54.052  3598  3616 I default : select: sel 107 107 
14:37:54.052  3598  3616 I default : select: cursor 107
14:37:57.453  3598  3616 I default : select: sel 122 122 
14:37:57.453  3598  3616 I default : select: cursor 122
```


## Crashes and hangs during the whole run

- none

Result: all steps ran
