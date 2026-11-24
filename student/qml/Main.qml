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

    // iOS: the window covers the whole screen, behind the status bar and the home indicator.
    // Kept inside the safe area (Qt's way without this), the strips above and below are
    // black with a white status bar, also in the light look. Android colours its bars
    // itself (res/values/student.xml) and the window ends at them.
    flags: Qt.platform.os === "ios" ? Qt.Window | Qt.MaximizeUsingFullscreenGeometryHint : Qt.Window

    readonly property bool coupled: StudentApp.teachers.length > 0
    // Room the status bar (or notch) and the home indicator take of the window: the pages
    // start below and end above it, their background goes behind it (SafeArea.h). 0
    // wherever the window ends at the system's bars.
    readonly property real topInset: StudentApp.safeTop
    readonly property real bottomInset: StudentApp.safeBottom

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

    // Behind the status bar: the page's colour, also over what scrolls up there (the
    // welcome page has no bar of its own)
    Rectangle {
        objectName: "statusBarRoom"
        z: 3
        width: parent.width
        height: window.topInset
        color: Theme.bg
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
