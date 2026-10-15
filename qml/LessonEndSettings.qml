// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Alert before the end of a lesson (this device): on/off, minutes, sound
// (Qupil's "ahem" or your own file), volume, and on iOS / Android whether the
// system lets Qupil send notifications.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Card {
    id: card
    Layout.fillWidth: true

    SwitchRow {
        text: qsTrId("settings.lesson_end.enable")
        hint: LessonEnd.background ? qsTrId("settings.lesson_end.hint_mobile") : qsTrId("settings.lesson_end.hint")
        checked: LessonEnd.enabled
        onToggled: LessonEnd.enabled = checked
    }

    // ------------------------------------------------------------ system permission (iOS, Android)
    Rectangle {
        id: permission
        readonly property bool notAsked: LessonEnd.permission === LessonEnd.NotAsked
        readonly property bool denied: LessonEnd.permission === LessonEnd.Denied
        readonly property bool notOnTime: LessonEnd.permission === LessonEnd.Granted && !LessonEnd.exactAlarms
        // Also needed for reminders, so independent of the switch above
        visible: LessonEnd.background && (notAsked || denied || notOnTime)
        Layout.fillWidth: true
        implicitHeight: permissionContent.implicitHeight + 28
        radius: 14
        color: Theme.accentSoft

        ColumnLayout {
            id: permissionContent
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 14
            spacing: 12
            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                Icon {
                    Layout.alignment: Qt.AlignTop
                    name: permission.notAsked ? "bell" : "warning"
                    color: Theme.accentText
                }
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: permission.notOnTime ? qsTrId("notify.exact_alarms")
                          : permission.denied ? qsTrId("notify.denied") : qsTrId("notify.permission")
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 14
                }
            }
            ActionButton {
                kind: "primary"
                text: permission.notAsked ? qsTrId("notify.allow") : qsTrId("notify.open_settings")
                onClicked: {
                    if (permission.notOnTime)
                        LessonEnd.openSystemSettings(true)
                    else if (permission.notAsked)
                        LessonEnd.requestPermission()
                    else
                        LessonEnd.openSystemSettings(false)
                }
            }
        }
    }

    Stepper {
        visible: LessonEnd.enabled
        label: qsTrId("settings.lesson_end.before")
        from: 1
        to: 30
        value: LessonEnd.minutes
        suffix: qsTrId("common.unit.min")
        onPicked: (value) => LessonEnd.minutes = value
    }

    SoundChooser {
        visible: LessonEnd.enabled
        Layout.topMargin: 4
        alert: LessonEnd
        defaultName: qsTrId("settings.sound.default_name")
    }
}
