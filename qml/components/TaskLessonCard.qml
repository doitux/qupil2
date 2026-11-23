// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// One lesson of a student coupled with the practice book (variant A): its tasks with the
// student's answers, and a line "Private" for the private note. A tap opens the lesson.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Control {
    id: card

    //! { date, tasks: [...], note: {…} or null }
    property var lesson: ({})
    property string relative
    //! What the student asked and nobody has marked as read yet: [{ text, date }] (the
    //! student page gives them to the newest lesson's card only)
    property var questions: []
    signal opened()
    signal questionsRead()

    topPadding: 16
    bottomPadding: 16
    leftPadding: 16
    rightPadding: 16
    focusPolicy: Qt.StrongFocus
    Accessible.role: Accessible.Button
    // The date and the tasks (a screen reader reads the card as one button)
    Accessible.name: qsTrId("task.lesson_of").arg(Data.formatDate(lesson.date || "", true)) + ": "
                     + (lesson.tasks || []).map(t => t.title).join(", ")
    Accessible.onPressAction: opened()
    Keys.onReturnPressed: opened()
    Keys.onSpacePressed: opened()

    TapHandler { id: tap; onTapped: card.opened() }

    background: Rectangle {
        radius: Theme.radius
        color: Theme.surface
        border.width: card.visualFocus ? 2 : 0
        border.color: Theme.accentText
        Rectangle {
            anchors.fill: parent
            radius: parent.radius
            color: Theme.text
            opacity: tap.pressed ? 0.06 : 0
        }
    }

    AppMenu {
        id: menu
        AppMenuItem { iconName: "newfrom"; text: qsTrId("task.new_from") }
        AppMenuItem { iconName: "plan"; text: qsTrId("plan.from_note.title") }
        AppMenuItem { iconName: "edit"; text: qsTrId("common.edit"); onTriggered: card.opened() }
        AppMenuSeparator {}
        AppMenuItem { iconName: "trash"; destructive: true; text: qsTrId("common.delete") }
    }

    contentItem: ColumnLayout {
        spacing: 8
        RowLayout {
            Layout.fillWidth: true
            Text {
                text: Data.formatDate(card.lesson.date || "", true)
                color: Theme.accentText
                font.family: Theme.fontFamily
                font.pixelSize: 13
                font.weight: Font.ExtraBold
            }
            Icon {
                Layout.preferredWidth: 16
                Layout.preferredHeight: 16
                name: "book"
                color: Theme.textMuted
            }
            Item { Layout.fillWidth: true }
            Text {
                text: card.relative
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 12
            }
            IconButton {
                id: more
                objectName: "taskMore"
                Layout.topMargin: -8
                Layout.bottomMargin: -8
                Layout.rightMargin: -8
                size: 36
                iconName: "more"
                text: qsTrId("common.more_actions")
                fill: "transparent"
                tint: Theme.textMuted
                onClicked: menu.popup(more, more.width - menu.width, more.height)
            }
        }

        Repeater {
            model: card.lesson.tasks || []
            delegate: ColumnLayout {
                id: row
                required property var modelData
                readonly property var fb: modelData.feedback || {}
                readonly property bool closed: (modelData.closed || "") !== ""
                Layout.fillWidth: true
                spacing: 2
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    // To do (get the music): a box; ticked when done
                    Rectangle {
                        visible: row.modelData.kind === 1
                        implicitWidth: 16
                        implicitHeight: 16
                        radius: 4
                        color: row.closed || row.fb.done ? Theme.accent : "transparent"
                        border.width: row.closed || row.fb.done ? 0 : 2
                        border.color: Theme.textMuted
                    }
                    Rectangle {
                        visible: row.modelData.kind !== 1
                        implicitWidth: 6
                        implicitHeight: 6
                        radius: 3
                        color: row.closed ? Theme.textMuted : Theme.accentText
                    }
                    Text {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: row.modelData.title
                        color: row.closed ? Theme.textMuted : Theme.text
                        font.strikeout: row.closed
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.DemiBold
                    }
                    Text {
                        visible: (row.modelData.hints || []).length > 0
                        text: "+" + (row.modelData.hints || []).length
                        color: Theme.textMuted
                        font.family: Theme.fontFamily
                        font.pixelSize: 12
                        font.weight: Font.Bold
                    }
                }
                // Carried over, and what the student answered
                Text {
                    readonly property var parts: {
                        const p = []
                        if (row.modelData.since && row.modelData.since !== row.modelData.date && !row.closed)
                            p.push(qsTrId("task.open_since").arg(Data.formatDate(row.modelData.since)))
                        if (row.modelData.kind !== 1) {
                            if (row.fb.practiced > 0) p.push(qsTrId("task.feedback.practiced").arg(Labels.practiced(row.fb.practiced)))
                            if (row.fb.works > 0) p.push(qsTrId("task.feedback.works").arg(Labels.works(row.fb.works)))
                        } else if (row.fb.done && !row.closed) {
                            p.push(qsTrId("task.feedback.done"))
                        }
                        return p
                    }
                    visible: parts.length > 0
                    Layout.fillWidth: true
                    Layout.leftMargin: 14
                    wrapMode: Text.WordWrap
                    text: parts.join(" · ")
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                }
            }
        }

        // The student's questions, until they are marked as read (they stay in the sheet
        // "Qupil Practice" of the student)
        Repeater {
            model: card.questions
            delegate: Rectangle {
                id: questionBox
                required property var modelData
                readonly property string label: qsTrId("task.question_from").arg(Data.formatDate(modelData.date || "", true))
                Layout.fillWidth: true
                Layout.topMargin: 2
                implicitHeight: questionColumn.implicitHeight + 16
                radius: Theme.radiusSmall + 2
                color: Theme.accentSoft
                Accessible.role: Accessible.StaticText
                Accessible.name: label + ": " + (modelData.text || "")
                Column {
                    id: questionColumn
                    x: 12
                    y: 8
                    width: parent.width - 24
                    spacing: 2
                    Text {
                        width: parent.width
                        text: questionBox.label
                        color: Theme.accentText
                        font.family: Theme.fontFamily
                        font.pixelSize: 12
                        font.weight: Font.Bold
                    }
                    Text {
                        width: parent.width
                        wrapMode: Text.WordWrap
                        text: questionBox.modelData.text || ""
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                    }
                }
            }
        }
        ActionButton {
            objectName: "questionsRead"
            visible: card.questions.length > 0
            Layout.alignment: Qt.AlignRight
            implicitHeight: 40
            kind: "quiet"
            iconName: "check"
            text: qsTrId("task.questions_read")
            onClicked: card.questionsRead()
        }

        // The private note: only its first line here
        RowLayout {
            visible: !!card.lesson.note
            Layout.fillWidth: true
            spacing: 6
            Icon {
                Layout.preferredWidth: 15
                Layout.preferredHeight: 15
                name: "lock"
                color: Theme.textMuted
            }
            Text {
                Layout.fillWidth: true
                elide: Text.ElideRight
                text: qsTrId("task.private") + ": " + (card.lesson.note ? Data.plainText(card.lesson.note.content || "", 80) : "")
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 13
            }
        }
    }
}
