// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Thin line between groups of an AppMenu

import QtQuick
import QtQuick.Controls

MenuSeparator {
    implicitHeight: visible ? contentItem.implicitHeight + topPadding + bottomPadding : 0
    leftPadding: 12
    rightPadding: 12
    topPadding: 5
    bottomPadding: 5
    contentItem: Rectangle {
        implicitWidth: 1
        implicitHeight: 1
        color: Theme.divider
    }
    background: null
}
