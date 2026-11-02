// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// After the lesson: "What did you cover?" All points of the plan are ticked; take away
// what was not covered. The ticked ones become the note of the lesson (saved at once, or
// first adjusted in the note editor), the rest stays planned for the next lesson.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: sheet

    property string membershipId
    property var plan: ({})
    property var checks: []
    readonly property var points: plan.points || []
    readonly property int checkedCount: checks.filter(Boolean).length

    //! "Edit before saving": the note editor takes over
    signal editRequested(string membershipId, string planId, string content, string rest, int restCount)

    function start(membership, id) {
        membershipId = membership
        plan = Data.plans(membership).find(p => p.id === id) || ({})
        checks = (plan.points || []).map(() => true)
        open()
    }
    //! [ticked points, the others, how many others], one point per line
    function parts() {
        const done = []
        const left = []
        points.forEach((p, i) => (checks[i] ? done : left).push(p))
        return [done.join("\n"), left.join("\n"), left.length]
    }
    function toggle(index) {
        const c = checks.slice()
        c[index] = !c[index]
        checks = c
    }

    title: qsTrId("plan.finish.title")
    acceptText: qsTrId("plan.finish.save")
    canAccept: checkedCount > 0
    onAccepted: {
        const [done, left] = parts()
        Data.planToNote(plan.id, done, left, (plan.shared || 1) > 1)
    }

    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: qsTrId("plan.title_for").arg((sheet.plan.when || ""))
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 14
    }

    Repeater {
        model: sheet.points
        delegate: AbstractButton {
            id: pointRow
            required property string modelData
            required property int index
            readonly property bool ticked: sheet.checks[index] === true
            Layout.fillWidth: true
            implicitHeight: Math.max(52, pointText.implicitHeight + 24)
            focusPolicy: Qt.StrongFocus
            checkable: true
            checked: ticked
            Accessible.role: Accessible.CheckBox
            Accessible.name: Data.plainText(modelData)
            onClicked: sheet.toggle(index)
            background: Rectangle {
                radius: 14
                color: pointRow.ticked ? Theme.accentSoft : Theme.surfaceAlt
                border.width: pointRow.visualFocus ? 2 : 0
                border.color: Theme.accentText
            }
            contentItem: RowLayout {
                spacing: 12
                Rectangle {
                    implicitWidth: 24
                    implicitHeight: 24
                    radius: 7
                    color: pointRow.ticked ? Theme.accent : "transparent"
                    border.width: pointRow.ticked ? 0 : 2
                    border.color: Theme.border
                    Icon {
                        visible: pointRow.ticked
                        anchors.centerIn: parent
                        width: 18
                        height: 18
                        name: "check"
                        color: Theme.textOnAccent
                        strokeWidth: 2.6
                    }
                }
                Text {
                    id: pointText
                    Layout.fillWidth: true
                    wrapMode: Text.Wrap
                    textFormat: Text.RichText
                    text: Data.noteHtml(pointRow.modelData, Theme.noteMark)
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                }
            }
            leftPadding: 14
            rightPadding: 14
        }
    }

    RowLayout {
        Layout.fillWidth: true
        spacing: 8
        Icon {
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: 18
            Layout.preferredHeight: 18
            name: "info"
            color: Theme.textMuted
        }
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: sheet.checkedCount > 0
                  ? qsTrId("plan.finish.info").arg(Data.formatDate(sheet.plan.date || ""))
                                              .arg(sheet.membershipId && sheet.plan.date
                                                   ? Data.formatDate(Data.nextLessonDate(sheet.membershipId, sheet.plan.date), true) : "")
                  : qsTrId("plan.finish.none")
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 13
        }
    }

    ActionButton {
        Layout.fillWidth: true
        Layout.preferredHeight: 48
        kind: "secondary"
        iconName: "edit"
        enabled: sheet.checkedCount > 0
        text: qsTrId("plan.finish.edit")
        onClicked: {
            const [done, left, count] = sheet.parts()
            const membership = sheet.membershipId
            const id = sheet.plan.id
            sheet.close()
            sheet.editRequested(membership, id, done, left, count)
        }
    }
}
