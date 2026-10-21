# Qupil 1.4.1 – builds for current systems

This branch only holds a GitHub Actions workflow
([`.github/workflows/legacy-1.4.1.yml`](.github/workflows/legacy-1.4.1.yml)).
It builds the **unchanged** Qupil 1.4.1 from
[doitux/qupil](https://github.com/doitux/qupil) (tag `v1.4.1`) with Qt 5.15:

| Platform | Package | Built with |
|---|---|---|
| Linux x86_64 | `Qupil-1.4.1-linux-x86_64.AppImage` | Ubuntu 22.04, linuxdeploy |
| Windows x64 | `Qupil-1.4.1-windows-x64.zip` (portable) | MSYS2 MinGW 64 |
| macOS Apple Silicon | `Qupil-1.4.1-macos-arm64.dmg` | Homebrew Qt 5, SDL 1.2 (sdl12-compat) |
| macOS Intel | `Qupil-1.4.1-macos-x86_64.dmg` | Homebrew Qt 5, SDL 1.2 (sdl12-compat) |

The source code is not modified; only build settings are passed to `qmake`
(include and library paths for SDL, and `SDL_main=main` so that no SDLmain
library is needed). Every job checks that SDL_mixer loads all sound files
and that the packaged app starts and creates its database.

The packages are kept as artifacts of the workflow run (90 days at most)
and are not signed: Windows SmartScreen and macOS Gatekeeper warn on first
start (macOS: right-click › Open).

The workflow runs on every push to this branch and can be started by hand
under *Actions › Qupil 1.4.1 (legacy) › Run workflow*.
