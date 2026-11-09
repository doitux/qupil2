// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// One student: lesson selector, then notes, pieces, activities, personal data
// and borrowed sheet music. Notes and pieces belong to a lesson membership,
// so the selector decides which ones are shown (also of earlier lessons).

import QtQml
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
    //! Note whose edit and delete buttons are shown ("" for none)
    property string revealedNote
    onCurrentMembershipChanged: revealedNote = ""
    onTabChanged: revealedNote = ""

    signal back()

    readonly property var student: { if (Data.revision < 0) return undefined; return Data.student(studentId) }
    readonly property var memberships: student.memberships || []
    //! One entry per lesson; lessons kept together are one entry (ids: all of them)
    readonly property var choices: student.choices || []
    readonly property var currentChoice: {
        const given = membershipId !== "" ? choices.find(c => c.ids.indexOf(membershipId) >= 0) : undefined
        return given || (choices.length > 0 ? choices[0] : null)
    }
    // Lessons kept together: the one asked for if it still runs, else the entry's first active one
    readonly property string currentMembership: {
        if (!currentChoice)
            return ""
        const asked = memberships.find(m => m.id === membershipId)
        return asked && asked.active && currentChoice.ids.indexOf(membershipId) >= 0 ? membershipId : currentChoice.id
    }
    readonly property var membership: memberships.find(m => m.id === currentMembership) || null
    readonly property string currentLessonId: membership && membership.lesson_id ? membership.lesson_id : ""
    readonly property int inactiveCount: choices.filter(c => !c.active).length
    //! Weekly individual lessons on several days: offer to keep them together / the switch in the menu
    readonly property var togetherOffer: student.togetherOffer || null
    readonly property var togetherSwitch: student.togetherSwitch || null
    readonly property var notes: { if (Data.revision < 0) return undefined; return currentMembership ? Data.notes(currentMembership) : [] }
    readonly property var plans: { if (Data.revision < 0) return undefined; return currentMembership ? Data.plans(currentMembership) : [] }
    readonly property var pieces: { if (Data.revision < 0) return undefined; return currentMembership ? Data.pieces(currentMembership, pieceFilter) : [] }
    readonly property var pieceCounts: { if (Data.revision < 0) return undefined; return currentMembership ? Data.pieceCounts(currentMembership) : [0, 0, 0, 0, 0, 0] }
    readonly property var activities: { if (Data.revision < 0) return undefined; return studentId ? Data.activities(studentId) : [] }
    readonly property var loans: { if (Data.revision < 0) return undefined; return studentId ? Data.loans(studentId) : [] }
    //! Reminders for this student (they also pop up when the student's lesson starts), and those
    //! for one of their individual lessons (mode 3: only there; a group's stay at the lesson)
    readonly property var reminders: {
        if (Data.revision < 0) return undefined
        if (!studentId)
            return []
        const single = memberships.filter(m => m.active && m.kind === 1).map(m => m.lesson_id)
        return Data.reminders().filter(r => (r.mode === 2 && r.student_id === studentId)
                                            || (r.mode === 3 && single.indexOf(r.lesson_id) >= 0))
    }
    //! Birthday today or since the last lesson (gone once ticked off)
    readonly property var birthday: { if (Data.revision < 0) return undefined; return studentId ? Data.birthdayNote(studentId) : ({}) }
    readonly property int groupSize: { if (Data.revision < 0) return undefined; return currentMembership ? Data.groupSize(currentMembership) : 1 }

    function relative(iso) {
        const d = new Date(iso + "T00:00:00")
        const today = new Date(Data.today() + "T00:00:00")
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
        unfolded = ({})
    }

    // ---------------------------------------------------------------- long notes
    //! Notes and plans unfolded with "Show more" (id → true); kept here, not in the
    //! cards, so they stay open when the list is built anew (e.g. after a sync)
    property var unfolded: ({})
    //! One line of note text (15 px, line height 1.15, a little for the paragraph gaps)
    readonly property real noteLine: Math.ceil(noteMetrics.lineSpacing * 1.15) + 2
    FontMetrics {
        id: noteMetrics
        font.family: Theme.fontFamily
        font.pixelSize: 15
    }
    function toggleFold(id, card) {
        const open = unfolded[id] !== true
        const u = Object.assign({}, unfolded)
        if (open)
            u[id] = true
        else
            delete u[id]
        unfolded = u
        // "Show less" at the end of a long note: the note shrinks above the finger, so the
        // page goes back to its beginning instead of leaving the reader somewhere further down
        if (!open) {
            const top = card.mapToItem(flick.contentItem, 0, 0).y
            if (top < flick.contentY)
                flick.contentY = Math.max(0, top - 12)
        }
    }

    // ---------------------------------------------------------------- one piece in view
    //! Piece to show (e.g. tapped in the pieces ready to perform): the pieces open with
    //! it in view and marked for a few seconds. Set it again to show it again.
    property string focusPiece
    //! The piece marked right now ("" for none)
    property string markedPiece
    onFocusPieceChanged: if (focusPiece !== "") Qt.callLater(showFocusPiece)
    Component.onCompleted: if (focusPiece !== "") Qt.callLater(showFocusPiece)
    function showFocusPiece() {
        if (focusPiece === "")
            return
        tab = 1
        pieceFilter = -1
        markedPiece = focusPiece
        unmark.restart()
    }
    //! Scrolls the page so the marked piece's card is in view (after the layout settled)
    function scrollToPiece(card) {
        scrollTarget = card
        scrollLater.restart()
    }
    property Item scrollTarget
    Timer {
        id: scrollLater
        interval: 60
        onTriggered: {
            if (!detail.scrollTarget)
                return
            const y = detail.scrollTarget.mapToItem(flick.contentItem, 0, 0).y
            const room = flick.height - detail.scrollTarget.height
            // Some room above it, or centred when it fits
            const target = y - Math.max(24, room / 3)
            flick.contentY = Math.max(0, Math.min(target, flick.contentHeight - flick.height))
            detail.scrollTarget = null
        }
    }
    Timer {
        id: unmark
        interval: 4000
        onTriggered: detail.markedPiece = ""
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
                    onClicked: detail.openMoreMenu()
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
                    onClicked: detail.openMoreMenu()
                }
            }

            // ------------------------------------------------------------ birthday
            // Lime band: today, or had a birthday since the last lesson. The tick puts it away
            // (on every device) until the next birthday.
            Rectangle {
                id: birthdayBand
                readonly property bool today: !!detail.birthday && detail.birthday.kind === "today"
                visible: !!detail.birthday && (detail.birthday.kind === "today" || detail.birthday.kind === "past")
                Layout.fillWidth: true
                implicitHeight: birthdayRow.implicitHeight + 24
                radius: 18
                color: Theme.lime
                RowLayout {
                    id: birthdayRow
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    anchors.rightMargin: 8
                    spacing: 12
                    Icon {
                        Layout.preferredWidth: 28
                        Layout.preferredHeight: 28
                        name: "cake"
                        color: Theme.textOnLime
                        strokeWidth: 2.2
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 1
                        Text {
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            text: birthdayBand.today ? qsTrId("birthday.today") : qsTrId("birthday.past")
                            color: Theme.textOnLime
                            font.family: Theme.fontFamily
                            font.pixelSize: 18
                            font.weight: Font.ExtraBold
                        }
                        Text {
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            text: !birthdayBand.visible ? ""
                                  : birthdayBand.today ? qsTrId("birthday.today_text").arg(detail.birthday.forename).arg(detail.birthday.turns)
                                  : qsTrId("birthday.past_text").arg(detail.birthday.forename).arg(detail.birthday.when).arg(detail.birthday.turns)
                            color: Theme.textOnLime
                            font.family: Theme.fontFamily
                            font.pixelSize: 14
                            font.weight: Font.DemiBold
                        }
                    }
                    IconButton {
                        iconName: "check"
                        text: qsTrId("birthday.greeted")
                        fill: Theme.textOnLime
                        tint: Theme.lime
                        onClicked: Data.greetBirthday(detail.studentId)
                    }
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
                            // Day and time behind the name when there are several lessons (same automatic names)
                            text: !detail.currentChoice ? ""
                                  : detail.currentChoice.label + (detail.currentChoice.suffix ? " · " + detail.currentChoice.suffix : "")
                            color: Theme.text
                            font.family: Theme.fontFamily
                            font.pixelSize: 15
                            font.weight: Font.Bold
                        }
                        RowLayout {
                            visible: !!detail.currentChoice && detail.currentChoice.together
                            Layout.fillWidth: true
                            spacing: 5
                            Icon {
                                Layout.preferredWidth: 14
                                Layout.preferredHeight: 14
                                name: "link"
                                color: Theme.accentText
                                strokeWidth: 2.2
                            }
                            Text {
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                                text: detail.currentChoice ? detail.currentChoice.togetherText : ""
                                color: Theme.textMuted
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                            }
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

                AppMenu {
                    id: lessonMenu
                    width: Math.max(lessonSelector.width, implicitWidth)
                    // (Instantiator: a Repeater's rows would land after the rows below)
                    Instantiator {
                        model: detail.choices
                        onObjectAdded: (index, object) => lessonMenu.insertItem(index, object)
                        onObjectRemoved: (index, object) => lessonMenu.removeItem(object)
                        delegate: AppMenuItem {
                            required property var modelData
                            text: modelData.label
                            // Several lessons: which day (lessons kept together: all their days)
                            subtitle: modelData.together ? modelData.togetherText
                                      : detail.choices.length > 1 ? modelData.detail : ""
                            checkable: true
                            checked: !!detail.currentChoice && modelData.id === detail.currentChoice.id
                            onTriggered: detail.membershipId = modelData.id
                        }
                    }
                    AppMenuSeparator {
                        visible: !!detail.togetherSwitch
                    }
                    AppMenuItem {
                        visible: !!detail.togetherSwitch
                        text: detail.togetherSwitch ? detail.togetherSwitch.title : ""
                        subtitle: detail.togetherSwitch ? detail.togetherSwitch.hint : ""
                        checkable: true
                        toggle: true
                        checked: !!detail.togetherSwitch && detail.togetherSwitch.on
                        onTriggered: Data.keepTogether(detail.togetherSwitch.ids, !detail.togetherSwitch.on)
                    }
                    AppMenuSeparator {
                        visible: detail.currentLessonId !== ""
                    }
                    AppMenuItem {
                        visible: detail.currentLessonId !== ""
                        iconName: "calendar"
                        text: qsTrId("lesson.edit")
                        onTriggered: Nav.editLesson(detail.currentLessonId, -1)
                    }
                }
            }

            // ------------------------------------------------------------ individual lessons on several days
            // Once, until "Keep together" or "Keep separate"; the switch stays in the lesson menu
            Rectangle {
                id: togetherCard
                visible: !!detail.togetherOffer
                Layout.fillWidth: true
                implicitHeight: togetherColumn.implicitHeight + 28
                radius: 16
                color: Theme.accentSoft
                Accessible.role: Accessible.Grouping
                Accessible.name: detail.togetherOffer ? detail.togetherOffer.title : ""
                ColumnLayout {
                    id: togetherColumn
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.margins: 14
                    spacing: 6
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 8
                        Icon {
                            Layout.preferredWidth: 18
                            Layout.preferredHeight: 18
                            Layout.alignment: Qt.AlignTop
                            Layout.topMargin: 1
                            name: "link"
                            color: Theme.accentText
                            strokeWidth: 2.2
                        }
                        Text {
                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignTop
                            wrapMode: Text.WordWrap
                            text: detail.togetherOffer ? detail.togetherOffer.title : ""
                            color: Theme.accentText
                            font.family: Theme.fontFamily
                            font.pixelSize: 15
                            font.weight: Font.ExtraBold
                        }
                    }
                    Text {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: detail.togetherOffer ? detail.togetherOffer.text : ""
                        color: Theme.textSecondary
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                        lineHeight: 1.1
                    }
                    Flow {
                        Layout.fillWidth: true
                        Layout.topMargin: 4
                        spacing: 8
                        ActionButton {
                            kind: "primary"
                            text: qsTrId("together.keep")
                            onClicked: Data.keepTogether(detail.togetherOffer.ids, true)
                        }
                        ActionButton {
                            kind: "quiet"
                            text: qsTrId("together.separate")
                            onClicked: Data.keepSeparate(detail.studentId, detail.togetherOffer.ids)
                        }
                    }
                }
            }

            // ------------------------------------------------------------ reminders
            // Visible whenever the student is open: tap the text to change it, the tick when done
            Repeater {
                model: detail.reminders
                delegate: Rectangle {
                    id: reminderRow
                    required property var modelData
                    readonly property color ink: Theme.dark ? Theme.lime : Theme.accentText
                    Layout.fillWidth: true
                    implicitHeight: Math.max(56, reminderText.implicitHeight + 24)
                    radius: 16
                    color: Theme.accentSoft
                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 14
                        anchors.rightMargin: 6
                        spacing: 10
                        // The bell dark on a lime dot
                        Rectangle {
                            implicitWidth: 30
                            implicitHeight: 30
                            radius: 15
                            color: Theme.lime
                            Icon {
                                anchors.centerIn: parent
                                width: 17
                                height: 17
                                name: "bell"
                                color: Theme.textOnLime
                                strokeWidth: 2.4
                            }
                        }
                        AbstractButton {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            focusPolicy: Qt.StrongFocus
                            hoverEnabled: true
                            Accessible.role: Accessible.Button
                            Accessible.name: qsTrId("reminder.edit.title") + ": " + reminderRow.modelData.text
                            onClicked: Nav.editReminder(reminderRow.modelData.id, detail.studentId, "")
                            background: Rectangle {
                                radius: 8
                                color: "transparent"
                                border.width: parent.visualFocus ? 2 : 0
                                border.color: Theme.accentText
                            }
                            contentItem: ColumnLayout {
                                id: reminderText
                                spacing: 1
                                // A reminder only for the individual lesson says so ("ONLY ON TU 14:00")
                                Text {
                                    visible: reminderRow.modelData.mode === 3
                                    Layout.fillWidth: true
                                    wrapMode: Text.Wrap
                                    text: reminderRow.modelData.lessonDay ? qsTrId("reminder.only_lesson_day").arg(reminderRow.modelData.lessonDay)
                                                                          : qsTrId("reminder.only_this_lesson")
                                    color: Theme.textMuted
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 11
                                    font.weight: Font.Bold
                                    font.letterSpacing: 0.4
                                    font.capitalization: Font.AllUppercase
                                }
                                Text {
                                    Layout.fillWidth: true
                                    text: reminderRow.modelData.text
                                    wrapMode: Text.Wrap
                                    verticalAlignment: Text.AlignVCenter
                                    color: Theme.text
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 15
                                    font.weight: Font.Medium
                                }
                            }
                        }
                        IconButton {
                            iconName: "check"
                            text: qsTrId("reminder.done")
                            tint: reminderRow.ink
                            fill: Theme.dark ? Qt.lighter(Theme.accentSoft, 1.35) : Qt.darker(Theme.accentSoft, 1.06)
                            onClicked: reminderDone.ask(reminderRow.modelData)
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

    // A plan for the next lesson, above "+ Note"
    Fab {
        visible: detail.tab === 0 && detail.currentMembership !== "" && detail.membership && detail.membership.active
        anchors.right: parent.right
        anchors.bottom: noteFab.top
        anchors.rightMargin: 20
        anchors.bottomMargin: 12
        outlined: true
        iconName: "plan"
        text: qsTrId("plan.label")
        onClicked: Nav.editPlan(detail.currentMembership, "")
    }
    Fab {
        id: noteFab
        visible: (detail.tab === 0 || detail.tab === 1) && detail.currentMembership !== ""
                 && detail.membership && detail.membership.active
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 20
        // Above the running metronome at the bottom right, not under it
        anchors.bottomMargin: 20 + Nav.playerInset
        Behavior on anchors.bottomMargin { NumberAnimation { duration: 180; easing.type: Easing.OutCubic } }
        text: detail.tab === 0 ? qsTrId("common.note") : qsTrId("common.piece")
        onClicked: detail.tab === 0 ? Nav.editNote(detail.currentMembership, "") : Nav.editPiece(detail.currentMembership, "")
    }

    //! The menu behind "…" (also for the screenshot tour)
    function openMoreMenu() {
        const button = moreButtonWide.visible ? moreButtonWide : moreButton
        studentMenu.popup(button, button.width - studentMenu.width, button.height + 4)
    }
    function closeMoreMenu() {
        studentMenu.close()
    }

    AppMenu {
        id: studentMenu
        AppMenuItem {
            iconName: "edit"
            text: qsTrId("student.edit_data")
            onTriggered: detail.tab = 3
        }
        AppMenuItem {
            visible: detail.currentLessonId !== ""
            iconName: "calendar"
            text: qsTrId("lesson.edit")
            onTriggered: Nav.editLesson(detail.currentLessonId, -1)
        }
        AppMenuItem {
            iconName: "bell"
            text: qsTrId("student.add_reminder")
            onTriggered: Nav.editReminder("", detail.studentId, "")
        }
        AppMenuItem {
            iconName: "archive"
            text: qsTrId("student.archive")
            onTriggered: {
                Docs.archiveStudent(detail.studentId)
                Nav.toast(qsTrId("archive.saved").arg(detail.student.name || ""))
            }
        }
        AppMenuSeparator {}
        AppMenuItem {
            iconName: "archive"
            destructive: true
            text: qsTrId("student.archive_delete")
            onTriggered: archiveAndDelete.open()
        }
        AppMenuItem {
            iconName: "trash"
            destructive: true
            text: qsTrId("student.delete")
            onTriggered: deleteStudent.open()
        }
    }

    AppDialog {
        id: reminderDone
        property string reminderId
        function ask(reminder) {
            reminderId = reminder.id
            text = reminder.text
            open()
        }
        iconName: "bell"
        title: qsTrId("reminder.done_remove")
        rejectText: qsTrId("common.no")
        acceptText: qsTrId("common.yes")
        onAccepted: Data.remove("reminder", reminderId)
    }

    AppDialog {
        id: archiveAndDelete
        iconName: "archive"
        destructive: true
        title: qsTrId("student.archive_delete")
        text: qsTrId("student.delete.confirm").arg(detail.student.name || "")
        rejectText: qsTrId("common.cancel")
        acceptText: qsTrId("student.archive_delete")
        onAccepted: {
            const name = detail.student.name || ""
            Docs.archiveStudent(detail.studentId)
            Data.deleteStudent(detail.studentId)
            Nav.toast(qsTrId("archive.saved").arg(name))
            detail.back()
        }
    }

    AppDialog {
        id: deleteNoteDialog
        property string noteId
        property string date
        property int shared: 1
        property bool forAll: true
        function ask(note) {
            noteId = note.id
            date = note.date || ""
            shared = note.shared || 1
            forAll = Data.setting("save_for_all_students", 1) !== 0
            open()
        }
        iconName: "trash"
        destructive: true
        title: qsTrId("note.delete")
        text: qsTrId("note.delete.confirm").arg(Data.formatDate(date))
        rejectText: qsTrId("common.cancel")
        acceptText: qsTrId("common.delete")
        onAccepted: {
            Data.deleteNote(noteId, forAll && shared > 1)
            detail.revealedNote = ""
        }
        CheckBox {
            visible: deleteNoteDialog.shared > 1
            Layout.leftMargin: -6
            text: qsTrId("note.delete.for_all", deleteNoteDialog.shared)
            checked: deleteNoteDialog.forAll
            onToggled: deleteNoteDialog.forAll = checked
            font.family: Theme.fontFamily
            font.pixelSize: 14
        }
    }

    AppDialog {
        id: discardPlan
        property var plan: ({})
        function ask(p) {
            plan = p
            text = qsTrId("plan.discard.confirm").arg((p.when || ""))
            open()
        }
        iconName: "trash"
        destructive: true
        title: qsTrId("plan.discard")
        rejectText: qsTrId("common.cancel")
        acceptText: qsTrId("plan.discard")
        onAccepted: Data.deletePlan(plan.id, (plan.shared || 1) > 1)
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
            // Plans first: what is planned for the next lessons (not notes yet)
            Repeater {
                model: detail.plans
                delegate: PlanCard {
                    required property var modelData
                    Layout.fillWidth: true
                    plan: modelData
                }
            }
            EmptyHint {
                visible: detail.notes.length === 0 && detail.plans.length === 0
                text: detail.memberships.length === 0 ? qsTrId("student.no_lesson") : qsTrId("notes.empty")
            }
            Repeater {
                model: detail.notes
                // Tap: a new note for today with this text, to adjust for the next lesson (as in
                // Qupil 1.x). Swipe left, press and hold or right click: edit and delete roll in
                // on the right; swipe right or tap hides them again.
                // Pointer handlers (not a button): a button would also react to swipes and to the
                // right-click handler on touch screens.
                delegate: Control {
                    id: noteCard
                    required property var modelData
                    // A note with blocks (Haltung, Technik, Mozart …): one line per block
                    readonly property var noteBlocks: Data.blocks(modelData.content)
                    readonly property bool asBlocks: noteBlocks.some(b => b.more > 0)
                    readonly property bool revealed: detail.revealedNote === modelData.id
                    // A finger moves the buttons sideways: they follow it, nothing else moves them
                    property bool dragging: false
                    function toggle() { detail.revealedNote = revealed ? "" : modelData.id }
                    onRevealedChanged: if (!dragging) actions.slideTo(revealed)
                    Component.onCompleted: actions.shown = revealed ? actions.width : 0
                    function copy() {
                        if (detail.revealedNote !== "")
                            detail.revealedNote = ""
                        else
                            Nav.newNoteFrom(detail.currentMembership, modelData.id)
                    }
                    Layout.fillWidth: true
                    implicitHeight: Math.max(implicitContentHeight + topPadding + bottomPadding, Theme.touchTarget + 20)
                    topPadding: 16
                    bottomPadding: 16
                    clip: true
                    focusPolicy: Qt.StrongFocus
                    hoverEnabled: true
                    Accessible.role: Accessible.Button
                    Accessible.name: Data.formatDate(modelData.date) + " " + Data.plainText(modelData.content, 120)
                    Accessible.onPressAction: copy()
                    Keys.onReturnPressed: copy()
                    Keys.onEnterPressed: copy()
                    Keys.onSpacePressed: copy()
                    Keys.onMenuPressed: toggle()

                    TapHandler {
                        id: tap
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        onTapped: (eventPoint, button) => {
                            if (button === Qt.RightButton)
                                noteCard.toggle()
                            else
                                noteCard.copy()
                        }
                        onLongPressed: noteCard.toggle()
                    }
                    // Only watches (passive): the page keeps scrolling up and down. Sideways, the
                    // buttons follow the finger; on release they stay when more than half is in
                    // (or after a flick to the left) and slide back otherwise.
                    PointHandler {
                        id: swipeTracker
                        property real startX
                        property real startY
                        property real base      // how far they were in when the finger started
                        property real velocity  // px per ms, last movement
                        property real lastX
                        property double lastTime
                        onPointChanged: {
                            if (!active)
                                return
                            const x = point.position.x
                            const dx = x - startX
                            const now = Date.now()
                            if (now > lastTime)
                                velocity = 0.5 * velocity + 0.5 * (x - lastX) / (now - lastTime)
                            lastX = x
                            lastTime = now
                            if (!noteCard.dragging && Math.abs(dx) > Qt.styleHints.startDragDistance && Math.abs(dx) > Math.abs(point.position.y - startY)) {
                                noteCard.dragging = true
                                actions.stopSliding()
                                base = actions.shown
                            }
                            if (noteCard.dragging)
                                actions.shown = Math.max(0, Math.min(actions.width, base - dx))
                        }
                        onActiveChanged: {
                            if (active) {
                                startX = lastX = point.position.x
                                startY = point.position.y
                                lastTime = Date.now()
                                velocity = 0
                                return
                            }
                            if (!noteCard.dragging)
                                return
                            const open = velocity < -0.5 ? true
                                       : velocity > 0.5 ? false
                                       : actions.shown > actions.width / 2
                            if (open)
                                detail.revealedNote = noteCard.modelData.id
                            else if (noteCard.revealed)
                                detail.revealedNote = ""
                            noteCard.dragging = false
                            actions.slideTo(open)
                        }
                    }
                    background: Rectangle {
                        radius: Theme.radius
                        color: Theme.surface
                        border.width: noteCard.visualFocus ? 2 : 0
                        border.color: Theme.accentText
                        Rectangle {
                            anchors.fill: parent
                            radius: parent.radius
                            color: Theme.text
                            opacity: tap.pressed ? 0.06 : 0
                        }
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
                        // Blocks: one line each, a tap shows one of them
                        BlockList {
                            visible: noteCard.asBlocks
                            Layout.fillWidth: true
                            blocks: noteCard.asBlocks ? noteCard.noteBlocks : []
                            keyPrefix: noteCard.modelData.id
                        }
                        // Otherwise a long note shows its first lines and "Show more"
                        Foldable {
                            visible: !noteCard.asBlocks
                            Layout.fillWidth: true
                            lineHeight: detail.noteLine
                            fadeColor: tap.pressed ? Qt.tint(Theme.surface, Qt.rgba(Theme.text.r, Theme.text.g, Theme.text.b, 0.06))
                                                   : Theme.surface
                            expanded: detail.unfolded[noteCard.modelData.id] === true
                            onToggled: detail.toggleFold(noteCard.modelData.id, noteCard)
                            Text {
                                width: parent.width
                                textFormat: Text.RichText
                                wrapMode: Text.Wrap
                                text: Data.noteHtml(noteCard.modelData.content, Theme.noteMark)
                                color: Theme.text
                                font.family: Theme.fontFamily
                                font.pixelSize: 15
                                lineHeight: 1.15
                            }
                        }
                    }
                    leftPadding: 16
                    rightPadding: 16

                    // Plan, edit and delete, rolled in from the right edge over the text
                    Rectangle {
                        id: actions
                        z: 2
                        y: 0
                        // Three round buttons (built only while rolled in, see actionRow)
                        width: 3 * Theme.touchTarget + 2 * 10 + 28
                        height: noteCard.height
                        // How far it is rolled in (0 … width): set by the finger or by slideTo().
                        // Only this value moves, so a new layout (another student) moves nothing.
                        property real shown: 0
                        function slideTo(open) {
                            slide.stop()
                            slide.to = open ? width : 0
                            if (shown !== slide.to)
                                slide.start()
                        }
                        function stopSliding() { slide.stop() }
                        x: noteCard.width - shown
                        visible: shown > 0.5
                        radius: Theme.radius
                        color: Theme.surface
                        NumberAnimation {
                            id: slide
                            target: actions
                            property: "shown"
                            duration: 200
                            easing.type: Easing.OutCubic
                        }
                        // Soft edge towards the text
                        Rectangle {
                            anchors.right: parent.left
                            anchors.rightMargin: -Theme.radius
                            width: 28
                            height: parent.height
                            gradient: Gradient {
                                orientation: Gradient.Horizontal
                                GradientStop { position: 0; color: Qt.rgba(Theme.surface.r, Theme.surface.g, Theme.surface.b, 0) }
                                GradientStop { position: 1; color: Theme.surface }
                            }
                        }
                        // Most notes never show their buttons: they are made when a finger or
                        // the mouse rolls them in, not for every note when the student opens
                        Loader {
                            id: actionRow
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.right: parent.right
                            anchors.rightMargin: 14
                            active: actions.visible
                            sourceComponent: actionButtons
                        }
                        Component {
                            id: actionButtons
                            Row {
                                spacing: 10
                                IconButton {
                                    iconName: "plan"
                                    text: qsTrId("plan.from_note")
                                    fill: Theme.surfaceAlt
                                    tint: Theme.accentText
                                    onClicked: {
                                        detail.revealedNote = ""
                                        Nav.planFromNote(detail.currentMembership, noteCard.modelData.id)
                                    }
                                }
                                IconButton {
                                    iconName: "edit"
                                    text: qsTrId("note.edit")
                                    fill: Theme.accent
                                    tint: Theme.textOnAccent
                                    onClicked: {
                                        detail.revealedNote = ""
                                        Nav.editNote(detail.currentMembership, noteCard.modelData.id)
                                    }
                                }
                                IconButton {
                                    iconName: "trash"
                                    text: qsTrId("note.delete")
                                    fill: Theme.dangerFill
                                    tint: Theme.textOnAccent
                                    onClicked: deleteNoteDialog.ask(noteCard.modelData)
                                }
                            }
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
                // Tap: edit; press and hold or right click: menu (pointer handlers, see the notes)
                delegate: Control {
                    id: pieceCard
                    required property var modelData
                    //! Shown on purpose (Nav.showPiece): scrolled to and marked for a moment
                    readonly property bool marked: detail.markedPiece !== "" && detail.markedPiece === modelData.id
                    onMarkedChanged: if (marked) detail.scrollToPiece(pieceCard)
                    Component.onCompleted: if (marked) detail.scrollToPiece(pieceCard)
                    function edit() { Nav.editPiece(detail.currentMembership, modelData.id) }
                    Layout.fillWidth: true
                    implicitHeight: implicitContentHeight + topPadding + bottomPadding
                    topPadding: 16
                    bottomPadding: 16
                    focusPolicy: Qt.StrongFocus
                    hoverEnabled: true
                    Accessible.role: Accessible.Button
                    Accessible.name: modelData.composer + " " + modelData.title
                    Accessible.onPressAction: edit()
                    Keys.onReturnPressed: edit()
                    Keys.onEnterPressed: edit()
                    Keys.onSpacePressed: edit()
                    Keys.onMenuPressed: pieceMenu.openAt(pieceCard, pieceCard.width / 2, pieceCard.height / 2)
                    TapHandler {
                        id: pieceTap
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        onTapped: (eventPoint, button) => {
                            if (button === Qt.RightButton)
                                pieceMenu.openAt(pieceCard, eventPoint.position.x, eventPoint.position.y)
                            else
                                pieceCard.edit()
                        }
                        // Where the finger is (popup() alone would use the last mouse position)
                        onLongPressed: pieceMenu.openAt(pieceCard, point.position.x, point.position.y)
                    }
                    background: Rectangle {
                        radius: Theme.radius
                        color: pieceCard.marked ? Theme.accentSoft : Theme.surface
                        Behavior on color { ColorAnimation { duration: 500 } }
                        border.width: pieceCard.visualFocus || pieceCard.marked ? 2 : 0
                        border.color: pieceCard.marked ? Theme.accent : Theme.accentText
                        Rectangle {
                            anchors.fill: parent
                            radius: parent.radius
                            color: Theme.text
                            opacity: pieceTap.pressed ? 0.06 : 0
                        }
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

                    AppMenu {
                        id: pieceMenu
                        AppMenu {
                            iconName: "star"
                            title: qsTrId("piece.change_status")
                            Repeater {
                                model: [0, 1, 2, 3, 4]
                                delegate: AppMenuItem {
                                    required property int modelData
                                    text: badgeLabel.label(modelData)
                                    checkable: true
                                    checked: pieceCard.modelData.state === modelData
                                    onTriggered: Data.updatePiece(pieceCard.modelData.id, { state: modelData }, true)
                                }
                            }
                        }
                        AppMenuItem {
                            iconName: "edit"
                            text: qsTrId("common.edit")
                            onTriggered: Nav.editPiece(detail.currentMembership, pieceCard.modelData.id)
                        }
                        AppMenuSeparator {}
                        AppMenuItem {
                            iconName: "trash"
                            destructive: true
                            text: qsTrId("piece.delete")
                            onTriggered: Data.deletePiece(pieceCard.modelData.id, false)
                        }
                        AppMenuItem {
                            iconName: "trash"
                            destructive: true
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
                                    implicitHeight: implicitContentHeight + topPadding + bottomPadding
                                    topPadding: 12
                                    bottomPadding: 12
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

    // ================================================================ plans
    //! A plan: dashed frame (not a note yet), its points; once the lesson has begun,
    //! "Turn into a note". Tap: change it. "…": change, move to the next lesson, discard.
    component PlanCard: Control {
        id: card
        property var plan: ({})
        readonly property bool shared: (plan.shared || 1) > 1
        implicitHeight: planColumn.implicitHeight + topPadding + bottomPadding
        topPadding: 10
        bottomPadding: 16
        leftPadding: 16
        rightPadding: 8
        focusPolicy: Qt.StrongFocus
        hoverEnabled: true
        Accessible.role: Accessible.Button
        Accessible.name: qsTrId("plan.title_for").arg((plan.when || "")) + ": " + (plan.points || []).join(", ")
        Accessible.onPressAction: Nav.editPlan(detail.currentMembership, plan.id)
        Keys.onReturnPressed: Nav.editPlan(detail.currentMembership, plan.id)
        Keys.onSpacePressed: Nav.editPlan(detail.currentMembership, plan.id)

        TapHandler {
            id: cardTap
            onTapped: Nav.editPlan(detail.currentMembership, card.plan.id)
        }

        // Dashed frame: one rounded rectangle on a canvas, painted again when the card
        // changes its size or colours (crisp on high-density screens as well)
        background: Item {
            Rectangle {
                anchors.fill: parent
                radius: Theme.radius
                color: cardTap.pressed ? Theme.surfaceAlt : Theme.surface
            }
            Canvas {
                id: frame
                anchors.fill: parent
                readonly property color stroke: card.visualFocus ? Theme.accent : Theme.accentText
                readonly property real lineWidth: card.visualFocus ? 2.5 : 1.6
                readonly property real radius: Theme.radius
                onStrokeChanged: requestPaint()
                onLineWidthChanged: requestPaint()
                onRadiusChanged: requestPaint()
                onWidthChanged: requestPaint()
                onHeightChanged: requestPaint()
                onPaint: {
                    const ctx = getContext("2d")
                    ctx.reset()
                    const w = lineWidth
                    ctx.lineWidth = w
                    ctx.strokeStyle = stroke
                    ctx.setLineDash([3, 2.5])
                    ctx.beginPath()
                    ctx.roundedRect(w / 2, w / 2, width - w, height - w, radius - w / 2, radius - w / 2)
                    ctx.stroke()
                }
            }
        }

        contentItem: ColumnLayout {
            id: planColumn
            spacing: 8
            RowLayout {
                Layout.fillWidth: true
                spacing: 6
                Icon {
                    Layout.preferredWidth: 18
                    Layout.preferredHeight: 18
                    name: "plan"
                    color: Theme.accentText
                    strokeWidth: 2
                }
                Text {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: qsTrId("plan.title_for").arg((card.plan.when || ""))
                    color: Theme.accentText
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                    font.weight: Font.ExtraBold
                }
                Icon {
                    visible: card.shared
                    Layout.preferredWidth: 16
                    Layout.preferredHeight: 16
                    name: "students"
                    color: Theme.textMuted
                }
                IconButton {
                    id: planMore
                    size: 36
                    iconName: "more"
                    text: qsTrId("common.more_actions")
                    fill: "transparent"
                    tint: Theme.textMuted
                    onClicked: planMenu.popup(planMore, planMore.width - planMenu.width, planMore.height)
                }
            }
            // A long plan shows its first points and "Show more", like a long note
            Foldable {
                Layout.fillWidth: true
                lineHeight: detail.noteLine + 4
                fadeColor: cardTap.pressed ? Theme.surfaceAlt : Theme.surface
                expanded: detail.unfolded[card.plan.id] === true
                // Blocks are short already (one line each)
                foldedLines: (card.plan.blocks || []).some(b => b.more > 0) ? 1000 : 7
                onToggled: detail.toggleFold(card.plan.id, card)
                BlockList {
                    width: parent.width
                    blocks: card.plan.blocks || []
                    keyPrefix: card.plan.id
                }
            }
            // Once the lesson has begun: what becomes of it
            Rectangle {
                visible: card.plan.due === true
                Layout.fillWidth: true
                Layout.rightMargin: 8
                Layout.topMargin: 4
                implicitHeight: 1
                color: Theme.divider
            }
            Text {
                visible: card.plan.due === true
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: qsTrId("plan.lesson_over")
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 13
            }
            ActionButton {
                visible: card.plan.due === true
                kind: "primary"
                iconName: "file"
                text: qsTrId("plan.make_note")
                onClicked: Nav.finishPlan(detail.currentMembership, card.plan.id)
            }
        }

        AppMenu {
            id: planMenu
            AppMenuItem {
                iconName: "edit"
                text: qsTrId("plan.edit")
                onTriggered: Nav.editPlan(detail.currentMembership, card.plan.id)
            }
            AppMenuItem {
                iconName: "calendar"
                text: qsTrId("plan.postpone")
                onTriggered: {
                    const date = Data.postponePlan(card.plan.id, card.shared)
                    if (date)
                        Nav.toast(qsTrId("plan.postponed").arg(Data.formatDate(date, true)))
                }
            }
            AppMenuSeparator {}
            AppMenuItem {
                iconName: "trash"
                destructive: true
                text: qsTrId("plan.discard")
                onTriggered: discardPlan.ask(card.plan)
            }
        }
    }

    //! Blocks (heading and lines below it, after an empty line): one line each with "+4" for
    //! the rest, a tap shows it. A single line is a plain point.
    component BlockList: Column {
        id: list
        property var blocks: []
        property string keyPrefix
        spacing: 8
        Repeater {
            model: list.blocks
            // A block (heading and lines below it, after an empty line) is one line
            // with "+4" for the rest; a tap shows it. A single line is a plain point.
            // Plain items, not a RowLayout: a wrapped rich text in a row layout keeps
            // the height it had at a narrower width and leaves gaps between the points
            delegate: Column {
                id: block
                required property var modelData
                required property int index
                readonly property bool hasMore: modelData.more > 0
                readonly property string key: list.keyPrefix + "/" + index
                readonly property bool open: detail.unfolded[key] === true
                width: parent ? parent.width : 0
                spacing: 2
                Item {
                    visible: !block.hasMore
                    width: parent.width
                    implicitHeight: pointText.implicitHeight
                    Rectangle {
                        y: 6
                        width: 8
                        height: 8
                        radius: 4
                        color: "transparent"
                        border.width: 1.6
                        border.color: Theme.accentText
                    }
                    Text {
                        id: pointText
                        x: 18
                        width: parent.width - x - 8
                        wrapMode: Text.Wrap
                        textFormat: Text.RichText
                        text: block.hasMore ? "" : Data.noteHtml(block.modelData.title, Theme.noteMark)
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        lineHeight: 1.15
                    }
                }
                AbstractButton {
                    id: blockHead
                    visible: block.hasMore
                    width: parent.width
                    height: 30
                    focusPolicy: Qt.StrongFocus
                    Accessible.role: Accessible.Button
                    Accessible.name: block.modelData.heading + ", " + qsTrId("block.more_lines", block.modelData.more)
                    onClicked: detail.toggleFold(block.key, block)
                    background: Rectangle {
                        x: -6
                        width: parent.width + 6 - 4
                        radius: 8
                        color: Theme.accentText
                        opacity: blockHead.down ? 0.10 : 0
                        border.width: blockHead.visualFocus ? 2 : 0
                        border.color: Theme.accentText
                    }
                    contentItem: Item {
                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter
                            width: 8
                            height: 8
                            radius: 4
                            color: Theme.accentText
                        }
                        Text {
                            x: 18
                            anchors.verticalCenter: parent.verticalCenter
                            width: parent.width - x - moreBadge.width - 14
                            elide: Text.ElideRight
                            text: block.modelData.heading
                            color: Theme.text
                            font.family: Theme.fontFamily
                            font.pixelSize: 15
                            font.weight: Font.Bold
                        }
                        MoreBadge {
                            id: moreBadge
                            anchors.right: parent.right
                            anchors.rightMargin: 8
                            anchors.verticalCenter: parent.verticalCenter
                            count: block.modelData.more
                            open: block.open
                        }
                    }
                }
                Text {
                    visible: block.open
                    x: 18
                    width: parent.width - x - 8
                    bottomPadding: 6
                    wrapMode: Text.Wrap
                    textFormat: Text.RichText
                    text: block.open ? Data.noteHtml(block.modelData.body, Theme.noteMark) : ""
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 14
                    lineHeight: 1.1
                }
            }
        }
    }
}
