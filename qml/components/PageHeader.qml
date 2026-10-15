// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Top row of a pushed page: "‹ Back" on the left, actions on the right.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

RowLayout {
    id: header

    property string backText: qsTrId("common.back")
    property bool showBack: true
    default property alias actions: actionRow.data

    signal back()

    Layout.fillWidth: true
    spacing: 4

    AbstractButton {
        id: backButton
        visible: header.showBack
        implicitHeight: 44
        implicitWidth: backRow.implicitWidth + 16
        focusPolicy: Qt.StrongFocus
        Accessible.role: Accessible.Button
        Accessible.name: header.backText
        onClicked: header.back()
        background: Rectangle {
            radius: 12
            color: backButton.down ? Theme.accentSoft : "transparent"
            border.width: backButton.visualFocus ? 2 : 0
            border.color: Theme.accentText
        }
        contentItem: Item {
            RowLayout {
                id: backRow
                anchors.centerIn: parent
                spacing: 2
                Icon {
                    Layout.preferredWidth: 22
                    Layout.preferredHeight: 22
                    name: "chevron"
                    rotation: 180
                    color: Theme.accentText
                    strokeWidth: 2.2
                }
                Text {
                    text: header.backText
                    color: Theme.accentText
                    font.family: Theme.fontFamily
                    font.pixelSize: 16
                    font.weight: Font.DemiBold
                }
            }
        }
    }
    Item { Layout.fillWidth: true }
    RowLayout {
        id: actionRow
        spacing: 8
    }
}
