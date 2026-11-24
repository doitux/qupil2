// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "Try the new idea": put an entry of the history right afterwards – a typing error, a
// task or a point that was wrong, the note. A changed text is changed at the task too;
// what is taken out is undone there (a new task or point is gone, what was ticked is open
// again, what was paused runs again): IdeaModel.saveEntry. Works on a copy.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: sheet

    property var demo
    property string studentId
    property string entryId
    property string date
    property string note
    property bool anyLine: false

    function label(kind) {
        return kind === "new" ? qsTrId("idea.log.new") : kind === "done" ? qsTrId("idea.log.done")
             : kind === "paused" ? qsTrId("idea.log.paused") : kind === "resumed" ? qsTrId("idea.log.resumed")
             : qsTrId("idea.log.on")
    }
    function start(id) {
        if (visible)
            return
        const s = demo.student(studentId)
        const e = s.log.find(x => x.id === id)
        if (!e)
            return
        entryId = id
        date = e.date
        note = e.note
        linesM.clear()
        changesM.clear()
        e.lines.forEach((l, li) => {
            linesM.append({ kind: l.kind, title: demo.lineTitle(s, l), gone: false,
                            // A task that simply went on is not "wrong"; its points can go
                            removable: l.kind !== "on" })
            l.changes.forEach((c, ci) => changesM.append({ li: li, ci: ci, kind: c.kind, text: demo.changeText(s, l, c), gone: false }))
        })
        anyLine = e.lines.length > 0
        open()
    }
    function draft() {
        const lines = []
        for (let i = 0; i < linesM.count; ++i)
            lines.push({ removed: linesM.get(i).gone, title: linesM.get(i).title, changes: [] })
        for (let k = 0; k < changesM.count; ++k) {
            const c = changesM.get(k)
            lines[c.li].changes[c.ci] = { removed: c.gone, text: c.text }
        }
        return { note: note, lines: lines }
    }

    title: qsTrId("idea.entry.edit")
    onAccepted: demo.saveEntry(studentId, entryId, draft())

    ListModel { id: linesM }
    ListModel { id: changesM }

    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: Data.formatDate(sheet.date, true) + (sheet.anyLine ? "\n" + qsTrId("idea.entry.edit.hint") : "")
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 14
    }

    Repeater {
        model: linesM
        ColumnLayout {
            id: lineBlock
            required property int index
            required property var model
            visible: !model.gone
            Layout.fillWidth: true
            spacing: 6
            RowLayout {
                Layout.fillWidth: true
                spacing: 4
                InputField {
                    label: sheet.label(lineBlock.model.kind)
                    text: lineBlock.model.title
                    onEdited: value => linesM.setProperty(lineBlock.index, "title", value)
                }
                IconButton {
                    visible: lineBlock.model.removable
                    Layout.alignment: Qt.AlignBottom
                    focusPolicy: Qt.TabFocus
                    iconName: "trash"
                    text: qsTrId("idea.entry.undo.name").arg(sheet.label(lineBlock.model.kind)).arg(lineBlock.model.title)
                    onClicked: linesM.setProperty(lineBlock.index, "gone", true)
                }
            }
            Repeater {
                model: changesM
                RowLayout {
                    id: changeRow
                    required property int index
                    required property var model
                    visible: model.li === lineBlock.index && !model.gone
                    Layout.fillWidth: true
                    Layout.leftMargin: 14
                    spacing: 4
                    Text {
                        Layout.preferredWidth: 14
                        text: changeRow.model.kind === "lock" ? "✓" : changeRow.model.kind
                        color: changeRow.model.kind === "–" ? Theme.textMuted
                             : changeRow.model.kind === "+" ? Theme.accentText : Theme.success
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.Bold
                    }
                    InputField {
                        wrap: true
                        placeholder: qsTrId("idea.task.point")
                        text: changeRow.model.text
                        onEdited: value => changesM.setProperty(changeRow.index, "text", value)
                    }
                    IconButton {
                        Layout.alignment: Qt.AlignVCenter
                        focusPolicy: Qt.TabFocus
                        iconName: "trash"
                        text: qsTrId("idea.remove.name").arg(changeRow.model.text)
                        onClicked: changesM.setProperty(changeRow.index, "gone", true)
                    }
                }
            }
        }
    }

    // (a field of its own for every time the window opens, see IdeaLessonSheet)
    Loader {
        id: noteLoader
        Layout.fillWidth: true
        active: sheet.visible
        sourceComponent: InputField {
            width: noteLoader.width
            label: sheet.anyLine ? qsTrId("idea.note.label") : qsTrId("common.note")
            multiline: true
            text: sheet.note
            onEdited: value => sheet.note = value
        }
    }
}
