# TestFlight test text

`whats-new.txt` is the text testers see under "What to Test" in TestFlight. It is kept up to
date with every change that matters on iOS ("New since build N", N = the build last given to
the external testers) and passed to the TestFlight workflow when a build is started:

    gh workflow run testflight.yml -R doitux/qupil2 --ref main -f whats_new="$(cat .github/testflight/whats-new.txt)"

Builds are collected: they are started at set times of the day (and by hand when needed),
not after every push, so testers are not notified for each small change.
