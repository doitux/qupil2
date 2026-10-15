// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Stand-in for areas of the rewrite that are not built yet.

import QtQuick
import QtQuick.Layouts

Item {
    id: page

    property string title
    property string iconName
    property string hint: qsTrId("app.under_construction")

    ColumnLayout {
        anchors.centerIn: parent
        width: Math.min(page.width - 40, 420)
        spacing: 12

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            implicitWidth: 64
            implicitHeight: 64
            radius: 20
            color: Theme.accentSoft
            Icon {
                anchors.centerIn: parent
                width: 32
                height: 32
                name: page.iconName
                color: Theme.accentText
            }
        }
        Text {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            text: page.title
            color: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 26
            font.weight: Font.ExtraBold
        }
        Text {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            visible: page.hint !== ""
            text: page.hint
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 15
        }
    }
}
