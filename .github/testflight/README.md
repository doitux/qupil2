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
