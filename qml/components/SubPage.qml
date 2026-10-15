// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Page pushed from "More": back button, title, scrolling content.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: sub

    property string title
    property string backText: qsTrId("nav.more")
    default property alias content: column.data
    //! Button(s) right of the title
    property alias actions: actionRow.data

    signal back()

    Flickable {
        id: flick
        anchors.fill: parent
        clip: true
        contentHeight: outer.implicitHeight + 64
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}

        ColumnLayout {
            id: outer
            width: Math.min(flick.width - 40, 680)
            x: 20
            y: 12
            spacing: 14

            PageHeader {
                Layout.leftMargin: -8
                backText: sub.backText
                onBack: sub.back()
            }
            RowLayout {
                Layout.fillWidth: true
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: sub.title
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 30
                    font.weight: Font.ExtraBold
                }
                RowLayout {
                    id: actionRow
                    spacing: 8
                }
            }
            ColumnLayout {
                id: column
                Layout.fillWidth: true
                spacing: 12
            }
        }
    }
}
