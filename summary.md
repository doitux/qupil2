# Android test run 60 (d30ff1f)

- device: Android SDK built for x86_64, Android 13, 1080x2400, 420 dpi
- date on the device: Tue Nov 10 14:20:00 CET 2026

## Qupil's own UI test (inside the app) and its restart half

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

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 4.7 s |  |
| ✅ | text-tap – finger: a tap places the cursor; a tap on the cursor shows and hides the menu | 11.4 s |  |
| ✅ | text-double-tap – finger: a double tap selects the word at the second touch; B and B again | 5.8 s |  |
| ✅ | text-triple-tap – finger: a triple tap selects the paragraph (without its end); B, B again | 5.5 s |  |
| ✅ | text-long-press – finger: a long press selects the word while the finger is down | 13.0 s |  |
| ✅ | text-handles – finger: the handles make the selection longer at either end | 7.6 s |  |
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 12.4 s |  |

7 of 7 cases passed.

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 4.7 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 10.5 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 5.9 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 15.1 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 6.3 s |  |

5 of 5 cases passed.

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 4.7 s |  |
| ✅ | event-open – open an event and go back | 3.1 s |  |
| ✅ | metronome – start and stop the metronome | 3.9 s |  |
| ✅ | appearance – switch to dark and back to light | 3.8 s |  |
| ✅ | language – switch the language to English and back | 5.1 s |  |
| ✅ | reminders – open the reminders | 3.4 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 6.2 s |  |

7 of 7 cases passed.

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 4.7 s |  |
| ✅ | plan-save – plan the next lesson of a student | 11.1 s |  |
| ✅ | piece-new – add a piece for a student | 14.8 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.1 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 6.7 s |  |
| ✅ | together – two individual lessons of a student kept together | 11.5 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 9.3 s |  |
| ✅ | reminder-new – add a reminder | 7.1 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 27.3 s |  |
| ✅ | loan-return – return sheet music from the student's page | 9.0 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.7 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 3.0 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 5.2 s |  |

13 of 13 cases passed.

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.6 s |  |

1 of 1 cases passed.


## Start with sample data

- screenshot `shots/01-start-today.png`
- process: `2170`
- top: `topResumedActivity=ActivityRecord{4ba368c u0 de.qupil.app/.QupilActivity} t11}`

```
14:26:56.097  2170  2188 I Qupil   : main: window visible 1, exposed 0
14:26:56.154  2170  2192 I Qupil   : main: graphics ready
14:26:56.404  2170  2188 I Qupil   : main: event loop
14:26:56.426  2170  2188 I Qupil   : main: sample data loaded
14:26:56.479  2170  2188 I Qupil   : main: first frame
14:26:56.479  2170  2188 I Qupil   : activity: content ready
14:26:56.508  2170  2170 I Qupil   : activity: focus true
14:26:56.783  2170  2188 I Qupil   : main: application state 4
14:27:01.992  2170  2170 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:01.992  2170  2170 W default : (Parent is QQuickPopupItem(0x77c14e0074d0), parent's thread is QThread(0x77c13df5d830), current thread is QThread(0x77c13df5de90)
14:27:01.993  2170  2170 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:01.993  2170  2170 W default : (Parent is ProgressBar_QMLTYPE_74(0x77c1ddf9e670), parent's thread is QThread(0x77c13df5d830), current thread is QThread(0x77c13df5de90)
14:27:01.996  2170  2170 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:01.996  2170  2170 W default : (Parent is QQuickPopupItem(0x77c14e0068d0), parent's thread is QThread(0x77c13df5d830), current thread is QThread(0x77c13df5de90)
14:27:01.996  2170  2170 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:01.996  2170  2170 W default : (Parent is QQuickPopupItem(0x77c14e008580), parent's thread is QThread(0x77c13df5d830), current thread is QThread(0x77c13df5de90)
14:27:01.996  2170  2170 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:01.996  2170  2170 W default : (Parent is QQuickPopupItem(0x77c14e00a950), parent's thread is QThread(0x77c13df5d830), current thread is QThread(0x77c13df5de90)
14:27:01.997  2170  2170 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:01.997  2170  2170 W default : (Parent is QQuickPopupItem(0x77c14e00b7f0), parent's thread is QThread(0x77c13df5d830), current thread is QThread(0x77c13df5de90)
14:27:01.997  2170  2170 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:01.997  2170  2170 W default : (Parent is QQuickPopupItem(0x77c14e00e100), parent's thread is QThread(0x77c13df5d830), current thread is QThread(0x77c13df5de90)
14:27:01.997  2170  2170 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:01.997  2170  2170 W default : (Parent is QQuickPopupItem(0x77c14e00ebe0), parent's thread is QThread(0x77c13df5d830), current thread is QThread(0x77c13df5de90)
14:27:01.997  2170  2170 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:01.997  2170  2170 W default : (Parent is QQuickPopupItem(0x77c14e04a2e0), parent's thread is QThread(0x77c13df5d830), current thread is QThread(0x77c13df5de90)
14:27:01.997  2170  2170 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:01.997  2170  2170 W default : (Parent is StackView_QMLTYPE_175(0x77c1de011810), parent's thread is QThread(0x77c13df5d830), current thread is QThread(0x77c13df5de90)
14:27:01.997  2170  2170 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:01.997  2170  2170 W default : (Parent is QQuickPopupItem(0x77c14e04cc80), parent's thread is QThread(0x77c13df5d830), current thread is QThread(0x77c13df5de90)
14:27:01.997  2170  2170 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:01.997  2170  2170 W default : (Parent is QQuickPopupItem(0x77c14e04c0e0), parent's thread is QThread(0x77c13df5d830), current thread is QThread(0x77c13df5de90)
14:27:01.997  2170  2170 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:01.997  2170  2170 W default : (Parent is ScrollIndicator_QMLTYPE_48(0x77c1de018bf0), parent's thread is QThread(0x77c13df5d830), current thread is QThread(0x77c13df5de90)
14:27:01.997  2170  2170 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:01.997  2170  2170 W default : (Parent is QQuickPopupItem(0x77c14e04eb40), parent's thread is QThread(0x77c13df5d830), current thread is QThread(0x77c13df5de90)
14:27:01.997  2170  2170 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:01.997  2170  2170 W default : (Parent is ScrollBar_QMLTYPE_13(0x77c1de020f90), parent's thread is QThread(0x77c13df5d830), current thread is QThread(0x77c13df5de90)
14:27:01.997  2170  2170 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:01.997  2170  2170 W default : (Parent is QQuickPopupItem(0x77c14e04cdd0), parent's thread is QThread(0x77c13df5d830), current thread is QThread(0x77c13df5de90)
```


## To the background and back (process stays)

- screenshot `shots/02-home-screen.png`
- screenshot `shots/03-back.png`
- process: `2170`
- top: `topResumedActivity=ActivityRecord{4ba368c u0 de.qupil.app/.QupilActivity} t11}`

```
14:27:03.997  2170  2188 I Qupil   : main: application state 2
14:27:04.010  2170  2170 I Qupil   : activity: focus false
14:27:04.114  2170  2170 I Qupil   : activity: stop
14:27:04.114  2170  2188 I Qupil   : main: application state 0
14:27:09.230  2170  2170 I Qupil   : activity: start
14:27:09.230  2170  2170 I Qupil   : activity: resume, content ready true
14:27:09.280  2170  2170 I Qupil   : activity: focus true
14:27:09.448  2170  2188 I Qupil   : main: application state 4
```


## Android ends Qupil in the background, then back

- process after ending: `` (was 2170)
- screenshot `shots/04-after-process-end.png`
- process: `2349`
- top: `topResumedActivity=ActivityRecord{a1d06e6 u0 de.qupil.app/.QupilActivity} t11}`

```
14:27:21.645  2349  2349 I Qupil   : activity: focus true
14:27:21.704  2349  2367 I Qupil   : main: application state 4
14:27:26.107  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.107  2349  2349 W default : (Parent is QQuickPopupItem(0x77c14e000090), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.110  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.110  2349  2349 W default : (Parent is ProgressBar_QMLTYPE_28(0x77c1ddf9a1d0), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.110  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.110  2349  2349 W default : (Parent is QQuickPopupItem(0x77c14e0004e0), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.110  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.110  2349  2349 W default : (Parent is QQuickPopupItem(0x77c14e0013b0), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.110  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.110  2349  2349 W default : (Parent is QQuickPopupItem(0x77c14e002d90), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.110  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.110  2349  2349 W default : (Parent is QQuickPopupItem(0x77c14e0042f0), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.110  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.110  2349  2349 W default : (Parent is QQuickPopupItem(0x77c14e006c90), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.110  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.110  2349  2349 W default : (Parent is QQuickPopupItem(0x77c14e007950), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.110  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.110  2349  2349 W default : (Parent is QQuickPopupItem(0x77c14e0203a0), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.110  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.110  2349  2349 W default : (Parent is StackView_QMLTYPE_47(0x77c1ddfcd050), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.110  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.110  2349  2349 W default : (Parent is QQuickPopupItem(0x77c14e022320), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.110  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.110  2349  2349 W default : (Parent is QQuickPopupItem(0x77c14e0227a0), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.110  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.110  2349  2349 W default : (Parent is ScrollIndicator_QMLTYPE_65(0x77c1ddfd09b0), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.110  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.110  2349  2349 W default : (Parent is QQuickPopupItem(0x77c14e023bb0), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.110  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.110  2349  2349 W default : (Parent is ScrollBar_QMLTYPE_19(0x77c1ddfd9d10), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.111  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.111  2349  2349 W default : (Parent is QQuickPopupItem(0x77c14e022440), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.111  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.111  2349  2349 W default : (Parent is CheckBox_QMLTYPE_83(0x77c1de01f370), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.111  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.111  2349  2349 W default : (Parent is CheckBox_QMLTYPE_83(0x77c1de038870), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
14:27:26.111  2349  2349 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:26.111  2349  2349 W default : (Parent is CheckBox_QMLTYPE_83(0x77c1de04f070), parent's thread is QThread(0x77c13df68650), current thread is QThread(0x77c13dfb10f0)
```


## Draft: typing in the note editor, Android ends Qupil, back

- tapped `^OK$` at 910 1579
- tapped `^Notiz$` at 312 1768
- screenshot `shots/05-editor-typing.png`
- screenshot `shots/06-draft-back.png`
- the draft is back in the editor
- process: `2525`
- top: `topResumedActivity=ActivityRecord{1dd9753 u0 de.qupil.app/.QupilActivity} t11}`

```
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de096410), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de097610), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de097190), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de096bf0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de096e30), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de098ff0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09bf30), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de099e90), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09a0d0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de099d70), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09ae50), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09c4d0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09d6d0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09e0f0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09ed50), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.139  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.139  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09f9b0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.139  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.139  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09f650), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.139  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.139  2525  2525 W default : (Parent is CheckBox_QMLTYPE_66(0x77c1de0a1ed0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.139  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.139  2525  2525 W default : (Parent is CheckBox_QMLTYPE_66(0x77c1de071c30), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.139  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.139  2525  2525 W default : (Parent is CheckBox_QMLTYPE_66(0x77c1de0ae4d0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
```

- screenshot `shots/07-editor-left.png`
- process: `2525`
- top: `topResumedActivity=ActivityRecord{a6f0280 u0 com.android.fakesystemapp/.launcher.EmptyHomeActivity} t12}`

```
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de096bf0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de096e30), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de098ff0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09bf30), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de099e90), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09a0d0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de099d70), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09ae50), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09c4d0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09d6d0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09e0f0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.138  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.138  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09ed50), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.139  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.139  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09f9b0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.139  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.139  2525  2525 W default : (Parent is AbstractButton_QMLTYPE_21_QML_111(0x77c1de09f650), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.139  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.139  2525  2525 W default : (Parent is CheckBox_QMLTYPE_66(0x77c1de0a1ed0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.139  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.139  2525  2525 W default : (Parent is CheckBox_QMLTYPE_66(0x77c1de071c30), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:54.139  2525  2525 W default : QObject: Cannot create children for a parent that is in a different thread.
14:27:54.139  2525  2525 W default : (Parent is CheckBox_QMLTYPE_66(0x77c1de0ae4d0), parent's thread is QThread(0x77c13df687d0), current thread is QThread(0x77c13dfbbff0)
14:27:58.328  2525  2525 I Qupil   : activity: focus false
14:27:58.334  2525  2543 I Qupil   : main: application state 2
14:27:58.416  2525  2543 I default : Draft note: kept, 230 characters
14:27:58.446  2525  2525 I Qupil   : activity: stop
14:27:58.446  2525  2543 I Qupil   : main: application state 0
14:27:58.448  2525  2543 I default : Draft note: kept, 230 characters
```


## Dark mode switched on while Qupil runs

- screenshot `shots/08-dark-runtime.png`
- screenshot `shots/09-dark-back.png`
- process: `2525`
- top: `topResumedActivity=ActivityRecord{1dd9753 u0 de.qupil.app/.QupilActivity} t11}`

```
14:28:13.821  2525  2525 I Qupil   : activity: start
14:28:13.822  2525  2525 I Qupil   : activity: resume, content ready true
14:28:13.848  2525  2525 I Qupil   : activity: focus true
14:28:13.866  2525  2546 W default : makeCurrent(): no EGLSurface, likely Surface destroyed by Android.
14:28:13.866  2525  2546 W default : QRhiGles2: Failed to make context current. Expect bad things to happen.
14:28:13.866  2525  2546 W default : Failed to start frame
14:28:13.866  2525  2543 I Qupil   : main: application state 4
```


## Fresh start in dark mode

- screenshot `shots/10-dark-start.png`
- screenshot `shots/11-light-again.png`
- process: `2729`
- top: `topResumedActivity=ActivityRecord{f33293d u0 de.qupil.app/.QupilActivity} t13}`

```
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de098390), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de0984b0), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de097970), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de096650), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de095330), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de09a9d0), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de098ed0), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de099fb0), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de09ad30), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de09b1b0), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de099110), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de0998f0), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de09c290), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de09ef90), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de09c950), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de0a0070), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is AbstractButton_QMLTYPE_11_QML_63(0x77c1de09f1d0), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is CheckBox_QMLTYPE_90(0x77c1de0802d0), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is CheckBox_QMLTYPE_90(0x77c1de06e1b0), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
14:28:26.646  2729  2729 W default : QObject: Cannot create children for a parent that is in a different thread.
14:28:26.646  2729  2729 W default : (Parent is CheckBox_QMLTYPE_90(0x77c1de0ae4d0), parent's thread is QThread(0x77c13df68590), current thread is QThread(0x77c13dfafe10)
```


## Landscape

- screenshot `shots/12-landscape.png`
- screenshot `shots/13-portrait-again.png`
- process: `2729`
- top: `topResumedActivity=ActivityRecord{f33293d u0 de.qupil.app/.QupilActivity} t13}`

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
- process: `3205`
- top: `topResumedActivity=ActivityRecord{eb89523 u0 de.qupil.app/.QupilActivity} t15}`

```
14:29:35.310  3205  3205 I Qupil   : activity: focus true
14:29:35.442  3205  3225 I Qupil   : main: application state 4
14:29:39.965  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.965  3205  3205 W default : (Parent is QQuickPopupItem(0x77c14dfffe50), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.965  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.965  3205  3205 W default : (Parent is ProgressBar_QMLTYPE_207(0x77c1ddf99450), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.965  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.965  3205  3205 W default : (Parent is QQuickPopupItem(0x77c14dfff6d0), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.965  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.965  3205  3205 W default : (Parent is QQuickPopupItem(0x77c14e0017a0), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.965  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.965  3205  3205 W default : (Parent is QQuickPopupItem(0x77c14e003480), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.965  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.965  3205  3205 W default : (Parent is QQuickPopupItem(0x77c14e004200), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.966  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.966  3205  3205 W default : (Parent is QQuickPopupItem(0x77c14e0067b0), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.966  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.966  3205  3205 W default : (Parent is QQuickPopupItem(0x77c14e007a10), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.966  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.966  3205  3205 W default : (Parent is QQuickPopupItem(0x77c14e020010), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.966  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.966  3205  3205 W default : (Parent is StackView_QMLTYPE_106(0x77c1ddfcb790), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.966  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.966  3205  3205 W default : (Parent is QQuickPopupItem(0x77c14e021f00), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.966  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.966  3205  3205 W default : (Parent is QQuickPopupItem(0x77c14e022290), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.966  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.966  3205  3205 W default : (Parent is ScrollIndicator_QMLTYPE_50(0x77c1ddfd1730), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.966  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.966  3205  3205 W default : (Parent is QQuickPopupItem(0x77c14e023d60), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.966  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.966  3205  3205 W default : (Parent is ScrollBar_QMLTYPE_12(0x77c1ddfdc6b0), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.966  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.966  3205  3205 W default : (Parent is QQuickPopupItem(0x77c14e022530), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.966  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.966  3205  3205 W default : (Parent is CheckBox_QMLTYPE_146(0x77c1ddfc95d0), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.966  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.966  3205  3205 W default : (Parent is CheckBox_QMLTYPE_146(0x77c1de04c370), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
14:29:39.966  3205  3205 W default : QObject: Cannot create children for a parent that is in a different thread.
14:29:39.966  3205  3205 W default : (Parent is CheckBox_QMLTYPE_146(0x77c1de0506f0), parent's thread is QThread(0x77c13df68970), current thread is QThread(0x77c13df5d870)
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
- double tap: 174 ms for both;double tap: 178 ms for both;double tap: 183 ms for both;
- process: `3473`
- top: `topResumedActivity=ActivityRecord{3abada u0 de.qupil.app/.QupilActivity} t16}`

```
14:31:19.387  3473  3491 I default : select: sel 115 115 
14:31:19.387  3473  3491 I default : select: cursor 115
14:31:19.587  3473  3491 I default : select: sel 111 121 Mittelteil
14:31:19.587  3473  3491 I default : select: cursor 121
14:31:19.590  3473  3491 I default : select: menu Ausschneiden, Kopieren, Einsetzen, Alles auswählen
14:31:19.591  3473  3491 I default : select: button Ausschneiden 69.1 193.0
14:31:19.592  3473  3491 I default : select: button Kopieren 175.2 193.0
14:31:19.592  3473  3491 I default : select: button Einsetzen 266.8 193.0
14:31:19.594  3473  3491 I default : select: handle start 288.2 232.0
14:31:19.594  3473  3491 I default : select: handle end 351.7 266.0
14:31:20.598  3473  3491 I default : select: menu 
14:31:20.604  3473  3491 I default : select: markup **Tonleitern** ++G-Dur und D-Dur++ langsam üben / Etüde Nr. 3 bis Takt 16 mit *Metronom* / Vortragsstück: Bogeneinteilung im ==Mittelteil==
14:31:23.987  3473  3491 I default : select: sel 121 121 
14:31:23.994  3473  3491 I default : select: handle start -
14:31:23.995  3473  3491 I default : select: handle end -
14:31:26.860  3473  3491 I default : select: sel 40 45 Etüde
14:31:26.865  3473  3491 I default : select: cursor 45
14:31:26.871  3473  3491 I default : select: menu Ausschneiden, Kopieren, Einsetzen, Alles auswählen
14:31:26.872  3473  3491 I default : select: button Ausschneiden 69.1 161.0
14:31:26.876  3473  3491 I default : select: button Kopieren 175.2 161.0
14:31:26.877  3473  3491 I default : select: button Einsetzen 266.8 161.0
14:31:26.881  3473  3491 I default : select: handle start 38.0 200.0
14:31:26.886  3473  3491 I default : select: handle end 80.5 234.0
14:31:28.398  3473  3491 I default : select: menu 
14:31:31.590  3473  3491 I default : select: sel 121 121 
14:31:31.590  3473  3491 I default : select: cursor 121
14:31:31.596  3473  3491 I default : select: handle start -
14:31:31.597  3473  3491 I default : select: handle end -
14:31:32.790  3473  3491 I default : select: menu Auswählen, Einsetzen, Alles auswählen
14:31:32.792  3473  3491 I default : select: button Einsetzen 221.1 207.0
14:31:32.793  3473  3491 I default : select: button Auswählen 122.8 207.0
14:31:38.385  3473  3491 I default : select: word Mittelteil 38.0 259.0 102.5 279.0
14:31:38.387  3473  3491 I default : select: sel 126 126 
14:31:38.387  3473  3491 I default : select: cursor 126
14:31:38.390  3473  3491 I default : select: menu 
14:31:38.394  3473  3491 I default : select: markup **Tonleitern** ++G-Dur und D-Dur++ langsam üben / Etüde Nr. 3 bis Takt 16 mit *Metronom* / Vortragsstück: Bogeneinteilung im ==Mittelteil==Etüde
14:31:43.036  3473  3491 I default : select: sel 107 107 
14:31:43.036  3473  3491 I default : select: cursor 107
14:31:46.388  3473  3491 I default : select: sel 122 122 
14:31:46.388  3473  3491 I default : select: cursor 122
```


## Crashes and hangs during the whole run

- none

Result: all steps ran
