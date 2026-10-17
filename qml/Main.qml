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
    property Item pageLayout: null

    /*!
        System back button (Android), called from C++ (BackKey): one step back in
        the current tab, then to the first tab. False on the first screen: Qupil
        then goes to the background.
    */
    function goBack() {
        if (App.phase !== App.Ready || !pageLayout)
            return false
        const page = pageLayout.children[currentPage]
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

    // First start (or unreadable data): setup screen, otherwise the app
    Loader {
        anchors.fill: parent
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
            Rectangle {
                visible: !window.compact
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
                            onClicked: window.currentPage = index
                        }
                    }

                    Item { Layout.fillHeight: true }
                }
            }

            // ------------------------------------------------------------ pages
            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: window.currentPage
                Component.onCompleted: window.pageLayout = this

                SchedulePage {}
                StudentsPage {}
                EventsPage {}
                MetronomePage {}
                MorePage {}
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
                    onClicked: window.currentPage = index
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
                text: qsTrId("migration.importing")
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

    //! Closes all editor sheets (used by the screenshot tour)
    function closeEditors() {
        for (const sheet of [noteEditor, pieceEditor, newStudent, lessonEditor, activityEditor, reminderEditor,
                             pairingSheet, reminderDialog])
            sheet.close()
    }

    // ---------------------------------------------------------------- editors used from every page
    NoteEditor { id: noteEditor }
    PieceEditor { id: pieceEditor }
    NewStudent { id: newStudent }
    LessonEditor { id: lessonEditor }
    ActivityEditor { id: activityEditor }
    ReminderEditor { id: reminderEditor }
    PairingSheet { id: pairingSheet }

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
        function onNewStudent() { newStudent.start() }
        function onEditLesson(id, weekday) { lessonEditor.edit(id, weekday) }
        function onEditNote(membership, note) { noteEditor.edit(membership, note) }
        function onNewNoteFrom(membership, note) { noteEditor.copy(membership, note) }
        function onEditPiece(membership, piece) { pieceEditor.edit(membership, piece) }
        function onEditActivity(student, activity, continuous) { activityEditor.edit(student, activity, continuous) }
        function onEditReminder(reminder, student) { reminderEditor.edit(reminder, student) }
        function onShowPage(index) { window.currentPage = index }
        function onShowStudent(id, membership) { window.currentPage = 1 }
        function onShowEvent(id) { window.currentPage = 2 }
        function onShowSync() { window.currentPage = 4 }
        function onShowMore(name) { window.currentPage = 4 }
        function onPairDevice() { pairingSheet.start() }
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
        onActivated: Nav.editLesson("", (new Date().getDay() + 6) % 7)
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
        function onLastTakeOverChanged() {
            if (App.phase === App.Ready && App.lastTakeOver.kind !== undefined)
                summaryDialog.open()
        }
    }
}
