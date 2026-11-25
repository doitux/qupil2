# TestFlight test text

`whats-new.txt` is the text testers see under "What to Test" in TestFlight. It is kept up to
date with every change that matters on iOS ("New since build N", N = the build last given to
the external testers) and passed to the TestFlight workflow when a build is started:

    gh workflow run testflight.yml -R doitux/qupil2 --ref main -f whats_new="$(cat .github/testflight/whats-new.txt)"

Builds are collected: the workflow "TestFlight (collect)" (testflight-collect.yml) starts one at
about 13:05 (Friday to Sunday) and 22:05 German time when something for iOS changed since the last upload, and by
hand when needed. Testers are not notified for each small change. The result of each collect
run is on the branch `testflight-status` (status.json, status.md).

**Only promise what was checked where the testers use it.** A point says "works" or "fixed"
only when it was checked on a real device, or when it is pure logic that the in-app UI test
covers on iOS. Anything that depends on the device itself – sound, background, lock screen,
silent switch, keyboard, notifications, performance – and was only checked in the simulator
says so and asks for a check on the device ("bitte am Gerät prüfen"). (Build 44 promised that
the metronome plays on in the background; it did not – the simulator check had not covered it.)

## The student app (Qupil Practice)

`student-whats-new.txt` is the same for the student app (`student/`, workflow "Student app").
Its workflow reads the file itself after an upload and sets it as "What to Test" in German
and English (U.S.); nothing is passed in. Kept up to date with every change that shows in
the student app on iOS, by the same rule as above.

The student app is collected with the teacher app: at each slot the collect run starts
`student.yml` with `upload` when something that ends up in the student's iOS app changed
since its last upload (`STUDENT_FILES` in `testflight-collect.py`: `student/` without its
tests, the page for qupil.de and what is for Android only, plus what it takes from the
teacher app: `src/shared`, `src/core/Crypto`, Monocypher, the QR code, the fonts). A change
of the student app alone starts no build of the teacher app. The status names both apps.

A text corrected after the upload, or a check that App Store Connect can be reached:
workflow "Student app: TestFlight text" (build number; "dry run" writes nothing).

## No builds by hand

Since 25.11.2026: builds reach TestFlight through the collect run only, so that the testers
can keep up. A build by hand (`gh workflow run student.yml -f upload=true`, or the collect
workflow with "now") only when the developer asks for it because he wants to test something
right away. Bugs are fixed on main in between; they wait for the next slot.

