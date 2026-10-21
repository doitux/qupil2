# Starts the bundled app and checks that it keeps running and creates its database.
export HOME=$RUNNER_TEMP/home SDL_AUDIODRIVER=dummy QT_DEBUG_PLUGINS=1 QT_FORCE_STDERR_LOGGING=1
mkdir -p "$HOME"
bin=dmgroot/Qupil.app/Contents/MacOS/qupil
out=$RUNNER_TEMP/qupil-out.txt
"$bin" > "$out" 2>&1 &
pid=$!
alive=yes
for i in $(seq 1 60); do
  sleep 1
  kill -0 $pid 2>/dev/null || { alive=no; break; }
  [ -n "$(ls "$HOME/.qupil/db" 2>/dev/null)" ] && break
done
echo "after $i s: running=$alive"
if [ $alive = no ]; then wait $pid; echo "exit code $?"; fi
if [ $alive = yes ] && [ -z "$(ls "$HOME/.qupil/db" 2>/dev/null)" ]; then
  echo "--- main thread of the running app"
  sample $pid 3 -file "$RUNNER_TEMP/sample.txt" >/dev/null 2>&1 || true
  grep -m1 -A80 'Thread_' "$RUNNER_TEMP/sample.txt" \
    | grep -oE '[^ +!:|]+  \(in [^)]+\)' | head -n 50 || true
fi
kill $pid 2>/dev/null || true
echo "--- app output"
tail -n 30 "$out" || true
echo "--- $HOME/.qupil"
ls -la "$HOME/.qupil" "$HOME/.qupil/db" || true
[ $alive = yes ]
[ -n "$(ls "$HOME/.qupil/db")" ]
