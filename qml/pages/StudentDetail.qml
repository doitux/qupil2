// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// One student: lesson selector, then notes, pieces, activities, personal data
// and borrowed sheet music. Notes and pieces belong to a lesson membership,
// so the selector decides which ones are shown (also of earlier lessons).

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Item {
    id: detail

    property string studentId
    property string membershipId
    property bool showBack: true
    property string backText: qsTrId("common.back")
    property int tab: 0
    property int pieceFilter: -1

    signal back()

    readonly property var student: { if (Data.revision < 0) return undefined; return Data.student(studentId) }
    readonly property var memberships: student.memberships || []
    readonly property string currentMembership: {
        if (membershipId !== "" && memberships.some(m => m.id === membershipId))
            return membershipId
        return memberships.length > 0 ? memberships[0].id : ""
    }
    readonly property var membership: memberships.find(m => m.id === currentMembership) || null
    readonly property int inactiveCount: memberships.filter(m => !m.active).length
    readonly property var notes: { if (Data.revision < 0) return undefined; return currentMembership ? Data.notes(currentMembership) : [] }
    readonly property var pieces: { if (Data.revision < 0) return undefined; return currentMembership ? Data.pieces(currentMembership, pieceFilter) : [] }
    readonly property var pieceCounts: { if (Data.revision < 0) return undefined; return currentMembership ? Data.pieceCounts(currentMembership) : [0, 0, 0, 0, 0, 0] }
    readonly property var activities: { if (Data.revision < 0) return undefined; return studentId ? Data.activities(studentId) : [] }
    readonly property var loans: { if (Data.revision < 0) return undefined; return studentId ? Data.loans(studentId) : [] }
    readonly property int groupSize: { if (Data.revision < 0) return undefined; return currentMembership ? Data.groupSize(currentMembership) : 1 }

    function relative(iso) {
        const d = new Date(iso + "T00:00:00")
        const today = new Date()
        today.setHours(0, 0, 0, 0)
        const days = Math.round((today - d) / 86400000)
        if (days === 0) return qsTrId("time.today")
        if (days === 1) return qsTrId("time.yesterday")
        if (days > 1 && days < 14) return qsTrId("time.days_ago", days)
        if (days >= 14 && days < 63) return qsTrId("time.weeks_ago", Math.round(days / 7))
        return ""
    }
    function intervalLabel(index) {
        const months = [0, 1, 2, 3, 4, 6, 9, 12, 18, 24]
        return index === 0 ? qsTrId("common.never") : qsTrId("common.duration_months", months[index])
    }

    onStudentIdChanged: {
        tab = 0
        pieceFilter = -1
    }

    Flickable {
        id: flick
        anchors.fill: parent
        clip: true
        contentHeight: column.implicitHeight + 120
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}

        ColumnLayout {
            id: column
            x: 20
            y: detail.showBack ? 12 : 28
            width: flick.width - 40
            spacing: 14

            PageHeader {
                visible: detail.showBack
                Layout.leftMargin: -8
                backText: detail.backText
                onBack: detail.back()
                IconButton {
                    id: moreButton
                    iconName: "more"
                    text: qsTrId("common.more_actions")
                    onClicked: studentMenu.popup(moreButton, moreButton.width - studentMenu.width, moreButton.height)
                }
            }

            // ------------------------------------------------------------ name
            RowLayout {
                Layout.fillWidth: true
                spacing: 14
                Avatar {
                    text: detail.student.initials || "?"
                    seed: detail.student.name || ""
                    size: 58
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 3
                    Text {
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: detail.student.name || ""
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 26
                        font.weight: Font.ExtraBold
                    }
                    Text {
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: {
                            const s = detail.student
                            const parts = []
                            if (s.age >= 0) parts.push(qsTrId("common.age_years", s.age))
                            const instrument = [s.instrument, s.instrument_size].filter(Boolean).join(" ")
                            if (instrument) parts.push(instrument)
                            if (s.since > 0) parts.push(qsTrId("common.since").arg(s.since))
                            return parts.join(" · ")
                        }
                        color: Theme.textMuted
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                    }
                }
                IconButton {
                    id: moreButtonWide
                    visible: !detail.showBack
                    iconName: "more"
                    text: qsTrId("common.more_actions")
                    onClicked: studentMenu.popup(moreButtonWide, moreButtonWide.width - studentMenu.width, moreButtonWide.height)
                }
            }

            // ------------------------------------------------------------ lesson selector
            AbstractButton {
                id: lessonSelector
                visible: detail.memberships.length > 0
                Layout.fillWidth: true
                implicitHeight: 54
                focusPolicy: Qt.StrongFocus
                Accessible.role: Accessible.ComboBox
                Accessible.name: qsTrId("lesson.choose")
                onClicked: lessonMenu.popup(lessonSelector, 0, lessonSelector.height + 4)
                background: Rectangle {
                    radius: 14
                    color: Theme.surface
                    border.width: lessonSelector.visualFocus ? 2 : 1
                    border.color: lessonSelector.visualFocus ? Theme.accent : Theme.border
                }
                contentItem: RowLayout {
                    spacing: 10
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 0
                        FieldLabel { label: qsTrId("common.lesson") }
                        Text {
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            text: detail.membership ? detail.membership.label : ""
                            color: Theme.text
                            font.family: Theme.fontFamily
                            font.pixelSize: 15
                            font.weight: Font.Bold
                        }
                    }
                    Rectangle {
                        visible: detail.inactiveCount > 0 && (detail.membership && detail.membership.active)
                        implicitWidth: inactiveText.implicitWidth + 16
                        implicitHeight: inactiveText.implicitHeight + 8
                        radius: height / 2
                        color: Theme.surfaceAlt
                        Text {
                            id: inactiveText
                            anchors.centerIn: parent
                            text: qsTrId("student.inactive_lessons", detail.inactiveCount)
                            color: Theme.textMuted
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                            font.weight: Font.DemiBold
                        }
                    }
                    Icon {
                        Layout.preferredWidth: 20
                        Layout.preferredHeight: 20
                        name: "chevronDown"
                        color: Theme.textMuted
                    }
                }
                leftPadding: 14
                rightPadding: 14

                Menu {
                    id: lessonMenu
                    width: lessonSelector.width
                    Repeater {
                        model: detail.memberships
                        delegate: MenuItem {
                            required property var modelData
                            text: modelData.label
                            checkable: true
                            checked: modelData.id === detail.currentMembership
                            onTriggered: detail.membershipId = modelData.id
                        }
                    }
                }
            }

            // ------------------------------------------------------------ sections
            Flickable {
                Layout.fillWidth: true
                Layout.leftMargin: -20
                Layout.rightMargin: -20
                Layout.preferredHeight: 38
                contentWidth: tabRow.implicitWidth + 40
                flickableDirection: Flickable.HorizontalFlick
                boundsBehavior: Flickable.StopAtBounds
                clip: true
                Row {
                    id: tabRow
                    x: 20
                    spacing: 8
                    Repeater {
                        model: 5
                        delegate: Chip {
                            required property int index
                            selected: detail.tab === index
                            text: {
                                switch (index) {
                                case 0: return qsTrId("common.notes")
                                case 1: return qsTrId("common.pieces")
                                case 2: return qsTrId("common.activities")
                                case 3: return qsTrId("student.tab.data_short")
                                default: return qsTrId("student.tab.loans_short")
                                }
                            }
                            onClicked: detail.tab = index
                        }
                    }
                }
            }

            Loader {
                Layout.fillWidth: true
                sourceComponent: [notesTab, piecesTab, activitiesTab, dataTab, loansTab][detail.tab]
            }
        }
    }

    Fab {
        visible: (detail.tab === 0 || detail.tab === 1) && detail.currentMembership !== ""
                 && detail.membership && detail.membership.active
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 20
        text: detail.tab === 0 ? qsTrId("common.note") : qsTrId("common.piece")
        onClicked: detail.tab === 0 ? Nav.editNote(detail.currentMembership, "") : Nav.editPiece(detail.currentMembership, "")
    }

    Menu {
        id: studentMenu
        MenuItem {
            text: qsTrId("common.edit")
            onTriggered: detail.tab = 3
        }
        MenuItem {
            text: qsTrId("student.add_reminder")
            onTriggered: Nav.editReminder("", detail.studentId)
        }
        MenuSeparator {}
        MenuItem {
            text: qsTrId("student.delete")
            onTriggered: deleteStudent.open()
        }
    }

    AppDialog {
        id: deleteStudent
        iconName: "trash"
        destructive: true
        title: qsTrId("student.delete")
        text: qsTrId("student.delete.confirm").arg(detail.student.name || "")
        rejectText: qsTrId("common.cancel")
        acceptText: qsTrId("common.delete")
        onAccepted: {
            Data.deleteStudent(detail.studentId)
            detail.back()
        }
    }

    // ================================================================ notes
    Component {
        id: notesTab
        ColumnLayout {
            spacing: 10
            EmptyHint {
                visible: detail.notes.length === 0
                text: detail.memberships.length === 0 ? qsTrId("student.no_lesson") : qsTrId("notes.empty")
            }
            Repeater {
                model: detail.notes
                delegate: AbstractButton {
                    id: noteCard
                    required property var modelData
                    Layout.fillWidth: true
                    implicitHeight: noteColumn.implicitHeight + 28
                    focusPolicy: Qt.StrongFocus
                    hoverEnabled: true
                    Accessible.role: Accessible.Button
                    Accessible.name: Data.formatDate(modelData.date) + " " + Data.plainText(modelData.content, 120)
                    onClicked: Nav.editNote(detail.currentMembership, modelData.id)
                    onPressAndHold: noteMenu.popup()
                    TapHandler {
                        acceptedButtons: Qt.RightButton
                        onTapped: noteMenu.popup()
                    }
                    background: Rectangle {
                        radius: Theme.radius
                        color: Theme.surface
                        border.width: noteCard.visualFocus ? 2 : 0
                        border.color: Theme.accentText
                    }
                    contentItem: ColumnLayout {
                        id: noteColumn
                        spacing: 6
                        RowLayout {
                            Layout.fillWidth: true
                            Text {
                                text: Data.formatDate(noteCard.modelData.date, true)
                                color: Theme.accentText
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                                font.weight: Font.ExtraBold
                            }
                            Icon {
                                visible: noteCard.modelData.shared > 1
                                Layout.preferredWidth: 16
                                Layout.preferredHeight: 16
                                name: "students"
                                color: Theme.textMuted
                            }
                            Item { Layout.fillWidth: true }
                            Text {
                                text: detail.relative(noteCard.modelData.date)
                                color: Theme.textMuted
                                font.family: Theme.fontFamily
                                font.pixelSize: 12
                            }
                        }
                        Text {
                            Layout.fillWidth: true
                            textFormat: Text.RichText
                            wrapMode: Text.Wrap
                            text: Data.noteHtml(noteCard.modelData.content, Theme.dark)
                            color: Theme.text
                            font.family: Theme.fontFamily
                            font.pixelSize: 15
                            lineHeight: 1.15
                        }
                    }
                    leftPadding: 16
                    rightPadding: 16

                    Menu {
                        id: noteMenu
                        MenuItem {
                            text: qsTrId("common.edit")
                            onTriggered: Nav.editNote(detail.currentMembership, noteCard.modelData.id)
                        }
                        MenuItem {
                            text: qsTrId("note.delete")
                            onTriggered: Data.deleteNote(noteCard.modelData.id, false)
                        }
                        MenuItem {
                            visible: noteCard.modelData.shared > 1
                            height: visible ? implicitHeight : 0
                            text: qsTrId("piece.delete_for_all")
                            onTriggered: Data.deleteNote(noteCard.modelData.id, true)
                        }
                    }
                }
            }
        }
    }

    // ================================================================ pieces
    Component {
        id: piecesTab
        ColumnLayout {
            spacing: 10
            Flickable {
                visible: detail.pieceCounts[5] > 0
                Layout.fillWidth: true
                Layout.preferredHeight: 34
                contentWidth: filterRow.implicitWidth
                flickableDirection: Flickable.HorizontalFlick
                boundsBehavior: Flickable.StopAtBounds
                clip: true
                Row {
                    id: filterRow
                    spacing: 6
                    // -1 = all, then the states in the order of the lesson flow
                    Repeater {
                        model: [-1, 1, 3, 0, 2, 4]
                        delegate: Chip {
                            required property int modelData
                            readonly property int count: modelData < 0 ? detail.pieceCounts[5] : detail.pieceCounts[modelData]
                            visible: modelData < 0 || count > 0
                            kind: "filter"
                            selected: detail.pieceFilter === modelData
                            text: (modelData < 0 ? qsTrId("common.all") : badgeLabel.label(modelData)) + " · " + count
                            onClicked: detail.pieceFilter = modelData
                        }
                    }
                }
            }
            StatusBadge { id: badgeLabel; visible: false }
            EmptyHint {
                visible: detail.pieces.length === 0
                text: detail.memberships.length === 0 ? qsTrId("student.no_lesson") : qsTrId("pieces.empty")
            }
            Repeater {
                model: detail.pieces
                delegate: AbstractButton {
                    id: pieceCard
                    required property var modelData
                    Layout.fillWidth: true
                    implicitHeight: pieceColumn.implicitHeight + 28
                    focusPolicy: Qt.StrongFocus
                    hoverEnabled: true
                    Accessible.role: Accessible.Button
                    Accessible.name: modelData.composer + " " + modelData.title
                    onClicked: Nav.editPiece(detail.currentMembership, modelData.id)
                    onPressAndHold: pieceMenu.popup()
                    TapHandler {
                        acceptedButtons: Qt.RightButton
                        onTapped: pieceMenu.popup()
                    }
                    background: Rectangle {
                        radius: Theme.radius
                        color: Theme.surface
                        border.width: pieceCard.visualFocus ? 2 : 0
                        border.color: Theme.accentText
                    }
                    contentItem: ColumnLayout {
                        id: pieceColumn
                        spacing: 4
                        RowLayout {
                            Layout.fillWidth: true
                            spacing: 8
                            Text {
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                                text: pieceCard.modelData.composer
                                color: Theme.textMuted
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                                font.weight: Font.DemiBold
                            }
                            StatusBadge { status: pieceCard.modelData.state }
                        }
                        Text {
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            text: pieceCard.modelData.title
                            color: Theme.text
                            font.family: Theme.fontFamily
                            font.pixelSize: 16
                            font.weight: Font.Bold
                        }
                        Text {
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            text: {
                                const p = pieceCard.modelData
                                const parts = []
                                if (p.genre) parts.push(p.genre)
                                if (p.duration > 0) parts.push(p.duration + " " + qsTrId("common.unit.min"))
                                if (p.state === 4 && p.end_date)
                                    parts.push(qsTrId("common.from_to").arg(Data.formatDate(p.start_date)).arg(Data.formatDate(p.end_date)))
                                else if (p.start_date)
                                    parts.push(qsTrId("common.since").arg(Data.formatDate(p.start_date)))
                                if (p.shared > 1) parts.push(qsTrId("summary.students", p.shared))
                                return parts.join(" · ")
                            }
                            color: Theme.textMuted
                            font.family: Theme.fontFamily
                            font.pixelSize: 13
                        }
                    }
                    leftPadding: 16
                    rightPadding: 16

                    Menu {
                        id: pieceMenu
                        Menu {
                            title: qsTrId("piece.change_status")
                            Repeater {
                                model: [0, 1, 2, 3, 4]
                                delegate: MenuItem {
                                    required property int modelData
                                    text: badgeLabel.label(modelData)
                                    checkable: true
                                    checked: pieceCard.modelData.state === modelData
                                    onTriggered: Data.updatePiece(pieceCard.modelData.id, { state: modelData }, true)
                                }
                            }
                        }
                        MenuItem {
                            text: qsTrId("common.edit")
                            onTriggered: Nav.editPiece(detail.currentMembership, pieceCard.modelData.id)
                        }
                        MenuItem {
                            text: qsTrId("piece.delete")
                            onTriggered: Data.deletePiece(pieceCard.modelData.id, false)
                        }
                        MenuItem {
                            visible: pieceCard.modelData.shared > 1
                            height: visible ? implicitHeight : 0
                            text: qsTrId("piece.delete_for_all")
                            onTriggered: Data.deletePiece(pieceCard.modelData.id, true)
                        }
                    }
                }
            }
        }
    }

    // ================================================================ activities
    Component {
        id: activitiesTab
        ColumnLayout {
            spacing: 10
            Repeater {
                model: [1, 0]
                delegate: ColumnLayout {
                    id: group
                    required property int modelData
                    readonly property var items: detail.activities.filter(a => a.continuous === modelData)
                    Layout.fillWidth: true
                    spacing: 8
                    RowLayout {
                        Layout.fillWidth: true
                        SectionLabel {
                            Layout.fillWidth: true
                            text: group.modelData === 1 ? qsTrId("activity.regular") : qsTrId("activity.irregular")
                        }
                        ActionButton {
                            kind: "quiet"
                            iconName: "plus"
                            text: qsTrId("common.add")
                            onClicked: Nav.editActivity(detail.studentId, "", group.modelData === 1)
                        }
                    }
                    Rectangle {
                        visible: group.items.length > 0
                        Layout.fillWidth: true
                        implicitHeight: activityColumn.implicitHeight
                        radius: Theme.radius
                        color: Theme.surface
                        clip: true
                        ColumnLayout {
                            id: activityColumn
                            width: parent.width
                            spacing: 0
                            Repeater {
                                model: group.items
                                delegate: AbstractButton {
                                    id: activityRow
                                    required property var modelData
                                    required property int index
                                    Layout.fillWidth: true
                                    implicitHeight: activityText.implicitHeight + 24
                                    focusPolicy: Qt.StrongFocus
                                    onClicked: Nav.editActivity(detail.studentId, modelData.id, modelData.continuous === 1)
                                    background: Rectangle {
                                        color: activityRow.down ? Theme.surfaceAlt : "transparent"
                                        Rectangle {
                                            visible: activityRow.index > 0
                                            width: parent.width
                                            height: 1
                                            color: Theme.divider
                                        }
                                    }
                                    contentItem: ColumnLayout {
                                        id: activityText
                                        spacing: 2
                                        Text {
                                            Layout.fillWidth: true
                                            wrapMode: Text.WordWrap
                                            text: activityRow.modelData.description
                                            color: Theme.text
                                            opacity: activityRow.modelData.continuous === 1 && !activityRow.modelData.running ? 0.6 : 1
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 15
                                            font.weight: Font.DemiBold
                                        }
                                        Text {
                                            Layout.fillWidth: true
                                            elide: Text.ElideRight
                                            text: {
                                                const a = activityRow.modelData
                                                if (a.continuous === 1) {
                                                    const parts = [Labels.continuousKind(a.continuous_kind)]
                                                    parts.push(Labels.weekdayLabel(a.weekday) + (a.time ? " " + a.time : ""))
                                                    parts.push(a.end_date ? qsTrId("common.from_to").arg(Data.formatDate(a.date)).arg(Data.formatDate(a.end_date))
                                                                          : qsTrId("common.since").arg(Data.formatDate(a.date)))
                                                    return parts.join(" · ")
                                                }
                                                return Data.formatDate(a.date, true) + " · " + Labels.singleKind(a.single_kind)
                                            }
                                            color: Theme.textMuted
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 13
                                        }
                                    }
                                    leftPadding: 16
                                    rightPadding: 16
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // ================================================================ personal data
    Component {
        id: dataTab
        StudentForm {
            studentId: detail.studentId
        }
    }

    // ================================================================ borrowed sheet music
    Component {
        id: loansTab
        ColumnLayout {
            spacing: 10
            EmptyHint {
                visible: detail.loans.length === 0
                text: qsTrId("loans.empty")
            }
            Repeater {
                model: detail.loans
                delegate: Rectangle {
                    required property var modelData
                    Layout.fillWidth: true
                    implicitHeight: loanColumn.implicitHeight + 24
                    radius: Theme.radius
                    color: Theme.surface
                    ColumnLayout {
                        id: loanColumn
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.margins: 16
                        spacing: 2
                        Text {
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            text: modelData.title
                            color: Theme.text
                            font.family: Theme.fontFamily
                            font.pixelSize: 15
                            font.weight: Font.Bold
                        }
                        Text {
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                            text: [modelData.author, modelData.publisher,
                                   modelData.lent_since ? qsTrId("common.since").arg(Data.formatDate(modelData.lent_since)) : ""]
                                  .filter(Boolean).join(" · ")
                            color: Theme.textMuted
                            font.family: Theme.fontFamily
                            font.pixelSize: 13
                        }
                    }
                }
            }
        }
    }
}
