# The student app (practice book)

`student/` holds a second, small app for students and parents: the **practice book**.
The teacher's Qupil writes it; the student app shows it and sends short feedback back.
It is a CMake project of its own and shares only the booklet format and the encryption
with Qupil.

Names (decided 20.11.2026): in the stores "Qupil – Musik üben" / "Qupil – Music Practice",
under the icon "Qupil Üben" / "Qupil Practice" (French and Spanish: the English name).
In the teacher's Qupil the practice book is called "Qupil Üben" ("Mit Qupil Üben koppeln …");
in the student app the area is "Aufgaben" / "Tasks", and each teacher is an entry in the
settings. "Practice book" and "booklet" stay the names in the code; the Nextcloud folder
keeps the name "Qupil Übeheft", and the identifiers (`de.qupil.student`) stay.

```
cmake -S student -B build-student -G Ninja && cmake --build build-student
QT_QPA_PLATFORM=offscreen ctest --test-dir build-student
./build-student/qupil-student --demo            # the example, without any cloud
```

Options: `--data-dir`, `--today yyyy-MM-dd`, `--lang de|en|fr|es`, `--demo`,
`--size WxH`, `--screenshot file.png` (save the window and quit), `--smoke-test`.
Qt 6.8 in CI; Qt 6.4 is enough on a Linux desktop.

## How it works

No server of Qupil's own and no account. Everything goes through the teacher's
Nextcloud, end-to-end encrypted:

1. For a coupled student the teacher's Qupil keeps a folder in its Nextcloud
   (`Qupil/Übeheft/<random>`), shared by a **public link with a password**. The link
   may read and add files, nothing else (permissions 5; read only where the admin
   forbids uploads by link – then no feedback comes back).
2. The teacher shows a **QR code** (or sends the link). It holds the Nextcloud address,
   the link token, its password and a 32-byte key:
   `https://qupil.de/u#<base64url JSON {v,s,t,p,k}>`. The part after `#` never reaches
   any web server.
3. Qupil writes `heft.qupil` into the folder: the booklet as JSON, compressed and
   sealed with XChaCha20-Poly1305 (Monocypher), associated data `qupil-booklet-1`.
4. The student app reads it through Nextcloud's public WebDAV
   (`/public.php/dav/files/<token>/heft.qupil`, Nextcloud 29+; older ones: the old
   `/public.php/webdav/`, signed in with token and password).
5. Feedback and questions go back as `fb-<device>-<ms>.qupil` (associated data
   `qupil-feedback-1`). Each file holds the device's **whole** current feedback, so a lost
   upload loses nothing. Qupil takes the newest file per device, the newer answer per task
   (`BookletInbox`), deletes the older files with its own login and echoes the merged
   feedback in the next booklet (the student's other devices see it).

Code: `src/shared/Booklet` (link, sealing, file names), `src/shared/BookletInbox`
(teacher: collecting feedback), `src/shared/NextcloudShares` (teacher: links via the OCS
share API), `src/shared/QrCode` (teacher: the QR code, `3rdparty/qrcodegen`),
`student/src/StudentApp` (the app's state), `student/qml` (interface).

## The booklet (format 1)

```json
{
  "format": 1, "written": "2026-11-20T13:00:00Z",
  "teacher": "Ms Rau", "student": "Jonas", "instrument": "Viola",
  "uploads": true, "allow": {"feedback": true, "questions": true},
  "next": [{"date": "2026-11-24", "start": "15:30", "end": "16:30", "place": "Room 6",
            "changed": "Room 6 instead of room 4", "cancelled": false, "reason": ""}],
  "tasks": [{"id": "…", "date": "2026-11-17", "title": "Bourrée bars 17–24 slowly",
             "hints": ["Right elbow relaxed"], "kind": "practice", "since": "2026-11-03"}],
  "feedback": {"<task id>": {"practiced": 3, "works": 2, "done": false, "at": 1763600000000}},
  "pieces": [{"title": "Bourrée", "composer": "J. S. Bach", "since": "2026-09-20", "ready": true}],
  "events": [{"date": "2026-12-11", "time": "18:00", "name": "Winter recital",
              "place": "Hall", "contribution": "Bourrée"}]
}
```

- `kind`: `practice` (feedback "practised: a lot / some / hardly" = 3/2/1 and
  "works: well / so-so / not yet" = 3/2/1) or `todo` (only a tick, e.g. get the music).
- `since`: a task that stays open over several lessons keeps its first date.
- The app refuses a higher `format` ("update the app") and a booklet it cannot open
  ("ask for a new code"); an ended link shows "ended by your teacher", the last state
  stays readable.
- Only what the teacher marked for the student goes in; private notes never do.
- `uploads` (since 11/2026): the teacher's Nextcloud takes uploads by link at all. Without
  the field the app goes by `allow` (both are off without uploads).

## What comes back, and which devices have the booklet

A device uploads `fb-<device>-<ms>.qupil` (sealed with the booklet's key, purpose
"feedback"): `{"device", "written", "feedback", "questions", "since"?, "left"?}`. Every file
holds the device's whole state, so the teacher's Qupil reads the newest file per device and
deletes the older ones (`Booklet::collect`, `src/shared/BookletInbox`).

- **"This device has the booklet":** after the first successful fetch the app uploads once
  (`since`: when the booklet was taken; empty feedback). Nothing of it shows as "sent" to
  the student. An app from before this says it with its next start.
- **"Removed here":** removing a teacher in the app uploads a last file with `left` (and
  everything sent so far). Without a connection the goodbye waits in `student.json`
  (`leaving`, without the tasks) and is tried again for 30 days; a link that is gone
  (ended by the teacher) needs no goodbye. Taking the booklet again on the same device:
  a newer file without `left`.
- **The teacher's side:** `DataService::couplingDevices()` gives counts and days for the
  coupling sheet ("1 Gerät verbunden (seit …)", "auf 1 Gerät entfernt am …", "Auf allen
  Geräten entfernt …", "Letzte Rückmeldung: …"). Kept in the local `meta` table per
  coupling, not in the coupling record: every device of the teacher reads it from the
  folder, nothing new syncs, the schema stays. While the sheet shows the QR code and no
  device has it yet, it looks into that one folder every 8 s (`Booklet.lookFor`), so the
  teacher sees in the lesson that scanning worked.
- **Limits, on purpose:** the teacher sees how many devices, never whose, and not when the
  app is used (no "last opened"). A deleted app says nothing: it stays "connected". One
  device cannot be shut out (all share link and key): ending the coupling is the way, and
  it is never done automatically. Whoever has the link could upload a false "left".

## Getting the code into the app

- **The scanner in the app** ("Open camera" on the welcome page and in the settings,
  `src/QrScanner*`): iOS shows the camera in a full-screen sheet and lets AVFoundation
  recognise the QR code (`QrScanner_ios.mm`, `NSCameraUsageDescription`); Android opens
  the scanner of zxing-android-embedded from `StudentActivity.startScan()` (its own
  activity, it asks for the camera permission itself; dependency in
  `platform/android/build.gradle`). The picture stays on the phone. Another QR code or a
  refused camera is said beside the button. Desktop: no scanner (`--pretend-scan <text>`
  for tests and screenshots).
- **The phone's own camera, or a link in a message**: `https://qupil.de/u#…` opens the app
  (Android App Links with `assetlinks.json`, iOS universal links with the Associated
  Domains entitlement; `qupil-student:…` as the fallback the page qupil.de/u uses).
  `src/IncomingLinks*`: iOS and desktop through Qt's URL handlers; Android through
  `StudentActivity` (the intent of the start waits for the native part, later ones go
  straight on), because Qt's handlers did not deliver the links there.
  `StudentApp::openUrl()` may be called on any thread.
- **Paste** the link.

`platform/android/build.gradle` is Qt 6.8.3's template with the scanner's dependency and
the app's four languages kept in the package (the template keeps English only: the name
under the icon was English on a German phone). Compare it when Qt is updated.

## The keyboard

`qml/components/PageFlickable.qml`: when a page gets lower (Android makes the window
smaller for the keyboard), the field with the cursor and what belongs to it below stay in
view; a tap beside the fields puts the keyboard away. "Send" sits beside the question
field, so both are in view together (iOS moves the view to the cursor itself).

## The example

`--demo` or "See an example" on the welcome page: an invented booklet (Ms Rau, Jonas,
viola) with dates around today. Nothing is sent. It is also what Apple's review sees,
since a review cannot scan a teacher's code.

## Tests

| Test | What it checks |
|---|---|
| `tst_booklet` | link round trip and forms, invalid links, sealing (wrong key, wrong purpose, tampered), file names, URLs |
| `tst_tasks` `couplingDevices` (the teacher's app) | counts and days of the devices, kept beside the coupling record; the booklet's `uploads` |
| `tst_teacherside` | links via OCS (read + create; read only; enforced expiry and renewal; wrong login), the QR code (size, quiet zone; `QUPIL_QR_OUT=x.png` saves it for a check with a real reader), the inbox (newest per device, newer answer wins, values cleaned; the devices with `since` and `left`) |
| `tst_studentapp` | the app against a fake Nextcloud: coupling, feedback upload and its content, echo from the teacher, wrong key, newer format, ended link, wrong password, old Nextcloud, state kept; the interface by tap: example, hints, feedback, to-do, closing the example, removing a teacher (asks first); a link through the system and from another thread; the scanner (another code, camera refused, closed, the teacher's code); fields in view with the keyboard; texts without "an <name>"; "this device has the booklet" once and never as "sent", the goodbye on removing (also later, when the cloud was down; not for a link that is gone; not without uploads) |
| `.github/scripts/student-android-test.sh` (emulator, job "Android emulator") | a link starts the app and adds the teacher; links while the app is running (https, `qupil-student:`, from the background); "Open camera" opens the scanner, "back" closes it |
| `.github/scripts/student-ios-test.sh` (simulator, job "iOS simulator") | the app starts; `qupil-student:` starts the app and adds the teacher, also while it runs; the button on the page `qupil.de/u` (served from `student/web`, in Safari) hands its link to the app, and without the app Safari says "the address is invalid" (the control: that is what a device without the app shows); the scanner's sheet opens and closes. Safari's page and alerts are not in idb's view: found in screenshots (`ios-find-colour.py`, `ios-find-alert.py`). A crash report goes to the results (`crash/`, with `ios-crash-summary.py`). Universal links: on a device only |

`student/tests/FakeNextcloud` is the stand-in for Nextcloud (public links with
permissions, OCS share API, capabilities).

## Builds

`.github/workflows/student.yml` (only when `student/` or the shared code changes):
tests and screenshots of the example in four languages (Linux), an Android test APK
(signed with the public test key; on main it goes to the "nightly" release as
`Qupil-Practice-Android-arm64-nightly-<commit>.apk`), the emulator run, the iOS archive.
Upload to TestFlight (`de.qupil.student`) only when started by hand with "upload".
The archive is built unsigned; before the export it gets an ad-hoc signature with the
Associated Domains entitlement, which the export's signing keeps.

Icons: `python3 -I student/tools/make_icons.py` (Qupil's logo with the colours turned
round: lime tile, plum figures).

## In the teacher's Qupil

- **Data** (schema 8): `task` (one record per task: title, hints, practise / to-do, the
  lesson it belongs to, `since`, `closed`, the student's answer) and `coupling` (folder,
  link, key, what the student may send, questions). `DataServiceTasks.cpp`, `tst_tasks`.
- **Tasks editor** (`TaskEditor.qml`, variant A): tasks at the top, the private note
  folded below. Enter / Enter on an empty hint / Ctrl+Enter; bar above the keyboard
  (+ Hint, + Task, Private). Open tasks of earlier lessons come along ("open since").
- **Student page**: "Tasks" instead of "Note"; one card per lesson (`TaskLessonCard.qml`);
  the running lesson's button and the lesson page offer "Tasks" for a coupled student.
- **Coupling** (student menu › Mit Qupil Üben koppeln …, `BookletSheet.qml`, `BookletService`):
  folder "Qupil Übeheft/<random>" next to the sync folder, link with password, key;
  QR code, link to send (desktop: copied to the clipboard, and said so), switches,
  all questions with their dates, ending (link and folder go).
- **Questions**: unread ones stand on the card of the newest lesson on the student's
  page, with their date; "Gelesen" marks them as read (`read` in the coupling's
  `questions`; kept when the answers are fetched again). The pop-up at the start of the
  lesson shows unread ones of the last two weeks.
- **Booklets**: written after each sync and shortly after changes; answers collected
  and shown at the tasks and in the pop-up at the start of the lesson.
- **Plans** of a coupled student: written in the tasks editor (tasks and hints, saved as
  plan blocks: heading bold, hints as lines). After the lesson "Turn into tasks": covered
  blocks become tasks; a block with the lock goes into the private note instead; "Edit
  before saving" opens the tasks editor with the new tasks (saved only there).
- **Homework as a message** (any student): a note through the system's share menu.

## Still to do

- Try it against the music school's Nextcloud (link passwords, uploads by link,
  expiry) and on devices.
- On devices: the scanner (iPhone, iPad, Android), links from the phone's camera
  (iOS: universal links need Apple to have fetched qupil.de's association file).
