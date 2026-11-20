// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
// A white card with padding; children go into the column
import QtQuick
import QupilStudent

Rectangle {
    id: card
    default property alias content: column.data
    property int padding: 14
    property alias spacing: column.spacing

    color: Theme.surface
    radius: Theme.radius
    implicitHeight: column.implicitHeight + 2 * padding

    Column {
        id: column
        x: card.padding
        y: card.padding
        width: card.width - 2 * card.padding
        spacing: 8
    }
}
