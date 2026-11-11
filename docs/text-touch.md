# Editing text with a finger (TextTouch)

One set of rules on iPhone, iPad and Android phones and tablets, in every text Qupil
lets the user edit or select: the note and plan editor, `InputField` (one line and
several), the archive entry and the licence text. `qml/components/TextTouch.qml`
implements them; a field uses it like this:

```qml
TextArea {
    id: notes
    inputMethodHints: Qt.ImhNoTextHandles
    TextTouch { target: notes }
}
```

## The rules

| Gesture | Result |
|---|---|
| Tap into a field without focus | focus, cursor there, keyboard; no menu |
| Tap somewhere else in the text | cursor there; selection and menu go |
| Tap on the cursor | menu on/off: Auswählen · Einsetzen · Alles auswählen |
| Double tap on a word | the word, at once on the second touch; handles; menu: Ausschneiden · Kopieren · Einsetzen · Alles auswählen |
| Triple tap | the paragraph (without its line end); menu |
| Long press (0.45 s) on a word | the word while the finger is still down; it stays when the finger lifts; menu |
| Long press beside the text | cursor there, menu |
| Drag a handle | the selection grows or shrinks; the menu hides while dragging and comes back |
| Tap into the selection | menu on/off |
| Wipe | scrolls (the Flickable takes the touch over), never selects |
| Typing | the menu goes |

- Word boundaries are the same everywhere (`TextTouch.wordRange`,
  `TextFormatting::wordAt` in C++): letters, digits, hyphens and apostrophes belong to the
  word ("G-Dur", "Bach’s"); punctuation around it does not.
- The menu is Qupil's own (German labels in a German Qupil), above the selection, below
  it when there is no room; never over the format bar (that is at the bottom of the
  sheet). It is a plain item in the window or the sheet, not a Popup: a popup above a
  modal sheet took the next touch on the text away.
- A mouse or trackpad works as usual (drag selects); its double and triple click take the
  same word and paragraph as a finger.
- Typing, autocorrection and dictation stay with the system.

## The system's own selection is off

Otherwise three systems answer the same touches and the last one wins ("sometimes
this, sometimes that"):

- iOS: `Qt.ImhNoTextHandles` on the field – Qt then leaves out the magnifier, the
  handles and the edit menu (`QIOSTextInputOverlay`, Qt 6.8).
- Android: `QT_QPA_NO_TEXT_HANDLES=1` at the start (`main.cpp`) – Qt's long press checks
  only this variable, not the hint (`QAndroidInputContext::longPress`).

## Formatting a selection (TextFormatting)

`src/app/TextFormatting.cpp`: "bold" means every character of the selection is bold
(paragraph separators left out); without a selection the format where the cursor types.
B, I, U and colour switch the whole selection (or the word at the cursor) on, or off when
all of it has it. A selection that ends at the start of the next paragraph (a double click
at a paragraph start used to do that) is read correctly.

## Tests

- **Unit** (`tests/tst_notes.cpp`): word boundaries, a paragraph selection, mixed formats,
  no selection.
- **In the app, every platform** (`UiTest::addTextTouchCases`, cases `text-*`): real touch
  events through `QTest::touchEvent`, timed like a finger – tap, double and triple tap,
  long press (selection while the finger is down and after), handles, tap into and beside
  the selection, the menu with the clipboard, wiping, a one-line field, the mouse
  (`text-mouse`: double click at a paragraph start, B twice, triple click).
- **From outside** (`.github/scripts/select-scenarios.sh`, Android with adb, iOS with idb):
  touches through the system; Qupil's menu and no item of the system's menu, B with the
  first tap, the end handle, wiping, Kopieren/Einsetzen. A failed check fails the run.
  idb cannot double tap (its touches are a second apart): the iOS run notes that and
  selects with a long press instead.

Locally with Qt 6.4 (software scene graph) a run can crash in
`QQuickShapeSoftwareRenderer::updateNode` – a Qt 6.4 bug (a new node at the address of
the old one keeps the old path list) fixed in later versions; the apps use Qt 6.8.
