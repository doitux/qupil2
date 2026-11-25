# Student app in the iOS simulator
- iPhone 16

## The app starts
- logged: `Qupil student: started`
- the app is running
- screenshot `shots/01-welcome.png`

## The screen's edges
- logged: `safe area: top [1-9][0-9]* bottom [1-9]`
- top edge: the page's colour (F4F3F6)
- bottom edge: the page's colour (F4F3F6)

## A link starts the app (qupil-student:)
- tapped `^(Open|Öffnen)$` at 270 461
- logged: `link received, teacher added`
- screenshot `shots/02-link-at-start.png`
- top edge: the page's colour (F4F3F6)
- bottom edge: the page's colour (F4F3F6)

## A link while the app is running
- no `^(Open|Öffnen)$` to tap
- logged: `link received, teacher added`

## The button on the page qupil.de/u (Safari hands the link to the app)
- no `^(Continue|Weiter|Not Now|Jetzt nicht)$` to tap
- the button found by its colour (idb does not see into the page)
- screenshot `shots/03-page.png`
- tapped the page's button at 195 713
- screenshot `shots/04-safari-alert.png`
- Safari asked "Open in …?", tapped `Open` at 331 429
- logged: `link received, teacher added`
- screenshot `shots/05-page-button.png`

## The same button without the app: Safari's message (the check of the check)
- screenshot `shots/06-page-button-without-app.png`
- Safari's message with one button, as expected

## The scanner's sheet
- logged: `Qupil student: started`
- screenshot `shots/07-scanner.png`
- the app is running with the scanner open
- tapped `^(Cancel|Abbrechen)$` at 196 773
- logged: `scanner closed, no code`
- screenshot `shots/08-after-scanner.png`

**All fine.**
