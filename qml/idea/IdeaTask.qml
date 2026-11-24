// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "Try the new idea": a task as it is read (the student's page, the look into the
// student's app): title, where it stands, the weeks so far, and after a tap its points.

import QtQuick
import QtQuick.Layouts
import Qupil

ColumnLayout {
    id: block

    property string title
    property string status
    //! How much was practised in the weeks so far (1 little, 2 some, 3 much)
    property var weeks: []
    //! [{ text, done, priv }]
    property var points: []
    property string badge
    property bool open: false
    readonly property bool foldable: points.length > 0
    //! A pencil at the right: change the task in between, without a lesson
    property bool editable: false

    signal toggled()
    signal edit()

    Layout.fillWidth: true
    spacing: 7

    Item {
        Layout.fillWidth: true
        implicitHeight: head.implicitHeight
        Accessible.role: Accessible.Button
        Accessible.name: block.title
        Accessible.onPressAction: block.toggled()

        RowLayout {
            id: head
            width: parent.width
            spacing: 10
            Rectangle {
                Layout.alignment: Qt.AlignTop
                Layout.topMargin: 8
                implicitWidth: 7
                implicitHeight: 7
                radius: 4
                color: Theme.accentText
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 3
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 6
                    Text {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: block.title
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 16
                        font.weight: Font.DemiBold
                    }
                    Rectangle {
                        visible: block.badge !== ""
                        Layout.alignment: Qt.AlignTop
                        implicitWidth: badgeText.implicitWidth + 12
                        implicitHeight: 20
                        radius: 10
                        color: Theme.accentSoft
                        Text {
                            id: badgeText
                            anchors.centerIn: parent
                            text: block.badge
                            color: Theme.accentText
                            font.family: Theme.fontFamily
                            font.pixelSize: 11
                            font.weight: Font.Bold
                        }
                    }
                }
                Text {
                    visible: block.status !== ""
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: block.status
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                }
                Row {
                    visible: block.weeks.length > 0
                    spacing: 3
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        rightPadding: 4
                        text: qsTrId("idea.practised")
                        color: Theme.textMuted
                        font.family: Theme.fontFamily
                        font.pixelSize: 11
                    }
                    Repeater {
                        model: block.weeks.slice(-8)
                        Rectangle {
                            required property int modelData
                            anchors.verticalCenter: parent.verticalCenter
                            width: 16
                            height: 6
                            radius: 3
                            color: modelData >= 3 ? Theme.accent
                                 : modelData === 2 ? Qt.tint(Theme.surface, Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.5))
                                 : Theme.border
                        }
                    }
                }
            }
            IconButton {
                visible: block.editable
                Layout.alignment: Qt.AlignTop
                Layout.topMargin: -8
                Layout.bottomMargin: -8
                Layout.rightMargin: -6
                size: 36
                iconName: "edit"
                text: qsTrId("idea.task.edit.name").arg(block.title)
                onClicked: block.edit()
            }
            Icon {
                visible: block.foldable
                Layout.alignment: Qt.AlignTop
                Layout.topMargin: 3
                Layout.preferredWidth: 16
                Layout.preferredHeight: 16
                name: "chevronDown"
                color: Theme.icon
                rotation: block.open ? 180 : 0
            }
        }
        TapHandler { onTapped: block.toggled() }
    }
    ColumnLayout {
        visible: block.open && block.foldable
        Layout.fillWidth: true
        Layout.leftMargin: 17
        spacing: 6
        Repeater {
            model: block.points
            IdeaPoint {
                required property var modelData
                text: modelData.text
                done: modelData.done
                priv: modelData.priv
                mark: modelData.done ? "✓" : "–"
            }
        }
    }
}
