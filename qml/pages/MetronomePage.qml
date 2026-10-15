// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Metronome (40–300 bpm, accent every 1–16 beats, tap tempo) and tuning tone.
// Keeps running while you switch to the notes; Space starts and stops it on desktops.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Flickable {
    id: page

    function noteName(note) {
        return note === "Bb" ? qsTrId("metronome.note.b_flat") : note === "B" ? qsTrId("metronome.note.b")
                                                                             : qsTrId("metronome.note.a")
    }

    clip: true
    contentHeight: column.implicitHeight + 48
    boundsBehavior: Flickable.StopAtBounds
    ScrollBar.vertical: ScrollBar {}

    ColumnLayout {
        id: column
        width: Math.min(page.width - 40, 560)
        x: (page.width - width) / 2
        y: 40
        spacing: 18

        Text {
            text: qsTrId("metronome.title")
            color: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 34
            font.weight: Font.ExtraBold
        }

        Text {
            visible: Metronome.error !== ""
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: Metronome.error
            color: Theme.danger
            font.family: Theme.fontFamily
            font.pixelSize: 14
            font.weight: Font.DemiBold
        }

        // ------------------------------------------------------------ metronome
        Card {
            Layout.fillWidth: true
            spacing: 16

            ColumnLayout {
                Layout.alignment: Qt.AlignHCenter
                spacing: 0
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: Metronome.bpm
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 88
                    font.weight: Font.ExtraBold
                    Accessible.role: Accessible.StaticText
                    Accessible.name: Metronome.bpm + " " + qsTrId("metronome.bpm_long")
                }
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: qsTrId("metronome.bpm_long")
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 14
                    font.weight: Font.DemiBold
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 12
                IconButton {
                    size: 52
                    fill: Theme.surfaceAlt
                    iconName: "minus"
                    text: qsTrId("metronome.slower")
                    autoRepeat: true
                    onClicked: Metronome.bpm = Metronome.bpm - 1
                }
                Slider {
                    id: tempo
                    Layout.fillWidth: true
                    from: 40
                    to: 300
                    stepSize: 1
                    value: Metronome.bpm
                    onMoved: Metronome.bpm = Math.round(value)
                    Accessible.name: qsTrId("metronome.label.tempo")
                    background: Rectangle {
                        x: tempo.leftPadding
                        y: tempo.topPadding + tempo.availableHeight / 2 - height / 2
                        width: tempo.availableWidth
                        height: 6
                        radius: 3
                        color: Theme.segment
                        Rectangle {
                            width: tempo.visualPosition * parent.width
                            height: parent.height
                            radius: 3
                            color: Theme.accent
                        }
                    }
                    handle: Rectangle {
                        x: tempo.leftPadding + tempo.visualPosition * (tempo.availableWidth - width)
                        y: tempo.topPadding + tempo.availableHeight / 2 - height / 2
                        width: 28
                        height: 28
                        radius: 14
                        color: Theme.surface
                        border.width: 2
                        border.color: Theme.accent
                    }
                }
                IconButton {
                    size: 52
                    fill: Theme.surfaceAlt
                    iconName: "plus"
                    text: qsTrId("metronome.faster")
                    autoRepeat: true
                    onClicked: Metronome.bpm = Metronome.bpm + 1
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                Text {
                    Layout.fillWidth: true
                    text: qsTrId("metronome.accent_every")
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                    font.weight: Font.DemiBold
                }
                Stepper {
                    Layout.fillWidth: false
                    Layout.preferredWidth: 200
                    value: Metronome.beatsPerBar
                    from: 1
                    to: 16
                    suffix: qsTrId("metronome.beats")
                    onPicked: (value) => Metronome.beatsPerBar = value
                }
            }

            // Beat dots: accent bigger, current beat lit
            Flow {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignHCenter
                spacing: 8
                Repeater {
                    model: Metronome.beatsPerBar
                    delegate: Rectangle {
                        required property int index
                        readonly property bool active: Metronome.beat === index
                        width: index === 0 && Metronome.beatsPerBar > 1 ? 22 : 16
                        height: width
                        radius: width / 2
                        color: active ? (index === 0 ? Theme.accent : Theme.lime) : Theme.segment
                        border.width: index === 0 && Metronome.beatsPerBar > 1 ? 2 : 0
                        border.color: Theme.accent
                    }
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 12
                AbstractButton {
                    id: tap
                    Layout.fillWidth: true
                    implicitHeight: 64
                    text: qsTrId("metronome.tap")
                    focusPolicy: Qt.StrongFocus
                    Accessible.role: Accessible.Button
                    Accessible.name: text
                    onPressed: Metronome.tap()
                    background: Rectangle {
                        radius: 18
                        color: tap.down ? Theme.accentSoft : Theme.surface
                        border.width: 2
                        border.color: tap.visualFocus ? Theme.accent : Theme.border
                    }
                    contentItem: Text {
                        text: tap.text
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 16
                        font.weight: Font.Bold
                    }
                }
                AbstractButton {
                    id: play
                    implicitHeight: 64
                    implicitWidth: playRow.implicitWidth + 52
                    focusPolicy: Qt.StrongFocus
                    Accessible.role: Accessible.Button
                    Accessible.name: Metronome.running ? qsTrId("metronome.stop") : qsTrId("metronome.start")
                    onClicked: Metronome.toggle()
                    background: Rectangle {
                        radius: 18
                        color: Metronome.running ? Theme.lime : Theme.accent
                        border.width: play.visualFocus ? 2 : 0
                        border.color: Theme.text
                    }
                    contentItem: Item {
                        RowLayout {
                            id: playRow
                            anchors.centerIn: parent
                            spacing: 8
                            Icon {
                                Layout.preferredWidth: 20
                                Layout.preferredHeight: 20
                                name: Metronome.running ? "stop" : "play"
                                color: Metronome.running ? Theme.textOnLime : Theme.textOnAccent
                                strokeWidth: 2.2
                            }
                            Text {
                                text: Metronome.running ? qsTrId("metronome.stop") : qsTrId("metronome.start")
                                color: Metronome.running ? Theme.textOnLime : Theme.textOnAccent
                                font.family: Theme.fontFamily
                                font.pixelSize: 17
                                font.weight: Font.ExtraBold
                            }
                        }
                    }
                }
            }
            Text {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                text: qsTrId("metronome.keeps_running")
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 13
            }
        }

        // ------------------------------------------------------------ tuning tone
        Card {
            Layout.fillWidth: true
            spacing: 14
            RowLayout {
                Layout.fillWidth: true
                Text {
                    Layout.fillWidth: true
                    text: qsTrId("metronome.tab.tuning")
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 18
                    font.weight: Font.ExtraBold
                }
                Segmented {
                    Layout.fillWidth: false
                    implicitWidth: 168
                    segmentHeight: 34
                    options: ["A", "Bb", "B"]
                    value: Metronome.toneNote
                    labelFor: (v) => page.noteName(v)
                    onActivated: (v) => Metronome.toneNote = v
                }
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: 10
                IconButton {
                    size: 48
                    fill: Theme.surfaceAlt
                    iconName: "minus"
                    text: qsTrId("metronome.pitch_down")
                    autoRepeat: true
                    onClicked: Metronome.pitch = Metronome.pitch - 1
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 0
                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: "a′ = " + Metronome.pitch + " Hz"
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 26
                        font.weight: Font.ExtraBold
                    }
                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: page.noteName(Metronome.toneNote) + " · " + Metronome.toneFrequency.toFixed(1) + " Hz"
                        color: Theme.textMuted
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                    }
                }
                IconButton {
                    size: 48
                    fill: Theme.surfaceAlt
                    iconName: "plus"
                    text: qsTrId("metronome.pitch_up")
                    autoRepeat: true
                    onClicked: Metronome.pitch = Metronome.pitch + 1
                }
            }
            AbstractButton {
                id: toneButton
                Layout.fillWidth: true
                implicitHeight: 52
                focusPolicy: Qt.StrongFocus
                Accessible.role: Accessible.Button
                Accessible.name: qsTrId("metronome.play_tone")
                onClicked: Metronome.toggleTone()
                background: Rectangle {
                    radius: 14
                    color: Metronome.tonePlaying ? Theme.lime : Theme.inverse
                    border.width: toneButton.visualFocus ? 2 : 0
                    border.color: Theme.accent
                }
                contentItem: Item {
                    RowLayout {
                        anchors.centerIn: parent
                        spacing: 8
                        Icon {
                            Layout.preferredWidth: 20
                            Layout.preferredHeight: 20
                            name: Metronome.tonePlaying ? "stop" : "speaker"
                            color: Metronome.tonePlaying ? Theme.textOnLime : Theme.textOnInverse
                        }
                        Text {
                            text: Metronome.tonePlaying ? qsTrId("metronome.stop") : qsTrId("metronome.play_tone")
                            color: Metronome.tonePlaying ? Theme.textOnLime : Theme.textOnInverse
                            font.family: Theme.fontFamily
                            font.pixelSize: 16
                            font.weight: Font.Bold
                        }
                    }
                }
            }
        }
    }
}
