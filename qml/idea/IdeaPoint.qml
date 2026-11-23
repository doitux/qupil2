// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "Try the new idea": one point of a task. In a lesson's window it has a small box to
// tick; where it is only read, a mark stands before it ("–", "+" new, "✓" done).

import QtQuick
import QtQuick.Layouts
import Qupil

Item {
    id: row

    property string text
    property bool done: false
    //! Only for the teacher (what I plan for the lesson): a lock before the text
    property bool priv: false
    property bool isNew: false
    property bool tickable: false
    property string mark: "–"

    signal toggled()

    Layout.fillWidth: true
    implicitHeight: Math.max(line.implicitHeight, tickable ? 28 : 0)
    Accessible.role: tickable ? Accessible.CheckBox : Accessible.StaticText
    Accessible.name: text
    Accessible.checked: done
    Accessible.onPressAction: if (tickable) toggled()
    Accessible.onToggleAction: if (tickable) toggled()

    RowLayout {
        id: line
        width: parent.width
        anchors.verticalCenter: parent.verticalCenter
        spacing: 8
        Rectangle {
            visible: row.tickable
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: 1
            implicitWidth: 20
            implicitHeight: 20
            radius: 6
            color: row.done ? Theme.success : "transparent"
            border.width: row.done ? 0 : 1.6
            border.color: Theme.icon
            Icon { anchors.centerIn: parent; width: 13; height: 13; name: "check"; color: Theme.surface; visible: row.done; strokeWidth: 3 }
        }
        Text {
            visible: !row.tickable
            Layout.alignment: Qt.AlignTop
            Layout.preferredWidth: 12
            text: row.mark
            color: row.mark === "✓" ? Theme.success : row.mark === "+" ? Theme.accentText : Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 14
            font.weight: Font.Bold
        }
        Icon {
            visible: row.priv
            Layout.alignment: Qt.AlignTop
            Layout.topMargin: 3
            Layout.preferredWidth: 14
            Layout.preferredHeight: 14
            name: "lock"
            color: Theme.accentText
        }
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: row.text
            color: row.done ? Theme.textMuted : row.mark === "+" || row.isNew ? Theme.accentText : Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 15
            font.strikeout: row.done && row.tickable
        }
        Rectangle {
            visible: row.isNew
            Layout.alignment: Qt.AlignTop
            implicitWidth: newText.implicitWidth + 12
            implicitHeight: 20
            radius: 10
            color: Theme.accentSoft
            Text {
                id: newText
                anchors.centerIn: parent
                text: qsTrId("idea.new")
                color: Theme.accentText
                font.family: Theme.fontFamily
                font.pixelSize: 11
                font.weight: Font.Bold
            }
        }
    }
    TapHandler {
        enabled: row.tickable
        onTapped: row.toggled()
    }
}
