# Qupil UI test

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
