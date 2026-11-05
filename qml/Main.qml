// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Main window. One layout for all devices:
//   narrow (phone, small window): tab bar at the bottom
//   wide (tablet, desktop):       sidebar on the left

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import Qupil

ApplicationWindow {
    id: window

    width: 1100
    height: 720
    minimumWidth: 360
    minimumHeight: 560
    // Shown from main.cpp: on desktop systems where it was last time
    color: Theme.bg
    title: qsTrId("app.window_title").arg(App.versionText)

    font.family: Theme.fontFamily

    // Standard controls (combo box popups, scroll bars, …) follow the theme
    palette.window: Theme.bg
    palette.windowText: Theme.text
    palette.base: Theme.surface
    palette.alternateBase: Theme.surfaceAlt
    palette.text: Theme.text
    palette.button: Theme.surfaceAlt
    palette.buttonText: Theme.text
    palette.brightText: Theme.textOnAccent
    palette.highlight: Theme.accent
    palette.highlightedText: Theme.textOnAccent
    palette.light: Theme.surfaceAlt
    palette.midlight: Theme.border
    palette.mid: Theme.border
    palette.dark: Theme.textMuted
    palette.placeholderText: Theme.textMuted
    palette.toolTipBase: Theme.inverse
    palette.toolTipText: Theme.textOnInverse

    readonly property bool compact: width < 720
    property int currentPage: 0
    //! A tab of the navigation was clicked: "More" always opens on its first level,
    //! whatever page of it was open before
    function selectPage(index) {
        const more = index === 4 ? loadedPage(4) : null
        if (more)
            more.home()
        currentPage = index
    }
    property Item pageLayout: null
    //! The page of a tab if it was made already (pages other than the schedule are made on
    //! their first visit: a faster start); null otherwise
    function loadedPage(index) {
        const loader = pageLayout ? pageLayout.children[index] : null
        return loader && loader.item ? loader.item : null
    }
    //! The page of a tab, made now if needed. fresh: it did not exist yet, so it missed
    //! the Nav signal that asked for it and has to be told directly
    function makePage(index) {
        const loader = pageLayout ? pageLayout.children[index] : null
        if (!loader)
            return { item: null, fresh: false }
        const fresh = !loader.active
        loader.active = true
        return { item: loader.item, fresh: fresh }
    }
    onCurrentPageChanged: makePage(currentPage)

    /*!
        System back button (Android), called from C++ (BackKey): one step back in
        the current tab, then to the first tab. False on the first screen: Qupil
        then goes to the background.
    */
    function goBack() {
        if (App.phase !== App.Ready || !pageLayout)
            return false
        const page = loadedPage(currentPage)
        if (page && typeof page.back === "function" && page.back())
            return true
        if (currentPage !== 0) {
            currentPage = 0
            return true
        }
        return false
    }

    // Static model: a language switch only re-evaluates the labels,
    // it does not destroy and re-create the navigation buttons.
    readonly property var navigation: ["calendar", "students", "events", "metronome", "more"]

    function navLabel(index) {
        switch (index) {
        case 0: return qsTrId("nav.schedule")
        case 1: return qsTrId("nav.students")
        case 2: return qsTrId("nav.events")
        case 3: return qsTrId("metronome.title")
        default: return qsTrId("nav.more")
        }
    }

    // First start (or unreadable data): setup screen, otherwise the app. Desktop: made after
    // the window's first frame, so it shows in its background colour at once, not black.
    // Phones and tablets show their launch screen until the first frame: it has to have the
    // content, an empty one ends the launch screen early and the content flickers in after it
    property bool firstFrame: !App.desktop
    onFrameSwapped: if (!firstFrame) Qt.callLater(() => window.firstFrame = true)

    // Room for the on-screen keyboard (Nav.keyboardInset). Editors (sheets) make room
    // themselves, the pages only when typing in a field on the page (search, settings).
    // - Android: the window keeps its size (App.keyboardHeight, measured by QupilActivity);
    //   the room grows and shrinks with the keyboard.
    // - iOS: the keyboard's height from Qt, at once. Otherwise iOS moves the whole view up
    //   to show the cursor, and on the iPad an editor's header with Save went off the top.
    //   An editor that opens for typing makes room before the keyboard comes (its height
    //   last time, Nav.expectKeyboard). While editors are open, iOS's own moving of the
    //   view is undone (App.holdViewForKeyboard): both together took the editor off the
    //   top. Pages keep iOS's way (they do not make room themselves there).
    readonly property bool ios: Qt.platform.os === "ios"
    readonly property real iosKeyboard: {
        if (!ios || !Qt.inputMethod.visible)
            return 0
        const r = Qt.inputMethod.keyboardRectangle
        return r.width >= width - 1 ? r.height : 0      // docked (a floating one on the iPad: no room)
    }
    property real keyboardPortrait: 0
    property real keyboardLandscape: 0
    onIosKeyboardChanged: {
        if (iosKeyboard <= 0)
            return
        if (width > height)
            keyboardLandscape = iosKeyboard
        else
            keyboardPortrait = iosKeyboard
    }
    readonly property real expectedKeyboard: Nav.expectKeyboard && iosKeyboard === 0
                                             ? (width > height ? keyboardLandscape : keyboardPortrait) : 0
    property real keyboardInset: ios ? Math.max(iosKeyboard, expectedKeyboard) : App.keyboardHeight
    Behavior on keyboardInset {
        enabled: !window.ios
        NumberAnimation { duration: 250; easing.type: Easing.OutCubic }
    }
    Binding {
        target: Nav
        property: "keyboardInset"
        value: window.keyboardInset
    }
    function inPopup(item) {
        for (let p = item; p; p = p.parent)
            if (p === Overlay.overlay)
                return true
        return false
    }
    readonly property real pageKeyboardInset: !ios && activeFocusItem && !inPopup(activeFocusItem) ? keyboardInset : 0
    Connections {
        target: Nav
        enabled: window.ios
        function onOpenSheetsChanged() { App.holdViewForKeyboard(Nav.openSheets > 0) }
    }

    Loader {
        anchors.fill: parent
        anchors.bottomMargin: window.pageKeyboardInset
        active: window.firstFrame
        sourceComponent: App.phase === App.Ready ? mainUi : setupUi
    }

    Component {
        id: setupUi
        SetupPage {}
    }

    Component {
        id: mainUi

        RowLayout {
            spacing: 0

            // ------------------------------------------------------------ sidebar (wide)
            // Always on top: pages sliding in or out pass underneath, never over it
            Rectangle {
                visible: !window.compact
                z: 1
                Layout.fillHeight: true
                Layout.preferredWidth: 216
                color: Theme.surface

                Rectangle {
                    anchors.right: parent.right
                    width: 1
                    height: parent.height
                    color: Theme.border
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 14
                    anchors.topMargin: 28
                    spacing: 4

                    RowLayout {
                        Layout.leftMargin: 10
                        Layout.bottomMargin: 18
                        spacing: 8
                        Text {
                            text: "Qupil"
                            color: Theme.accentText
                            font.family: Theme.fontFamily
                            font.pixelSize: 24
                            font.weight: Font.ExtraBold
                        }
                        BuildChip {}
                    }

                    Repeater {
                        model: window.navigation
                        delegate: NavButton {
                            required property string modelData
                            required property int index
                            Layout.fillWidth: true
                            vertical: false
                            text: window.navLabel(index)
                            iconName: modelData
                            selected: window.currentPage === index
                            onClicked: window.selectPage(index)
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }

            // ------------------------------------------------------------ pages
            // Clipped: a page sliding in (student, lesson …) stays right of the sidebar
            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                currentIndex: window.currentPage
                Component.onCompleted: window.pageLayout = this

                // The schedule at once, the others on their first visit (they stay then)
                Loader { sourceComponent: Component { SchedulePage {} } }
                Loader { active: false; sourceComponent: Component { StudentsPage {} } }
                Loader { active: false; sourceComponent: Component { EventsPage {} } }
                Loader { active: false; sourceComponent: Component { MetronomePage {} } }
                Loader { active: false; sourceComponent: Component { MorePage {} } }
            }
        }
    }

    // ---------------------------------------------------------------- tab bar (narrow)
    footer: Rectangle {
        readonly property bool shown: window.compact && App.phase === App.Ready
        visible: shown
        implicitHeight: shown ? 64 : 0
        color: Theme.surface

        Rectangle {
            width: parent.width
            height: 1
            color: Theme.border
        }

        RowLayout {
            anchors.fill: parent
            anchors.topMargin: 4
            anchors.leftMargin: 4
            anchors.rightMargin: 4
            spacing: 0

            Repeater {
                model: window.navigation
                delegate: NavButton {
                    required property string modelData
                    required property int index
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    vertical: true
                    text: window.navLabel(index)
                    iconName: modelData
                    selected: window.currentPage === index
                    onClicked: window.selectPage(index)
                }
            }
        }
    }

    // ---------------------------------------------------------------- take-over / restore feedback
    // Shared by the setup screen and More › Data

    Popup {
        id: progressPopup
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: Math.min(parent.width - 32, 400)
        modal: true
        closePolicy: Popup.NoAutoClose
        visible: App.busy
        padding: 24
        Overlay.modal: Rectangle { color: Theme.overlay }
        background: Rectangle {
            radius: Theme.radiusLarge
            color: Theme.surface
        }
        contentItem: ColumnLayout {
            spacing: 14
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: App.testMode ? qsTrId("testmode.preparing") : qsTrId("migration.importing")
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 17
                font.weight: Font.Bold
            }
            ProgressBar {
                id: bar
                Layout.fillWidth: true
                from: 0
                to: 100
                value: App.progress
                background: Rectangle {
                    implicitHeight: 8
                    radius: 4
                    color: Theme.segment
                }
                contentItem: Item {
                    implicitHeight: 8
                    Rectangle {
                        width: bar.visualPosition * parent.width
                        height: parent.height
                        radius: 4
                        color: Theme.accent
                    }
                }
            }
            Text {
                Layout.fillWidth: true
                elide: Text.ElideRight
                text: window.stepLabel(App.progressStep)
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 14
            }
        }
    }

    function stepLabel(step) {
        switch (step) {
        case "students": return qsTrId("nav.students")
        case "archive": return qsTrId("common.archive")
        case "lessons": return qsTrId("summary.step.lessons")
        case "pieces": return qsTrId("common.pieces")
        case "notes": return qsTrId("common.notes")
        case "activities": return qsTrId("common.activities")
        case "library": return qsTrId("library.title")
        case "events": return qsTrId("nav.events")
        case "reminders": return qsTrId("reminders.title")
        case "settings": return qsTrId("settings.title")
        default: return ""
        }
    }

    AppDialog {
        id: failedDialog
        iconName: "warning"
        destructive: true
        title: qsTrId("takeover.failed")
    }

    // After a take-over from More › Data (the setup screen shows its own summary)
    AppDialog {
        id: summaryDialog
        title: App.lastTakeOver.kind === "backup" ? qsTrId("summary.title.backup")
             : App.lastTakeOver.kind === "demo" ? qsTrId("summary.title.demo")
             : qsTrId("summary.title.legacy")
        acceptText: qsTrId("common.done")
        onAccepted: App.dismissTakeOver()
        TakeOverSummary {
            Layout.fillWidth: true
            result: App.lastTakeOver
        }
    }

    // Qupil from the Microsoft Store while the version from the setup is still installed:
    // offer to remove it, so there is one Qupil in the Start menu (the data is shared and stays)
    AppDialog {
        id: oldSetupDialog
        iconName: "backup"
        title: qsTrId("app.old_setup.title")
        text: qsTrId("app.old_setup.text").arg(App.setupInstallation.version || "")
        acceptText: qsTrId("app.old_setup.remove")
        rejectText: qsTrId("app.old_setup.later")
        onAccepted: {
            if (!App.removeSetupInstallation())
                Nav.toast(qsTrId("app.old_setup.failed"))
        }
    }
    Timer {
        interval: 2500
        running: App.storePackage && App.phase === App.Ready
        onTriggered: {
            if (App.setupInstallation.version === undefined)
                return
            // Not on top of the reminders at start or another question
            if (reminderDialog.visible || summaryDialog.visible || joinDialog.visible) {
                restart()
                return
            }
            oldSetupDialog.open()
        }
    }

    //! Closes all editor sheets (used by the screenshot tour)
    function closeEditors() {
        for (const name in sheets)
            sheets[name].close()
        reminderDialog.close()
    }

    // ---------------------------------------------------------------- editors used from every page
    // Made on first use, not at the start (a faster start); they stay then
    property var sheets: ({})
    function sheet(name) {
        if (!sheets[name]) {
            const components = { note: noteEditor, planFinish: planFinish, piece: pieceEditor, student: newStudent,
                                 lesson: lessonEditor, activity: activityEditor, reminder: reminderEditor,
                                 pairing: pairingSheet }
            sheets[name] = components[name].createObject(window.contentItem)
        }
        return sheets[name]
    }
    Component { id: noteEditor; NoteEditor {} }
    Component {
        id: planFinish
        PlanFinish {
            onEditRequested: (membership, plan, content, rest, restCount) =>
                             window.sheet("note").fromPlan(membership, plan, content, rest, restCount)
        }
    }
    Component { id: pieceEditor; PieceEditor {} }
    Component { id: newStudent; NewStudent {} }
    Component { id: lessonEditor; LessonEditor {} }
    Component { id: activityEditor; ActivityEditor {} }
    Component { id: reminderEditor; ReminderEditor {} }
    Component { id: pairingSheet; PairingSheet {} }

    // Both devices had data when they were paired (or the data here was replaced)
    AppDialog {
        id: joinDialog
        property string peerId
        property string peerName
        property int mode: Sync.ReplaceLocal

        function ask(id, name) {
            peerId = id
            peerName = name
            mode = Sync.ReplaceLocal
            open()
        }

        iconName: "wifi"
        title: qsTrId("wifi.join.title")
        text: qsTrId("wifi.join.text").arg(peerName)
        rejectText: qsTrId("common.cancel")
        acceptText: qsTrId("wifi.join.action")
        onAccepted: Wifi.resolveJoin(peerId, mode)

        ChoiceOption {
            text: qsTrId("wifi.join.replace").arg(joinDialog.peerName)
            hint: qsTrId("sync.join.replace.hint")
            selected: joinDialog.mode === Sync.ReplaceLocal
            onClicked: joinDialog.mode = Sync.ReplaceLocal
        }
        ChoiceOption {
            text: qsTrId("wifi.join.merge")
            hint: qsTrId("sync.join.merge.hint")
            selected: joinDialog.mode === Sync.Merge
            onClicked: joinDialog.mode = Sync.Merge
        }
    }

    Connections {
        target: Nav
        function onNewStudent() { window.sheet("student").start() }
        function onEditLesson(id, weekday) { window.sheet("lesson").edit(id, weekday) }
        function onEditNote(membership, note) { window.sheet("note").edit(membership, note) }
        function onNewNoteFrom(membership, note) { window.sheet("note").copy(membership, note) }
        function onEditPlan(membership, plan) { window.sheet("note").editPlan(membership, plan) }
        function onFinishPlan(membership, plan) { window.sheet("planFinish").start(membership, plan) }
        function onPlanFromNote(membership, note) { window.sheet("planFinish").startFromNote(membership, note) }
        function onEditPiece(membership, piece) { window.sheet("piece").edit(membership, piece) }
        function onEditActivity(student, activity, continuous) { window.sheet("activity").edit(student, activity, continuous) }
        function onEditReminder(reminder, student) { window.sheet("reminder").edit(reminder, student) }
        function onShowPage(index) { window.currentPage = index }
        // A page made just now missed the request: it is told directly
        function onShowStudent(id, membership) {
            const p = window.makePage(1)
            window.currentPage = 1
            if (p.fresh && p.item)
                p.item.show(id, membership, "")
        }
        function onShowPiece(id, membership, piece) {
            const p = window.makePage(1)
            window.currentPage = 1
            if (p.fresh && p.item)
                p.item.show(id, membership, piece)
        }
        function onShowEvent(id) {
            const p = window.makePage(2)
            window.currentPage = 2
            if (p.fresh && p.item)
                p.item.open(id)
        }
        function onShowSync() {
            const p = window.makePage(4)
            window.currentPage = 4
            if (p.fresh && p.item)
                p.item.openSettings("sync")
        }
        function onShowMore(name) {
            const p = window.makePage(4)
            window.currentPage = 4
            if (p.fresh && p.item)
                p.item.showSubPage(name)
        }
        function onPairDevice() { window.sheet("pairing").start() }
        function onDecideJoin(id, name) { joinDialog.ask(id, name) }
        function onToast(message) {
            toast.text = message
            toast.show = true
            toastTimer.restart()
        }
    }

    // Space starts and stops the metronome on its page (desktop)
    Shortcut {
        sequence: "Space"
        enabled: window.currentPage === 3 && App.phase === App.Ready
        onActivated: Metronome.toggle()
    }

    // Keyboard shortcuts of Qupil 1.x (Ctrl = ⌘ on a Mac); both the English and
    // the German letters of the two-key ones
    Shortcut {
        enabled: App.phase === App.Ready
        sequences: ["Ctrl+N, S", "Ctrl+N, P"]
        onActivated: Nav.newStudent()
    }
    Shortcut {
        enabled: App.phase === App.Ready
        sequences: ["Ctrl+N, U", "Ctrl+N, L"]
        onActivated: Nav.editLesson("", (Data.now().getDay() + 6) % 7)
    }
    Shortcut {
        enabled: App.phase === App.Ready
        sequence: "Ctrl+D"
        onActivated: Nav.showMore("dayOverview")
    }
    Shortcut {
        enabled: App.phase === App.Ready
        sequence: "Ctrl+L"
        onActivated: Nav.showMore("library")
    }
    Shortcut {
        enabled: App.phase === App.Ready
        sequence: "Ctrl+M"
        onActivated: window.currentPage = 3
    }
    Shortcut {
        enabled: App.phase === App.Ready
        sequence: "Ctrl+B"
        onActivated: Nav.showMore("birthdays")
    }
    Shortcut {
        enabled: App.phase === App.Ready
        sequence: StandardKey.Preferences
        onActivated: Nav.showMore("settings")
    }
    Shortcut {
        enabled: App.desktop
        sequence: "Ctrl+Q"
        onActivated: Qt.quit()
    }

    // ---------------------------------------------------------------- test mode (sample data)
    // Unmistakable: an amber frame around everything and a label at the bottom left with
    // the way back. Not at the top: the status bar of iOS lies over Qupil there.
    readonly property color testColour: "#F2A531"
    Rectangle {
        anchors.fill: parent
        visible: App.testMode
        color: "transparent"
        border.width: 3
        border.color: window.testColour
        z: 1000
    }
    Rectangle {
        id: testLabel
        visible: App.testMode && App.phase === App.Ready
        z: 1000
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: 14
        anchors.bottomMargin: 14
        implicitWidth: testRow.implicitWidth + 12
        implicitHeight: 40
        radius: 20
        color: window.testColour
        RowLayout {
            id: testRow
            anchors.verticalCenter: parent.verticalCenter
            x: 14
            spacing: 6
            Text {
                text: qsTrId("testmode.badge")
                color: "#1D1A22"
                font.family: Theme.fontFamily
                font.pixelSize: 14
                font.weight: Font.ExtraBold
            }
            AbstractButton {
                id: testEnd
                implicitWidth: endText.implicitWidth + 20
                implicitHeight: 30
                focusPolicy: Qt.TabFocus
                enabled: !App.busy
                Accessible.role: Accessible.Button
                Accessible.name: endText.text
                onClicked: {
                    Nav.toast(qsTrId("testmode.off"))
                    App.setTestMode(false)
                }
                background: Rectangle {
                    radius: 15
                    color: testEnd.down ? "#33000000" : "#1F000000"
                }
                contentItem: Text {
                    id: endText
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: qsTrId("testmode.end")
                    color: "#1D1A22"
                    font.family: Theme.fontFamily
                    font.pixelSize: 14
                    font.weight: Font.Bold
                }
            }
        }
    }

    // ---------------------------------------------------------------- running metronome, everywhere else
    Rectangle {
        id: miniPlayer
        visible: Metronome.running && window.currentPage !== 3 && App.phase === App.Ready
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 20
        anchors.bottomMargin: 20
        implicitWidth: playerRow.implicitWidth + 26
        implicitHeight: 56
        radius: 28
        color: Theme.player
        z: 10
        RowLayout {
            id: playerRow
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 18
            spacing: 12
            Rectangle {
                implicitWidth: 12
                implicitHeight: 12
                radius: 6
                color: Metronome.beat === 0 ? Theme.lime : Theme.playerMuted
            }
            ColumnLayout {
                spacing: 0
                Text {
                    text: Metronome.bpm + " " + qsTrId("metronome.unit.bpm")
                    color: Theme.textOnPlayer
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                    font.weight: Font.ExtraBold
                }
                Text {
                    text: qsTrId("metronome.accent_every") + " " + Metronome.beatsPerBar
                    color: Theme.playerMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 12
                }
            }
            IconButton {
                size: 42
                fill: Theme.lime
                tint: Theme.textOnLime
                iconName: "stop"
                text: qsTrId("metronome.stop")
                onClicked: Metronome.stop()
            }
        }
        TapHandler { onTapped: window.currentPage = 3 }
    }
    Binding {
        target: Nav
        property: "playerInset"
        value: miniPlayer.visible ? miniPlayer.height + miniPlayer.anchors.bottomMargin - 8 : 0
    }

    // ---------------------------------------------------------------- short messages
    Rectangle {
        id: toast
        property alias text: toastText.text
        property bool show: false
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 24
        width: Math.min(toastText.implicitWidth + 40, parent.width - 40)
        height: toastText.implicitHeight + 24
        radius: 14
        color: Theme.inverse
        opacity: show ? 1 : 0
        visible: opacity > 0
        z: 20
        Behavior on opacity { NumberAnimation { duration: 180 } }
        Text {
            id: toastText
            anchors.centerIn: parent
            width: Math.min(implicitWidth, toast.parent ? toast.parent.width - 80 : 400)
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            color: Theme.textOnInverse
            font.family: Theme.fontFamily
            font.pixelSize: 14
            font.weight: Font.DemiBold
        }
        Timer {
            id: toastTimer
            interval: Math.max(2800, toast.text.length * 60)   // time to read longer messages
            onTriggered: toast.show = false
        }
    }

    Connections {
        target: Sync
        function onMessage(text) { Nav.toast(text) }
    }
    Connections {
        target: Docs
        function onFailed(message) { Nav.toast(message) }
        function onExported(message) { Nav.toast(message) }
        function onSaveRequested(file, name) {
            pdfSaveDialog.file = file
            pdfSaveDialog.selectedFile = pdfSaveDialog.currentFolder + "/" + name
            pdfSaveDialog.open()
        }
    }
    // Android: where a PDF goes (the system's save dialog)
    FileDialog {
        id: pdfSaveDialog
        property url file
        title: qsTrId("doc.save_title")
        fileMode: FileDialog.SaveFile
        defaultSuffix: "pdf"
        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        onAccepted: Docs.saveCopy(file, selectedFile, App.displayPath(selectedFile))
        onRejected: Docs.discard(file)
    }

    // Reminders "when Qupil starts" and at the start of a lesson (Qupil 1.x pop-ups);
    // student reminders can be ticked as done
    AppDialog {
        id: reminderDialog
        // Changed only while open: layouts must not lose items while hidden
        property var group: null
        property var done: ({})
        iconName: "bell"
        title: group ? group.title : ""
        text: group ? group.subtitle : ""
        function showNext() {
            if (ReminderAlert.pending.length > 0 && !visible) {
                done = {}
                open()
                group = ReminderAlert.pending[0]
                window.alert(0)
            }
        }
        // OK, Escape: seen; ticked student reminders are deleted
        onClosed: {
            const ids = Object.keys(done).filter(id => done[id])
            Qt.callLater(() => {
                ReminderAlert.acknowledge(ids)
                showNext()
            })
        }
        Repeater {
            model: reminderDialog.group ? reminderDialog.group.items : []
            delegate: Rectangle {
                id: reminderItem
                required property var modelData
                Layout.fillWidth: true
                implicitHeight: itemColumn.implicitHeight + 24
                radius: 14
                color: Theme.surfaceAlt
                ColumnLayout {
                    id: itemColumn
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 12
                    spacing: 6
                    Text {
                        visible: reminderItem.modelData.mode === 2
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: qsTrId("reminder.for_student").arg(reminderItem.modelData.studentName || "")
                        color: Theme.accentText
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                        font.weight: Font.Bold
                    }
                    Text {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: reminderItem.modelData.text
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 16
                        font.weight: Font.Bold
                    }
                    CheckBox {
                        visible: reminderItem.modelData.mode === 2
                        Layout.leftMargin: -6
                        text: qsTrId("reminder.done_remove")
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                        onToggled: reminderDialog.done[reminderItem.modelData.id] = checked
                    }
                }
            }
        }
    }
    Connections {
        target: ReminderAlert
        function onPendingChanged() { reminderDialog.showNext() }
        function onMessage(text) { Nav.toast(text) }
    }

    // Desktop: message (the sound already plays) shortly before a lesson ends.
    // iOS and Android show a system notification instead.
    Connections {
        target: LessonEnd
        function onAlert(title, text) {
            Nav.toast(text ? title + " · " + text : title)
            window.alert(0)   // taskbar / dock, if Qupil is not in front
        }
        function onMessage(text) { Nav.toast(text) }
    }

    Connections {
        target: Wifi
        function onMessage(text) { Nav.toast(text) }
        function onJoinChoiceNeeded(id, name) { joinDialog.ask(id, name) }
    }

    Connections {
        target: App
        function onTakeOverFailed(message) {
            failedDialog.text = message
            failedDialog.open()
        }
        function onSetupRemoval(removed) {
            Nav.toast(qsTrId(removed ? "app.old_setup.removed" : "app.old_setup.failed"))
        }
        function onLastTakeOverChanged() {
            if (App.phase === App.Ready && App.lastTakeOver.kind !== undefined)
                summaryDialog.open()
        }
    }
}
