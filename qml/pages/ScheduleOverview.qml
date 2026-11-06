// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// The day: week strip, notices (what is still to come today, reminders "at start"),
// the running lesson and the lessons of the selected weekday.
// Used alone on phones and as the left column on tablets/desktop.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Item {
    id: overview
    objectName: "scheduleOverview"

    //! Tablet/desktop column: lessons as cards with their students
    property bool columnMode: false
    property int day: todayIndex
    property string selectedMembership: ""
    //! The lesson shown next to the column (tablet, desktop): its card head is marked
    property string selectedLesson: ""
    property date now: Data.now()

    signal studentClicked(string studentId, string membershipId)
    signal lessonClicked(string lessonId)

    readonly property int todayIndex: (now.getDay() + 6) % 7
    readonly property bool isToday: day === todayIndex
    readonly property string nowTime: Qt.formatTime(now, "hh:mm")
    readonly property var lessons: { if (Data.revision < 0) return undefined; return Data.lessonsOn(day) }
    readonly property var counts: { if (Data.revision < 0) return undefined; return Data.lessonCountPerWeekday() }
    readonly property var irregular: { if (Data.revision < 0) return undefined; return Data.irregularLessons() }
    //! Memberships with a plan for the day shown (a symbol at the lesson or student)
    readonly property var planned: { if (Data.revision < 0) return undefined; return Data.plannedOn(Qt.formatDate(dateOf(day), "yyyy-MM-dd")) }
    readonly property var running: {
        if (!isToday)
            return null
        for (const l of lessons)
            if (l.start_time <= nowTime && l.end_time > nowTime)
                return l
        return null
    }
    //! Column (tablet, desktop): the running or next lesson stands in the middle while the list
    //! can scroll that far; the lessons that are over move up out of sight. Near the end of the
    //! day the list stays scrolled to the bottom
    readonly property string focusLesson: {
        if (!columnMode || !isToday || !lessons)
            return ""
        const next = lessons.find(l => l.end_time > nowTime)
        return next ? next.id : ""
    }
    onFocusLessonChanged: focusTimer.restart()
    Component.onCompleted: focusTimer.restart()
    // Once the cards are laid out
    Timer {
        id: focusTimer
        interval: 60
        onTriggered: overview.scrollToFocus()
    }
    function scrollToFocus() {
        if (focusLesson === "" || scroller.dragging || scroller.flicking)
            return
        for (let i = 0; i < columnCards.count; ++i) {
            const card = columnCards.itemAt(i)
            if (!card || card.lesson.id !== focusLesson)
                continue
            const middle = card.mapToItem(scroller.contentItem, 0, card.height / 2).y - scroller.height / 2
            scroller.contentY = Math.max(0, Math.min(middle, scroller.contentHeight - scroller.height))
            return
        }
    }
    readonly property var startReminders: { if (Data.revision < 0) return undefined; return isToday ? Data.reminders().filter(r => r.mode === 0) : [] }
    //! Reminders for a student whose lesson is still to come today (they pop up when it begins),
    //! by the time of the lesson
    readonly property var lessonReminders: {
        if (Data.revision < 0) return undefined
        if (!isToday || !lessons) return []
        const out = []
        const seen = {}
        for (const l of lessons) {
            if (l.start_time <= nowTime)
                continue
            for (const r of l.reminders || []) {
                if (r.mode !== 2 || seen[r.id])
                    continue
                seen[r.id] = true
                out.push({ reminder: r, time: l.start_time })
            }
        }
        return out
    }
    // Only what has no other place in the day: birthdays show at the lesson and the student,
    // recital candidates are looked up when needed
    readonly property bool hasNotices: startReminders.length > 0 || todayBirthdays.length > 0 || lessonReminders.length > 0
    //! Birthdays of students who still come today: not in the running lesson (its band shows
    //! them), not in a lesson that is over. Today's birthdays first, then by the time they come
    readonly property var todayBirthdays: {
        if (Data.revision < 0) return undefined
        if (!isToday || !lessons) return []
        const seen = {}
        for (const st of running ? running.students || [] : [])
            seen[st.id] = true
        const out = []
        for (const l of lessons) {
            if (l.end_time <= nowTime || (running && running.id === l.id))
                continue
            for (const n of birthdaysOf(l.students)) {
                if (seen[n.id])
                    continue
                seen[n.id] = true
                n.time = l.irregular ? "" : (l.start_time || "")
                out.push(n)
            }
        }
        return out.sort((a, b) => ((a.kind === "today" ? 0 : 1) - (b.kind === "today" ? 0 : 1)) || a.time.localeCompare(b.time))
    }
    //! The notices as lines: birthdays today one each, those since the last lesson together
    //! (one alone with its day), then the reminders; at most 4 lines until opened
    property bool noticesOpen: false
    onDayChanged: noticesOpen = false
    readonly property var noticeRows: {
        const rows = []
        const past = todayBirthdays.filter(n => n.kind !== "today")
        for (const n of todayBirthdays.filter(n => n.kind === "today"))
            rows.push({ kind: "today", note: n })
        if (past.length === 1)
            rows.push({ kind: "past", note: past[0] })
        else if (past.length > 1)
            rows.push({ kind: "several", notes: past })
        for (const lr of lessonReminders)
            rows.push({ kind: "lessonReminder", reminder: lr.reminder, time: lr.time })
        for (const r of startReminders)
            rows.push({ kind: "reminder", reminder: r })
        if (!noticesOpen && rows.length > 4) {
            const shown = rows.slice(0, 3)
            shown.push({ kind: "more", count: rows.length - 3 })
            return shown
        }
        return rows
    }
    //! Birthdays today or since the last lesson among these students (Data.birthdayNote), today first
    function birthdaysOf(students) {
        const out = []
        for (const st of students || []) {
            const n = Data.birthdayNote(st.id)
            if (n.kind === "today" || n.kind === "past") {
                n.id = st.id
                out.push(n)
            }
        }
        return out.sort((a, b) => a.days - b.days)
    }
    readonly property var runningBirthdays: { if (Data.revision < 0) return undefined; return running ? birthdaysOf(running.students) : [] }
    //! What counts now for a student of the running lesson: today's plan, otherwise the
    //! last note. key: the same for copies saved for everybody of a group
    function focusOf(student) {
        const plan = Data.plans(student.membership_id).find(p => p.today)
        if (plan)
            return { kind: "plan", key: plan.group_key || plan.id, points: plan.points }
        if (student.lastNote)
            return { kind: "note", key: student.lastNoteDate + "\n" + student.lastNote, date: student.lastNoteDate, text: student.lastNote }
        return null
    }
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
        onTriggered: overview.now = Data.now()
    }

    // Header and week strip stay put (tablet/desktop: the notices too); the rest scrolls
    ColumnLayout {
        id: fixedTop
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
                AppMenu {
                    id: addMenu
                    AppMenuItem {
                        iconName: "userPlus"
                        text: qsTrId("schedule.new_student")
                        onTriggered: Nav.newStudent()
                    }
                    AppMenuItem {
                        iconName: "calendar"
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
                    implicitHeight: overview.columnMode ? 54 : 60
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
                    // Weekday, date and a dot when there are lessons: kept together in the
                    // middle (also in the narrow column next to a student)
                    contentItem: Item {
                        Column {
                            anchors.centerIn: parent
                            spacing: 0
                            Text {
                                anchors.horizontalCenter: parent.horizontalCenter
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
                                anchors.horizontalCenter: parent.horizontalCenter
                                text: overview.dateOf(dayButton.index).getDate()
                                color: dayButton.selected ? Theme.textOnAccent : Theme.text
                                font.family: Theme.fontFamily
                                font.pixelSize: overview.columnMode ? 15 : 17
                                font.weight: Font.Bold
                            }
                            Item { width: 1; height: 3 }
                            Rectangle {
                                anchors.horizontalCenter: parent.horizontalCenter
                                width: 6
                                height: 6
                                radius: 3
                                color: overview.counts[dayButton.index] > 0 ? (dayButton.selected ? Theme.lime : Theme.accent)
                                                                            : "transparent"
                            }
                        }
                    }
                }
            }
        }


        Loader {
            active: overview.columnMode
            visible: active && overview.hasNotices
            Layout.fillWidth: true
            sourceComponent: noticesComponent
        }
    }

    Flickable {
        id: scroller
        objectName: "scheduleScroller"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: fixedTop.bottom
        anchors.topMargin: 8
        anchors.bottom: parent.bottom
        clip: true
        contentHeight: column.implicitHeight + 50
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}

        ColumnLayout {
            id: column
            x: fixedTop.x
            y: 10
            width: fixedTop.width
            spacing: 18

            // Phone: the notices scroll with the lessons (the screen is small)
            Loader {
                active: !overview.columnMode
                visible: active && overview.hasNotices
                Layout.fillWidth: true
                sourceComponent: noticesComponent
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

                    // Time, room and lesson: opens the lesson (times, students, reminders)
                    AbstractButton {
                        id: heroHead
                        Layout.fillWidth: true
                        implicitHeight: heroHeadColumn.implicitHeight
                        focusPolicy: Qt.StrongFocus
                        Accessible.role: Accessible.Button
                        Accessible.name: overview.running ? overview.running.name : ""
                        onClicked: if (overview.running) overview.lessonClicked(overview.running.id)
                        background: Rectangle {
                            color: "transparent"
                            radius: 8
                            border.width: heroHead.visualFocus ? 2 : 0
                            border.color: Theme.lime
                        }
                        contentItem: ColumnLayout {
                            id: heroHeadColumn
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
                            RowLayout {
                                Layout.fillWidth: true
                                spacing: 4
                                Text {
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                    text: overview.running ? overview.running.name + " · " + overview.kindLabel(overview.running.kind) : ""
                                    color: Theme.textOnAccent
                                    opacity: 0.85
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 13
                                }
                                Icon {
                                    Layout.preferredWidth: 16
                                    Layout.preferredHeight: 16
                                    name: "chevron"
                                    color: Theme.heroMuted
                                }
                            }
                        }
                    }

                    // Birthdays of this lesson: one lime band, the tick puts them away
                    Rectangle {
                        id: heroBirthdays
                        readonly property var list: overview.runningBirthdays || []
                        readonly property var first: list.length > 0 ? list[0] : ({})
                        visible: list.length > 0
                        Layout.fillWidth: true
                        implicitHeight: heroBirthdayRow.implicitHeight + 18
                        radius: 14
                        color: Theme.lime
                        RowLayout {
                            id: heroBirthdayRow
                            anchors.fill: parent
                            anchors.leftMargin: 14
                            anchors.rightMargin: 6
                            spacing: 10
                            Icon {
                                Layout.preferredWidth: 22
                                Layout.preferredHeight: 22
                                name: "cake"
                                color: Theme.textOnLime
                                strokeWidth: 2.2
                            }
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 0
                                Text {
                                    Layout.fillWidth: true
                                    elide: Text.ElideRight
                                    text: heroBirthdays.list.length > 1 ? qsTrId("birthday.several")
                                          : heroBirthdays.first.kind === "today" ? qsTrId("birthday.today") : qsTrId("birthday.past")
                                    color: Theme.textOnLime
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 15
                                    font.weight: Font.ExtraBold
                                }
                                Text {
                                    Layout.fillWidth: true
                                    wrapMode: Text.WordWrap
                                    text: {
                                        const l = heroBirthdays.list
                                        if (l.length === 0)
                                            return ""
                                        if (l.length === 1)
                                            return l[0].kind === "today" ? qsTrId("today.birthday_turns").arg(l[0].forename).arg(l[0].turns)
                                                                         : qsTrId("birthday.past_short").arg(l[0].forename).arg(l[0].when).arg(l[0].turns)
                                        const named = n => n.forename + " (" + n.turns + ")"
                                        const today = l.filter(n => n.kind === "today").map(named)
                                        const past = l.filter(n => n.kind !== "today").map(named)
                                        return [today.length ? qsTrId("birthday.several_today").arg(today.join(", ")) : "",
                                                past.length ? qsTrId("birthday.several_past").arg(past.join(", ")) : ""].filter(Boolean).join(" · ")
                                    }
                                    color: Theme.textOnLime
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 13
                                    font.weight: Font.DemiBold
                                }
                            }
                            IconButton {
                                iconName: "check"
                                text: qsTrId("birthday.greeted")
                                size: 36
                                fill: Theme.textOnLime
                                tint: Theme.lime
                                onClicked: {
                                    for (const n of heroBirthdays.list)
                                        Data.greetBirthday(n.id)
                                }
                            }
                        }
                    }

                    // Group or ensemble: the students as small chips; what they share once,
                    // otherwise one line each
                    Rectangle {
                        id: heroGroup
                        readonly property var students: overview.running ? overview.running.students : []
                        readonly property var foci: { if (Data.revision < 0) return undefined; return students.map(st => overview.focusOf(st)) }
                        readonly property bool shared: foci.length > 1 && !!foci[0]
                                                       && foci.every(f => !!f && f.kind === foci[0].kind && f.key === foci[0].key)
                        visible: students.length > 1
                        Layout.fillWidth: true
                        implicitHeight: heroGroupColumn.implicitHeight + 24
                        radius: Theme.radius
                        color: Theme.surface
                        ColumnLayout {
                            id: heroGroupColumn
                            x: 12
                            y: 12
                            width: parent.width - 24
                            spacing: 10
                            Flow {
                                Layout.fillWidth: true
                                spacing: 6
                                Repeater {
                                    model: heroGroup.students
                                    delegate: AbstractButton {
                                        id: chip
                                        required property var modelData
                                        readonly property var birthday: { if (Data.revision < 0) return undefined; return Data.birthdayNote(modelData.id) }
                                        implicitWidth: chipRow.implicitWidth + 18
                                        implicitHeight: 38
                                        focusPolicy: Qt.StrongFocus
                                        Accessible.role: Accessible.Button
                                        Accessible.name: modelData.name
                                        onClicked: overview.studentClicked(modelData.id, modelData.membership_id)
                                        background: Rectangle {
                                            radius: 19
                                            color: chip.down ? Theme.surfaceAlt : Theme.surfaceSunken
                                            border.width: chip.visualFocus ? 2 : 0
                                            border.color: Theme.accentText
                                        }
                                        contentItem: Item {
                                            Row {
                                                id: chipRow
                                                x: 5
                                                anchors.verticalCenter: parent.verticalCenter
                                                spacing: 7
                                                Avatar {
                                                    anchors.verticalCenter: parent.verticalCenter
                                                    soft: true
                                                    size: 28
                                                    text: chip.modelData.initials
                                                    birthday: chip.birthday
                                                }
                                                Text {
                                                    anchors.verticalCenter: parent.verticalCenter
                                                    text: chip.modelData.forename
                                                    color: Theme.text
                                                    font.family: Theme.fontFamily
                                                    font.pixelSize: 14
                                                    font.weight: Font.DemiBold
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            // Shared: today's plan or the last note, once "for everybody"
                            ColumnLayout {
                                visible: heroGroup.shared
                                Layout.fillWidth: true
                                spacing: 3
                                RowLayout {
                                    visible: heroGroup.shared && heroGroup.foci[0].kind === "plan"
                                    spacing: 6
                                    Icon {
                                        Layout.preferredWidth: 16
                                        Layout.preferredHeight: 16
                                        name: "plan"
                                        color: Theme.accentText
                                        strokeWidth: 2
                                    }
                                    Text {
                                        text: qsTrId("plan.planned_today_all")
                                        color: Theme.accentText
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 13
                                        font.weight: Font.ExtraBold
                                    }
                                }
                                Repeater {
                                    model: heroGroup.shared && heroGroup.foci[0].kind === "plan" ? heroGroup.foci[0].points.slice(0, 3) : []
                                    delegate: Text {
                                        required property string modelData
                                        Layout.fillWidth: true
                                        elide: Text.ElideRight
                                        text: "•  " + modelData
                                        color: Theme.text
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 14
                                    }
                                }
                                Text {
                                    visible: heroGroup.shared && heroGroup.foci[0].kind === "plan" && heroGroup.foci[0].points.length > 3
                                    text: visible ? qsTrId("plan.more_points", heroGroup.foci[0].points.length - 3) : ""
                                    color: Theme.textMuted
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 13
                                }
                                Text {
                                    visible: heroGroup.shared && heroGroup.foci[0].kind === "note"
                                    Layout.fillWidth: true
                                    wrapMode: Text.WordWrap
                                    maximumLineCount: 2
                                    elide: Text.ElideRight
                                    text: visible ? qsTrId("lesson.for_all").arg(Data.formatDate(heroGroup.foci[0].date)).arg(heroGroup.foci[0].text) : ""
                                    color: Theme.textSecondary
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 13
                                }
                            }
                            // Different: one line each (plan or last note)
                            Repeater {
                                model: heroGroup.shared ? [] : heroGroup.students.slice(0, 4)
                                delegate: RowLayout {
                                    id: focusRow
                                    required property var modelData
                                    required property int index
                                    readonly property var studentFocus: heroGroup.foci[index]
                                    visible: !!studentFocus
                                    Layout.fillWidth: true
                                    spacing: 6
                                    Icon {
                                        visible: !!focusRow.studentFocus && focusRow.studentFocus.kind === "plan"
                                        Layout.preferredWidth: 15
                                        Layout.preferredHeight: 15
                                        name: "plan"
                                        color: Theme.accentText
                                        strokeWidth: 2
                                    }
                                    Text {
                                        Layout.fillWidth: true
                                        elide: Text.ElideRight
                                        text: !focusRow.studentFocus ? ""
                                              : focusRow.modelData.forename + ": "
                                                + (focusRow.studentFocus.kind === "plan" ? focusRow.studentFocus.points.join(" · ") : focusRow.studentFocus.text)
                                        color: Theme.textSecondary
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 13
                                    }
                                }
                            }
                        }
                    }

                    // A single student: the card, then the plan (or, without one, the last note in the card)
                    Repeater {
                        model: overview.running && overview.running.students.length === 1 ? overview.running.students : []
                        delegate: ColumnLayout {
                            id: heroStudent
                            required property var modelData
                            // What is planned for this lesson
                            readonly property var plan: {
                                if (Data.revision < 0)
                                    return undefined
                                return Data.plans(modelData.membership_id).find(p => p.today)
                            }
                            Layout.fillWidth: true
                            spacing: 8
                            StudentCard {
                                Layout.fillWidth: true
                                student: heroStudent.modelData
                                // The plan is what counts now; without one, the last note
                                showLastNote: heroStudent.plan === undefined
                                onClicked: overview.studentClicked(heroStudent.modelData.id, heroStudent.modelData.membership_id)
                            }
                            AbstractButton {
                                id: heroPlan
                                visible: heroStudent.plan !== undefined
                                Layout.fillWidth: true
                                implicitHeight: heroPlanColumn.implicitHeight + 24
                                focusPolicy: Qt.StrongFocus
                                Accessible.role: Accessible.Button
                                Accessible.name: qsTrId("plan.planned_today") + ": "
                                                 + (heroStudent.plan ? heroStudent.plan.points.join(", ") : "")
                                onClicked: overview.studentClicked(heroStudent.modelData.id, heroStudent.modelData.membership_id)
                                background: Rectangle {
                                    radius: 14
                                    color: Qt.rgba(1, 1, 1, heroPlan.down ? 0.16 : 0.10)
                                    border.width: heroPlan.visualFocus ? 2 : 1
                                    border.color: heroPlan.visualFocus ? Theme.lime : Qt.rgba(1, 1, 1, 0.22)
                                }
                                contentItem: ColumnLayout {
                                    id: heroPlanColumn
                                    spacing: 6
                                    RowLayout {
                                        spacing: 6
                                        Icon {
                                            Layout.preferredWidth: 17
                                            Layout.preferredHeight: 17
                                            name: "plan"
                                            color: Theme.lime
                                            strokeWidth: 2
                                        }
                                        Text {
                                            text: qsTrId("plan.planned_today")
                                            color: Theme.heroText
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 13
                                            font.weight: Font.ExtraBold
                                        }
                                    }
                                    // The first points; a long plan does not push the lesson off the screen
                                    Repeater {
                                        model: heroStudent.plan ? heroStudent.plan.points.slice(0, 3) : []
                                        delegate: Text {
                                            required property string modelData
                                            Layout.fillWidth: true
                                            wrapMode: Text.Wrap
                                            maximumLineCount: 3
                                            elide: Text.ElideRight
                                            text: "•  " + modelData
                                            color: Theme.heroText
                                            opacity: 0.92
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 14
                                        }
                                    }
                                    Text {
                                        visible: heroStudent.plan !== undefined && heroStudent.plan.points.length > 3
                                        text: heroStudent.plan ? qsTrId("plan.more_points", heroStudent.plan.points.length - 3) : ""
                                        color: Theme.heroText
                                        opacity: 0.75
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 13
                                        font.weight: Font.DemiBold
                                    }
                                }
                                leftPadding: 14
                                rightPadding: 14
                                topPadding: 12
                                bottomPadding: 12
                            }
                        }
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
                    // "Later today" only under the running lesson of the phone, and only when no lesson
                    // of the list was earlier (a class in the morning); the column lists the whole day
                    text: overview.isToday && overview.running && !overview.columnMode
                          && !overview.lessons.some(l => l.start_time < overview.running.start_time)
                          ? qsTrId("today.later") : qsTrId("schedule.day_lessons")
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
                                planned: overview.planned
                                past: overview.isToday && modelData.end_time <= overview.nowTime
                                divider: index > 0
                                onClicked: overview.openLesson(modelData)
                                // A lesson with one student opens the student; holding opens the lesson
                                onPressAndHold: overview.lessonClicked(modelData.id)
                            }
                        }
                    }
                }

                // Tablet/desktop column: card per lesson with its students
                Repeater {
                    id: columnCards
                    model: overview.columnMode ? overview.lessons : []
                    delegate: LessonCard {
                        required property var modelData
                        Layout.fillWidth: true
                        lesson: modelData
                        running: overview.running !== null && overview.running.id === modelData.id
                        past: overview.isToday && modelData.end_time <= overview.nowTime
                        minutesLeft: running ? overview.minutesLeft(modelData) : 0
                        selectedMembership: overview.selectedMembership
                        selectedLesson: overview.selectedLesson
                        planned: overview.planned
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
                        selectedLesson: overview.selectedLesson
                        planned: overview.planned
                        onLessonClicked: overview.lessonClicked(modelData.id)
                        onStudentClicked: (studentId, membershipId) => overview.studentClicked(studentId, membershipId)
                    }
                }
            }
        }
    }

    // A line under the fixed part once the lessons scroll beneath it
    Rectangle {
        visible: scroller.contentY > 2
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: scroller.top
        height: 1
        color: Theme.divider
    }

    // ------------------------------------------------------------ notices
    // (today only: birthdays and reminders of students who still come, reminders "at start")
    Component {
        id: noticesComponent
        ColumnLayout {
            spacing: 8
            SectionLabel { text: qsTrId("today.notices") }
            // One card, a line each: nothing hides behind a swipe
            Rectangle {
                Layout.fillWidth: true
                implicitHeight: noticeList.implicitHeight + 8
                radius: Theme.radius
                color: Theme.surface
                Column {
                    id: noticeList
                    y: 4
                    width: parent.width
                    Repeater {
                        model: overview.noticeRows
                        delegate: NoticeLine {
                            required property var modelData
                            required property int index
                            readonly property var n: modelData.note || ({})
                            width: noticeList.width
                            divider: index > 0
                            iconName: modelData.kind === "reminder" || modelData.kind === "lessonReminder" ? "bell"
                                    : modelData.kind === "more" ? "chevronDown" : "cake"
                            lime: modelData.kind === "today"
                            link: modelData.kind === "more"
                            text: modelData.kind === "lessonReminder" ? (modelData.reminder.forename || modelData.reminder.studentName)
                                                                         + ": " + modelData.reminder.text
                                  : modelData.kind === "reminder" ? (modelData.reminder.studentName !== ""
                                                                   ? modelData.reminder.studentName + ": " + modelData.reminder.text
                                                                   : modelData.reminder.text)
                                  : modelData.kind === "more" ? qsTrId("notice.more", modelData.count)
                                  : modelData.kind === "several" ? qsTrId("notice.birthday_several").arg(modelData.notes.map(x => x.forename).join(", "))
                                  : modelData.kind === "today" ? qsTrId("notice.birthday_today").arg(n.forename).arg(n.turns)
                                  : qsTrId("notice.birthday_one").arg(n.forename).arg(n.when).arg(n.turns)
                            trailing: modelData.kind === "today" || modelData.kind === "past" ? (n.time || "")
                                      : modelData.kind === "lessonReminder" ? modelData.time : ""
                            onClicked: {
                                if (modelData.kind === "more") {
                                    overview.noticesOpen = true
                                } else if (modelData.kind === "several") {
                                    // All of them on the birthday list, pointed out there
                                    Nav.birthdayFocus = modelData.notes.map(x => x.id)
                                    Nav.showMore("birthdays")
                                } else if (modelData.kind === "lessonReminder") {
                                    // At the student it stands above the tabs (edit it, tick it off)
                                    overview.studentClicked(modelData.reminder.student_id, "")
                                } else if (modelData.kind === "reminder") {
                                    reminderDone.reminderId = modelData.reminder.id
                                    reminderDone.text = modelData.reminder.text
                                    reminderDone.open()
                                } else {
                                    overview.studentClicked(n.id, "")
                                }
                            }
                        }
                    }
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

    //! A notice: one line with its symbol, a time on the right
    component NoticeLine: AbstractButton {
        id: line
        property string iconName
        property bool lime: false
        //! "+ N more": in the accent colour
        property bool link: false
        property bool divider: false
        property string trailing
        implicitHeight: 46
        focusPolicy: Qt.StrongFocus
        hoverEnabled: true
        Accessible.role: Accessible.Button
        Accessible.name: text + (trailing !== "" ? " " + trailing : "")
        background: Item {
            Rectangle {
                visible: line.divider
                x: 56
                width: parent.width - x - 12
                height: 1
                color: Theme.divider
            }
            Rectangle {
                anchors.fill: parent
                anchors.leftMargin: 4
                anchors.rightMargin: 4
                anchors.topMargin: 1
                anchors.bottomMargin: 1
                radius: Theme.radius - 4
                color: line.down ? Theme.surfaceAlt : line.hovered ? Theme.surfaceSunken : "transparent"
                border.width: line.visualFocus ? 2 : 0
                border.color: Theme.accentText
            }
        }
        contentItem: RowLayout {
            spacing: 10
            Rectangle {
                implicitWidth: 30
                implicitHeight: 30
                radius: 9
                color: line.lime ? Theme.lime : Theme.tile.plum.bg
                Icon {
                    anchors.centerIn: parent
                    width: 18
                    height: 18
                    name: line.iconName
                    color: line.lime ? Theme.textOnLime : Theme.tile.plum.fg
                }
            }
            Text {
                Layout.fillWidth: true
                elide: Text.ElideRight
                text: line.text
                color: line.link ? Theme.accentText : Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 14
                font.weight: line.lime || line.link ? Font.Bold : Font.DemiBold
            }
            Text {
                visible: line.trailing !== ""
                text: line.trailing
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 13
                font.weight: Font.DemiBold
            }
        }
        leftPadding: 14
        rightPadding: 14
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
        property bool showLastNote: true
        implicitHeight: implicitContentHeight + topPadding + bottomPadding
        topPadding: 12
        bottomPadding: 12
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
                    visible: card.showLastNote && !!card.student.lastNote
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
        property var planned: []
        implicitHeight: implicitContentHeight + topPadding + bottomPadding
        topPadding: 14
        bottomPadding: 14
        focusPolicy: Qt.StrongFocus
        hoverEnabled: true
        opacity: past ? 0.6 : 1
        Accessible.role: Accessible.Button
        Accessible.name: (lesson.start_time || "") + " " + (lesson.name || "")
        background: Item {
            Rectangle {
                visible: row.divider
                width: parent.width
                height: 1
                color: Theme.divider
            }
            // Inset and rounded like the card around it (clip cuts square corners)
            Rectangle {
                anchors.fill: parent
                anchors.margins: 4
                radius: Theme.radius - 2
                color: row.down ? Theme.surfaceAlt : row.hovered ? Theme.surfaceSunken : "transparent"
                border.width: row.visualFocus ? 2 : 0
                border.color: Theme.accentText
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
            // One sign per row: birthday, else reminder, else plan
            BirthdayTag {
                id: rowBirthday
                readonly property var list: { if (Data.revision < 0) return undefined; return overview.birthdaysOf(row.lesson.students) }
                note: list.length > 0 ? list[0] : ({})
                count: list.length
            }
            Icon {
                visible: !rowBirthday.visible && row.lesson.reminderCount === 0
                         && (row.lesson.students || []).some(s => row.planned.indexOf(s.membership_id) >= 0)
                Layout.preferredWidth: 18
                Layout.preferredHeight: 18
                name: "plan"
                color: Theme.accentText
                strokeWidth: 2
                Accessible.role: Accessible.Graphic
                Accessible.name: qsTrId("plan.planned")
            }
            Icon {
                visible: !rowBirthday.visible && row.lesson.reminderCount > 0
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
        property string selectedLesson
        property var planned: []
        readonly property bool lessonSelected: selectedLesson !== "" && selectedLesson === lesson.id
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
                background: Item {
                    // Selected like a student row: accent colour, inset, rounded at the top
                    // with the card and square towards the students below
                    CardRowHighlight {
                        anchors.fill: parent
                        anchors.margins: 4
                        anchors.bottomMargin: 2
                        roundBottom: studentRows.count === 0
                        color: lessonCard.lessonSelected ? Theme.accent
                               : lessonHead.down ? Theme.surfaceAlt : lessonHead.hovered ? Theme.surfaceSunken : "transparent"
                        focusFrame: lessonHead.visualFocus
                        frameColor: lessonCard.lessonSelected ? Theme.textOnAccent : Theme.accentText
                    }
                }
                contentItem: RowLayout {
                    spacing: 10
                    Text {
                        visible: !lessonCard.lesson.irregular
                        Layout.preferredWidth: 96
                        text: (lessonCard.lesson.start_time || "") + " – " + (lessonCard.lesson.end_time || "")
                        color: lessonCard.lessonSelected ? Theme.textOnAccent : Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                        font.weight: Font.Bold
                    }
                    Text {
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: lessonCard.lesson.name || ""
                        color: lessonCard.lessonSelected ? Theme.textOnAccent : Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                        font.weight: lessonCard.lessonSelected ? Font.Bold : Font.DemiBold
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
                        color: lessonCard.lessonSelected ? Theme.textOnAccent : Theme.accentText
                    }
                    // Ensembles list no students here: the cake with how many, else how many students
                    BirthdayTag {
                        id: headBirthday
                        readonly property var list: { if (Data.revision < 0) return undefined; return lessonCard.lesson.kind === 3 ? overview.birthdaysOf(lessonCard.lesson.students) : [] }
                        note: list.length > 0 ? list[0] : ({})
                        count: list.length
                    }
                    Text {
                        visible: lessonCard.lesson.kind === 3 && !headBirthday.visible
                        text: qsTrId("summary.students", lessonCard.lesson.studentCount || 0)
                        color: lessonCard.lessonSelected ? Theme.textOnAccent : Theme.textMuted
                        font.family: Theme.fontFamily
                        font.pixelSize: 12
                    }
                }
                leftPadding: 12
                rightPadding: 12
            }
            Repeater {
                id: studentRows
                model: lessonCard.lesson.kind === 3 ? [] : (lessonCard.lesson.students || [])
                delegate: AbstractButton {
                    id: studentRow
                    required property var modelData
                    required property int index
                    readonly property bool selected: lessonCard.selectedMembership === modelData.membership_id
                    readonly property bool last: index === studentRows.count - 1
                    Layout.fillWidth: true
                    implicitHeight: last ? 42 : 40
                    focusPolicy: Qt.StrongFocus
                    hoverEnabled: true
                    Accessible.role: Accessible.Button
                    Accessible.name: modelData.name
                    onClicked: lessonCard.studentClicked(modelData.id, modelData.membership_id)
                    background: Item {
                        // Inside the card's frame: across it, it cut the lime frame of the running lesson
                        Rectangle {
                            x: lessonCard.border.width
                            width: parent.width - 2 * lessonCard.border.width
                            height: 1
                            color: Theme.divider
                            visible: !studentRow.selected && !(studentRow.index === 0 && lessonCard.lessonSelected)
                        }
                        // Inset; rounded only where it meets the card's corners (the last row
                        // at the bottom), square towards the head and the other rows
                        CardRowHighlight {
                            anchors.fill: parent
                            anchors.leftMargin: 4
                            anchors.rightMargin: 4
                            anchors.topMargin: 2
                            anchors.bottomMargin: studentRow.last ? 4 : 2
                            roundTop: false
                            roundBottom: studentRow.last
                            color: studentRow.selected ? Theme.accent
                                   : studentRow.down ? Theme.surfaceAlt
                                   : studentRow.hovered ? Theme.surfaceSunken : "transparent"
                            focusFrame: studentRow.visualFocus
                            frameColor: studentRow.selected ? Theme.textOnAccent : Theme.accentText
                        }
                    }
                    contentItem: RowLayout {
                        spacing: 8
                        Text {
                            Layout.fillWidth: true
                            text: studentRow.modelData.listName
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                            color: studentRow.selected ? Theme.textOnAccent : Theme.text
                            font.family: Theme.fontFamily
                            font.pixelSize: 14
                            font.weight: studentRow.selected ? Font.Bold : Font.Normal
                        }
                        BirthdayTag {
                            id: rowBirthday
                            note: { if (Data.revision < 0) return ({}); return Data.birthdayNote(studentRow.modelData.id) }
                        }
                        Icon {
                            visible: !rowBirthday.visible && lessonCard.planned.indexOf(studentRow.modelData.membership_id) >= 0
                            Layout.preferredWidth: 17
                            Layout.preferredHeight: 17
                            name: "plan"
                            color: studentRow.selected ? Theme.textOnAccent : Theme.accentText
                            strokeWidth: 2
                            Accessible.role: Accessible.Graphic
                            Accessible.name: qsTrId("plan.planned")
                        }
                    }
                    leftPadding: lessonCard.lesson.irregular ? 12 : 118
                    rightPadding: 12
                }
            }
        }
    }
}
