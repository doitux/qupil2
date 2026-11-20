// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// First start: what the app is for, and how the teacher gets in.

import QtQuick
import QtQuick.Controls
import QupilStudent

Page {
    background: Rectangle { color: Theme.bg }

    Flickable {
        anchors.fill: parent
        contentHeight: column.implicitHeight + window.topInset + window.bottomInset + 48
        boundsBehavior: Flickable.StopAtBounds

        Column {
            id: column
            x: Theme.gutter
            y: window.topInset + 28
            width: parent.width - 2 * Theme.gutter
            spacing: 18

            Rectangle {
                width: 72; height: 72; radius: 20
                color: Theme.lime
                Icon { anchors.centerIn: parent; width: 38; height: 38; name: "book"; color: "#6E3C5A"; strokeWidth: 2.2 }
            }
            Column {
                width: parent.width
                spacing: 8
                T {
                    width: parent.width
                    text: qsTrId("s.welcome.title")
                    font.pixelSize: 30
                    font.weight: Font.ExtraBold
                }
                T {
                    width: parent.width
                    text: qsTrId("s.welcome.text")
                    font.pixelSize: 16
                    color: Theme.textSecondary
                }
            }
            AddTeacher { width: parent.width }
            T {
                width: parent.width
                text: qsTrId("s.welcome.privacy")
                font.pixelSize: 13
                color: Theme.textMuted
            }
        }
    }
}
