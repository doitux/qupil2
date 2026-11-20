// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// How a teacher gets into the app: scan the QR code with the phone's camera (opens the
// app by itself), or paste the link the teacher sent.

import QtQuick
import QtQuick.Controls
import QupilStudent

Column {
    id: add
    property bool showDemo: true
    property string error

    spacing: 12

    function tryLink(text) {
        const result = StudentApp.addLink(text)
        add.error = result === "" ? "" : qsTrId("s.add.invalid")
        if (result === "")
            field.text = ""
    }

    Card {
        width: parent.width
        spacing: 10
        Row {
            spacing: 12
            width: parent.width
            Rectangle {
                width: 44; height: 44; radius: 12
                color: Theme.accentSoft
                Icon { anchors.centerIn: parent; name: "qr"; color: Theme.accentText }
            }
            Column {
                width: parent.width - 56
                spacing: 3
                T { width: parent.width; text: qsTrId("s.add.scan.title"); font.weight: Font.Bold; font.pixelSize: 16 }
                T { width: parent.width; text: qsTrId("s.add.scan.text"); color: Theme.textMuted; font.pixelSize: 14 }
            }
        }
    }

    Card {
        width: parent.width
        spacing: 10
        Row {
            spacing: 12
            width: parent.width
            Rectangle {
                width: 44; height: 44; radius: 12
                color: Theme.accentSoft
                Icon { anchors.centerIn: parent; name: "link"; color: Theme.accentText }
            }
            Column {
                width: parent.width - 56
                spacing: 3
                T { width: parent.width; text: qsTrId("s.add.link.title"); font.weight: Font.Bold; font.pixelSize: 16 }
                T { width: parent.width; text: qsTrId("s.add.link.text"); color: Theme.textMuted; font.pixelSize: 14 }
            }
        }
        Rectangle {
            width: parent.width
            height: 46
            radius: 12
            color: Theme.surfaceAlt
            border.width: field.activeFocus ? 2 : 0
            border.color: Theme.accent
            TextField {
                id: field
                objectName: "linkField"
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 12
                placeholderText: "https://qupil.de/u#…"
                placeholderTextColor: Theme.textMuted
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 15
                background: null
                inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
                onAccepted: add.tryLink(text)
            }
        }
        Row {
            width: parent.width
            spacing: 8
            AppButton {
                objectName: "pasteButton"
                width: (parent.width - 8) / 2
                text: qsTrId("s.add.paste")
                onClicked: {
                    const link = StudentApp.clipboardLink()
                    if (link === "")
                        add.error = qsTrId("s.add.clipboard_empty")
                    else
                        add.tryLink(link)
                }
            }
            AppButton {
                objectName: "openLinkButton"
                width: (parent.width - 8) / 2
                primary: true
                enabled: field.text.trim() !== ""
                text: qsTrId("s.add.open")
                onClicked: add.tryLink(field.text)
            }
        }
        T {
            objectName: "addError"
            visible: add.error !== ""
            width: parent.width
            text: add.error
            color: Theme.danger
            font.pixelSize: 14
            font.weight: Font.DemiBold
        }
    }

    AppButton {
        objectName: "demoButton"
        visible: add.showDemo
        width: parent.width
        icon: "eye"
        text: qsTrId("s.add.demo")
        onClicked: StudentApp.openDemo()
    }
}
