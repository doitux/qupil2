// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Birthdays through time: those of the last 8 weeks (to congratulate afterwards, e.g. after
// the summer holidays), today's, then the coming ones. Whoever comes to a lesson today says
// so ("heute 14:45"); ticked off ("Gratuliert") ones stay, paler, with a check.
// "Hatten Geburtstag: …" in the notices opens it and points out those students once
// with a stripe (Nav.birthdayFocus).

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page
    title: qsTrId("birthday.title")
    readonly property var birthdays: { if (Data.revision < 0) return undefined; return Data.birthdays(60, 56) }
    readonly property var past: birthdays.filter(b => b.days < 0)
    readonly property var today: birthdays.filter(b => b.days === 0)
    readonly property var soon: birthdays.filter(b => b.days > 0)
    //! Student → start of a lesson today that is not over yet
    readonly property var lessonToday: {
        if (Data.revision < 0) return undefined
        const now = Data.now()
        const time = Qt.formatTime(now, "hh:mm")
        const out = {}
        for (const l of Data.lessonsOn((now.getDay() + 6) % 7)) {
            if (l.irregular || l.end_time <= time)
                continue
            for (const st of l.students || [])
                if (out[st.id] === undefined)
                    out[st.id] = l.start_time
        }
        return out
    }
    //! Pointed out with a stripe while the page is open (from the notices)
    property var focusIds: []
    Component.onCompleted: {
        if (Nav.birthdayFocus.length > 0) {
            focusIds = Nav.birthdayFocus
            Nav.birthdayFocus = []
        }
    }

    EmptyHint {
        visible: page.birthdays.length === 0
        text: qsTrId("birthday.none_soon")
    }

    BirthdaySection {
        visible: page.past.length > 0
        title: qsTrId("birthday.section_past")
        model: page.past
    }
    BirthdaySection {
        visible: page.today.length > 0
        title: qsTrId("time.today")
        model: page.today
    }
    BirthdaySection {
        visible: page.soon.length > 0
        title: qsTrId("birthday.section_soon")
        model: page.soon
    }

    component BirthdaySection: ColumnLayout {
        id: section
        property string title
        property var model: []
        Layout.fillWidth: true
        spacing: 8
        SectionLabel { text: section.title }
        Rectangle {
            Layout.fillWidth: true
            implicitHeight: list.implicitHeight
            radius: Theme.radius
            color: Theme.surface
            clip: true
            ColumnLayout {
                id: list
                width: parent.width
                spacing: 0
                Repeater {
                    model: section.model
                    delegate: BirthdayRow {}
                }
            }
        }
    }

    component BirthdayRow: AbstractButton {
        id: row
        required property var modelData
        required property int index
        readonly property bool isToday: modelData.days === 0
        readonly property string lesson: page.lessonToday[modelData.id] || ""
        readonly property bool pointed: page.focusIds.indexOf(modelData.id) >= 0
        Layout.fillWidth: true
        implicitHeight: 60
        hoverEnabled: true
        focusPolicy: Qt.StrongFocus
        Accessible.role: Accessible.Button
        Accessible.name: nameText.text + ", " + whenText.text
        onClicked: Nav.showStudent(modelData.id, "")
        background: Rectangle {
            color: row.down ? Theme.surfaceAlt : row.hovered ? Theme.surfaceSunken : "transparent"
            border.width: row.visualFocus ? 2 : 0
            border.color: Theme.accentText
            // The students from the notice: a stripe on the left, calm, it does not fade
            Rectangle {
                visible: row.pointed
                x: 0
                y: 8
                width: 4
                height: parent.height - 16
                radius: 2
                color: Theme.accentText
            }
            Rectangle {
                visible: row.index > 0
                x: 64
                width: parent.width - x
                height: 1
                color: Theme.divider
            }
        }
        contentItem: RowLayout {
            spacing: 12
            opacity: row.modelData.greeted ? 0.55 : 1
            Item {
                implicitWidth: 38
                implicitHeight: 38
                Avatar {
                    size: 38
                    text: row.modelData.initials
                    seed: row.modelData.name
                }
                // A cake on the day itself, as everywhere else
                Rectangle {
                    visible: row.isToday
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.margins: -3
                    width: 18
                    height: 18
                    radius: 9
                    color: Theme.lime
                    border.width: 2
                    border.color: Theme.surface
                    Icon {
                        anchors.centerIn: parent
                        width: 11
                        height: 11
                        name: "cake"
                        color: Theme.textOnLime
                        strokeWidth: 2.4
                    }
                }
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 1
                Text {
                    id: nameText
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: row.modelData.name
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                    font.weight: Font.Bold
                }
                Text {
                    id: whenText
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    // "13 geworden · Do, 29.10.2026 · vor 5 Tagen", "wird 11 · Heute", "wird 14 · So, 13.12.2026 · in 40 Tagen"
                    text: (row.modelData.days < 0 ? qsTrId("birthday.turned_short") : qsTrId("birthday.turns_short")).arg(row.modelData.turns)
                          + " · " + (row.isToday ? qsTrId("time.today")
                                     : Data.formatDate(row.modelData.date, true) + " · "
                                       + (row.modelData.days < 0 ? qsTrId("time.days_ago", -row.modelData.days)
                                                                 : qsTrId("time.in_days", row.modelData.days)))
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                }
            }
            // Comes to a lesson today
            Rectangle {
                visible: row.lesson !== "" && row.modelData.days <= 0
                implicitWidth: lessonText.implicitWidth + 16
                implicitHeight: 24
                radius: 12
                color: row.isToday ? Theme.lime : Theme.accentSoft
                Text {
                    id: lessonText
                    anchors.centerIn: parent
                    text: qsTrId("birthday.lesson_today").arg(row.lesson)
                    color: row.isToday ? Theme.textOnLime : Theme.accentText
                    font.family: Theme.fontFamily
                    font.pixelSize: 12
                    font.weight: Font.Bold
                }
            }
            // Congratulated
            Icon {
                visible: row.modelData.greeted === true
                Layout.preferredWidth: 20
                Layout.preferredHeight: 20
                name: "check"
                color: Theme.accentText
                strokeWidth: 2.4
                Accessible.role: Accessible.Graphic
                Accessible.name: qsTrId("birthday.greeted")
            }
        }
        leftPadding: 14
        rightPadding: 14
    }
}
