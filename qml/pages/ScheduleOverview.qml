// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// The day: week strip, notices (birthdays, reminders, recital candidates),
// the running lesson and the lessons of the selected weekday.
// Used alone on phones and as the left column on tablets/desktop.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Flickable {
    id: overview
    objectName: "scheduleOverview"

    //! Tablet/desktop column: lessons as cards with their students
    property bool columnMode: false
    property int day: todayIndex
    property string selectedMembership: ""
    property date now: new Date()

    signal studentClicked(string studentId, string membershipId)
    signal lessonClicked(string lessonId)

    readonly property int todayIndex: (now.getDay() + 6) % 7
    readonly property bool isToday: day === todayIndex
    readonly property string nowTime: Qt.formatTime(now, "hh:mm")
    readonly property var lessons: { if (Data.revision < 0) return undefined; return Data.lessonsOn(day) }
    readonly property var counts: { if (Data.revision < 0) return undefined; return Data.lessonCountPerWeekday() }
    readonly property var irregular: { if (Data.revision < 0) return undefined; return Data.irregularLessons() }
    readonly property var running: {
        if (!isToday)
            return null
        for (const l of lessons)
            if (l.start_time <= nowTime && l.end_time > nowTime)
                return l
        return null
    }
    readonly property var birthdays: {
        if (Data.revision < 0) return undefined
        return isToday && Data.setting("birthday_reminder", 1) !== 0 ? Data.birthdays(6) : []
    }
    readonly property var startReminders: { if (Data.revision < 0) return undefined; return isToday ? Data.reminders().filter(r => r.mode === 0) : [] }
    readonly property int candidateCount: { if (Data.revision < 0) return undefined; return isToday ? Data.recitalCandidates().length : 0 }
    readonly property bool hasNotices: birthdays.length + startReminders.length + (candidateCount > 0 ? 1 : 0) > 0
    property bool showIrregular: false

    function dateOf(index) {
        const d = new Date(now.getFullYear(), now.getMonth(), now.getDate())
        d.setDate(d.getDate() + index - todayIndex)
        return d
    }
    function minutesLeft(lesson) {
        const [h, m] = lesson.end_time.split(":").map(Number)
        return Math.max(0, h * 60 + m - (now.getHours() * 60 + now.getMinutes()))
    }
    function kindLabel(kind) {
        switch (kind) {
        case 2: return qsTrId("lesson.type.group")
        case 3: return qsTrId("lesson.type.ensemble")
        default: return qsTrId("lesson.type.individual")
        }
    }
    function openLesson(lesson) {
        if (lesson.studentCount === 1 && !overview.columnMode)
            overview.studentClicked(lesson.students[0].id, lesson.students[0].membership_id)
        else
            overview.lessonClicked(lesson.id)
    }

    Timer {
        interval: 20000
        running: true
        repeat: true
        onTriggered: overview.now = new Date()
    }

    clip: true
    contentHeight: column.implicitHeight + 40
    boundsBehavior: Flickable.StopAtBounds
    ScrollBar.vertical: ScrollBar {}

    ColumnLayout {
        id: column
        x: overview.columnMode ? 12 : 20
        y: overview.columnMode ? 28 : 40
        width: overview.width - 2 * x
        spacing: 18

        // ------------------------------------------------------------ header
        RowLayout {
            Layout.fillWidth: true
            Layout.leftMargin: overview.columnMode ? 8 : 0
            spacing: 8
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                Text {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: {
                        if (LanguageManager.effectiveLanguage === "") return ""
                        return Data.dayHeading(overview.dateOf(overview.day)).toUpperCase()
                    }
                    color: Theme.accentText
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                    font.weight: Font.Bold
                    font.letterSpacing: 0.5
                }
                Text {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: {
                        if (LanguageManager.effectiveLanguage === "") return ""
                        return overview.isToday ? qsTrId("nav.today")
                                                : Qt.locale().standaloneDayName((overview.day + 1) % 7, Locale.LongFormat)
                    }
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: overview.columnMode ? 26 : 34
                    font.weight: Font.ExtraBold
                }
            }
            IconButton {
                id: addButton
                Layout.alignment: Qt.AlignBottom
                filled: true
                iconName: "plus"
                text: qsTrId("menu.new")
                onClicked: addMenu.popup(addButton, addButton.width - addMenu.width, addButton.height + 4)
                Menu {
                    id: addMenu
                    MenuItem {
                        text: qsTrId("schedule.new_student")
                        onTriggered: Nav.newStudent()
                    }
                    MenuItem {
                        text: qsTrId("schedule.new_lesson")
                        onTriggered: Nav.editLesson("", overview.day)
                    }
                }
            }
        }

        // ------------------------------------------------------------ week
        GridLayout {
            Layout.fillWidth: true
            columns: 7
            columnSpacing: overview.columnMode ? 4 : 6
            Repeater {
                model: 7
                delegate: AbstractButton {
                    id: dayButton
                    required property int index
                    readonly property bool selected: overview.day === index
                    Layout.fillWidth: true
                    Layout.preferredWidth: 10
                    implicitHeight: overview.columnMode ? 48 : 60
                    focusPolicy: Qt.StrongFocus
                    Accessible.role: Accessible.Button
                    Accessible.name: Qt.locale().standaloneDayName((index + 1) % 7, Locale.LongFormat)
                    onClicked: overview.day = index
                    background: Rectangle {
                        radius: 14
                        color: dayButton.selected ? Theme.accent : Theme.surface
                        border.width: dayButton.visualFocus ? 2 : (index === overview.todayIndex && !dayButton.selected ? 1.5 : 0)
                        border.color: dayButton.visualFocus ? Theme.accentText : Theme.accent
                    }
                    contentItem: ColumnLayout {
                        spacing: 1
                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: {
                                if (LanguageManager.effectiveLanguage === "") return ""
                                return Qt.locale().standaloneDayName((dayButton.index + 1) % 7, Locale.ShortFormat).replace(".", "").slice(0, 2)
                            }
                            color: dayButton.selected ? Theme.textOnAccent : Theme.textMuted
                            font.family: Theme.fontFamily
                            font.pixelSize: 11
                            font.weight: Font.DemiBold
                        }
                        Text {
                            Layout.alignment: Qt.AlignHCenter
                            text: overview.dateOf(dayButton.index).getDate()
                            color: dayButton.selected ? Theme.textOnAccent : Theme.text
                            font.family: Theme.fontFamily
                            font.pixelSize: overview.columnMode ? 14 : 17
                            font.weight: Font.Bold
                        }
                        Rectangle {
                            visible: !overview.columnMode
                            Layout.alignment: Qt.AlignHCenter
                            implicitWidth: 5
                            implicitHeight: 5
                            radius: 3
                            color: overview.counts[dayButton.index] > 0 ? (dayButton.selected ? Theme.lime : Theme.accent)
                                                                        : "transparent"
                        }
                    }
                }
            }
        }

        // ------------------------------------------------------------ notices
        ColumnLayout {
            visible: overview.hasNotices
            Layout.fillWidth: true
            spacing: 8
            SectionLabel { text: qsTrId("today.notices") }
            Flickable {
                Layout.fillWidth: true
                Layout.preferredHeight: 64
                contentWidth: noticeRow.implicitWidth
                flickableDirection: Flickable.HorizontalFlick
                boundsBehavior: Flickable.StopAtBounds
                clip: true
                Row {
                    id: noticeRow
                    spacing: 10
                    Repeater {
                        model: overview.birthdays
                        delegate: NoticeCard {
                            required property var modelData
                            iconName: "cake"
                            tile: Theme.tile.orange
                            caption: modelData.days === 0 ? qsTrId("today.birthday")
                                                          : qsTrId("today.birthday_on").arg(Labels.weekday((new Date(modelData.date + "T12:00:00").getDay() + 6) % 7))
                            title: qsTrId("today.birthday_turns").arg(modelData.name).arg(modelData.turns)
                            onClicked: overview.studentClicked(modelData.id, "")
                        }
                    }
                    Repeater {
                        model: overview.startReminders
                        delegate: NoticeCard {
                            required property var modelData
                            iconName: "bell"
                            tile: Theme.tile.plum
                            caption: modelData.studentName !== "" ? qsTrId("today.reminder") + " · " + modelData.studentName
                                                                  : qsTrId("today.reminder")
                            title: modelData.text
                            onClicked: {
                                reminderDone.reminderId = modelData.id
                                reminderDone.text = modelData.text
                                reminderDone.open()
                            }
                        }
                    }
                    NoticeCard {
                        visible: overview.candidateCount > 0
                        iconName: "star"
                        tile: Theme.tile.blue
                        caption: qsTrId("candidates.recital.title")
                        title: qsTrId("today.overdue", overview.candidateCount)
                        onClicked: Nav.showPage(4)
                    }
                }
            }
        }

        // ------------------------------------------------------------ running lesson
        Rectangle {
            id: hero
            visible: overview.running !== null && !overview.columnMode
            Layout.fillWidth: true
            implicitHeight: heroColumn.implicitHeight + 32
            radius: Theme.radiusLarge
            color: Theme.hero

            ColumnLayout {
                id: heroColumn
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 16
                spacing: 12

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    Text {
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: overview.running ? overview.running.start_time + " – " + overview.running.end_time
                                                 + (overview.running.location ? " · " + overview.running.location : "") : ""
                        color: Theme.heroText
                        font.family: Theme.fontFamily
                        font.pixelSize: 16
                        font.weight: Font.Bold
                    }
                    Rectangle {
                        implicitWidth: runningLabel.implicitWidth + 20
                        implicitHeight: runningLabel.implicitHeight + 10
                        radius: height / 2
                        color: Theme.lime
                        Text {
                            id: runningLabel
                            anchors.centerIn: parent
                            text: overview.running ? qsTrId("today.running") + " · "
                                                     + qsTrId("today.remaining", overview.minutesLeft(overview.running)) : ""
                            color: Theme.textOnLime
                            font.family: Theme.fontFamily
                            font.pixelSize: 12
                            font.weight: Font.ExtraBold
                        }
                    }
                }
                Text {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: overview.running ? overview.running.name + " · " + overview.kindLabel(overview.running.kind) : ""
                    color: Theme.textOnAccent
                    opacity: 0.85
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                }

                Repeater {
                    model: overview.running ? overview.running.students.slice(0, 3) : []
                    delegate: StudentCard {
                        required property var modelData
                        Layout.fillWidth: true
                        student: modelData
                        onClicked: overview.studentClicked(modelData.id, modelData.membership_id)
                    }
                }
                Text {
                    visible: overview.running !== null && overview.running.students.length > 3
                    text: overview.running ? "+ " + qsTrId("summary.students", overview.running.students.length - 3) : ""
                    color: Theme.textOnAccent
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                    font.weight: Font.DemiBold
                }

                Repeater {
                    model: overview.running ? overview.running.reminders : []
                    delegate: RowLayout {
                        required property var modelData
                        Layout.fillWidth: true
                        spacing: 8
                        Icon {
                            Layout.preferredWidth: 18
                            Layout.preferredHeight: 18
                            name: "bell"
                            color: Theme.lime
                        }
                        Text {
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            text: modelData.text
                            color: Theme.textOnAccent
                            font.family: Theme.fontFamily
                            font.pixelSize: 14
                            font.weight: Font.DemiBold
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8
                    HeroButton {
                        Layout.fillWidth: true
                        lime: true
                        iconName: "plus"
                        text: qsTrId("common.note")
                        enabled: overview.running !== null && overview.running.students.length > 0
                        onClicked: Nav.editNote(overview.running.students[0].membership_id, "")
                    }
                    HeroButton {
                        Layout.fillWidth: true
                        iconName: "metronome"
                        text: qsTrId("metronome.title")
                        onClicked: Nav.showPage(3)
                    }
                }
            }
        }

        // ------------------------------------------------------------ lessons of the day
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 8

            SectionLabel {
                Layout.leftMargin: overview.columnMode ? 8 : 0
                text: overview.isToday && overview.running ? qsTrId("today.later") : qsTrId("schedule.day_lessons")
            }

            Rectangle {
                visible: overview.lessons.length === 0
                Layout.fillWidth: true
                implicitHeight: emptyText.implicitHeight + 40
                radius: Theme.radius + 2
                color: Theme.surface
                Text {
                    id: emptyText
                    anchors.centerIn: parent
                    width: parent.width - 32
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    text: overview.isToday ? qsTrId("today.no_lessons") : qsTrId("schedule.no_lessons_day")
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                    font.weight: Font.DemiBold
                }
            }

            // Phone: one card with rows
            Rectangle {
                visible: !overview.columnMode && overview.lessons.length > 0
                Layout.fillWidth: true
                implicitHeight: lessonRows.implicitHeight
                radius: Theme.radius + 2
                color: Theme.surface
                clip: true
                ColumnLayout {
                    id: lessonRows
                    width: parent.width
                    spacing: 0
                    Repeater {
                        model: overview.columnMode ? [] : overview.lessons.filter(l => !overview.running || l.id !== overview.running.id)
                        delegate: LessonRow {
                            required property var modelData
                            required property int index
                            Layout.fillWidth: true
                            lesson: modelData
                            past: overview.isToday && modelData.end_time <= overview.nowTime
                            divider: index > 0
                            onClicked: overview.openLesson(modelData)
                        }
                    }
                }
            }

            // Tablet/desktop column: card per lesson with its students
            Repeater {
                model: overview.columnMode ? overview.lessons : []
                delegate: LessonCard {
                    required property var modelData
                    Layout.fillWidth: true
                    lesson: modelData
                    running: overview.running !== null && overview.running.id === modelData.id
                    past: overview.isToday && modelData.end_time <= overview.nowTime
                    minutesLeft: running ? overview.minutesLeft(modelData) : 0
                    selectedMembership: overview.selectedMembership
                    onLessonClicked: overview.lessonClicked(modelData.id)
                    onStudentClicked: (studentId, membershipId) => overview.studentClicked(studentId, membershipId)
                }
            }

            ActionButton {
                visible: overview.irregular.length > 0
                kind: "quiet"
                text: qsTrId("today.show_irregular").arg(overview.irregular.length)
                onClicked: overview.showIrregular = !overview.showIrregular
            }
            Repeater {
                model: overview.showIrregular ? overview.irregular : []
                delegate: LessonCard {
                    required property var modelData
                    Layout.fillWidth: true
                    lesson: modelData
                    selectedMembership: overview.selectedMembership
                    onLessonClicked: overview.lessonClicked(modelData.id)
                    onStudentClicked: (studentId, membershipId) => overview.studentClicked(studentId, membershipId)
                }
            }
        }
    }

    AppDialog {
        id: reminderDone
        property string reminderId
        iconName: "bell"
        title: qsTrId("reminder.done_remove")
        rejectText: qsTrId("common.no")
        acceptText: qsTrId("common.yes")
        onAccepted: Data.remove("reminder", reminderId)
    }

    // ------------------------------------------------------------ pieces of this page

    component NoticeCard: AbstractButton {
        id: notice
        property string iconName
        property var tile
        property string caption
        property string title
        width: 230
        height: 64
        focusPolicy: Qt.StrongFocus
        Accessible.role: Accessible.Button
        Accessible.name: caption + " " + title
        background: Rectangle {
            radius: Theme.radius
            color: Theme.surface
            border.width: notice.visualFocus ? 2 : 0
            border.color: Theme.accentText
        }
        contentItem: RowLayout {
            spacing: 10
            Rectangle {
                implicitWidth: 40
                implicitHeight: 40
                radius: 12
                color: notice.tile.bg
                Icon {
                    anchors.centerIn: parent
                    width: 22
                    height: 22
                    name: notice.iconName
                    color: notice.tile.fg
                }
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 1
                Text {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: notice.caption
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 12
                    font.weight: Font.DemiBold
                }
                Text {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: notice.title
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 14
                    font.weight: Font.Bold
                }
            }
        }
        leftPadding: 12
        rightPadding: 12
    }

    component HeroButton: AbstractButton {
        id: heroButton
        property bool lime: false
        property string iconName
        implicitHeight: 46
        focusPolicy: Qt.StrongFocus
        Accessible.role: Accessible.Button
        Accessible.name: text
        background: Rectangle {
            radius: 14
            color: heroButton.lime ? Theme.lime : Theme.heroButton
            opacity: heroButton.enabled ? 1 : 0.5
            border.width: heroButton.visualFocus ? 2 : 0
            border.color: Theme.textOnAccent
        }
        contentItem: Item {
            RowLayout {
                anchors.centerIn: parent
                spacing: 6
                Icon {
                    Layout.preferredWidth: 18
                    Layout.preferredHeight: 18
                    name: heroButton.iconName
                    color: heroButton.lime ? Theme.textOnLime : Theme.heroText
                    strokeWidth: 2.2
                }
                Text {
                    text: heroButton.text
                    color: heroButton.lime ? Theme.textOnLime : Theme.heroText
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                    font.weight: heroButton.lime ? Font.ExtraBold : Font.Bold
                }
            }
        }
    }

    component StudentCard: AbstractButton {
        id: card
        property var student: ({})
        implicitHeight: cardRow.implicitHeight + 24
        focusPolicy: Qt.StrongFocus
        Accessible.role: Accessible.Button
        Accessible.name: student.name || ""
        background: Rectangle {
            radius: Theme.radius
            color: Theme.surface
            border.width: card.visualFocus ? 2 : 0
            border.color: Theme.lime
        }
        contentItem: RowLayout {
            id: cardRow
            spacing: 12
            Avatar {
                soft: true
                text: card.student.initials || ""
                size: 42
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                Text {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: card.student.name || ""
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 16
                    font.weight: Font.Bold
                }
                Text {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: {
                        const parts = []
                        const instrument = [card.student.instrument, card.student.instrument_size].filter(Boolean).join(" ")
                        if (instrument)
                            parts.push(instrument)
                        if (card.student.inProgress > 0)
                            parts.push(qsTrId("today.pieces_in_progress", card.student.inProgress))
                        if (card.student.ready > 0)
                            parts.push(qsTrId("today.pieces_ready", card.student.ready))
                        return parts.join(" · ")
                    }
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                }
                Text {
                    visible: !!card.student.lastNote
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: card.student.lastNote ? Data.formatDate(card.student.lastNoteDate) + ": " + card.student.lastNote : ""
                    color: Theme.textSecondary
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                }
            }
            Icon {
                Layout.preferredWidth: 20
                Layout.preferredHeight: 20
                name: "chevron"
                color: Theme.icon
            }
        }
        leftPadding: 12
        rightPadding: 12
    }

    component LessonRow: AbstractButton {
        id: row
        property var lesson: ({})
        property bool past: false
        property bool divider: false
        implicitHeight: rowLayout.implicitHeight + 28
        focusPolicy: Qt.StrongFocus
        hoverEnabled: true
        opacity: past ? 0.6 : 1
        Accessible.role: Accessible.Button
        Accessible.name: (lesson.start_time || "") + " " + (lesson.name || "")
        background: Rectangle {
            color: row.down ? Theme.surfaceAlt : row.hovered ? Theme.surfaceSunken : "transparent"
            border.width: row.visualFocus ? 2 : 0
            border.color: Theme.accentText
            Rectangle {
                visible: row.divider
                width: parent.width
                height: 1
                color: Theme.divider
            }
        }
        contentItem: RowLayout {
            id: rowLayout
            spacing: 12
            ColumnLayout {
                Layout.preferredWidth: 52
                spacing: 0
                Text {
                    text: row.lesson.start_time || ""
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                    font.weight: Font.Bold
                }
                Text {
                    text: row.lesson.end_time || ""
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                }
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                Text {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: row.lesson.name || ""
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                    font.weight: Font.Bold
                }
                Text {
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    text: row.lesson.kind === 3
                          ? [qsTrId("lesson.type.ensemble"), row.lesson.location, qsTrId("summary.students", row.lesson.studentCount)].filter(Boolean).join(" · ")
                          : row.lesson.studentNames
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                }
            }
            Icon {
                visible: row.lesson.reminderCount > 0
                Layout.preferredWidth: 18
                Layout.preferredHeight: 18
                name: "bell"
                color: Theme.accentText
                strokeWidth: 2
            }
            Icon {
                Layout.preferredWidth: 20
                Layout.preferredHeight: 20
                name: "chevron"
                color: Theme.icon
            }
        }
        leftPadding: 14
        rightPadding: 14
    }

    component LessonCard: Rectangle {
        id: lessonCard
        property var lesson: ({})
        property bool running: false
        property bool past: false
        property int minutesLeft: 0
        property string selectedMembership
        signal lessonClicked()
        signal studentClicked(string studentId, string membershipId)

        implicitHeight: cardColumn.implicitHeight
        radius: Theme.radius
        color: Theme.surface
        border.width: running ? 2 : 0
        border.color: Theme.lime
        opacity: past ? 0.62 : 1
        clip: true

        ColumnLayout {
            id: cardColumn
            width: parent.width
            spacing: 0
            AbstractButton {
                id: lessonHead
                Layout.fillWidth: true
                implicitHeight: 48
                focusPolicy: Qt.StrongFocus
                hoverEnabled: true
                Accessible.role: Accessible.Button
                Accessible.name: lessonCard.lesson.name || ""
                onClicked: lessonCard.lessonClicked()
                background: Rectangle {
                    color: lessonHead.hovered ? Theme.surfaceSunken : "transparent"
                    border.width: lessonHead.visualFocus ? 2 : 0
                    border.color: Theme.accentText
                }
                contentItem: RowLayout {
                    spacing: 10
                    Text {
                        visible: !lessonCard.lesson.irregular
                        Layout.preferredWidth: 96
                        text: (lessonCard.lesson.start_time || "") + " – " + (lessonCard.lesson.end_time || "")
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                        font.weight: Font.Bold
                    }
                    Text {
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: lessonCard.lesson.name || ""
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                        font.weight: Font.DemiBold
                    }
                    Rectangle {
                        visible: lessonCard.running
                        implicitWidth: leftLabel.implicitWidth + 16
                        implicitHeight: leftLabel.implicitHeight + 6
                        radius: height / 2
                        color: Theme.lime
                        Text {
                            id: leftLabel
                            anchors.centerIn: parent
                            text: qsTrId("today.remaining", lessonCard.minutesLeft)
                            color: Theme.textOnLime
                            font.family: Theme.fontFamily
                            font.pixelSize: 11
                            font.weight: Font.ExtraBold
                        }
                    }
                    Icon {
                        visible: lessonCard.lesson.reminderCount > 0
                        Layout.preferredWidth: 18
                        Layout.preferredHeight: 18
                        name: "bell"
                        color: Theme.accentText
                    }
                    Text {
                        visible: lessonCard.lesson.kind === 3
                        text: qsTrId("summary.students", lessonCard.lesson.studentCount || 0)
                        color: Theme.textMuted
                        font.family: Theme.fontFamily
                        font.pixelSize: 12
                    }
                }
                leftPadding: 12
                rightPadding: 12
            }
            Repeater {
                model: lessonCard.lesson.kind === 3 ? [] : (lessonCard.lesson.students || [])
                delegate: AbstractButton {
                    id: studentRow
                    required property var modelData
                    readonly property bool selected: lessonCard.selectedMembership === modelData.membership_id
                    Layout.fillWidth: true
                    implicitHeight: 40
                    focusPolicy: Qt.StrongFocus
                    hoverEnabled: true
                    Accessible.role: Accessible.Button
                    Accessible.name: modelData.name
                    onClicked: lessonCard.studentClicked(modelData.id, modelData.membership_id)
                    background: Rectangle {
                        color: studentRow.selected ? Theme.accent : studentRow.hovered ? Theme.surfaceSunken : "transparent"
                        border.width: studentRow.visualFocus ? 2 : 0
                        border.color: Theme.accentText
                        Rectangle {
                            width: parent.width
                            height: 1
                            color: Theme.divider
                            visible: !studentRow.selected
                        }
                    }
                    contentItem: Text {
                        text: studentRow.modelData.listName
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                        color: studentRow.selected ? Theme.textOnAccent : Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                        font.weight: studentRow.selected ? Font.Bold : Font.Normal
                    }
                    leftPadding: lessonCard.lesson.irregular ? 12 : 118
                    rightPadding: 12
                }
            }
        }
    }
}
