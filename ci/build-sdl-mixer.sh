#!/usr/bin/env bash
# Builds SDL_mixer 1.2 (WAV + Ogg Vorbis only) into $1.
# Usage: build-sdl-mixer.sh <prefix> <sdl-config> [extra configure args]
set -euo pipefail
prefix=$1
sdlconfig=$2
shift 2
commit=${SDL_MIXER_COMMIT:?}

work=$(mktemp -d)
git clone -q --filter=blob:none -b SDL-1.2 https://github.com/libsdl-org/SDL_mixer "$work/src"
git -C "$work/src" checkout -q "$commit"
cd "$work/src"
./configure -q --prefix="$prefix" SDL_CONFIG="$sdlconfig" \
  --disable-sdltest --disable-music-mod --disable-music-midi \
  --disable-music-mp3 --disable-music-flac \
  --enable-music-wave --enable-music-ogg --disable-music-ogg-shared \
  "$@"
grep -q -- '-DOGG_MUSIC' Makefile || { echo "Ogg Vorbis support not enabled"; exit 1; }
make -j4 install-hdrs install-lib
