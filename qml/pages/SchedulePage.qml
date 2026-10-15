// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Schedule tab. Phones: the day, tapping a lesson or student opens it on top.
// Tablets/desktop: the day as a column next to the selected student or lesson.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Item {
    id: page

    readonly property bool wide: width >= 900
    property string studentId
    property string membershipId
    property string lessonId

    Loader {
        anchors.fill: parent
        sourceComponent: page.wide ? wideUi : compactUi
    }

    Component {
        id: compactUi
        StackView {
            id: stack
            initialItem: ScheduleOverview {
                onStudentClicked: (studentId, membershipId) => stack.push(studentComponent, {
                    studentId: studentId, membershipId: membershipId, backText: qsTrId("nav.today")
                })
                onLessonClicked: (lessonId) => stack.push(lessonComponent, { lessonId: lessonId, backText: qsTrId("nav.today") })
            }
            Component {
                id: studentComponent
                StudentDetail {
                    onBack: stack.pop()
                }
            }
            Component {
                id: lessonComponent
                LessonDetail {
                    onBack: stack.pop()
                    onStudentClicked: (studentId, membershipId) => stack.push(studentComponent, {
                        studentId: studentId, membershipId: membershipId, backText: lessonName
                    })
                }
            }
        }
    }

    Component {
        id: wideUi
        RowLayout {
            spacing: 0
            ScheduleOverview {
                Layout.preferredWidth: Math.min(380, page.width * 0.4)
                Layout.fillHeight: true
                columnMode: true
                selectedMembership: page.membershipId
                onStudentClicked: (studentId, membershipId) => {
                    page.lessonId = ""
                    page.studentId = studentId
                    page.membershipId = membershipId
                }
                onLessonClicked: (lessonId) => {
                    page.studentId = ""
                    page.membershipId = ""
                    page.lessonId = lessonId
                }
            }
            Rectangle {
                Layout.fillHeight: true
                implicitWidth: 1
                color: Theme.border
            }
            Loader {
                Layout.fillWidth: true
                Layout.fillHeight: true
                sourceComponent: page.lessonId ? wideLesson : page.studentId ? wideStudent : wideEmpty
            }
            Component {
                id: wideStudent
                StudentDetail {
                    showBack: false
                    studentId: page.studentId
                    membershipId: page.membershipId
                    onBack: page.studentId = ""
                }
            }
            Component {
                id: wideLesson
                LessonDetail {
                    showBack: false
                    lessonId: page.lessonId
                    onBack: page.lessonId = ""
                    onStudentClicked: (studentId, membershipId) => {
                        page.lessonId = ""
                        page.studentId = studentId
                        page.membershipId = membershipId
                    }
                }
            }
            Component {
                id: wideEmpty
                PlaceholderPage {
                    iconName: "students"
                    title: qsTrId("schedule.pick_hint")
                    hint: ""
                }
            }
        }
    }
}
