// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// The booklet of one teacher: next lesson, tasks with feedback, a question, pieces and dates.
// Several teachers: tabs at the top.

import QtQuick
import QtQuick.Controls
import QupilStudent

Page {
    id: page
    background: Rectangle { color: Theme.bg }

    readonly property var b: StudentApp.current
    readonly property var allow: b.allow ?? ({})
    readonly property bool allowFeedback: allow.feedback ?? true
    readonly property bool allowQuestions: allow.questions ?? true
    readonly property var nextLesson: (b.next ?? []).length > 0 ? b.next[0] : null
    // Without a name set in the teacher's Qupil: "your teacher"
    readonly property bool teacherNamed: (b.teacher ?? "") !== ""
    readonly property string teacherName: teacherNamed ? b.teacher : qsTrId("s.teacher_fallback")
    property var openTasks: ({})       // task id -> hints shown

    // ---- top bar
    Rectangle {
        id: bar
        z: 2
        width: parent.width
        height: window.topInset + 56 + (tabs.visible ? tabs.height + 8 : 0)
        color: Theme.bg

        T {
            x: Theme.gutter
            y: window.topInset + 12
            text: qsTrId("s.app.section")
            font.pixelSize: 26
            font.weight: Font.ExtraBold
        }
        Row {
            anchors.right: parent.right
            anchors.rightMargin: 8
            y: window.topInset + 6
            IconButton {
                objectName: "refreshButton"
                icon: "refresh"
                label: qsTrId("s.home.refresh")
                visible: !page.b.demo
                RotationAnimator on rotation { running: StudentApp.busy; from: 0; to: 360; duration: 900; loops: Animation.Infinite }
                onClicked: StudentApp.refresh()
            }
            IconButton {
                objectName: "settingsButton"
                icon: "settings"
                label: qsTrId("s.settings.title")
                onClicked: window.openSettings()
            }
        }

        // Several teachers (violin at the music school, piano privately)
        Rectangle {
            id: tabs
            visible: StudentApp.teachers.length > 1
            x: Theme.gutter
            y: window.topInset + 58
            width: parent.width - 2 * Theme.gutter
            height: 40
            radius: 12
            color: Theme.segment
            Row {
                anchors.fill: parent
                anchors.margins: 3
                Repeater {
                    model: StudentApp.teachers
                    Rectangle {
                        required property var modelData
                        required property int index
                        width: (tabs.width - 6) / StudentApp.teachers.length
                        height: 34
                        radius: 10
                        color: index === StudentApp.currentIndex ? Theme.segmentActive : "transparent"
                        T {
                            anchors.centerIn: parent
                            width: parent.width - 12
                            horizontalAlignment: Text.AlignHCenter
                            elide: Text.ElideRight
                            wrapMode: Text.NoWrap
                            font.pixelSize: 14
                            font.weight: Font.Bold
                            color: index === StudentApp.currentIndex ? Theme.text : Theme.textMuted
                            // Not loaded yet (no connection at the first start): still a name to tap
                            text: modelData.instrument !== "" ? modelData.instrument
                                : modelData.teacher !== "" ? modelData.teacher
                                : qsTrId("s.settings.unknown_teacher") + " " + (index + 1)
                        }
                        MouseArea { anchors.fill: parent; onClicked: StudentApp.currentIndex = index }
                    }
                }
            }
        }
    }

    PageFlickable {
        id: flick
        anchors.fill: parent
        anchors.topMargin: bar.height
        revealBelow: 40
        contentHeight: column.implicitHeight + window.bottomInset + 40
        boundsBehavior: Flickable.DragOverBounds
        onDragEnded: if (contentY < -70 && !page.b.demo) StudentApp.refresh()

        Column {
            id: column
            x: Theme.gutter
            y: 4
            width: parent.width - 2 * Theme.gutter
            spacing: 12

            // ---- example: invented data
            Rectangle {
                visible: page.b.demo ?? false
                width: parent.width
                height: demoRow.implicitHeight + 20
                radius: Theme.radius
                color: Theme.warningSoft
                Row {
                    id: demoRow
                    x: 14; y: 10
                    width: parent.width - 28
                    spacing: 10
                    T {
                        width: parent.width - endDemo.width - 10
                        anchors.verticalCenter: parent.verticalCenter
                        text: qsTrId("s.demo.banner")
                        color: Theme.warningText
                        font.pixelSize: 13
                        font.weight: Font.DemiBold
                    }
                    Rectangle {
                        id: endDemo
                        anchors.verticalCenter: parent.verticalCenter
                        width: endText.implicitWidth + 22; height: 34; radius: 17
                        color: Theme.surface
                        objectName: "endDemo"
                        T { id: endText; anchors.centerIn: parent; text: qsTrId("s.demo.end"); font.weight: Font.Bold; font.pixelSize: 14; wrapMode: Text.NoWrap }
                        MouseArea { anchors.fill: parent; onClicked: StudentApp.removeTeacher(page.b.id) }
                    }
                }
            }

            // ---- could not load
            Rectangle {
                visible: (page.b.error ?? "") !== ""
                width: parent.width
                height: errorRow.implicitHeight + 20
                radius: Theme.radius
                color: page.b.error === "offline" ? Theme.surface : Theme.warningSoft
                Row {
                    id: errorRow
                    x: 14; y: 10
                    width: parent.width - 28
                    spacing: 10
                    Icon { name: page.b.error === "offline" ? "cloudOff" : "warning"; color: Theme.warningText; width: 20; height: 20 }
                    T {
                        width: parent.width - 30
                        font.pixelSize: 13
                        color: page.b.error === "offline" ? Theme.textSecondary : Theme.warningText
                        text: {
                            switch (page.b.error) {
                            case "offline": return qsTrId("s.error.offline")
                            case "gone": return qsTrId("s.error.gone").arg(page.teacherName)
                            case "key": return qsTrId("s.error.key")
                            case "newer": return qsTrId("s.error.newer")
                            }
                            return ""
                        }
                    }
                }
            }

            // ---- not loaded yet
            Card {
                visible: page.b.empty ?? false
                width: parent.width
                T { width: parent.width; text: StudentApp.busy ? qsTrId("s.home.loading") : qsTrId("s.home.not_loaded"); color: Theme.textMuted }
            }

            // ---- next lesson
            Rectangle {
                id: hero
                visible: !(page.b.empty ?? true)
                width: parent.width
                height: heroColumn.implicitHeight + 32
                radius: Theme.radiusLarge
                color: Theme.hero

                Column {
                    id: heroColumn
                    x: 18; y: 16
                    width: parent.width - 36
                    spacing: 6
                    T {
                        width: parent.width
                        text: [page.teacherName, page.b.instrument ?? ""].filter(s => s !== "").join(" · ")
                        color: Theme.heroMuted
                        font.pixelSize: 13
                        font.weight: Font.Bold
                    }
                    T {
                        visible: page.nextLesson === null
                        width: parent.width
                        text: qsTrId("s.next.none")
                        color: "white"
                        font.pixelSize: 20
                        font.weight: Font.ExtraBold
                    }
                    Item {
                        visible: page.nextLesson !== null
                        width: parent.width
                        height: Math.max(nextTitle.implicitHeight, distanceChip.height)
                        T {
                            id: nextTitle
                            width: parent.width - distanceChip.width - 8
                            text: qsTrId("s.next.label")
                            color: "white"
                            font.pixelSize: 13
                            font.weight: Font.ExtraBold
                            font.capitalization: Font.AllUppercase
                            font.letterSpacing: 0.9
                            anchors.verticalCenter: parent.verticalCenter
                        }
                        Rectangle {
                            id: distanceChip
                            readonly property string label: page.nextLesson ? Fmt.distance(page.nextLesson.daysAway) : ""
                            visible: label !== "" && !(page.nextLesson?.cancelled ?? false)
                            anchors.right: parent.right
                            width: chipText.implicitWidth + 20; height: 28; radius: 14
                            color: Theme.lime
                            T { id: chipText; anchors.centerIn: parent; text: distanceChip.label; color: Theme.textOnLime; font.pixelSize: 13; font.weight: Font.ExtraBold; wrapMode: Text.NoWrap }
                        }
                    }
                    T {
                        visible: page.nextLesson !== null
                        width: parent.width
                        text: page.nextLesson ? Fmt.date(page.nextLesson.date, "s.fmt.long") : ""
                        color: "white"
                        font.pixelSize: 24
                        font.weight: Font.ExtraBold
                        font.strikeout: page.nextLesson?.cancelled ?? false
                    }
                    T {
                        visible: page.nextLesson !== null
                        width: parent.width
                        text: page.nextLesson ? [page.nextLesson.start && page.nextLesson.end ? page.nextLesson.start + "–" + page.nextLesson.end : (page.nextLesson.start ?? ""),
                                                 page.nextLesson.place ?? ""].filter(s => s !== "").join(" · ") : ""
                        color: Theme.heroMuted
                        font.pixelSize: 15
                    }
                    // Changed or cancelled: what the teacher wrote
                    Rectangle {
                        readonly property string note: !page.nextLesson ? ""
                            : page.nextLesson.cancelled ? qsTrId("s.next.cancelled") + ((page.nextLesson.reason ?? "") !== "" ? " – " + page.nextLesson.reason : "")
                            : (page.nextLesson.changed ?? "")
                        visible: note !== ""
                        width: parent.width
                        height: noteText.implicitHeight + 14
                        radius: 10
                        color: Qt.rgba(1, 1, 1, 0.14)
                        T {
                            id: noteText
                            x: 10; y: 7
                            width: parent.width - 20
                            text: parent.note
                            color: Theme.lime
                            font.pixelSize: 14
                            font.weight: Font.Bold
                        }
                    }
                    // The lessons after that (changes and cancellations only, so the card stays short)
                    Repeater {
                        model: (page.b.next ?? []).slice(1).filter(l => l.cancelled || (l.changed ?? "") !== "")
                        T {
                            required property var modelData
                            width: heroColumn.width
                            font.pixelSize: 13
                            color: Theme.heroMuted
                            text: Fmt.date(modelData.date, "s.fmt.weekday_short") + ": "
                                  + (modelData.cancelled ? qsTrId("s.next.cancelled") + ((modelData.reason ?? "") !== "" ? " – " + modelData.reason : "")
                                                         : modelData.changed)
                        }
                    }
                }
            }

            // ---- tasks, newest lesson first
            Repeater {
                model: page.b.groups ?? []
                Column {
                    required property var modelData
                    required property int index
                    width: column.width
                    spacing: 8
                    topPadding: 8
                    SectionLabel {
                        width: parent.width
                        text: qsTrId("s.tasks.from").arg(Fmt.date(modelData.date, "s.fmt.weekday_short"))
                    }
                    Repeater {
                        model: modelData.tasks
                        TaskCard {
                            required property var modelData
                            width: column.width
                            task: modelData
                            allowFeedback: page.allowFeedback
                            open: page.openTasks[modelData.id] === true
                            onToggleHints: {
                                const next = Object.assign({}, page.openTasks)
                                next[modelData.id] = !open
                                page.openTasks = next
                            }
                        }
                    }
                }
            }
            Card {
                visible: !(page.b.empty ?? true) && (page.b.groups ?? []).length === 0
                width: parent.width
                T { width: parent.width; text: qsTrId("s.tasks.none"); color: Theme.textMuted }
            }

            // ---- what happened to the feedback
            Row {
                readonly property string state: page.b.sendState ?? "none"
                visible: page.allowFeedback && (state === "pending" || state === "sent")
                spacing: 6
                x: 4
                Icon {
                    width: 16; height: 16
                    anchors.verticalCenter: parent.verticalCenter
                    name: parent.state === "sent" ? "check" : "send"
                    color: parent.state === "sent" ? Theme.success : Theme.textMuted
                    strokeWidth: 2.4
                }
                T {
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 13
                    color: parent.state === "sent" ? Theme.success : Theme.textMuted
                    font.weight: Font.DemiBold
                    text: parent.state === "sent" ? (page.teacherNamed ? qsTrId("s.send.sent").arg(page.teacherName).arg(Fmt.dateTime(page.b.sentAt))
                                                                    : qsTrId("s.send.sent_plain").arg(Fmt.dateTime(page.b.sentAt)))
                                                  : qsTrId("s.send.pending")
                }
            }

            // ---- question to the teacher
            Column {
                visible: page.allowQuestions && !(page.b.empty ?? true)
                width: parent.width
                spacing: 8
                topPadding: 8
                SectionLabel { text: page.teacherNamed ? qsTrId("s.question.title").arg(page.teacherName) : qsTrId("s.question.title_plain") }
                Card {
                    width: parent.width
                    spacing: 10
                    // The field with "send" beside it, at the field's lower edge: both stay
                    // in view together when the keyboard is up
                    Item {
                        width: parent.width
                        height: fieldBox.height
                        Rectangle {
                            id: fieldBox
                            width: parent.width - sendButton.width - 10
                            height: Math.max(72, question.implicitHeight + 8)
                            radius: 12
                            color: Theme.surfaceAlt
                            border.width: question.activeFocus ? 2 : 0
                            border.color: Theme.accent
                            TextArea {
                                id: question
                                objectName: "questionField"
                                anchors.fill: parent
                                placeholderText: qsTrId("s.question.placeholder")
                                placeholderTextColor: Theme.textMuted
                                color: Theme.text
                                font.family: Theme.fontFamily
                                font.pixelSize: 15
                                wrapMode: TextEdit.Wrap
                                background: null
                                onCursorRectangleChanged: if (activeFocus) flick.reveal()
                            }
                        }
                        Rectangle {
                            id: sendButton
                            objectName: "sendQuestion"
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            width: 48; height: 48; radius: 24
                            color: question.text.trim() === "" ? Theme.border : Theme.accent
                            Icon { anchors.centerIn: parent; name: "send"; width: 20; height: 20; color: "white"; strokeWidth: 2.2 }
                            Accessible.role: Accessible.Button
                            Accessible.name: qsTrId("s.question.send")
                            MouseArea {
                                anchors.fill: parent
                                enabled: question.text.trim() !== ""
                                onClicked: {
                                    StudentApp.askQuestion(question.text)
                                    question.text = ""
                                    question.focus = false
                                }
                            }
                        }
                    }
                    T {
                        width: parent.width
                        text: page.b.demo ? qsTrId("s.question.demo") : qsTrId("s.question.hint")
                        color: Theme.textMuted
                        font.pixelSize: 12
                    }
                    // Already asked (newest first)
                    Repeater {
                        model: (page.b.questions ?? []).slice().reverse()
                        Rectangle {
                            required property var modelData
                            width: parent.width
                            height: askedText.implicitHeight + 16
                            radius: 10
                            color: Theme.accentSoft
                            T {
                                id: askedText
                                x: 12; y: 8
                                width: parent.width - 24
                                text: modelData.text
                                font.pixelSize: 14
                                color: Theme.textSecondary
                            }
                        }
                    }
                }
            }

            // ---- pieces
            Column {
                visible: (page.b.pieces ?? []).length > 0
                width: parent.width
                spacing: 8
                topPadding: 8
                SectionLabel { text: qsTrId("s.pieces.title") }
                Card {
                    width: parent.width
                    padding: 4
                    spacing: 0
                    Repeater {
                        model: page.b.pieces ?? []
                        Item {
                            required property var modelData
                            required property int index
                            width: parent.width
                            height: pieceColumn.implicitHeight + 22
                            Rectangle { visible: index > 0; x: 12; width: parent.width - 24; height: 1; color: Theme.divider }
                            Rectangle {
                                x: 10
                                anchors.verticalCenter: parent.verticalCenter
                                width: 36; height: 36; radius: 10
                                color: Theme.accentSoft
                                Icon { anchors.centerIn: parent; width: 20; height: 20; name: "music"; color: Theme.accentText }
                            }
                            Column {
                                id: pieceColumn
                                x: 58
                                anchors.verticalCenter: parent.verticalCenter
                                width: parent.width - x - (readyChip.visible ? readyChip.width + 14 : 10)
                                spacing: 1
                                T { width: parent.width; text: modelData.title; font.weight: Font.Bold }
                                T { width: parent.width; text: modelData.composer ?? ""; visible: text !== ""; color: Theme.textMuted; font.pixelSize: 13 }
                            }
                            Rectangle {
                                id: readyChip
                                visible: modelData.ready ?? false
                                anchors.right: parent.right
                                anchors.rightMargin: 10
                                anchors.verticalCenter: parent.verticalCenter
                                width: readyText.implicitWidth + 18; height: 26; radius: 13
                                color: Theme.successSoft
                                T { id: readyText; anchors.centerIn: parent; text: qsTrId("s.pieces.ready"); color: Theme.success; font.pixelSize: 12; font.weight: Font.ExtraBold; wrapMode: Text.NoWrap }
                            }
                        }
                    }
                }
            }

            // ---- dates (recitals, concerts)
            Column {
                visible: (page.b.events ?? []).length > 0
                width: parent.width
                spacing: 8
                topPadding: 8
                SectionLabel { text: qsTrId("s.events.title") }
                Repeater {
                    model: page.b.events ?? []
                    Card {
                        required property var modelData
                        width: column.width
                        Row {
                            width: parent.width
                            spacing: 12
                            Rectangle {
                                width: 52; height: 56; radius: 12
                                color: Theme.lime
                                Column {
                                    anchors.centerIn: parent
                                    T { anchors.horizontalCenter: parent.horizontalCenter; text: Fmt.date(modelData.date, "s.fmt.month_short"); font.pixelSize: 12; font.weight: Font.ExtraBold; color: Theme.textOnLime; font.capitalization: Font.AllUppercase; wrapMode: Text.NoWrap }
                                    T { anchors.horizontalCenter: parent.horizontalCenter; text: Fmt.date(modelData.date, "s.fmt.day_number"); font.pixelSize: 22; font.weight: Font.ExtraBold; color: Theme.textOnLime; wrapMode: Text.NoWrap }
                                }
                            }
                            Column {
                                width: parent.width - 64
                                anchors.verticalCenter: parent.verticalCenter
                                spacing: 2
                                T { width: parent.width; text: modelData.name; font.weight: Font.Bold; font.pixelSize: 16 }
                                T {
                                    width: parent.width
                                    text: [Fmt.date(modelData.date, "s.fmt.weekday_short"), modelData.time ?? "", modelData.place ?? ""].filter(s => s !== "").join(" · ")
                                    color: Theme.textMuted
                                    font.pixelSize: 13
                                }
                                T {
                                    visible: (modelData.contribution ?? "") !== ""
                                    width: parent.width
                                    text: qsTrId("s.events.you_play").arg(modelData.contribution ?? "")
                                    color: Theme.accentText
                                    font.pixelSize: 13
                                    font.weight: Font.Bold
                                }
                            }
                        }
                    }
                }
            }

            // ---- last update
            T {
                visible: !(page.b.demo ?? false) && page.b.updated !== undefined && !isNaN(page.b.updated?.getTime?.() ?? NaN)
                width: parent.width
                topPadding: 6
                horizontalAlignment: Text.AlignHCenter
                text: qsTrId("s.home.updated").arg(Fmt.dateTime(page.b.updated))
                color: Theme.textMuted
                font.pixelSize: 12
            }
        }
    }
}
