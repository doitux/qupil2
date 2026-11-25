# Student app on the emulator
- device: Android SDK built for x86_64, Android 13

## A link starts the app
- logged: `kept until the app is ready`
- logged: `link received, teacher added`
- screenshot `shots/01-link-at-start.png`

## A link while the app is running
- logged: `link received, teacher added`
- logged: `link received, teacher added`
- logged: `link received, teacher added`
- screenshot `shots/02-link-while-running.png`

## The QR code scanner
- screenshot `shots/03-welcome.png`
- tapped `^(Open camera|Kamera öffnen)$` at 538 1217
- logged: `scanner opened`
- on top: `    topResumedActivity=ActivityRecord{f115be0 u0 de.qupil.student/com.journeyapps.barcodescanner.CaptureActivity} t10}`
- screenshot `shots/04-scanner.png`
- logged: `scanner closed, no code`
- screenshot `shots/05-after-scanner.png`

## The on-screen keyboard
- keyboards of the emulator: `com.android.inputmethod.latin/.LatinIME `
- tapped `qupil\.de/u#` at 539 1590
- logged: `keyboard: [1-9][0-9]* px`
- screenshot `shots/06-keyboard.png`
- the field (y 1125) is above the keyboard (775 px of 2400)
- logged: `keyboard: 0 px`
- screenshot `shots/07-keyboard-gone.png`

**All fine.**
