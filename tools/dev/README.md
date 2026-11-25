# Helpers for working on Qupil from a Linux box without the devices

These scripts bundle steps that otherwise take many single commands. They are made for a
throw-away Linux environment (Ubuntu with the Qt 6.4 packages) in which the app is built and
tested without a screen, while the device tests run on GitHub.

| Script | What it does |
|---|---|
| `qt64build.sh` | Copies the working tree, adapts the Qt 6.5+ APIs to Qt 6.4 and builds. Exits with the build's exit code. |
| `local-test.sh` | Builds (stops when the build fails), runs the given UI test cases, prints only the failures. |
| `ci-start.sh` | Starts the desktop, Android and iOS UI test runs for a branch; never a second iOS test while one runs. |
| `ci-status.sh` | One look at a branch's runs and at the results of the device tests. |

`QUPIL_WORK` is the scratch folder (default `/home/claude/work`); build tree and results live there,
nothing is written into the repository.

Typical round:

```
tools/dev/local-test.sh tasks-long-texts,card-long-texts      # build + two cases, phone size
tools/dev/local-test.sh --no-build idea-demo 1180x820 wide    # same binary, tablet size
git push origin fix/whatever
tools/dev/ci-start.sh fix/whatever
# … about 70 minutes later, once:
tools/dev/ci-status.sh fix/whatever
```

Packages for the local build (Ubuntu 24.04):

```
apt-get install -y qt6-base-dev qt6-base-dev-tools qt6-declarative-dev qt6-declarative-dev-tools \
  qt6-multimedia-dev qt6-tools-dev qt6-tools-dev-tools qml6-module-qtquick qml6-module-qtquick-controls \
  qml6-module-qtquick-layouts qml6-module-qtquick-shapes qml6-module-qtquick-templates qml6-module-qtquick-window \
  qml6-module-qtquick-dialogs qml6-module-qtqml qml6-module-qtqml-models qml6-module-qtqml-workerscript \
  qml6-module-qtcore qml6-module-qt-labs-settings qml6-module-qtmultimedia ninja-build cmake imagemagick ffmpeg gdb
```

Things to know:

- Qt 6.4 crashes at random without a screen (exit code 139); `local-test.sh` simply runs again.
- The local build is Qt 6.4, CI and the devices are Qt 6.8: what passes here is a first check only.
- Sharp pictures for showing a design (2x), with a virtual screen instead of `offscreen` (which loses
  the icons at 2x):
  `QT_SCALE_FACTOR=2 QT_QPA_PLATFORM=xcb LIBGL_ALWAYS_SOFTWARE=1 xvfb-run -a -s "-screen 0 2400x2400x24" $QUPIL_WORK/qt64test/build/qupil --ui-test <dir> --lang de --ui-test-size 390x844 --data-dir <dir> --ui-test-phase only:<cases>`
- See `docs/testing.md` for the UI test itself.
