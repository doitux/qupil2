// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// The practice book of a student: coupling (needs the Nextcloud of the sync), the QR
// code the student scans with the phone's camera, the link to send instead, what the
// student may send back, the student's questions, and ending the coupling.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes
import Qupil

Sheet {
    id: sheet

    property string studentId
    property var student: ({})
    readonly property var coupling: { if (Data.revision < 0) return undefined; return studentId ? Data.coupling(studentId) : ({}) }
    readonly property bool coupled: !!coupling.id
    readonly property var qr: coupled ? Booklet.qrCode(studentId) : ({})
    //! Newest first: { text, date, read }
    readonly property var questions: { if (Data.revision < 0) return undefined; return coupled ? Data.questions(studentId) : [] }
    // The devices that have the booklet, as far as they said so (counts and days, never whose)
    readonly property var devices: { if (Data.revision < 0) return undefined; return coupled ? Data.couplingDevices(studentId) : ({}) }
    readonly property bool receives: (coupling.receives || 0) !== 0
    readonly property string devicesText: {
        const d = devices || ({}), here = d.connected || 0, gone = d.left || 0
        const lines = []
        if (here === 0 && gone === 0) {
            lines.push(qsTrId("booklet.devices.none"))
        } else if (here === 0) {
            lines.push(qsTrId("booklet.devices.left_all").arg(Data.formatDate(d.leftDate || "")))
        } else {
            let line = here === 1 ? (d.since ? qsTrId("booklet.devices.one_since").arg(Data.formatDate(d.since))
                                             : qsTrId("booklet.devices.one"))
                                  : qsTrId("booklet.devices.many").arg(here)
            if (gone === 1)
                line += ", " + qsTrId("booklet.devices.left_one").arg(Data.formatDate(d.leftDate || ""))
            else if (gone > 1)
                line += ", " + qsTrId("booklet.devices.left_many").arg(gone).arg(Data.formatDate(d.leftDate || ""))
            lines.push(line)
        }
        if (d.lastAnswer)
            lines.push(qsTrId("booklet.devices.last_answer").arg(Data.formatDate(d.lastAnswer)))
        return lines.join("\n")
    }
    property string error
    property bool confirmEnd: false
    property int looked: 0

    // While the code is shown and no device has it yet: look into the folder now and then,
    // so "1 device connected" appears in the lesson and not only after the next sync
    Timer {
        interval: 8000
        repeat: true
        running: sheet.opened && sheet.coupled && sheet.receives && (sheet.devices.connected || 0) === 0 && sheet.looked < 40
        onTriggered: { sheet.looked++; Booklet.lookFor(sheet.studentId) }
    }

    function show(id) {
        studentId = id
        student = Data.student(id)
        error = ""
        confirmEnd = false
        teacherName.text = Data.setting("booklet_teacher_name", "")
        open()
    }

    title: qsTrId("booklet.title")
    showAccept: false
    rejectText: qsTrId("common.close")

    Connections {
        target: Booklet
        function onCoupled(id, error) { if (id === sheet.studentId) sheet.error = error }
        function onEnded(id, error) {
            if (id !== sheet.studentId)
                return
            sheet.error = error
            if (error === "")
                Nav.toast(qsTrId("booklet.ended_toast").arg(sheet.student.forename || ""))
        }
    }

    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: sheet.coupled ? qsTrId("booklet.coupled_text").arg(sheet.student.forename || "")
                            : qsTrId("booklet.intro").arg(sheet.student.forename || "")
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 14
    }

    // How the teacher is called in the student's app ("Frau Rau"): one name for all students
    InputField {
        id: teacherName
        label: qsTrId("booklet.teacher_name")
        placeholder: qsTrId("booklet.teacher_name_placeholder")
        onEdited: (value) => { text = value; Data.setSetting("booklet_teacher_name", value.trim()) }
    }

    // ---- not coupled
    ColumnLayout {
        visible: !sheet.coupled
        Layout.fillWidth: true
        spacing: 10
        InfoBox {
            visible: !Booklet.available
            Layout.fillWidth: true
            text: qsTrId("booklet.needs_nextcloud")
        }
        ActionButton {
            Layout.fillWidth: true
            kind: "primary"
            iconName: "link"
            enabled: Booklet.available && !Booklet.busy
            text: Booklet.busy ? qsTrId("booklet.coupling") : qsTrId("booklet.couple")
            onClicked: {
                sheet.error = ""
                Booklet.couple(sheet.studentId)
            }
        }
    }

    // ---- coupled: the QR code and the link
    ColumnLayout {
        visible: sheet.coupled
        Layout.fillWidth: true
        spacing: 12

        InfoBox {
            objectName: "bookletDevices"
            visible: sheet.receives
            Layout.fillWidth: true
            text: sheet.devicesText
            Accessible.role: Accessible.StaticText
            Accessible.name: text
        }
        Rectangle {
            objectName: "bookletQr"
            Layout.alignment: Qt.AlignHCenter
            implicitWidth: Math.min(260, sheet.width - 80)
            implicitHeight: implicitWidth
            radius: 12
            color: "white"
            Accessible.role: Accessible.Graphic
            Accessible.name: qsTrId("booklet.qr")
            Shape {
                id: qrShape
                anchors.fill: parent
                visible: !!sheet.qr.path
                ShapePath {
                    readonly property real k: qrShape.width / (sheet.qr.size || 1)
                    scale: Qt.size(k, k)
                    strokeWidth: -1
                    fillColor: "black"
                    PathSvg { path: sheet.qr.path || "" }
                }
            }
        }
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            text: qsTrId("booklet.qr_hint")
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 13
        }
        RowLayout {
            Layout.fillWidth: true
            spacing: 8
            ActionButton {
                Layout.fillWidth: true
                objectName: "bookletLink"
                iconName: "share"
                // The desktop has no share menu: the link goes to the clipboard, and says so
                text: App.desktop ? qsTrId("booklet.copy_link") : qsTrId("booklet.send_link")
                onClicked: {
                    const text = qsTrId("booklet.link_message").arg(sheet.student.forename || "") + "\n\n" + Booklet.link(sheet.studentId)
                    if (!App.shareText(text, qsTrId("booklet.title")))
                        Nav.toast(App.desktop ? qsTrId("booklet.link_copied_desktop") : qsTrId("booklet.link_copied"))
                }
            }
        }
        InfoBox {
            visible: !sheet.receives
            Layout.fillWidth: true
            text: qsTrId("booklet.read_only")
        }

        SwitchRow {
            visible: (sheet.coupling.receives || 0) !== 0
            text: qsTrId("booklet.allow_feedback")
            hint: qsTrId("booklet.allow_feedback_hint")
            checked: (sheet.coupling.allow_feedback || 0) !== 0
            onToggled: Data.update("coupling", sheet.coupling.id, { allow_feedback: checked ? 1 : 0 })
        }
        SwitchRow {
            visible: (sheet.coupling.receives || 0) !== 0
            text: qsTrId("booklet.allow_questions")
            checked: (sheet.coupling.allow_questions || 0) !== 0
            onToggled: Data.update("coupling", sheet.coupling.id, { allow_questions: checked ? 1 : 0 })
        }

        // The student's questions (newest first)
        FieldLabel {
            visible: sheet.questions.length > 0
            label: qsTrId("booklet.questions")
        }
        Repeater {
            model: sheet.questions
            delegate: Rectangle {
                id: questionBox
                required property var modelData
                readonly property string label: qsTrId("task.question_from").arg(Data.formatDate(modelData.date || "", true))
                Layout.fillWidth: true
                implicitHeight: questionColumn.implicitHeight + 20
                radius: 12
                color: Theme.surfaceAlt
                Accessible.role: Accessible.StaticText
                Accessible.name: label + ": " + (modelData.text || "")
                Column {
                    id: questionColumn
                    x: 12
                    y: 10
                    width: parent.width - 24
                    spacing: 2
                    Text {
                        width: parent.width
                        text: questionBox.label
                        // Not read yet: in the accent colour
                        color: questionBox.modelData.read ? Theme.textMuted : Theme.accentText
                        font.family: Theme.fontFamily
                        font.pixelSize: 12
                        font.weight: Font.Bold
                    }
                    Text {
                        width: parent.width
                        wrapMode: Text.WordWrap
                        text: questionBox.modelData.text || ""
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                    }
                }
            }
        }

        // Ending: asks first
        ActionButton {
            visible: !sheet.confirmEnd
            Layout.alignment: Qt.AlignLeft
            kind: "quiet"
            iconName: "close"
            text: qsTrId("booklet.end")
            onClicked: sheet.confirmEnd = true
        }
        ColumnLayout {
            visible: sheet.confirmEnd
            Layout.fillWidth: true
            spacing: 8
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: qsTrId("booklet.end_confirm").arg(sheet.student.forename || "")
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 14
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                ActionButton {
                    Layout.fillWidth: true
                    text: qsTrId("common.cancel")
                    onClicked: sheet.confirmEnd = false
                }
                ActionButton {
                    Layout.fillWidth: true
                    kind: "danger"
                    text: qsTrId("booklet.end")
                    onClicked: {
                        sheet.confirmEnd = false
                        Booklet.endCoupling(sheet.studentId)
                    }
                }
            }
        }
    }

    component InfoBox: Rectangle {
        property alias text: info.text
        implicitHeight: info.implicitHeight + 20
        radius: 12
        color: Theme.surfaceAlt
        Text {
            id: info
            x: 12
            y: 10
            width: parent.width - 24
            wrapMode: Text.WordWrap
            color: Theme.textSecondary
            font.family: Theme.fontFamily
            font.pixelSize: 14
        }
    }

    Text {
        visible: sheet.error !== ""
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: {
            switch (sheet.error) {
            case "": return ""
            case "links-disabled": return qsTrId("booklet.error.links")
            case "access": return qsTrId("booklet.error.access")
            case "no-nextcloud": return qsTrId("booklet.needs_nextcloud")
            default: return qsTrId("booklet.error.offline")
            }
        }
        color: Theme.danger
        font.family: Theme.fontFamily
        font.pixelSize: 14
    }
}
