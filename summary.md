# Android test run 22 (08413b7)

- device: Android SDK built for x86_64, Android 13, 1080x2400, 420 dpi
- date on the device: Tue Nov  3 14:20:00 CET 2026

## Qupil's own UI test (inside the app) and its restart half

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.6 s |  |
| ✅ | pages – every page opens | 7.4 s |  |
| ✅ | today – today with the running lesson, switching days | 2.5 s |  |
| ✅ | note-save – write and save a note for the running lesson | 6.8 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 6.2 s |  |
| ✅ | student-search – search a student, open and close the student page | 7.8 s |  |
| ✅ | student-new – add a student | 10.1 s |  |

7 of 7 cases passed.

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 4.6 s |  |
| ✅ | event-open – open an event and go back | 3.1 s |  |
| ✅ | metronome – start and stop the metronome | 2.4 s |  |
| ✅ | appearance – switch to dark and back to light | 3.9 s |  |
| ✅ | language – switch the language to English and back | 5.4 s |  |
| ✅ | reminders – open the reminders | 3.7 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 6.3 s |  |

7 of 7 cases passed.

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 4.6 s |  |
| ✅ | plan-save – plan the next lesson of a student | 11.4 s |  |
| ✅ | piece-new – add a piece for a student | 15.0 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 4.3 s |  |
| ✅ | together – two individual lessons of a student kept together | 12.1 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 9.3 s |  |
| ✅ | reminder-new – add a reminder | 7.5 s |  |
| ✅ | more-pages – birthdays and data pages open | 4.2 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 5.0 s |  |

9 of 9 cases passed.

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | restart-draft – after a restart the unsaved note is in the editor again | 3.2 s |  |

1 of 1 cases passed.


## Start with sample data

- screenshot `shots/01-start-today.png`
- process: `2035`
- top: `topResumedActivity=ActivityRecord{22351b8 u0 de.qupil.app/.QupilActivity} t9}`

```
14:23:48.383  2035  2053 I Qupil   : main: window visible 1, exposed 0
14:23:48.453  2035  2056 I Qupil   : main: graphics ready
14:23:48.772  2035  2053 I Qupil   : main: event loop
14:23:48.800  2035  2053 I Qupil   : main: sample data loaded
14:23:48.885  2035  2053 I Qupil   : main: first frame
14:23:48.885  2035  2053 I Qupil   : activity: content ready
14:23:48.914  2035  2035 I Qupil   : activity: focus true
14:23:49.310  2035  2053 I Qupil   : main: application state 4
14:23:54.448  2035  2035 W default : QObject: Cannot create children for a parent that is in a different thread.
14:23:54.448  2035  2035 W default : (Parent is QQuickPopupItem(0x797337341470), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x79732729d4d0)
14:23:54.449  2035  2035 W default : QObject: Cannot create children for a parent that is in a different thread.
14:23:54.449  2035  2035 W default : (Parent is ProgressBar_QMLTYPE_6(0x7973c72cd850), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x79732729d4d0)
14:23:54.449  2035  2035 W default : QObject: Cannot create children for a parent that is in a different thread.
14:23:54.449  2035  2035 W default : (Parent is QQuickPopupItem(0x797337341cb0), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x79732729d4d0)
14:23:54.450  2035  2035 W default : QObject: Cannot create children for a parent that is in a different thread.
14:23:54.450  2035  2035 W default : (Parent is QQuickPopupItem(0x797337342760), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x79732729d4d0)
14:23:54.450  2035  2035 W default : QObject: Cannot create children for a parent that is in a different thread.
14:23:54.450  2035  2035 W default : (Parent is QQuickPopupItem(0x797337344740), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x79732729d4d0)
14:23:54.450  2035  2035 W default : QObject: Cannot create children for a parent that is in a different thread.
14:23:54.450  2035  2035 W default : (Parent is QQuickPopupItem(0x7973373459d0), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x79732729d4d0)
14:23:54.450  2035  2035 W default : QObject: Cannot create children for a parent that is in a different thread.
14:23:54.450  2035  2035 W default : (Parent is QQuickPopupItem(0x797337348280), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x79732729d4d0)
14:23:54.450  2035  2035 W default : QObject: Cannot create children for a parent that is in a different thread.
14:23:54.450  2035  2035 W default : (Parent is QQuickPopupItem(0x797337348310), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x79732729d4d0)
14:23:54.450  2035  2035 W default : QObject: Cannot create children for a parent that is in a different thread.
14:23:54.450  2035  2035 W default : (Parent is QQuickPopupItem(0x797337384fa0), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x79732729d4d0)
14:23:54.452  2035  2035 W default : QObject: Cannot create children for a parent that is in a different thread.
14:23:54.452  2035  2035 W default : (Parent is StackView_QMLTYPE_88(0x7973c7341650), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x79732729d4d0)
14:23:54.452  2035  2035 W default : QObject: Cannot create children for a parent that is in a different thread.
14:23:54.452  2035  2035 W default : (Parent is QQuickPopupItem(0x797337386c50), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x79732729d4d0)
14:23:54.453  2035  2035 W default : QObject: Cannot create children for a parent that is in a different thread.
14:23:54.453  2035  2035 W default : (Parent is QQuickPopupItem(0x7973373868c0), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x79732729d4d0)
14:23:54.453  2035  2035 W default : QObject: Cannot create children for a parent that is in a different thread.
14:23:54.453  2035  2035 W default : (Parent is ScrollIndicator_QMLTYPE_30(0x7973c73458b0), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x79732729d4d0)
14:23:54.453  2035  2035 W default : QObject: Cannot create children for a parent that is in a different thread.
14:23:54.453  2035  2035 W default : (Parent is QQuickPopupItem(0x797337388c00), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x79732729d4d0)
14:23:54.454  2035  2035 W default : QObject: Cannot create children for a parent that is in a different thread.
14:23:54.454  2035  2035 W default : (Parent is ScrollBar_QMLTYPE_17(0x7973c7350b90), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x79732729d4d0)
14:23:54.454  2035  2035 W default : QObject: Cannot create children for a parent that is in a different thread.
14:23:54.454  2035  2035 W default : (Parent is QQuickPopupItem(0x7973373867d0), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x79732729d4d0)
```


## To the background and back (process stays)

- screenshot `shots/02-home-screen.png`
- screenshot `shots/03-back.png`
- process: `2035`
- top: `topResumedActivity=ActivityRecord{22351b8 u0 de.qupil.app/.QupilActivity} t9}`

```
14:23:56.526  2035  2053 I Qupil   : main: application state 2
14:23:56.624  2035  2035 I Qupil   : activity: focus false
14:23:56.734  2035  2035 I Qupil   : activity: stop
14:23:56.735  2035  2053 I Qupil   : main: application state 0
14:24:01.958  2035  2035 I Qupil   : activity: start
14:24:01.959  2035  2035 I Qupil   : activity: resume, content ready true
14:24:02.057  2035  2035 I Qupil   : activity: focus true
14:24:02.209  2035  2053 I Qupil   : main: application state 4
```


## Android ends Qupil in the background, then back

- process after ending: `` (was 2035)
- screenshot `shots/04-after-process-end.png`
- process: `2210`
- top: `topResumedActivity=ActivityRecord{a21cbb2 u0 de.qupil.app/.QupilActivity} t9}`

```
14:24:14.862  2210  2210 I Qupil   : activity: focus true
14:24:15.077  2210  2228 I Qupil   : main: application state 4
14:24:19.268  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.268  2210  2210 W default : (Parent is QQuickPopupItem(0x797337339df0), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.269  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.269  2210  2210 W default : (Parent is ProgressBar_QMLTYPE_80(0x7973c72caeb0), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.269  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.269  2210  2210 W default : (Parent is QQuickPopupItem(0x79733733a4b0), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.269  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.269  2210  2210 W default : (Parent is QQuickPopupItem(0x79733733bd70), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.269  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.269  2210  2210 W default : (Parent is QQuickPopupItem(0x79733733dcc0), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.269  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.269  2210  2210 W default : (Parent is QQuickPopupItem(0x79733733eb60), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.269  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.269  2210  2210 W default : (Parent is QQuickPopupItem(0x797337340bd0), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.270  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.270  2210  2210 W default : (Parent is QQuickPopupItem(0x797337341980), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.270  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.270  2210  2210 W default : (Parent is QQuickPopupItem(0x797337359f20), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.270  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.270  2210  2210 W default : (Parent is StackView_QMLTYPE_88(0x7973c72f8d50), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.270  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.270  2210  2210 W default : (Parent is QQuickPopupItem(0x79733735bf90), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.270  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.270  2210  2210 W default : (Parent is QQuickPopupItem(0x79733735bd20), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.270  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.270  2210  2210 W default : (Parent is ScrollIndicator_QMLTYPE_9(0x7973c72ffa70), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.270  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.270  2210  2210 W default : (Parent is QQuickPopupItem(0x79733735e420), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.270  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.270  2210  2210 W default : (Parent is ScrollBar_QMLTYPE_31(0x7973c7308710), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.270  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.270  2210  2210 W default : (Parent is QQuickPopupItem(0x79733735c470), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.275  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.275  2210  2210 W default : (Parent is CheckBox_QMLTYPE_111(0x7973c734d590), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.276  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.276  2210  2210 W default : (Parent is CheckBox_QMLTYPE_111(0x7973c737e910), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
14:24:19.276  2210  2210 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:19.276  2210  2210 W default : (Parent is CheckBox_QMLTYPE_111(0x7973c737d5f0), parent's thread is QThread(0x7973272a27f0), current thread is QThread(0x7973272ea7f0)
```


## Draft: typing in the note editor, Android ends Qupil, back

- tapped `^OK$` at 910 1579
- tapped `^Notiz$` at 312 1768
- screenshot `shots/05-editor-typing.png`
- screenshot `shots/06-draft-back.png`
- the draft is back in the editor
- process: `2385`
- top: `topResumedActivity=ActivityRecord{187fb76 u0 de.qupil.app/.QupilActivity} t9}`

```
14:24:49.071  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.071  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c2e90), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.071  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.071  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c4750), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.071  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.071  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c6130), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.071  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.071  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c5170), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.071  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.071  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c5830), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.071  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.071  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c5950), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c4bd0), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c67f0), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c6eb0), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73cab70), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c8650), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c8890), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c8410), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73ca5d0), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c81d0), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73ca930), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73cb6b0), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.073  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.073  2385  2385 W default : (Parent is CheckBox_QMLTYPE_112(0x7973c73c8f50), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.073  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.073  2385  2385 W default : (Parent is CheckBox_QMLTYPE_112(0x7973c73d87f0), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.073  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.073  2385  2385 W default : (Parent is CheckBox_QMLTYPE_112(0x7973c73d7ef0), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
```

- screenshot `shots/07-editor-left.png`
- process: `2385`
- top: `topResumedActivity=ActivityRecord{73fc0d4 u0 com.android.fakesystemapp/.launcher.EmptyHomeActivity} t10}`

```
14:24:49.071  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.071  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c5170), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.071  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.071  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c5830), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.071  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.071  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c5950), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c4bd0), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c67f0), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c6eb0), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73cab70), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c8650), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c8890), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c8410), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73ca5d0), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73c81d0), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73ca930), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.072  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.072  2385  2385 W default : (Parent is AbstractButton_QMLTYPE_12_QML_39(0x7973c73cb6b0), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.073  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.073  2385  2385 W default : (Parent is CheckBox_QMLTYPE_112(0x7973c73c8f50), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.073  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.073  2385  2385 W default : (Parent is CheckBox_QMLTYPE_112(0x7973c73d87f0), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:49.073  2385  2385 W default : QObject: Cannot create children for a parent that is in a different thread.
14:24:49.073  2385  2385 W default : (Parent is CheckBox_QMLTYPE_112(0x7973c73d7ef0), parent's thread is QThread(0x7973272a2630), current thread is QThread(0x79732729a0d0)
14:24:53.470  2385  2403 I Qupil   : main: application state 2
14:24:53.539  2385  2385 I Qupil   : activity: focus false
14:24:53.588  2385  2403 I default : Draft note: kept, 230 characters
14:24:53.591  2385  2385 I Qupil   : activity: stop
14:24:53.594  2385  2403 I Qupil   : main: application state 0
14:24:53.598  2385  2403 I default : Draft note: kept, 230 characters
```


## Dark mode switched on while Qupil runs

- screenshot `shots/08-dark-runtime.png`
- screenshot `shots/09-dark-back.png`
- process: `2385`
- top: `topResumedActivity=ActivityRecord{187fb76 u0 de.qupil.app/.QupilActivity} t9}`

```
14:25:09.262  2385  2385 I Qupil   : activity: start
14:25:09.262  2385  2385 I Qupil   : activity: resume, content ready true
14:25:09.291  2385  2385 I Qupil   : activity: focus true
14:25:09.325  2385  2405 W default : makeCurrent(): no EGLSurface, likely Surface destroyed by Android.
14:25:09.325  2385  2405 W default : QRhiGles2: Failed to make context current. Expect bad things to happen.
14:25:09.325  2385  2405 W default : Failed to start frame
14:25:09.325  2385  2403 I Qupil   : main: application state 4
```


## Fresh start in dark mode

- screenshot `shots/10-dark-start.png`
- screenshot `shots/11-light-again.png`
- process: `2590`
- top: `topResumedActivity=ActivityRecord{5e6da7 u0 de.qupil.app/.QupilActivity} t11}`

```
14:25:22.440  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.440  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73c4cf0), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.440  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.440  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73c4090), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.440  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.440  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73c4f30), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.440  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.440  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73c5ef0), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.440  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.440  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73c6d90), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.440  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.440  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73c5dd0), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.440  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.440  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73c41b0), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.440  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.440  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73c65b0), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.440  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.440  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73c9970), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.440  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.440  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73c7f90), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.440  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.440  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73c9070), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.441  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.441  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73c7450), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.441  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.441  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73c9df0), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.441  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.441  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73ca270), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.441  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.441  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73c7b10), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.441  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.441  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73c94f0), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.441  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.441  2590  2590 W default : (Parent is AbstractButton_QMLTYPE_8_QML_23(0x7973c73cdbd0), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.441  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.441  2590  2590 W default : (Parent is CheckBox_QMLTYPE_129(0x7973c73a38d0), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.444  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.444  2590  2590 W default : (Parent is CheckBox_QMLTYPE_129(0x7973c73d7a70), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
14:25:22.444  2590  2590 W default : QObject: Cannot create children for a parent that is in a different thread.
14:25:22.444  2590  2590 W default : (Parent is CheckBox_QMLTYPE_129(0x7973c73d9c30), parent's thread is QThread(0x79732729ccd0), current thread is QThread(0x7973272f53b0)
```


## Landscape

- screenshot `shots/12-landscape.png`
- screenshot `shots/13-portrait-again.png`
- process: `2590`
- top: `topResumedActivity=ActivityRecord{5e6da7 u0 de.qupil.app/.QupilActivity} t11}`

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
- process: `3066`
- top: `topResumedActivity=ActivityRecord{1b778e5 u0 de.qupil.app/.QupilActivity} t13}`

```
14:26:33.348  3066  3066 I Qupil   : activity: focus true
14:26:33.490  3066  3084 I Qupil   : main: application state 4
14:26:37.884  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.884  3066  3066 W default : (Parent is QQuickPopupItem(0x797337339df0), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.885  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.885  3066  3066 W default : (Parent is ProgressBar_QMLTYPE_128(0x7973c72cbd50), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.885  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.885  3066  3066 W default : (Parent is QQuickPopupItem(0x79733733a4b0), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.885  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.885  3066  3066 W default : (Parent is QQuickPopupItem(0x79733733bd70), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.886  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.886  3066  3066 W default : (Parent is QQuickPopupItem(0x79733733dcc0), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.886  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.886  3066  3066 W default : (Parent is QQuickPopupItem(0x79733733eb60), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.886  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.886  3066  3066 W default : (Parent is QQuickPopupItem(0x797337340bd0), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.886  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.886  3066  3066 W default : (Parent is QQuickPopupItem(0x797337341980), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.886  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.886  3066  3066 W default : (Parent is QQuickPopupItem(0x797337359da0), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.887  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.887  3066  3066 W default : (Parent is StackView_QMLTYPE_99(0x7973c72fca10), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.899  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.899  3066  3066 W default : (Parent is QQuickPopupItem(0x79733735c020), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.899  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.899  3066  3066 W default : (Parent is QQuickPopupItem(0x79733735b5a0), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.899  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.899  3066  3066 W default : (Parent is ScrollIndicator_QMLTYPE_28(0x7973c72fd9d0), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.899  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.899  3066  3066 W default : (Parent is QQuickPopupItem(0x79733735ddc0), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.899  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.899  3066  3066 W default : (Parent is ScrollBar_QMLTYPE_8(0x7973c7307e10), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.899  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.899  3066  3066 W default : (Parent is QQuickPopupItem(0x79733735bab0), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.900  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.900  3066  3066 W default : (Parent is CheckBox_QMLTYPE_34(0x7973c7348910), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.900  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.900  3066  3066 W default : (Parent is CheckBox_QMLTYPE_34(0x7973c737ccf0), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
14:26:37.900  3066  3066 W default : QObject: Cannot create children for a parent that is in a different thread.
14:26:37.900  3066  3066 W default : (Parent is CheckBox_QMLTYPE_34(0x7973c737c870), parent's thread is QThread(0x79732729ceb0), current thread is QThread(0x79732729d4d0)
```

- tapped `^OK$` at 910 1579
- tapped `^Mehr$` at 962 2258
- tapped `^Mehr$`, `^Einstellungen$` is there
- tapped `^Einstellungen$` at 968 290
- tapped `^Einstellungen$`, `^Darstellung` is there
- tapped `^Darstellung` at 539 975
- tapped `^Darstellung`, `^System$` is there
- tapped `^System$` at 280 700

## Crashes and hangs during the whole run

- none

Result: all steps ran
