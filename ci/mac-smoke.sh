# Starts the bundled app and checks that it keeps running and creates its database.
export HOME=$RUNNER_TEMP/home SDL_AUDIODRIVER=dummy QT_DEBUG_PLUGINS=1
mkdir -p "$HOME"
bin=dmgroot/Qupil.app/Contents/MacOS/qupil
DYLD_PRINT_LIBRARIES=1 "$bin" > "$RUNNER_TEMP/qupil-out.txt" 2>&1 &
pid=$!
sleep 15
alive=yes
kill -0 $pid 2>/dev/null || alive=no
echo "running after 15 s: $alive"
if [ $alive = no ]; then wait $pid; echo "exit code $?"; fi
kill $pid 2>/dev/null || true
echo "--- libraries loaded from outside the bundle and the system"
grep -E "dyld.*loaded" "$RUNNER_TEMP/qupil-out.txt" | grep -vE "Qupil.app|/System/|/usr/lib/" || true
echo "--- app output"
grep -v "dyld" "$RUNNER_TEMP/qupil-out.txt" | tail -n 40 || true
echo "--- $HOME/.qupil"
ls -la "$HOME/.qupil" "$HOME/.qupil/db" || true
[ $alive = yes ]
[ -n "$(ls "$HOME/.qupil/db")" ]
