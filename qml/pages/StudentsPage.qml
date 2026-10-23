// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// All students A–Z with search. Phones: the student opens on top;
// tablets/desktop: list and student side by side.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Item {
    id: page

    readonly property bool wide: width >= 900
    property string studentId
    property string membershipId
    //! Piece to show on the student's page (Nav.showPiece)
    property string focusPiece
    property string filter

    readonly property var students: { if (Data.revision < 0) return undefined; return Data.students(filter) }

    function show(id, membership, piece) {
        studentId = id
        membershipId = membership
        focusPiece = ""   // so the same piece can be shown again
        focusPiece = piece || ""
        if (!wide && loader.item)
            loader.item.openStudent(id, membership, focusPiece)
    }

    //! System back button: closes the student (phone: the page on top)
    function back() {
        if (!wide)
            return Nav.stackBack(loader.item)
        if (studentId === "")
            return false
        studentId = ""
        membershipId = ""
        return true
    }

    Connections {
        target: Nav
        function onShowStudent(id, membership) { page.show(id, membership, "") }
        function onShowPiece(id, membership, piece) { page.show(id, membership, piece) }
    }

    Loader {
        id: loader
        anchors.fill: parent
        sourceComponent: page.wide ? wideUi : compactUi
    }

    Component {
        id: compactUi
        StackView {
            id: stack
            function openStudent(id, membership, piece) {
                stack.pop(null)
                stack.push(studentComponent, { studentId: id, membershipId: membership, focusPiece: piece || "",
                                               backText: qsTrId("nav.students") })
            }
            initialItem: StudentList {
                students: page.students
                filter: page.filter
                onFilterEdited: (value) => page.filter = value
                onPicked: (id) => stack.openStudent(id, "", "")
            }
            Component.onCompleted: if (page.studentId) openStudent(page.studentId, page.membershipId, page.focusPiece)
            Component {
                id: studentComponent
                StudentDetail {
                    onBack: {
                        page.studentId = ""
                        stack.pop()
                    }
                }
            }
        }
    }

    Component {
        id: wideUi
        RowLayout {
            spacing: 0
            StudentList {
                Layout.preferredWidth: Math.min(380, page.width * 0.4)
                Layout.fillHeight: true
                students: page.students
                filter: page.filter
                onFilterEdited: (value) => page.filter = value
                selected: page.studentId
                onPicked: (id) => {
                    page.studentId = id
                    page.membershipId = ""
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
                sourceComponent: page.studentId ? wideStudent : wideEmpty
            }
            Component {
                id: wideStudent
                StudentDetail {
                    showBack: false
                    studentId: page.studentId
                    membershipId: page.membershipId
                    focusPiece: page.focusPiece
                    onBack: page.studentId = ""
                }
            }
            Component {
                id: wideEmpty
                PlaceholderPage {
                    iconName: "students"
                    title: qsTrId("students.pick_hint")
                    hint: ""
                }
            }
        }
    }

    component StudentList: Item {
        id: list
        property string selected
        property var students: []
        property string filter
        signal picked(string id)
        signal filterEdited(string value)

        ColumnLayout {
            anchors.fill: parent
            anchors.topMargin: 28
            spacing: 12

            RowLayout {
                Layout.fillWidth: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                spacing: 8
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 0
                    Text {
                        text: qsTrId("nav.students")
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 30
                        font.weight: Font.ExtraBold
                    }
                    Text {
                        text: {
                            if (Data.revision < 0) return undefined
                            return qsTrId("summary.students", Data.students("").length)
                        }
                        color: Theme.textMuted
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                    }
                }
                IconButton {
                    filled: true
                    iconName: "plus"
                    text: qsTrId("schedule.new_student")
                    onClicked: Nav.newStudent()
                }
            }

            InputField {
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                placeholder: qsTrId("student.search")
                text: list.filter
                onEdited: (value) => list.filterEdited(value)
            }

            ListView {
                id: view
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: list.students
                boundsBehavior: Flickable.StopAtBounds
                ScrollBar.vertical: ScrollBar {}
                section.property: "section"
                section.delegate: Text {
                    required property string section
                    width: ListView.view ? ListView.view.width : 100
                    leftPadding: 24
                    topPadding: 10
                    bottomPadding: 4
                    text: section
                    color: Theme.accentText
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                    font.weight: Font.ExtraBold
                }
                delegate: AbstractButton {
                    id: row
                    required property var modelData
                    readonly property bool isSelected: list.selected === modelData.id
                    width: ListView.view.width
                    implicitHeight: 64
                    focusPolicy: Qt.StrongFocus
                    hoverEnabled: true
                    Accessible.role: Accessible.Button
                    Accessible.name: modelData.name
                    onClicked: list.picked(modelData.id)
                    background: Rectangle {
                        anchors.fill: parent
                        anchors.leftMargin: 12
                        anchors.rightMargin: 12
                        radius: 14
                        color: row.isSelected ? Theme.accentSoft : row.hovered ? Theme.surface : "transparent"
                        border.width: row.visualFocus ? 2 : 0
                        border.color: Theme.accentText
                    }
                    contentItem: RowLayout {
                        spacing: 12
                        opacity: row.modelData.active ? 1 : 0.6
                        Avatar {
                            size: 40
                            text: row.modelData.initials
                            seed: row.modelData.name
                        }
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 1
                            Text {
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                                text: row.modelData.listName
                                color: row.isSelected ? Theme.accentText : Theme.text
                                font.family: Theme.fontFamily
                                font.pixelSize: 15
                                font.weight: Font.Bold
                            }
                            Text {
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                                text: [row.modelData.instrument, row.modelData.lessons || qsTrId("students.no_lesson_short")]
                                      .filter(Boolean).join(" · ")
                                color: Theme.textMuted
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                            }
                        }
                    }
                    leftPadding: 24
                    rightPadding: 24
                }
                footer: Item { height: 24 }
                EmptyHint {
                    visible: view.count === 0
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 20
                    text: list.filter ? qsTrId("students.no_match") : qsTrId("students.empty")
                }
            }
        }
    }
}
