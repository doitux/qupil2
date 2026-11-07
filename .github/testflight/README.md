# TestFlight test text

`whats-new.txt` is the text testers see under "What to Test" in TestFlight. It is kept up to
date with every change that matters on iOS ("New since build N", N = the build last given to
the external testers) and passed to the TestFlight workflow when a build is started:

    gh workflow run testflight.yml -R doitux/qupil2 --ref main -f whats_new="$(cat .github/testflight/whats-new.txt)"

Builds are collected: the workflow "TestFlight (collect)" (testflight-collect.yml) starts one at
about 13:05 (Friday to Sunday) and 22:05 German time when something for iOS changed since the last upload, and by
hand when needed. Testers are not notified for each small change. The result of each collect
run is on the branch `testflight-status` (status.json, status.md).
