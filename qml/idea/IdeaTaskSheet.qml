// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "Try the new idea": change a task in between, without a lesson or its preparation – its
// title, its points (text, done, only for me, away), pause it or take it up again, or
// delete it. Also makes a new task. What comes this way stands in the entry of the next
// lesson (IdeaModel.saveTask). Works on a copy; nothing changes before "Save".

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: sheet

    property var demo
    property string studentId
    property string taskId
    property string taskTitle
    property string phase: "run"
    property string until
    property string pending
    property bool pendingPriv: false
    property bool sure: false
    readonly property alias pointsModel: pointsM

    function start(id) {
        if (visible)
            return
        const s = demo.student(studentId)
        const t = demo.taskOf(s, id)
        taskId = t ? t.id : ""
        taskTitle = t ? t.title : ""
        phase = t ? t.state : "run"
        until = t ? t.until : ""
        pending = ""
        pendingPriv = false
        sure = false
        pointsM.clear()
        if (t)
            for (const p of t.points)
                pointsM.append({ pid: p.id, text: p.text, done: p.done, priv: p.priv, gone: false })
        open()
    }
    function addPoint() {
        const text = pending.trim()
        if (text === "")
            return
        pointsM.append({ pid: "", text: text, done: false, priv: pendingPriv, gone: false })
        pending = ""
    }
    function draft() {
        addPoint()
        const points = []
        for (let i = 0; i < pointsM.count; ++i) {
            const p = pointsM.get(i)
            if (!p.gone)
                points.push({ id: p.pid, text: p.text, done: p.done, priv: p.priv })
        }
        return { id: taskId, title: taskTitle, state: phase, until: until, points: points }
    }

    title: taskId === "" ? qsTrId("idea.task.add") : qsTrId("idea.task.edit")
    onAccepted: demo.saveTask(studentId, draft())

    ListModel { id: pointsM }

    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: qsTrId("idea.task.edit.hint")
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 14
    }
    InputField {
        label: qsTrId("idea.task.title")
        text: sheet.taskTitle
        onEdited: value => sheet.taskTitle = value
    }

    SectionLabel { text: qsTrId("idea.task.points") }
    Repeater {
        model: pointsM
        RowLayout {
            id: pointRow
            required property int index
            required property var model
            visible: !model.gone
            Layout.fillWidth: true
            spacing: 4
            AbstractButton {
                Layout.alignment: Qt.AlignVCenter
                implicitWidth: 34
                implicitHeight: 34
                focusPolicy: Qt.TabFocus
                Accessible.role: Accessible.CheckBox
                Accessible.checked: pointRow.model.done
                Accessible.name: qsTrId("idea.point.done.name").arg(pointRow.model.text)
                onClicked: pointsM.setProperty(pointRow.index, "done", !pointRow.model.done)
                background: Rectangle {
                    anchors.centerIn: parent
                    width: 22
                    height: 22
                    radius: 6
                    color: pointRow.model.done ? Theme.success : "transparent"
                    border.width: pointRow.model.done ? 0 : 1.6
                    border.color: Theme.icon
                    Icon { anchors.centerIn: parent; width: 14; height: 14; name: "check"; color: Theme.surface; visible: pointRow.model.done; strokeWidth: 3 }
                }
            }
            InputField {
                wrap: true
                placeholder: qsTrId("idea.task.point")
                text: pointRow.model.text
                onEdited: value => pointsM.setProperty(pointRow.index, "text", value)
            }
            IconButton {
                Layout.alignment: Qt.AlignVCenter
                focusPolicy: Qt.TabFocus
                iconName: pointRow.model.priv ? "lock" : "lockOpen"
                filled: pointRow.model.priv
                text: qsTrId("idea.point.private.name").arg(pointRow.model.text)
                Accessible.role: Accessible.CheckBox
                Accessible.checked: pointRow.model.priv
                onClicked: pointsM.setProperty(pointRow.index, "priv", !pointRow.model.priv)
            }
            IconButton {
                Layout.alignment: Qt.AlignVCenter
                focusPolicy: Qt.TabFocus
                iconName: "trash"
                text: qsTrId("idea.remove.name").arg(pointRow.model.text)
                onClicked: pointsM.setProperty(pointRow.index, "gone", true)
            }
        }
    }
    RowLayout {
        Layout.fillWidth: true
        spacing: 4
        InputField {
            wrap: true
            placeholder: sheet.pendingPriv ? qsTrId("idea.point.add_private") : qsTrId("idea.point.add")
            text: sheet.pending
            onEdited: value => sheet.pending = value
            onAccepted: sheet.addPoint()
        }
        IconButton {
            Layout.alignment: Qt.AlignVCenter
            focusPolicy: Qt.TabFocus
            iconName: sheet.pendingPriv ? "lock" : "lockOpen"
            filled: sheet.pendingPriv
            text: qsTrId("idea.point.private")
            Accessible.role: Accessible.CheckBox
            Accessible.checked: sheet.pendingPriv
            onClicked: sheet.pendingPriv = !sheet.pendingPriv
        }
        IconButton {
            Layout.alignment: Qt.AlignVCenter
            focusPolicy: Qt.TabFocus
            iconName: "plus"
            enabled: sheet.pending.trim() !== ""
            opacity: enabled ? 1 : 0.4
            text: qsTrId("idea.point.add_button")
            onClicked: sheet.addPoint()
        }
    }

    // Pause, take up again, delete: only for a task that is there already
    ColumnLayout {
        visible: sheet.taskId !== ""
        Layout.fillWidth: true
        Layout.topMargin: 6
        spacing: 4
        IdeaLink {
            visible: sheet.phase === "run"
            iconName: "pause"
            text: qsTrId("idea.pause")
            onClicked: sheet.phase = "pause"
        }
        DateField {
            visible: sheet.phase === "pause"
            label: qsTrId("idea.pause.until")
            allowEmpty: true
            date: sheet.until
            onPicked: value => sheet.until = value
        }
        IdeaLink {
            visible: sheet.phase === "pause"
            iconName: "play"
            text: qsTrId("idea.resume")
            onClicked: sheet.phase = "run"
        }
        IdeaLink {
            iconName: "trash"
            text: sheet.sure ? qsTrId("idea.task.delete.sure") : qsTrId("idea.task.delete")
            onClicked: {
                if (!sheet.sure) {
                    sheet.sure = true
                    return
                }
                sheet.demo.deleteTask(sheet.studentId, sheet.taskId)
                sheet.dismiss()
            }
        }
    }
}
