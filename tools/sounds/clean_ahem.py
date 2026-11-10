#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2006-2026 Felix Hammer
# SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
"""How assets/sounds/lesson-end.wav was made from the "Ähem" of Qupil 1.4.1 (10.11.2026).

The original had a DC offset, rumble below 90 Hz and a noise floor only 16 dB below the
voice; made denser for iOS notifications, the noise became as loud as the voice. Here:
DC and rumble removed, hiss above 9 kHz cut, spectral subtraction with the noise of the
pause (0.10-0.20 s), a soft gate for the pauses, then gently compressed (ffmpeg) while the fade-out at the end keeps its natural
course (lifted, its single creaks of the voice rattled), the few sharp peaks limited by
5 dB, and normalised to a peak of -1 dBFS with short fades.

Usage: clean_ahem.py original.wav lesson-end.wav   (needs numpy, scipy, ffmpeg)
"""
import os, sys, subprocess, tempfile, wave
import numpy as np
from scipy import signal
sr = 44100
def read(f):
    w = wave.open(f); return np.frombuffer(w.readframes(w.getnframes()), dtype=np.int16).astype(np.float64) / 32768.0
def write(f, x):
    w = wave.open(f, 'wb'); w.setnchannels(1); w.setsampwidth(2); w.setframerate(sr)
    w.writeframes((np.clip(x, -1, 1) * 32767).astype(np.int16).tobytes()); w.close()
def seg_db(x, step=0.02):
    n = int(sr * step); return [round(20*np.log10(np.sqrt(np.mean(x[i:i+n]**2))+1e-9)) for i in range(0, len(x), n)]

src, dst = sys.argv[1], sys.argv[2]
tmp = tempfile.mkdtemp()
clean, dense = os.path.join(tmp, 'clean.wav'), os.path.join(tmp, 'dense.wav')
x = read(src)
length = len(x)
x = x - x.mean()                                     # DC offset
sos = signal.butter(4, 90, 'highpass', fs=sr, output='sos')
x = signal.sosfiltfilt(sos, x)                       # rumble below 90 Hz, no phase shift
sos = signal.butter(4, 9000, 'lowpass', fs=sr, output='sos')
x = signal.sosfiltfilt(sos, x)                       # hiss above 9 kHz (voice is below)
# spectral subtraction with the noise of the quiet part 0.10-0.20 s
n_fft, hop = 1024, 256
f, t, Z = signal.stft(x, sr, nperseg=n_fft, noverlap=n_fft-hop)
noise = np.mean(np.abs(Z[:, (t >= 0.10) & (t <= 0.20)]), axis=1, keepdims=True)
mag = np.abs(Z); ph = np.angle(Z)
mag2 = np.maximum(mag - 1.5 * noise, 0.05 * mag)
_, x = signal.istft(mag2 * np.exp(1j*ph), sr, nperseg=n_fft, noverlap=n_fft-hop)
x = x[:length]
# envelope gate: below -32 dB (relative to peak) faded down smoothly
env = np.sqrt(signal.sosfiltfilt(signal.butter(2, 30, 'lowpass', fs=sr, output='sos'), x**2).clip(0))
ref = env.max()
rel = 20*np.log10(env/ref + 1e-9)
gain = np.clip((rel + 34) / 8, 0, 1)                 # -34 dB -> 0, -26 dB -> 1
gain = signal.sosfiltfilt(signal.butter(2, 40, 'lowpass', fs=sr, output='sos'), gain).clip(0, 1)
x = x * gain
write(clean, x)
# dense: compressor in ffmpeg (as before), then peak -1 dBFS
subprocess.run(["ffmpeg","-hide_banner","-loglevel","error","-y","-i",clean,"-af",
  "aresample=176400,acompressor=threshold=-30dB:ratio=3:attack=4:release=120:knee=6:makeup=1,aresample=44100:resampler=soxr",
  "-c:a","pcm_s16le",dense], check=True)
d = read(dense)
# The end of the "hem" fades out in single creaks of the voice; lifted by the compressor
# they rattle. From 0.33 s on keep the natural fade of the cleaned sound instead.
smooth = lambda y: np.sqrt(signal.sosfiltfilt(signal.butter(2, 25, 'lowpass', fs=sr, output='sos'), y**2).clip(1e-12))
ec, ed = smooth(x[:len(d)]), smooth(d)
tail = np.clip(((ec / ec.max()) / (ed / ed.max())), 0, 1)
ramp = np.clip((np.arange(len(d)) / sr - 0.33) / 0.03, 0, 1)
g = 1 - ramp * (1 - tail)
d = d * signal.sosfiltfilt(signal.butter(2, 30, 'lowpass', fs=sr, output='sos'), g).clip(0, 1)
d = d / np.abs(d).max()
# Single sharp creaks set the peak: a limiter (4x oversampled) takes 5 dB off them, so
# the rest can be that much louder
shaped, limited = os.path.join(tmp, 'shaped.wav'), os.path.join(tmp, 'limited.wav')
write(shaped, d * 10**(-6/20))
subprocess.run(["ffmpeg","-hide_banner","-loglevel","error","-y","-i",shaped,"-af",
  "aresample=176400,alimiter=level_in=3.55:limit=0.89:attack=2:release=60:level=false,aresample=44100:resampler=soxr",
  "-c:a","pcm_s16le",limited], check=True)
d = read(limited)[:len(d)]
d = d / np.abs(d).max() * 10**(-1/20)
# fades 3 ms in, 15 ms out
fi, fo = int(0.003*sr), int(0.015*sr)
d[:fi] *= np.linspace(0, 1, fi); d[-fo:] *= np.linspace(1, 0, fo)
write(dst, d)
print("level per 20 ms (dB):", seg_db(d))
