# Bundles Qt, SDL and Ogg Vorbis into qupil.app and builds the dmg.
app=qupil/qupil.app
cp -R qupil/data "$app/Contents/Resources/data"
rm -f "$app/Contents/Resources/data/.directory" "$app/Contents/Resources/data/sounds/.directory"
"$QT5/bin/macdeployqt" "$app" -verbose=1

# sdl12-compat loads SDL2 at runtime from next to itself (as libSDL2-2.0.0.dylib).
# Homebrew's sdl2 is sdl2-compat, which in turn loads SDL3 as libSDL3.dylib.
bundle_lib() {
  cp -L "$1" "$app/Contents/Frameworks/$2"
  chmod u+w "$app/Contents/Frameworks/$2"
  install_name_tool -id "@executable_path/../Frameworks/$2" "$app/Contents/Frameworks/$2"
  echo "bundled $2 from $1"
}
bundle_lib "$(brew --prefix sdl2)/lib/libSDL2-2.0.0.dylib" libSDL2-2.0.0.dylib
if [ -f "$(brew --prefix sdl3)/lib/libSDL3.dylib" ]; then
  bundle_lib "$(brew --prefix sdl3)/lib/libSDL3.dylib" libSDL3.dylib
fi
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
