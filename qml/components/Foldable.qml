// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Long content shortened: its first lines, fading out, and "Show more" below to unfold
// it ("Show less" folds it again). Content that is not much longer is shown as it is,
// without a button. The content is one item that takes the width of its parent:
//
//   Foldable {
//       Layout.fillWidth: true
//       lineHeight: …
//       expanded: page.unfolded[id] === true
//       onToggled: page.toggle(id)
//       Text { width: parent.width; wrapMode: Text.Wrap; … }
//   }
//
// The owner keeps the state (expanded), so it survives a reload of the list.

import QtQuick
import QtQuick.Controls
import Qupil

Item {
    id: root

    default property alias content: holder.data
    //! Height of one line of the content
    property real lineHeight: 20
    //! Lines shown while folded
    property int foldedLines: 7
    //! What is behind the content, for the fade at the bottom
    property color fadeColor: Theme.surface
    property bool expanded: false

    //! "Show more" or "Show less" was pressed
    signal toggled()

    readonly property real fullHeight: holder.children.length > 0 ? holder.children[0].implicitHeight : 0
    //! Folded only when a good part would be hidden, not for a line or two more
    readonly property bool foldable: fullHeight > lineHeight * (foldedLines + 3.5)
    readonly property bool folded: foldable && !expanded
    property bool animating: false

    implicitHeight: holder.height + (toggle.item ? toggle.item.height + 2 : 0)
    onExpandedChanged: {
        animating = true
        settle.restart()
    }

    Timer {
        id: settle
        interval: 260
        onTriggered: root.animating = false
    }

    Item {
        id: holder
        width: root.width
        height: root.folded ? root.lineHeight * root.foldedLines : root.fullHeight
        clip: root.foldable
        Behavior on height {
            enabled: root.animating
            NumberAnimation { duration: 220; easing.type: Easing.OutCubic }
        }
    }

    // The last lines fade out: there is more
    Rectangle {
        anchors.bottom: holder.bottom
        width: holder.width
        height: root.lineHeight * 2.2
        opacity: root.folded ? 1 : 0
        visible: opacity > 0
        Behavior on opacity {
            enabled: root.animating
            NumberAnimation { duration: 180 }
        }
        gradient: Gradient {
            GradientStop { position: 0; color: Qt.rgba(root.fadeColor.r, root.fadeColor.g, root.fadeColor.b, 0) }
            GradientStop { position: 1; color: root.fadeColor }
        }
    }

    // Only for long content: short notes (most of them) get no button at all
    Loader {
        id: toggle
        active: root.foldable
        visible: active
        y: holder.height + 2
        sourceComponent: AbstractButton {
            id: button
            height: 36
            leftPadding: 0
            rightPadding: 8
            focusPolicy: Qt.StrongFocus
            hoverEnabled: true
            text: root.expanded ? qsTrId("common.show_less") : qsTrId("common.show_more")
            Accessible.role: Accessible.Button
            Accessible.name: text
            onClicked: root.toggled()
            background: Rectangle {
                x: -6
                width: parent.width + 6
                radius: 8
                color: Theme.accentText
                opacity: button.down ? 0.12 : button.hovered ? 0.06 : 0
                border.width: button.visualFocus ? 2 : 0
                border.color: Theme.accentText
            }
            contentItem: Row {
                spacing: 4
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: button.text
                    color: Theme.accentText
                    font.family: Theme.fontFamily
                    font.pixelSize: 14
                    font.weight: Font.ExtraBold
                }
                Icon {
                    anchors.verticalCenter: parent.verticalCenter
                    width: 18
                    height: 18
                    name: "chevronDown"
                    color: Theme.accentText
                    strokeWidth: 2.4
                    rotation: root.expanded ? 180 : 0
                    Behavior on rotation { NumberAnimation { duration: 180 } }
                }
            }
        }
    }
}
