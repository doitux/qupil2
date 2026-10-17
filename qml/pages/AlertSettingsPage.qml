// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Settings › Alerts and sounds: alert before the end of a lesson and the sound
// of reminders. Stored on this device only.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page
    title: qsTrId("settings.alerts")
    backText: qsTrId("settings.title")

    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: qsTrId("settings.device_only")
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 13
    }
    LessonEndSettings {}
    Card {
        Layout.fillWidth: true
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTrId("settings.reminder_sound")
            color: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 15
            font.weight: Font.Bold
        }
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTrId("settings.reminder_sound.hint")
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 13
        }
        SoundChooser {
            alert: ReminderAlert
            defaultName: qsTrId("settings.sound.reminder_default")
        }
    }
}
