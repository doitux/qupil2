# Packages the Windows build into dist/Qupil-$VERSION and out/*.zip (MSYS2 MINGW64 shell)
D=dist/Qupil-$VERSION
mkdir -p "$D"
cp qupil/release/qupil.exe "$D/"
cp -r qupil/data "$D/data"
rm -f "$D/data/.directory" "$D/data/sounds/.directory"

DEPLOY=$(command -v windeployqt-qt5 || command -v windeployqt)
echo "windeployqt: $DEPLOY"
"$DEPLOY" --release --no-translations --no-compiler-runtime --no-opengl-sw --no-angle "$D/qupil.exe"

# MSYS2 runtime DLLs (Qt's dependencies, SDL, Ogg Vorbis, GCC runtime)
for pass in 1 2 3 4 5; do
  find "$D" \( -iname '*.exe' -o -iname '*.dll' \) -print0 \
    | xargs -0 -n1 ldd 2>/dev/null \
    | awk '/\/mingw64\/|\/sdlmixer\//{print $3}' | sort -u \
    | while read -r dll; do
        [ -e "$D/$(basename "$dll")" ] || cp "$dll" "$D/"
      done
done
# sdl12-compat loads SDL2 at runtime
if [ -f /mingw64/bin/SDL2.dll ] && [ ! -e "$D/SDL2.dll" ]; then cp /mingw64/bin/SDL2.dll "$D/"; fi

find "$D" -maxdepth 2 -type f \( -iname '*.dll' -o -iname '*.exe' \) | sort
mkdir -p out
(cd dist && zip -qr "../out/Qupil-$VERSION-windows-x64.zip" "Qupil-$VERSION")
ls -la out
