// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Teachers (remove one, add another) and what the app sends where.

import QtQuick
import QtQuick.Controls
import QupilStudent

Page {
    id: page
    background: Rectangle { color: Theme.bg }

    property string confirmId     // teacher whose removal waits for a second tap

    Rectangle {
        id: bar
        z: 2
        width: parent.width
        height: window.topInset + 56
        color: Theme.bg
        IconButton {
            objectName: "backButton"
            x: 6
            y: window.topInset + 6
            icon: "back"
            label: qsTrId("s.back")
            onClicked: window.back()
        }
        T {
            anchors.horizontalCenter: parent.horizontalCenter
            y: window.topInset + 16
            text: qsTrId("s.settings.title")
            font.pixelSize: 18
            font.weight: Font.ExtraBold
        }
    }

    Flickable {
        anchors.fill: parent
        anchors.topMargin: bar.height
        contentHeight: column.implicitHeight + window.bottomInset + 40
        boundsBehavior: Flickable.StopAtBounds

        Column {
            id: column
            x: Theme.gutter
            y: 4
            width: parent.width - 2 * Theme.gutter
            spacing: 10

            SectionLabel { text: qsTrId("s.settings.teachers") }
            Card {
                width: parent.width
                padding: 4
                spacing: 0
                Repeater {
                    model: StudentApp.teachers
                    Item {
                        required property var modelData
                        required property int index
                        readonly property bool confirming: page.confirmId === modelData.id
                        width: parent.width
                        height: Math.max(62, info.implicitHeight + 22)
                        Rectangle { visible: index > 0; x: 12; width: parent.width - 24; height: 1; color: Theme.divider }
                        Column {
                            id: info
                            x: 12
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width - x - removeButton.width - 16
                            spacing: 2
                            T {
                                width: parent.width
                                text: modelData.teacher !== "" ? modelData.teacher : qsTrId("s.settings.unknown_teacher")
                                font.weight: Font.Bold
                                font.pixelSize: 16
                            }
                            T {
                                width: parent.width
                                text: modelData.demo ? qsTrId("s.settings.demo")
                                                     : [modelData.instrument, modelData.student].filter(s => s && s !== "").join(" · ")
                                visible: text !== ""
                                color: Theme.textMuted
                                font.pixelSize: 13
                            }
                        }
                        Rectangle {
                            id: removeButton
                            objectName: "remove-" + modelData.id
                            anchors.right: parent.right
                            anchors.rightMargin: 8
                            anchors.verticalCenter: parent.verticalCenter
                            width: confirming ? removeText.implicitWidth + 24 : Theme.touchTarget
                            height: confirming ? 36 : Theme.touchTarget
                            radius: height / 2
                            color: confirming ? Theme.danger : "transparent"
                            Icon { visible: !confirming; anchors.centerIn: parent; name: "trash"; width: 20; height: 20; color: Theme.icon }
                            T { id: removeText; visible: confirming; anchors.centerIn: parent; text: qsTrId("s.settings.remove_confirm"); color: "white"; font.weight: Font.Bold; font.pixelSize: 14; wrapMode: Text.NoWrap }
                            Accessible.role: Accessible.Button
                            Accessible.name: confirming ? qsTrId("s.settings.remove_confirm") : qsTrId("s.settings.remove")
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    if (confirming) {
                                        page.confirmId = ""
                                        StudentApp.removeTeacher(modelData.id)
                                    } else {
                                        page.confirmId = modelData.id
                                    }
                                }
                            }
                        }
                    }
                }
            }
            T {
                visible: page.confirmId !== ""
                width: parent.width
                text: qsTrId("s.settings.remove_hint")
                color: Theme.textMuted
                font.pixelSize: 13
            }

            SectionLabel { text: qsTrId("s.settings.add"); topPadding: 12 }
            AddTeacher { width: parent.width; showDemo: !StudentApp.teachers.some(t => t.demo) }

            SectionLabel { text: qsTrId("s.settings.privacy"); topPadding: 12 }
            Card {
                width: parent.width
                T { width: parent.width; text: qsTrId("s.settings.privacy_text"); color: Theme.textSecondary; font.pixelSize: 14 }
            }

            T {
                width: parent.width
                topPadding: 10
                horizontalAlignment: Text.AlignHCenter
                text: qsTrId("s.app.name") + " " + StudentApp.version
                color: Theme.textMuted
                font.pixelSize: 12
            }
        }
    }
}
