// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Context menu in Qupil's look: a rounded card with a soft shadow, roomy rows
// (AppMenuItem, AppMenuSeparator) with icons, red rows for deleting. As wide as
// its longest row; submenus are AppMenus as well (iconName shows in the parent).
//
//   AppMenu {
//       AppMenuItem { iconName: "edit"; text: …; onTriggered: … }
//       AppMenuSeparator {}
//       AppMenuItem { iconName: "trash"; destructive: true; text: …; onTriggered: … }
//   }

import QtQuick
import QtQuick.Controls

Menu {
    id: menu

    //! Icon of the row that opens this menu as a submenu
    property string iconName
    property real minWidth: 220
    property real maxWidth: 340

    // The longest visible row decides the width
    readonly property real widestRow: {
        let widest = 0
        for (let i = 0; i < count; ++i) {
            const row = itemAt(i)
            if (row && row.visible)
                widest = Math.max(widest, row.implicitWidth)
        }
        return widest
    }

    implicitWidth: Math.min(maxWidth, Math.max(minWidth, widestRow + leftPadding + rightPadding))
    implicitHeight: contentItem.implicitHeight + topPadding + bottomPadding
    padding: 6
    margins: 8
    overlap: 4
    transformOrigin: Popup.Top

    delegate: AppMenuItem {}

    contentItem: ListView {
        implicitHeight: contentHeight
        model: menu.contentModel
        interactive: Window.window ? contentHeight + menu.topPadding + menu.bottomPadding > Window.window.height : false
        clip: true
        currentIndex: menu.currentIndex
        boundsBehavior: Flickable.StopAtBounds
        ScrollIndicator.vertical: ScrollIndicator {}
    }

    background: Item {
        implicitWidth: menu.minWidth
        implicitHeight: 40
        // Soft shadow from a few widening layers
        Repeater {
            model: [{ grow: 1, y: 2, alpha: 0.5 }, { grow: 4, y: 6, alpha: 0.25 }, { grow: 9, y: 12, alpha: 0.12 }]
            delegate: Rectangle {
                required property var modelData
                x: -modelData.grow
                y: modelData.y - modelData.grow
                width: parent.width + 2 * modelData.grow
                height: parent.height + 2 * modelData.grow
                radius: Theme.radius + modelData.grow
                color: Theme.shadow
                opacity: modelData.alpha
            }
        }
        Rectangle {
            anchors.fill: parent
            radius: Theme.radius
            color: Theme.dark ? Theme.surfaceAlt : Theme.surface
            border.width: 1
            border.color: Theme.border
        }
    }

    enter: Transition {
        NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 140; easing.type: Easing.OutCubic }
        NumberAnimation { property: "scale"; from: 0.94; to: 1; duration: 160; easing.type: Easing.OutCubic }
    }
    exit: Transition {
        NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 100; easing.type: Easing.InCubic }
    }
}
