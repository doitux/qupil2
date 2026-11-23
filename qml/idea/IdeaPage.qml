// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Settings › "Try the new idea": a trial of the proposal "tasks, lesson, history" with
// three invented students. It is there to be judged before anything is rebuilt: it uses
// none of the real data and stores nothing (IdeaModel).

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page

    signal openStudent(var demo, string studentId)

    title: qsTrId("idea.title")
    backText: qsTrId("settings.title")

    IdeaModel { id: model }

    Card {
        Layout.fillWidth: true
        color: Theme.accentSoft
        spacing: 8
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTrId("idea.intro.head")
            color: Theme.accentText
            font.family: Theme.fontFamily
            font.pixelSize: 16
            font.weight: Font.Bold
        }
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTrId("idea.intro.text")
            color: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 15
        }
    }

    SectionLabel { Layout.topMargin: 6; text: qsTrId("idea.students") }
    MenuGroup {
        Repeater {
            model: { model.revision; return model.students }
            AbstractButton {
                id: studentRow
                required property var modelData
                required property int index
                Layout.fillWidth: true
                implicitHeight: rowLayout.implicitHeight + 24
                focusPolicy: Qt.StrongFocus
                text: modelData.name
                Accessible.role: Accessible.Button
                Accessible.name: text
                Accessible.description: modelData.sub
                onClicked: page.openStudent(model, modelData.id)
                background: Rectangle {
                    color: studentRow.down ? Theme.surfaceAlt : "transparent"
                    Rectangle { visible: studentRow.index > 0; x: 16; width: parent.width - 32; height: 1; color: Theme.divider }
                    Rectangle { visible: studentRow.visualFocus; anchors.fill: parent; anchors.margins: 2; radius: Theme.radius
                                color: "transparent"; border.width: 2; border.color: Theme.accent }
                }
                contentItem: Item {
                    RowLayout {
                        id: rowLayout
                        x: 16
                        width: parent.width - 32
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 12
                        Avatar { text: studentRow.modelData.name.split(" ").map(w => w[0]).join(""); seed: studentRow.modelData.name }
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 2
                            Text {
                                Layout.fillWidth: true
                                text: studentRow.modelData.name
                                color: Theme.text
                                font.family: Theme.fontFamily
                                font.pixelSize: 16
                                font.weight: Font.DemiBold
                            }
                            Text {
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                                text: studentRow.modelData.sub
                                color: Theme.textMuted
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                            }
                        }
                        Icon { Layout.preferredWidth: 16; Layout.preferredHeight: 16; name: "chevron"; color: Theme.icon }
                    }
                }
            }
        }
    }

    SectionLabel { Layout.topMargin: 6; text: qsTrId("idea.setting") }
    MenuGroup {
        SwitchRow {
            text: qsTrId("idea.use_tasks")
            hint: qsTrId("idea.use_tasks.hint")
            checked: model.useTasks
            onToggled: {
                model.useTasks = checked
                model.revision++
            }
        }
    }

    ActionButton {
        Layout.topMargin: 6
        Layout.alignment: Qt.AlignLeft
        iconName: "restore"
        text: qsTrId("idea.reset")
        onClicked: model.reset()
    }
}
