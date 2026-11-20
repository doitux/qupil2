// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// One task from the lesson: title, hints to fold open, and the student's feedback.
// Tasks to practise get "Practised" and "Works"; to-dos (get the music) just a tick.

import QtQuick
import QupilStudent

Card {
    id: card
    property var task: ({})
    property bool allowFeedback: true
    property bool open: false          // kept by the page: the list is rebuilt after every tap
    signal toggleHints()
    readonly property var hints: task.hints ?? []
    readonly property bool todo: task.kind === "todo"

    objectName: "task-" + (task.id ?? "")
    spacing: 4

    // ---- title row (to-dos with their tick in front)
    Item {
        width: parent.width
        height: Math.max(titleText.implicitHeight, card.todo ? 28 : 0)

        Rectangle {
            id: tick
            objectName: "tick-" + (card.task.id ?? "")
            visible: card.todo
            width: 24; height: 24; radius: 7
            anchors.verticalCenter: parent.verticalCenter
            color: card.task.done ? Theme.accent : "transparent"
            border.width: card.task.done ? 0 : 2
            border.color: Theme.icon
            Icon { anchors.centerIn: parent; width: 16; height: 16; name: "check"; color: "white"; strokeWidth: 2.8; visible: card.task.done }
            Accessible.role: Accessible.CheckBox
            Accessible.name: card.task.title ?? ""
            Accessible.checked: card.task.done ?? false
            MouseArea {
                anchors.fill: parent
                anchors.margins: -10
                enabled: card.allowFeedback || card.todo
                onClicked: StudentApp.setDone(card.task.id, !card.task.done)
            }
        }
        T {
            id: titleText
            x: card.todo ? tick.width + 10 : 0
            width: parent.width - x - (badge.visible ? badge.width + 8 : 0)
            anchors.verticalCenter: parent.verticalCenter
            text: card.task.title ?? ""
            font.pixelSize: 15
            font.weight: Font.Bold
            color: card.todo && card.task.done ? Theme.textMuted : Theme.text
            font.strikeout: card.todo && card.task.done
        }
        Rectangle {
            id: badge
            objectName: "hints-" + (card.task.id ?? "")
            visible: card.hints.length > 0
            anchors.right: parent.right
            anchors.top: parent.top
            height: 26; radius: 13
            width: badgeRow.implicitWidth + 16
            color: Theme.surfaceAlt
            Row {
                id: badgeRow
                anchors.centerIn: parent
                spacing: 4
                Icon { width: 14; height: 14; name: "lines"; color: Theme.textSecondary; strokeWidth: 2.4; anchors.verticalCenter: parent.verticalCenter }
                T { text: card.hints.length; font.pixelSize: 12; font.weight: Font.Bold; color: Theme.textSecondary; anchors.verticalCenter: parent.verticalCenter }
                Icon { width: 14; height: 14; name: card.open ? "chevronUp" : "chevronDown"; color: Theme.textSecondary; strokeWidth: 2.4; anchors.verticalCenter: parent.verticalCenter }
            }
            Accessible.role: Accessible.Button
            Accessible.name: qsTrId("s.task.hints").arg(card.hints.length)
            MouseArea { anchors.fill: parent; anchors.margins: -8; onClicked: card.toggleHints() }
        }
    }

    // ---- carried over from an earlier lesson
    Rectangle {
        visible: (card.task.since ?? "") !== "" && card.task.since !== card.task.date
        height: 22; radius: 8
        width: sinceText.implicitWidth + 14
        color: Theme.warningSoft
        T {
            id: sinceText
            anchors.centerIn: parent
            font.pixelSize: 11; font.weight: Font.ExtraBold
            color: Theme.warningText
            wrapMode: Text.NoWrap
            text: qsTrId("s.task.open_since").arg(Fmt.date(card.task.since, "s.fmt.short"))
        }
    }

    // ---- hints
    Rectangle {
        objectName: "hintList-" + (card.task.id ?? "")
        visible: card.open && card.hints.length > 0
        width: parent.width
        height: visible ? hintColumn.implicitHeight + 16 : 0
        radius: Theme.radiusSmall
        color: Theme.bg
        Column {
            id: hintColumn
            x: 10; y: 8
            width: parent.width - 20
            spacing: 4
            Repeater {
                model: card.hints
                Row {
                    required property string modelData
                    width: hintColumn.width
                    spacing: 8
                    Rectangle { width: 5; height: 5; radius: 3; color: Theme.accentText; y: 8 }
                    T { width: parent.width - 13; text: modelData; font.pixelSize: 14; color: Theme.textSecondary }
                }
            }
        }
    }

    // ---- feedback
    Item { width: 1; height: 4; visible: !card.todo && card.allowFeedback }
    ChoiceRow {
        objectName: "practiced-" + (card.task.id ?? "")
        visible: !card.todo && card.allowFeedback
        label: qsTrId("s.fb.practiced")
        choices: [qsTrId("s.fb.practiced.3"), qsTrId("s.fb.practiced.2"), qsTrId("s.fb.practiced.1")]
        value: card.task.practiced ?? 0
        onPicked: v => StudentApp.setPracticed(card.task.id, v)
    }
    ChoiceRow {
        objectName: "works-" + (card.task.id ?? "")
        visible: !card.todo && card.allowFeedback
        label: qsTrId("s.fb.works")
        choices: [qsTrId("s.fb.works.3"), qsTrId("s.fb.works.2"), qsTrId("s.fb.works.1")]
        value: card.task.works ?? 0
        selectedColor: Theme.lime
        selectedText: Theme.textOnLime
        onPicked: v => StudentApp.setWorks(card.task.id, v)
    }
}
