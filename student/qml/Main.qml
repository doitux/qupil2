// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// The student's booklet: the welcome page until a teacher is coupled, then the booklet;
// settings slide in from the right.

import QtQuick
import QtQuick.Controls
import QtQuick.Window
import QupilStudent

ApplicationWindow {
    id: window

    width: 390
    height: 844
    visible: true
    title: qsTrId("s.app.name")
    color: Theme.bg
    font.family: Theme.fontFamily

    readonly property bool coupled: StudentApp.teachers.length > 0
    // Room for notch and home indicator: Qt keeps the window inside the safe area (as in
    // Qupil); only if the window ever reaches below the status bar these come into play
    readonly property real topInset: 0
    readonly property real bottomInset: 0

    // Room for the on-screen keyboard. Android: the window keeps its size (the system
    // making it smaller let the page jump and go empty for a moment); the app measures the
    // keyboard (StudentApp.keyboardHeight) and the pages get lower by it, gliding with the
    // keyboard. The page keeps the field with the cursor in view (PageFlickable).
    // Elsewhere the height is 0: iOS moves the view itself.
    property real keyboardInset: StudentApp.keyboardHeight
    Behavior on keyboardInset { NumberAnimation { duration: 250; easing.type: Easing.OutCubic } }

    function showNotice(text) {
        toast.text = text
        toast.opacity = 1
        toastTimer.restart()
    }

    StackView {
        id: stack
        anchors.fill: parent
        anchors.bottomMargin: window.keyboardInset
        initialItem: window.coupled ? homePage : welcomePage
    }

    Component { id: welcomePage; WelcomePage {} }
    Component { id: homePage; HomePage {} }
    Component { id: settingsPage; SettingsPage {} }

    Connections {
        target: StudentApp
        function onNotice(text) { window.showNotice(text) }
        function onChanged() {
            // Coupled for the first time, or the last teacher removed
            const onHome = stack.depth > 0 && stack.get(0) instanceof HomePage
            if (window.coupled !== onHome)
                stack.replace(null, window.coupled ? homePage : welcomePage)
        }
    }

    // Pages ask for these
    function openSettings() { stack.push(settingsPage) }
    function back() { if (stack.depth > 1) stack.pop() }

    // ---- small message at the bottom
    Rectangle {
        id: toast
        property alias text: toastText.text
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: window.bottomInset + window.keyboardInset + 24
        width: Math.min(parent.width - 40, toastText.implicitWidth + 32)
        height: toastText.implicitHeight + 22
        radius: 14
        color: Theme.inverse
        opacity: 0
        visible: opacity > 0
        Behavior on opacity { NumberAnimation { duration: 180 } }
        T {
            id: toastText
            anchors.centerIn: parent
            width: Math.min(implicitWidth, window.width - 72)
            color: Theme.textOnInverse
            font.weight: Font.DemiBold
            horizontalAlignment: Text.AlignHCenter
        }
        Timer { id: toastTimer; interval: 3200; onTriggered: toast.opacity = 0 }
    }

    // Android back button
    onClosing: close => {
        if (stack.depth > 1) {
            close.accepted = false
            stack.pop()
        }
    }
}
