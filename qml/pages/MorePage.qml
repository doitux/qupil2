// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "More": tools, sync, backup and settings. Appearance and language are ready.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Flickable {
    id: page

    clip: true
    contentHeight: column.implicitHeight + 64
    boundsBehavior: Flickable.StopAtBounds
    ScrollBar.vertical: ScrollBar {}

    ColumnLayout {
        id: column
        width: Math.min(page.width - 40, 560)
        x: 20
        y: 40
        spacing: 18

        Text {
            text: qsTrId("nav.more")
            color: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 34
            font.weight: Font.ExtraBold
        }

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: appearance.implicitHeight + 32
            radius: Theme.radius + 2
            color: Theme.surface

            AppearanceSettings {
                id: appearance
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 16
            }
        }

        Rectangle {
            Layout.fillWidth: true
            implicitHeight: about.implicitHeight + 28
            radius: Theme.radius + 2
            color: Theme.surface

            RowLayout {
                id: about
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 14
                spacing: 12

                Rectangle {
                    implicitWidth: 34
                    implicitHeight: 34
                    radius: 10
                    color: Theme.tile.gray.bg
                    Icon {
                        anchors.centerIn: parent
                        width: 20
                        height: 20
                        name: "info"
                        color: Theme.tile.gray.fg
                    }
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 1
                    Text {
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: qsTrId("about.title")
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.DemiBold
                    }
                    Text {
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: qsTrId("about.version").arg(Qt.application.version) + " · qupil.de"
                        color: Theme.textMuted
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                    }
                }
            }
        }
    }
}
