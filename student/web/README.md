# Files for qupil.de

Uploaded by hand (FTP) to the root of qupil.de:

- `u/index.html` – what a phone without the app shows for `https://qupil.de/u#…`
  (the QR code of the teacher's Qupil): open in the app (`qupil-student:` link), copy
  the link, store links (`APP_STORE`, `PLAY_STORE` at the bottom, empty until the app is
  in the stores). No tracking; the part after `#` never reaches the server.
- `.well-known/apple-app-site-association` – iOS opens `qupil.de/u` links in the app
  (team 8LBKF4942K, `de.qupil.student`; the app needs `QUPIL_STUDENT_ASSOCIATED_DOMAINS`)
- `.well-known/assetlinks.json` – the same for Android; for now the fingerprint of the
  public test key (`.github/android/qupil-test.keystore`); the Play Store key is added
  when the app goes to Google Play
- `.well-known/.htaccess` – serves both as JSON (Apache)
