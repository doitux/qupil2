// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Rounded surface with padding; children are laid out in a column.

import QtQuick
import QtQuick.Layouts

Rectangle {
    id: card

    default property alias content: column.data
    property int padding: 18
    property alias spacing: column.spacing

    implicitHeight: column.implicitHeight + 2 * padding
    radius: Theme.radiusLarge
    color: Theme.surface

    ColumnLayout {
        id: column
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: card.padding
        spacing: 12
    }
}
