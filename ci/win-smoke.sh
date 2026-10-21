# Starts the packaged qupil.exe with only its own DLLs and checks that it
# keeps running and creates its database (MSYS2 shell).
appdata=$(cygpath -u "$APPDATA")
rm -rf "$appdata/qupil"
cd "dist/Qupil-$VERSION"
export PATH=/usr/bin:/c/Windows/system32:/c/Windows SDL_AUDIODRIVER=dummy
./qupil.exe > "$RUNNER_TEMP/qupil-out.txt" 2>&1 &
pid=$!
sleep 15
alive=yes
kill -0 $pid 2>/dev/null || alive=no
echo "running after 15 s: $alive"
if [ $alive = no ]; then wait $pid; echo "exit code $?"; fi
kill $pid 2>/dev/null || true
echo "--- app output"; cat "$RUNNER_TEMP/qupil-out.txt" || true
echo "--- $appdata/qupil"; ls -la "$appdata/qupil" "$appdata/qupil/db" || true
[ $alive = yes ]
[ -n "$(ls "$appdata/qupil/db")" ]
