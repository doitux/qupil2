# Android test run 100 (ea506c5)

- device: Android SDK built for x86_64, Android 13, 1080x2400, 420 dpi
- date on the device: Tue Nov 24 14:20:00 CET 2026

## Qupil's own UI test (inside the app) and its restart half

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 10.6 s |  |
| ✅ | pages – every page opens | 7.3 s |  |
| ✅ | today – today with the running lesson, switching days | 2.5 s |  |
| ✅ | note-save – write and save a note for the running lesson | 6.5 s |  |
| ✅ | note-cancel – cancel a note: nothing saved, no draft | 6.1 s |  |
| ✅ | keyboard-smooth – the keyboard comes in an open editor: the editor makes room smoothly | 15.7 s | No on-screen keyboard came: not measured.  |
| ✅ | note-touch-select – double tap with a finger selects a word, B makes it bold | 9.6 s |  |
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
| ✅ | text-tap-selection – finger: a tap into the selection hides/shows the menu, outside ends it | 12.4 s |  |

7 of 7 cases passed.

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 4.6 s |  |
| ✅ | text-menu – finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden | 10.3 s |  |
| ✅ | text-menu-paste-unasked – finger: Einsetzen is offered without reading the clipboard; empty, it changes nothing | 7.1 s |  |
| ✅ | text-wipe – finger: wiping over the text scrolls and selects nothing | 5.7 s |  |
| ✅ | text-field – finger: the same in a one-line field (title of a piece) | 14.9 s |  |
| ✅ | text-field-wipe – finger: wiping sideways moves a long text in a one-line field; without focus it shows its start | 25.4 s |  |
| ✅ | text-mouse – mouse: double click at the start of a paragraph takes the word; B, B again; triple click | 6.2 s |  |

7 of 7 cases passed.

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 4.6 s |  |
| ✅ | event-open – open an event and go back | 2.9 s |  |
| ✅ | metronome – start and stop the metronome | 3.9 s |  |
| ✅ | appearance – switch to dark and back to light | 3.5 s |  |
| ✅ | language – switch the language to English and back | 4.9 s |  |
| ✅ | reminders – open the reminders | 3.1 s |  |
| ✅ | test-mode – switch the test mode on (7 taps on the version) and off | 6.0 s |  |

7 of 7 cases passed.

- platform: Android 13 (13.0) (android), Qt 6.8.3
- window: 411 × 866

| | Case | Time | Failure / note |
|---|---|---|---|
| ✅ | start – close the reminders shown at the start | 4.7 s |  |
| ✅ | note-menu – a note: a tap only shows a block, the menu has new note, plan, edit, share, delete | 17.5 s |  |
| ✅ | tasks-menu – tasks of a coupled student: a tap shows the hints, the menu plans, edits and deletes | 14.1 s |  |
| ✅ | idea-demo – The trial of the new idea: a lesson, the history, preparing, pausing; nothing is stored | 64.4 s |  |
| ✅ | idea-point-wrap – trial: a long point wraps in its field while it is written | 11.8 s |  |
| ✅ | idea-fold-arrow – trial: the arrow of a task unfolds it in the window of a lesson | 6.5 s |  |
| ✅ | plan-save – plan the next lesson of a student | 10.8 s |  |
| ✅ | plan-finish-lines – what was covered: single lines of a block into the note, the rest planned | 11.0 s |  |
| ✅ | plan-from-note-lines – plan the next lesson from a note: one line of a block | 8.7 s |  |
| ✅ | note-share – share a note as a message (homework for a student without the app) | 8.1 s |  |
| ✅ | tasks-long-texts – tasks: a long hint wraps in its field, a long title is wiped sideways and shows its start | 31.0 s |  |
| ✅ | card-long-texts – student page: a long private note and a long block heading wrap instead of being cut | 7.7 s |  |
| ✅ | plan-icon-coming – a plan for next week shows in the schedule of this week, next to a birthday too | 3.4 s |  |
| ✅ | piece-new – add a piece for a student | 14.7 s |  |
| ✅ | lesson-new – add a lesson from the schedule | 3.6 s |  |
| ✅ | lesson-times – a lesson of 11 minutes, times typed and saved at once | 6.4 s |  |
| ✅ | together – two individual lessons of a student kept together | 11.3 s |  |
| ✅ | together-new – new lesson for a student who has one: kept together | 9.1 s |  |
| ✅ | reminder-new – add a reminder | 7.0 s |  |
| ✅ | lesson-plan-for-all – plan for a whole ensemble from the lesson view | 9.9 s |  |
| ✅ | lesson-reminder – reminders for one lesson | 26.2 s |  |
| ✅ | loan-return – return sheet music from the student's page | 8.8 s |  |
| ✅ | more-pages – birthdays and data pages open | 3.1 s |  |
| ✅ | alert-volume – volume of alert sounds (iOS: hint to the system setting) | 2.8 s |  |
| ✅ | restart-prepare – an unsaved note when Qupil quits | 5.2 s |  |

25 of 25 cases passed.

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
| ✅ | start – close the reminders shown at the start | 4.7 s |  |
| ✅ | reset – reset Qupil on this device, then restore the data from before | 8.1 s |  |
| ✅ | update-hint – changes from a newer Qupil: More asks for an update | 2.3 s |  |

3 of 3 cases passed.


## Start with sample data

- screenshot `shots/01-start-today.png`
- process: `2260`
- top: `topResumedActivity=ActivityRecord{11c79cd u0 de.qupil.app/.QupilActivity} t13}`

```
14:33:27.916  2260  2278 I Qupil   : main: window visible 1, exposed 0
14:33:27.960  2260  2282 I Qupil   : main: graphics ready
14:33:28.210  2260  2278 I Qupil   : main: event loop
14:33:28.242  2260  2278 I Qupil   : main: sample data loaded
14:33:28.302  2260  2278 I Qupil   : main: first frame
14:33:28.302  2260  2278 I Qupil   : activity: content ready
14:33:28.341  2260  2260 I Qupil   : activity: focus true
14:33:28.534  2260  2278 I Qupil   : main: application state 4
14:33:33.860  2260  2260 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:33.860  2260  2260 W default : (Parent is QQuickPopupItem(0x750295f0f200), parent's thread is QThread(0x750285e616d0), current thread is QThread(0x750285ead630)
14:33:33.860  2260  2260 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:33.860  2260  2260 W default : (Parent is ProgressBar_QMLTYPE_25(0x750325e9c950), parent's thread is QThread(0x750285e616d0), current thread is QThread(0x750285ead630)
14:33:33.861  2260  2260 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:33.861  2260  2260 W default : (Parent is QQuickPopupItem(0x750295f0e5a0), parent's thread is QThread(0x750285e616d0), current thread is QThread(0x750285ead630)
14:33:33.861  2260  2260 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:33.861  2260  2260 W default : (Parent is QQuickPopupItem(0x750295f10460), parent's thread is QThread(0x750285e616d0), current thread is QThread(0x750285ead630)
14:33:33.861  2260  2260 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:33.861  2260  2260 W default : (Parent is QQuickPopupItem(0x750295f122c0), parent's thread is QThread(0x750285e616d0), current thread is QThread(0x750285ead630)
14:33:33.861  2260  2260 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:33.861  2260  2260 W default : (Parent is QQuickPopupItem(0x750295f13670), parent's thread is QThread(0x750285e616d0), current thread is QThread(0x750285ead630)
14:33:33.861  2260  2260 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:33.861  2260  2260 W default : (Parent is QQuickPopupItem(0x750295f15920), parent's thread is QThread(0x750285e616d0), current thread is QThread(0x750285ead630)
14:33:33.861  2260  2260 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:33.861  2260  2260 W default : (Parent is QQuickPopupItem(0x750295f16760), parent's thread is QThread(0x750285e616d0), current thread is QThread(0x750285ead630)
14:33:33.861  2260  2260 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:33.861  2260  2260 W default : (Parent is QQuickPopupItem(0x750295f53ed0), parent's thread is QThread(0x750285e616d0), current thread is QThread(0x750285ead630)
14:33:33.861  2260  2260 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:33.861  2260  2260 W default : (Parent is StackView_QMLTYPE_29(0x750325f16c90), parent's thread is QThread(0x750285e616d0), current thread is QThread(0x750285ead630)
14:33:33.861  2260  2260 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:33.861  2260  2260 W default : (Parent is QQuickPopupItem(0x750295f567b0), parent's thread is QThread(0x750285e616d0), current thread is QThread(0x750285ead630)
14:33:33.861  2260  2260 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:33.861  2260  2260 W default : (Parent is QQuickPopupItem(0x750295f55970), parent's thread is QThread(0x750285e616d0), current thread is QThread(0x750285ead630)
14:33:33.861  2260  2260 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:33.861  2260  2260 W default : (Parent is ScrollIndicator_QMLTYPE_40(0x750325f18c10), parent's thread is QThread(0x750285e616d0), current thread is QThread(0x750285ead630)
14:33:33.861  2260  2260 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:33.861  2260  2260 W default : (Parent is QQuickPopupItem(0x750295f58160), parent's thread is QThread(0x750285e616d0), current thread is QThread(0x750285ead630)
14:33:33.861  2260  2260 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:33.861  2260  2260 W default : (Parent is ScrollBar_QMLTYPE_23(0x750325f21c10), parent's thread is QThread(0x750285e616d0), current thread is QThread(0x750285ead630)
14:33:33.861  2260  2260 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:33.861  2260  2260 W default : (Parent is QQuickPopupItem(0x750295f55e50), parent's thread is QThread(0x750285e616d0), current thread is QThread(0x750285ead630)
```


## To the background and back (process stays)

- screenshot `shots/02-home-screen.png`
- screenshot `shots/03-back.png`
- process: `2260`
- top: `topResumedActivity=ActivityRecord{11c79cd u0 de.qupil.app/.QupilActivity} t13}`

```
14:33:35.881  2260  2278 I Qupil   : main: application state 2
14:33:35.886  2260  2260 I Qupil   : activity: focus false
14:33:35.937  2260  2260 I Qupil   : activity: stop
14:33:35.938  2260  2278 I Qupil   : main: application state 0
14:33:41.125  2260  2260 I Qupil   : activity: start
14:33:41.125  2260  2260 I Qupil   : activity: resume, content ready true
14:33:41.175  2260  2260 I Qupil   : activity: focus true
14:33:41.243  2260  2278 I Qupil   : main: application state 4
```


## Android ends Qupil in the background, then back

- process after ending: `` (was 2260)
- screenshot `shots/04-after-process-end.png`
- process: `2439`
- top: `topResumedActivity=ActivityRecord{7390a5f u0 de.qupil.app/.QupilActivity} t13}`

```
14:33:53.570  2439  2439 I Qupil   : activity: focus true
14:33:53.650  2439  2456 I Qupil   : main: application state 4
14:33:58.040  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.040  2439  2439 W default : (Parent is QQuickPopupItem(0x750295f06e60), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.041  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.041  2439  2439 W default : (Parent is ProgressBar_QMLTYPE_227(0x750325e9cef0), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.041  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.041  2439  2439 W default : (Parent is QQuickPopupItem(0x750295f06f50), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.041  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.041  2439  2439 W default : (Parent is QQuickPopupItem(0x750295f08de0), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.041  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.041  2439  2439 W default : (Parent is QQuickPopupItem(0x750295f0ac10), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.041  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.041  2439  2439 W default : (Parent is QQuickPopupItem(0x750295f0bb40), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.041  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.041  2439  2439 W default : (Parent is QQuickPopupItem(0x750295f0d8b0), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.041  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.041  2439  2439 W default : (Parent is QQuickPopupItem(0x750295f0ef60), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.042  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.042  2439  2439 W default : (Parent is QQuickPopupItem(0x750295f271d0), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.042  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.042  2439  2439 W default : (Parent is StackView_QMLTYPE_38(0x750325eca910), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.042  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.042  2439  2439 W default : (Parent is QQuickPopupItem(0x750295f29e40), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.042  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.042  2439  2439 W default : (Parent is QQuickPopupItem(0x750295f29270), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.042  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.042  2439  2439 W default : (Parent is ScrollIndicator_QMLTYPE_50(0x750325ecffb0), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.042  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.042  2439  2439 W default : (Parent is QQuickPopupItem(0x750295f2b640), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.042  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.042  2439  2439 W default : (Parent is ScrollBar_QMLTYPE_37(0x750325ed9d30), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.042  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.042  2439  2439 W default : (Parent is QQuickPopupItem(0x750295f28e50), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.042  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.042  2439  2439 W default : (Parent is CheckBox_QMLTYPE_93(0x750325ef12b0), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.043  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.043  2439  2439 W default : (Parent is CheckBox_QMLTYPE_93(0x750325f50b90), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
14:33:58.043  2439  2439 W default : QObject: Cannot create children for a parent that is in a different thread.
14:33:58.043  2439  2439 W default : (Parent is CheckBox_QMLTYPE_93(0x750325f51910), parent's thread is QThread(0x750285e60ff0), current thread is QThread(0x750285ea37b0)
```


## Draft: typing in the note editor, Android ends Qupil, back

- tapped `^OK$` at 910 1579
- tapped `^Notiz$` at 312 1768
- tapped `^Notiz$`, `^Notiz speichern$` is there
- screenshot `shots/05-editor-typing.png`
- screenshot `shots/06-draft-back.png`
- the draft is back in the editor
- process: `2667`
- top: `topResumedActivity=ActivityRecord{6845dd7 u0 de.qupil.app/.QupilActivity} t13}`

```
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9ae70), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9a570), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9be30), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9cbb0), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9d930), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9c970), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9d4b0), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9f550), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325fa0f30), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9f1f0), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9ea10), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325fa15f0), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325fa13b0), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9f430), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325fa01b0), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325fa3d50), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.428  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.428  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325fa2d90), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.428  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.428  2667  2667 W default : (Parent is CheckBox_QMLTYPE_202(0x750325fa4e30), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.428  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.428  2667  2667 W default : (Parent is CheckBox_QMLTYPE_202(0x750325f78a90), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.428  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.428  2667  2667 W default : (Parent is CheckBox_QMLTYPE_202(0x750325fb0e90), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
```

- screenshot `shots/07-editor-left.png`
- process: `2667`
- top: `topResumedActivity=ActivityRecord{e2ef7f u0 com.android.fakesystemapp/.launcher.EmptyHomeActivity} t14}`

```
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9cbb0), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9d930), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9c970), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9d4b0), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9f550), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325fa0f30), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9f1f0), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9ea10), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325fa15f0), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325fa13b0), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325f9f430), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325fa01b0), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.427  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.427  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325fa3d50), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.428  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.428  2667  2667 W default : (Parent is AbstractButton_QMLTYPE_7_QML_40(0x750325fa2d90), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.428  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.428  2667  2667 W default : (Parent is CheckBox_QMLTYPE_202(0x750325fa4e30), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.428  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.428  2667  2667 W default : (Parent is CheckBox_QMLTYPE_202(0x750325f78a90), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:34.428  2667  2667 W default : QObject: Cannot create children for a parent that is in a different thread.
14:34:34.428  2667  2667 W default : (Parent is CheckBox_QMLTYPE_202(0x750325fb0e90), parent's thread is QThread(0x750285e617d0), current thread is QThread(0x750285eb6ef0)
14:34:38.593  2667  2685 I Qupil   : main: application state 2
14:34:38.644  2667  2667 I Qupil   : activity: focus false
14:34:38.653  2667  2685 I default : Draft note: kept, 230 characters
14:34:38.688  2667  2667 I Qupil   : activity: stop
14:34:38.688  2667  2685 I Qupil   : main: application state 0
14:34:38.690  2667  2685 I default : Draft note: kept, 230 characters
```


## Dark mode switched on while Qupil runs

- screenshot `shots/08-dark-runtime.png`
- screenshot `shots/09-dark-back.png`
- process: `2667`
- top: `topResumedActivity=ActivityRecord{6845dd7 u0 de.qupil.app/.QupilActivity} t13}`

```
14:34:54.143  2667  2667 I Qupil   : activity: start
14:34:54.143  2667  2667 I Qupil   : activity: resume, content ready true
14:34:54.186  2667  2667 I Qupil   : activity: focus true
14:34:54.365  2667  2685 I Qupil   : main: application state 4
```


## Fresh start in dark mode

- screenshot `shots/10-dark-start.png`
- screenshot `shots/11-light-again.png`
- process: `2873`
- top: `topResumedActivity=ActivityRecord{2240b15 u0 de.qupil.app/.QupilActivity} t15}`

```
14:35:07.020  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.020  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325f98dd0), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.020  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.020  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325f9ac30), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325f9a8d0), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325f99c70), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325f9d810), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325f9cf10), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325f9da50), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325f9c190), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325fa0e10), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325f9e6b0), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325fa14d0), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325f9fc10), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325fa0510), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325fa0870), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325f9efb0), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325fa2eb0), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is AbstractButton_QMLTYPE_9_QML_60(0x750325fa3450), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is CheckBox_QMLTYPE_153(0x750325fa1cb0), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is CheckBox_QMLTYPE_153(0x750325f74950), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
14:35:07.021  2873  2873 W default : QObject: Cannot create children for a parent that is in a different thread.
14:35:07.021  2873  2873 W default : (Parent is CheckBox_QMLTYPE_153(0x750325fb3a70), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eb6910)
```


## Landscape

- screenshot `shots/12-landscape.png`
- screenshot `shots/13-portrait-again.png`
- process: `2873`
- top: `topResumedActivity=ActivityRecord{2240b15 u0 de.qupil.app/.QupilActivity} t15}`

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
- process: `3352`
- top: `topResumedActivity=ActivityRecord{20408b5 u0 de.qupil.app/.QupilActivity} t17}`

```
14:36:15.875  3352  3352 I Qupil   : activity: focus true
14:36:15.953  3352  3371 I Qupil   : main: application state 4
14:36:20.514  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.514  3352  3352 W default : (Parent is QQuickPopupItem(0x750295f07130), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.515  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.515  3352  3352 W default : (Parent is ProgressBar_QMLTYPE_152(0x750325e98ed0), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.515  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.515  3352  3352 W default : (Parent is QQuickPopupItem(0x750295f074c0), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.515  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.515  3352  3352 W default : (Parent is QQuickPopupItem(0x750295f08f30), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.515  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.515  3352  3352 W default : (Parent is QQuickPopupItem(0x750295f0a610), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.515  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.515  3352  3352 W default : (Parent is QQuickPopupItem(0x750295f0bcc0), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.515  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.515  3352  3352 W default : (Parent is QQuickPopupItem(0x750295f0d790), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.515  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.515  3352  3352 W default : (Parent is QQuickPopupItem(0x750295f0e7e0), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.515  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.515  3352  3352 W default : (Parent is QQuickPopupItem(0x750295f27260), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.515  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.515  3352  3352 W default : (Parent is StackView_QMLTYPE_91(0x750325ecc1d0), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.515  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.515  3352  3352 W default : (Parent is QQuickPopupItem(0x750295f29b70), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.515  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.515  3352  3352 W default : (Parent is QQuickPopupItem(0x750295f29390), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.515  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.515  3352  3352 W default : (Parent is ScrollIndicator_QMLTYPE_52(0x750325ecdbb0), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.515  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.515  3352  3352 W default : (Parent is QQuickPopupItem(0x750295f2b6d0), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.515  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.515  3352  3352 W default : (Parent is ScrollBar_QMLTYPE_20(0x750325edae10), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.515  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.515  3352  3352 W default : (Parent is QQuickPopupItem(0x750295f28f10), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.516  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.516  3352  3352 W default : (Parent is CheckBox_QMLTYPE_71(0x750325f31c90), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.516  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.516  3352  3352 W default : (Parent is CheckBox_QMLTYPE_71(0x750325f4db30), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
14:36:20.516  3352  3352 W default : QObject: Cannot create children for a parent that is in a different thread.
14:36:20.516  3352  3352 W default : (Parent is CheckBox_QMLTYPE_71(0x750325f52690), parent's thread is QThread(0x750285e614d0), current thread is QThread(0x750285eab370)
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
- double tap: 168 ms for both;double tap: 170 ms for both;double tap: 171 ms for both;
- process: `3621`
- top: `topResumedActivity=ActivityRecord{860ec07 u0 de.qupil.app/.QupilActivity} t18}`

```
14:38:00.115  3621  3638 I default : select: sel 111 121 Mittelteil
14:38:00.115  3621  3638 I default : select: cursor 121
14:38:00.121  3621  3638 I default : select: menu Ausschneiden, Kopieren, Einsetzen, Alles auswählen
14:38:00.122  3621  3638 I default : select: button Ausschneiden 69.1 193.0
14:38:00.122  3621  3638 I default : select: button Kopieren 175.2 193.0
14:38:00.123  3621  3638 I default : select: button Einsetzen 266.8 193.0
14:38:00.125  3621  3638 I default : select: handle start 288.2 232.0
14:38:00.126  3621  3638 I default : select: handle end 351.7 266.0
14:38:01.319  3621  3638 I default : select: menu 
14:38:01.322  3621  3638 I default : select: markup **Tonleitern** ++G-Dur und D-Dur++ langsam üben / Etüde Nr. 3 bis Takt 16 mit *Metronom* / Vortragsstück: Bogeneinteilung im ==Mittelteil==
14:38:04.716  3621  3638 I default : select: sel 121 121 
14:38:04.722  3621  3638 I default : select: handle start -
14:38:04.722  3621  3638 I default : select: handle end -
14:38:07.357  3621  3638 I default : select: sel 40 45 Etüde
14:38:07.357  3621  3638 I default : select: cursor 45
14:38:07.362  3621  3638 I default : select: menu Ausschneiden, Kopieren, Einsetzen, Alles auswählen
14:38:07.363  3621  3638 I default : select: button Ausschneiden 69.1 161.0
14:38:07.364  3621  3638 I default : select: button Kopieren 52.9 161.0
14:38:07.366  3621  3638 I default : select: button Einsetzen 55.7 161.0
14:38:07.368  3621  3638 I default : select: handle start 38.0 200.0
14:38:07.374  3621  3638 I default : select: handle end 80.5 234.0
14:38:07.557  3621  3638 I default : select: button Kopieren 175.2 161.0
14:38:07.559  3621  3638 I default : select: button Einsetzen 266.8 161.0
14:38:09.118  3621  3638 I default : select: menu 
14:38:12.315  3621  3638 I default : select: sel 121 121 
14:38:12.315  3621  3638 I default : select: cursor 121
14:38:12.321  3621  3638 I default : select: handle start -
14:38:12.321  3621  3638 I default : select: handle end -
14:38:13.518  3621  3638 I default : select: menu Auswählen, Einsetzen, Alles auswählen
14:38:13.520  3621  3638 I default : select: button Einsetzen 221.1 207.0
14:38:13.521  3621  3638 I default : select: button Auswählen 122.8 207.0
14:38:19.113  3621  3638 I default : select: word Mittelteil 38.0 259.0 102.5 279.0
14:38:19.117  3621  3638 I default : select: sel 126 126 
14:38:19.117  3621  3638 I default : select: cursor 126
14:38:19.121  3621  3638 I default : select: menu 
14:38:19.126  3621  3638 I default : select: markup **Tonleitern** ++G-Dur und D-Dur++ langsam üben / Etüde Nr. 3 bis Takt 16 mit *Metronom* / Vortragsstück: Bogeneinteilung im ==Mittelteil==Etüde
14:38:23.718  3621  3638 I default : select: sel 107 107 
14:38:23.718  3621  3638 I default : select: cursor 107
14:38:27.116  3621  3638 I default : select: sel 122 122 
14:38:27.116  3621  3638 I default : select: cursor 122
```


## Crashes and hangs during the whole run

- none

Result: all steps ran
