// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "Today": start page of the schedule tab. Lessons, notices and the running
// lesson card follow; for now it shows the date and the empty state.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Flickable {
    id: page

    clip: true
    contentHeight: column.implicitHeight + 48
    boundsBehavior: Flickable.StopAtBounds
    ScrollBar.vertical: ScrollBar {}

    // Re-evaluated when the UI language changes (locale-dependent date)
    readonly property string dateLine: {
        LanguageManager.effectiveLanguage
        return new Date().toLocaleDateString(Qt.locale(), Locale.LongFormat)
    }

    ColumnLayout {
        id: column
        width: Math.min(page.width - 40, 720)
        x: 20
        y: 40
        spacing: 18

        ColumnLayout {
            spacing: 2
            Text {
                text: page.dateLine.toUpperCase()
                color: Theme.accentText
                font.family: Theme.fontFamily
                font.pixelSize: 13
                font.weight: Font.Bold
                font.letterSpacing: 0.5
            }
            Text {
                text: qsTrId("nav.today")
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 34
                font.weight: Font.ExtraBold
            }
        }

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: empty.implicitHeight + 48
            radius: Theme.radiusLarge
            color: Theme.surface

            ColumnLayout {
                id: empty
                anchors.centerIn: parent
                width: parent.width - 48
                spacing: 10

                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    implicitWidth: 56
                    implicitHeight: 56
                    radius: 18
                    color: Theme.accentSoft
                    Icon {
                        anchors.centerIn: parent
                        width: 28
                        height: 28
                        name: "calendar"
                        color: Theme.accentText
                    }
                }
                Text {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    text: qsTrId("today.no_lessons")
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 18
                    font.weight: Font.Bold
                }
                Text {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                    text: qsTrId("app.under_construction")
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 14
                }
            }
        }
    }
}
