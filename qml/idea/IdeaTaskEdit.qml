// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "Try the new idea": a running task in the window of a lesson (or of its preparation).
// Lesson: the big tick means "done, the whole task"; unfolded, every point has a small
// box of its own. Below the points the line to add one – with the lock it is only for
// the teacher (what I plan for the lesson). Preparation: "due" instead of the tick.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

ColumnLayout {
    id: task

    required property int index
    required property string tid
    required property string title
    required property bool ticked
    required property bool due
    required property bool open
    required property string status
    required property string fbText
    required property bool isNew
    required property string pending
    required property bool pendingPriv
    required property int oldDone
    //! The window (IdeaLessonSheet)
    property var win
    property bool divider: false
    property bool showDone: false

    Layout.fillWidth: true
    spacing: 10

    Rectangle { visible: task.divider; Layout.fillWidth: true; implicitHeight: 1; color: Theme.divider }

    RowLayout {
        Layout.fillWidth: true
        spacing: 10
        // Done, the whole task
        AbstractButton {
            visible: !task.win.prepare && !task.isNew
            Layout.alignment: Qt.AlignTop
            implicitWidth: 30
            implicitHeight: 30
            // (like the buttons of the other editors: a tap does not take the cursor out of a
            // field – the on-screen keyboard would go, the window move, and the tap be lost)
            focusPolicy: Qt.TabFocus
            Accessible.role: Accessible.CheckBox
            Accessible.checked: task.ticked
            Accessible.name: qsTrId("idea.tick.done").arg(task.title)
            onClicked: task.win.setTask(task.index, "ticked", !task.ticked)
            background: Rectangle {
                anchors.centerIn: parent
                width: 26
                height: 26
                radius: 13
                color: task.ticked ? Theme.success : "transparent"
                border.width: task.ticked ? 0 : 2
                border.color: parent.visualFocus ? Theme.accent : Theme.textMuted
                Icon { anchors.centerIn: parent; width: 16; height: 16; name: "check"; color: Theme.surface; visible: task.ticked; strokeWidth: 2.8 }
            }
        }
        Rectangle {
            visible: task.isNew && !task.win.prepare
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: 9
            Layout.leftMargin: 4
            implicitWidth: 7
            implicitHeight: 7
            radius: 4
            color: Theme.accentText
        }
        Item {
            Layout.fillWidth: true
            implicitHeight: titleColumn.implicitHeight
            Accessible.role: Accessible.Button
            Accessible.name: task.title
            Accessible.onPressAction: task.win.setTask(task.index, "open", !task.open)
            ColumnLayout {
                id: titleColumn
                width: parent.width
                spacing: 3
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 6
                    Text {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: task.title
                        color: task.ticked ? Theme.textMuted : Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 16
                        font.weight: Font.DemiBold
                        font.strikeout: task.ticked
                    }
                    Rectangle {
                        visible: task.isNew
                        Layout.alignment: Qt.AlignTop
                        implicitWidth: newText.implicitWidth + 12
                        implicitHeight: 20
                        radius: 10
                        color: Theme.accentSoft
                        Text {
                            id: newText
                            anchors.centerIn: parent
                            text: qsTrId("idea.new")
                            color: Theme.accentText
                            font.family: Theme.fontFamily
                            font.pixelSize: 11
                            font.weight: Font.Bold
                        }
                    }
                }
                Text {
                    visible: text !== ""
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: task.ticked ? qsTrId("idea.done") : task.status
                    color: task.ticked ? Theme.success : Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                }
            }
            TapHandler { onTapped: task.win.setTask(task.index, "open", !task.open) }
        }
        Chip {
            focusPolicy: Qt.TabFocus
            visible: task.win.prepare
            Layout.alignment: Qt.AlignTop
            kind: "filter"
            text: qsTrId("idea.due")
            selected: task.due
            Accessible.name: qsTrId("idea.due.name").arg(task.title)
            onClicked: task.win.setTask(task.index, "due", !task.due)
        }
        Icon {
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: 5
            Layout.preferredWidth: 16
            Layout.preferredHeight: 16
            name: "chevronDown"
            color: Theme.icon
            rotation: task.open ? 180 : 0
        }
    }

    ColumnLayout {
        visible: task.open
        Layout.fillWidth: true
        Layout.leftMargin: task.win.prepare ? 4 : 40
        spacing: 8
        Text {
            visible: task.fbText !== ""
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: task.fbText
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 13
            font.italic: true
        }
        Repeater {
            model: task.win.pointsModel
            IdeaPoint {
                required property int index
                required property var model
                visible: model.tid === task.tid && (!model.hidden || task.showDone)
                text: model.text
                done: model.done
                priv: model.priv
                isNew: model.isNew
                tickable: !task.win.prepare && !model.hidden
                mark: model.done ? "✓" : "–"
                onToggled: task.win.pointsModel.setProperty(index, "done", !model.done)
            }
        }
        IdeaLink {
            visible: task.oldDone > 0
            iconName: "check"
            text: qsTrId("idea.points.done").arg(task.oldDone)
            onClicked: task.showDone = !task.showDone
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 6
            InputField {
                placeholder: task.pendingPriv ? qsTrId("idea.point.add_private") : qsTrId("idea.point.add")
                text: task.pending
                onEdited: value => task.win.setTask(task.index, "pending", value)
                onAccepted: task.win.addPoint(task.index)
            }
            IconButton {
                focusPolicy: Qt.TabFocus
                Layout.alignment: Qt.AlignVCenter
                iconName: task.pendingPriv ? "lock" : "lockOpen"
                filled: task.pendingPriv
                text: qsTrId("idea.point.private")
                Accessible.role: Accessible.CheckBox
                Accessible.checked: task.pendingPriv
                onClicked: task.win.setTask(task.index, "pendingPriv", !task.pendingPriv)
            }
            IconButton {
                focusPolicy: Qt.TabFocus
                Layout.alignment: Qt.AlignVCenter
                iconName: "plus"
                enabled: task.pending.trim() !== ""
                opacity: enabled ? 1 : 0.4
                text: qsTrId("idea.point.add_button")
                onClicked: task.win.addPoint(task.index)
            }
        }
        IdeaLink {
            visible: !task.isNew
            iconName: "pause"
            text: qsTrId("idea.pause")
            Accessible.name: qsTrId("idea.pause.name").arg(task.title)
            onClicked: task.win.setTask(task.index, "phase", "pause")
        }
        IdeaLink {
            visible: task.isNew
            iconName: "minus"
            text: qsTrId("idea.remove")
            onClicked: task.win.removeTask(task.index)
        }
    }
}
