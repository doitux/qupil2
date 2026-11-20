# The student app (practice book)

`student/` holds a second, small app for students and parents: the **practice book**
(German "Übeheft"). The teacher's Qupil writes it; the student app shows it and sends
short feedback back. It is a CMake project of its own and shares only the booklet
format and the encryption with Qupil.

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
  "allow": {"feedback": true, "questions": true},
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

## The example

`--demo` or "See an example" on the welcome page: an invented booklet (Ms Rau, Jonas,
viola) with dates around today. Nothing is sent. It is also what Apple's review sees,
since a review cannot scan a teacher's code.

## Tests

| Test | What it checks |
|---|---|
| `tst_booklet` | link round trip and forms, invalid links, sealing (wrong key, wrong purpose, tampered), file names, URLs |
| `tst_teacherside` | links via OCS (read + create; read only; enforced expiry and renewal; wrong login), the QR code (size, quiet zone; `QUPIL_QR_OUT=x.png` saves it for a check with a real reader), the inbox (newest per device, newer answer wins, values cleaned) |
| `tst_studentapp` | the app against a fake Nextcloud: coupling, feedback upload and its content, echo from the teacher, wrong key, newer format, ended link, wrong password, old Nextcloud, state kept; the interface by tap: example, hints, feedback, to-do, closing the example, removing a teacher (asks first) |

`student/tests/FakeNextcloud` is the stand-in for Nextcloud (public links with
permissions, OCS share API, capabilities).

## Builds

`.github/workflows/student.yml` (only when `student/` or the shared code changes):
tests and screenshots of the example in four languages (Linux), an Android test APK
(signed with the public test key), the iOS archive. Upload to TestFlight
(`de.qupil.student`) only when started by hand with "upload".

Icons: `python3 -I student/tools/make_icons.py` (Qupil's logo with the colours turned
round: lime tile, plum figures).

## Still to do

- Teacher app: tasks in the lesson entry for coupled students, QR code, the student's
  page (couple, end, what is shared), Settings › Practice book, collecting feedback,
  the start-of-lesson pop-up, sharing homework as a message.
- qupil.de: `/u` page (opens the app, otherwise store links), `apple-app-site-association`
  and `assetlinks.json` for links that open the app directly; then
  `QUPIL_STUDENT_ASSOCIATED_DOMAINS=ON`.
- Scanning the QR code inside the app (now: the phone's camera opens the link).
