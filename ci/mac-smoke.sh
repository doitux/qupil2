# Starts the bundled app and checks that it keeps running and creates its database.
export HOME=$RUNNER_TEMP/home SDL_AUDIODRIVER=dummy QT_DEBUG_PLUGINS=1 QT_FORCE_STDERR_LOGGING=1
mkdir -p "$HOME"
bin=dmgroot/Qupil.app/Contents/MacOS/qupil
fw=dmgroot/Qupil.app/Contents/Frameworks
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
kill $pid 2>/dev/null || true

if [ -z "$(ls "$HOME/.qupil/db" 2>/dev/null)" ]; then
  echo "--- dlopen calls of SDL libraries during start"
  DYLD_PRINT_APIS=1 "$bin" > "$RUNNER_TEMP/apis.txt" 2>&1 &
  p2=$!; sleep 8; kill $p2 2>/dev/null || true
  grep -iE 'dlopen' "$RUNNER_TEMP/apis.txt" | grep -i sdl | sort | uniq -c | head -n 20 || true
  echo "--- strings in sdl2-compat"
  strings "$fw/libSDL2-2.0.0.dylib" | grep -iE 'sdl3|_path/|too old|failed load' | head -n 20 || true
  echo "--- loading the bundled SDL3 directly"
  python3 -c "import ctypes,sys; l=ctypes.CDLL(sys.argv[1]); print('SDL_GetVersion', l.SDL_GetVersion())" "$fw/libSDL3.0.dylib" 2>&1 || true
  ls -la "$fw" | grep -i sdl || true
  echo "--- app output"
  tail -n 15 "$out" || true
fi
[ $alive = yes ]
[ -n "$(ls "$HOME/.qupil/db")" ]
