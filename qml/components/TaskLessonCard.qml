// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// One lesson of a student coupled with the practice book (variant A): its tasks with the
// student's answers, and a line "Private" for the private note. A tap on a task shows its
// hints; editing, planning and deleting are in the menu ("⋯", press and hold, right click).

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
    //! Tasks whose hints are shown (key → true), kept by the page; key: date/task id
    property var unfolded: ({})
    signal foldToggled(string key)
    //! The page shows its menu for this lesson at x, y of \a item (alignRight: the menu's
    //! right edge at x)
    signal menuRequested(Item item, real x, real y, bool alignRight)
    signal questionsRead()
    function openMenu(x, y) {
        if (x === undefined)
            menuRequested(more, more.width, more.height, true)
        else
            menuRequested(card, x, y, false)
    }

    topPadding: 16
    bottomPadding: 16
    leftPadding: 16
    rightPadding: 16
    focusPolicy: Qt.StrongFocus
    Accessible.role: Accessible.Button
    // The date and the tasks (a screen reader reads the card as one button)
    Accessible.name: qsTrId("task.lesson_of").arg(Data.formatDate(lesson.date || "", true)) + ": "
                     + (lesson.tasks || []).map(t => t.title).join(", ")
    Accessible.onPressAction: openMenu()
    Keys.onReturnPressed: openMenu()
    Keys.onEnterPressed: openMenu()
    Keys.onSpacePressed: openMenu()
    Keys.onMenuPressed: openMenu()

    TapHandler {
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onTapped: (eventPoint, button) => {
            if (button === Qt.RightButton)
                card.openMenu(eventPoint.position.x, eventPoint.position.y)
        }
        onLongPressed: card.openMenu(point.position.x, point.position.y)
    }

    background: Rectangle {
        radius: Theme.radius
        color: Theme.surface
        border.width: card.visualFocus ? 2 : 0
        border.color: Theme.accentText
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
                objectName: "taskMore:" + (card.lesson.date || "")
                Layout.topMargin: -8
                Layout.bottomMargin: -8
                Layout.rightMargin: -8
                size: 36
                iconName: "more"
                text: qsTrId("common.more_actions")
                fill: "transparent"
                tint: Theme.textMuted
                onClicked: card.openMenu()
            }
        }

        Repeater {
            model: card.lesson.tasks || []
            delegate: ColumnLayout {
                id: row
                required property var modelData
                readonly property var fb: modelData.feedback || {}
                readonly property bool closed: (modelData.closed || "") !== ""
                readonly property var hints: modelData.hints || []
                readonly property string key: (card.lesson.date || "") + "/" + (modelData.id || "")
                readonly property bool open: hints.length > 0 && card.unfolded[key] === true
                Layout.fillWidth: true
                spacing: 2
                // A tap on the task (its title, its hints) shows or hides the hints
                TapHandler {
                    enabled: row.hints.length > 0
                    onTapped: card.foldToggled(row.key)
                }
                RowLayout {
                    id: head
                    Layout.fillWidth: true
                    spacing: 8
                    Accessible.role: row.hints.length > 0 ? Accessible.Button : Accessible.StaticText
                    Accessible.name: row.modelData.title + (row.hints.length > 0 ? ", " + qsTrId("block.more_lines", row.hints.length) : "")
                    Accessible.onPressAction: if (row.hints.length > 0) card.foldToggled(row.key)
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
                    MoreBadge {
                        visible: row.hints.length > 0
                        count: row.hints.length
                        open: row.open
                    }
                }
                // The hints, shown by a tap
                Repeater {
                    model: row.open ? row.hints : []
                    delegate: Text {
                        required property string modelData
                        Layout.fillWidth: true
                        Layout.leftMargin: 14
                        wrapMode: Text.WordWrap
                        text: "–  " + modelData
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                        Accessible.role: Accessible.StaticText
                        Accessible.name: modelData
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
