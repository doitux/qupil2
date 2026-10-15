// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Alert before the end of a lesson (this device): on/off, minutes, sound
// (Qupil's "ahem" or your own file), volume, and on iOS / Android whether the
// system lets Qupil send notifications.

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
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
        visible: LessonEnd.enabled && LessonEnd.background && (notAsked || denied || notOnTime)
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

    // ------------------------------------------------------------ sound
    ColumnLayout {
        visible: LessonEnd.enabled
        Layout.fillWidth: true
        Layout.topMargin: 4
        spacing: 8

        FieldLabel { label: qsTrId("settings.play_sound") }
        Rectangle {
            Layout.fillWidth: true
            implicitHeight: 56
            radius: 14
            color: Theme.surfaceAlt
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 14
                anchors.rightMargin: 6
                spacing: 10
                Icon {
                    name: "speaker"
                    color: Theme.accentText
                }
                Text {
                    Layout.fillWidth: true
                    elide: Text.ElideMiddle
                    text: LessonEnd.soundName !== "" ? LessonEnd.soundName : qsTrId("settings.sound.default_name")
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                    font.weight: Font.Bold
                }
                BusyIndicator {
                    visible: LessonEnd.busy
                    running: visible
                    implicitWidth: 32
                    implicitHeight: 32
                }
                IconButton {
                    iconName: "play"
                    text: qsTrId("settings.sound.test")
                    fill: Theme.surface
                    enabled: !LessonEnd.busy
                    onClicked: LessonEnd.test()
                }
            }
        }
        Flow {
            Layout.fillWidth: true
            spacing: 8
            ActionButton {
                iconName: "folder"
                text: qsTrId("settings.select_sound")
                enabled: !LessonEnd.busy
                onClicked: soundPicker.open()
            }
            ActionButton {
                visible: LessonEnd.soundName !== ""
                kind: "quiet"
                iconName: "restore"
                text: qsTrId("settings.sound.use_default")
                enabled: !LessonEnd.busy
                onClicked: LessonEnd.useDefaultSound()
            }
        }
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTrId("settings.sound.hint")
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 13
        }
    }

    // ------------------------------------------------------------ volume (heard when let go)
    ColumnLayout {
        visible: LessonEnd.enabled
        Layout.fillWidth: true
        Layout.topMargin: 4
        spacing: 2

        RowLayout {
            Layout.fillWidth: true
            FieldLabel { label: qsTrId("settings.volume") }
            Text {
                text: Math.round(volume.value)
                color: Theme.accentText
                font.family: Theme.fontFamily
                font.pixelSize: 15
                font.weight: Font.Bold
            }
        }
        Slider {
            id: volume
            Layout.fillWidth: true
            from: 1
            to: 10
            stepSize: 1
            snapMode: Slider.SnapAlways
            value: LessonEnd.volume
            onMoved: LessonEnd.volume = Math.round(value)
            onPressedChanged: if (!pressed) LessonEnd.test()
            Accessible.name: qsTrId("settings.volume")
            background: Rectangle {
                x: volume.leftPadding
                y: volume.topPadding + volume.availableHeight / 2 - height / 2
                width: volume.availableWidth
                height: 6
                radius: 3
                color: Theme.segment
                Rectangle {
                    width: volume.visualPosition * parent.width
                    height: parent.height
                    radius: 3
                    color: Theme.accent
                }
            }
            handle: Rectangle {
                x: volume.leftPadding + volume.visualPosition * (volume.availableWidth - width)
                y: volume.topPadding + volume.availableHeight / 2 - height / 2
                width: 28
                height: 28
                radius: 14
                color: Theme.surface
                border.width: 2
                border.color: Theme.accent
            }
        }
    }

    FileDialog {
        id: soundPicker
        title: qsTrId("settings.select_sound")
        fileMode: FileDialog.OpenFile
        // Mobile pickers filter by MIME type: show everything there
        nameFilters: App.desktop ? [qsTrId("settings.filter_sound"), qsTrId("common.filter_all")] : []
        onAccepted: LessonEnd.chooseSound(selectedFile)
    }
}
