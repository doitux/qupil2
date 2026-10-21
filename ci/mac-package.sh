# Bundles Qt, SDL and Ogg Vorbis into qupil.app and builds the dmg.
app=qupil/qupil.app
cp -R qupil/data "$app/Contents/Resources/data"
rm -f "$app/Contents/Resources/data/.directory" "$app/Contents/Resources/data/sounds/.directory"
"$QT5/bin/macdeployqt" "$app" -verbose=1

# sdl12-compat loads SDL2 at runtime from next to itself. Homebrew's sdl2 is
# sdl2-compat, which in turn loads SDL3 the same way.
for lib in "$(brew --prefix sdl2)/lib/libSDL2-2.0.0.dylib" "$(brew --prefix sdl3 2>/dev/null)/lib/libSDL3.0.dylib"; do
  [ -f "$lib" ] || continue
  name=$(basename "$lib")
  cp -L "$lib" "$app/Contents/Frameworks/$name"
  chmod u+w "$app/Contents/Frameworks/$name"
  install_name_tool -id "@executable_path/../Frameworks/$name" "$app/Contents/Frameworks/$name"
  echo "bundled $name from $lib"
done
codesign --force --deep --sign - "$app"

echo "--- linked libraries"
otool -L "$app/Contents/MacOS/qupil" "$app"/Contents/Frameworks/*.dylib
if otool -L "$app/Contents/MacOS/qupil" "$app"/Contents/Frameworks/*.dylib \
     | grep -E "^\s+($BREW/|$MIXER)"; then
  echo "unbundled library"
  exit 1
fi
echo "--- bundle"
ls "$app/Contents" "$app/Contents/Frameworks" "$app/Contents/PlugIns"/* "$app/Contents/Resources"
cat "$app/Contents/Resources/qt.conf" || true
otool -l "$app/Contents/Frameworks/QtCore.framework/QtCore" | grep -A3 LC_BUILD_VERSION | grep minos || true

mkdir -p dmgroot out
cp -R "$app" dmgroot/Qupil.app
ln -s /Applications dmgroot/Applications
hdiutil create -volname "Qupil $VERSION" -srcfolder dmgroot -ov -format UDZO \
  "out/Qupil-$VERSION-macos-$ARCH.dmg"
ls -la out
