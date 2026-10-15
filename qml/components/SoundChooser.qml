// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Sound of an alert: Qupil's own or a file, listen, volume (heard when let go).
// alert: LessonEnd or ReminderAlert (soundName, busy, volume, chooseSound(),
// useDefaultSound(), test()).

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import Qupil

ColumnLayout {
    id: chooser

    property QtObject alert
    //! Name of the sound that comes with Qupil
    property string defaultName

    Layout.fillWidth: true
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
                text: chooser.alert.soundName !== "" ? chooser.alert.soundName : chooser.defaultName
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 15
                font.weight: Font.Bold
            }
            BusyIndicator {
                visible: chooser.alert.busy
                running: visible
                implicitWidth: 32
                implicitHeight: 32
            }
            IconButton {
                iconName: "play"
                text: qsTrId("settings.sound.test")
                fill: Theme.surface
                enabled: !chooser.alert.busy
                onClicked: chooser.alert.test()
            }
        }
    }
    Flow {
        Layout.fillWidth: true
        spacing: 8
        ActionButton {
            iconName: "folder"
            text: qsTrId("settings.select_sound")
            enabled: !chooser.alert.busy
            onClicked: soundPicker.open()
        }
        ActionButton {
            visible: chooser.alert.soundName !== ""
            kind: "quiet"
            iconName: "restore"
            text: qsTrId("settings.sound.use_default")
            enabled: !chooser.alert.busy
            onClicked: chooser.alert.useDefaultSound()
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

    RowLayout {
        Layout.fillWidth: true
        Layout.topMargin: 4
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
        value: chooser.alert.volume
        onMoved: chooser.alert.volume = Math.round(value)
        onPressedChanged: if (!pressed) chooser.alert.test()
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

    FileDialog {
        id: soundPicker
        title: qsTrId("settings.select_sound")
        fileMode: FileDialog.OpenFile
        // Mobile pickers filter by MIME type: show everything there
        nameFilters: App.desktop ? [qsTrId("settings.filter_sound"), qsTrId("common.filter_all")] : []
        onAccepted: chooser.alert.chooseSound(selectedFile)
    }
}
