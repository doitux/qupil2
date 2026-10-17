// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Rounded card holding MenuRows (with dividers between them)

import QtQuick
import QtQuick.Layouts

Rectangle {
    default property alias rows: groupColumn.data
    Layout.fillWidth: true
    implicitHeight: groupColumn.implicitHeight
    radius: Theme.radius + 2
    color: Theme.surface
    clip: true
    ColumnLayout {
        id: groupColumn
        width: parent.width
        spacing: 0
    }
}
