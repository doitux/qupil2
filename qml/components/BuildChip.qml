// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// The commit of this build, always in sight while testing; a tap copies
// version and commit (for bug reports).

import QtQuick
import QtQuick.Controls
import Qupil

AbstractButton {
    id: chip

    implicitWidth: label.implicitWidth + 16
    implicitHeight: label.implicitHeight + 8
    focusPolicy: Qt.StrongFocus
    hoverEnabled: true
    Accessible.role: Accessible.Button
    Accessible.name: "Qupil " + App.versionText

    Tip {
        text: "Qupil " + App.versionText
        delay: 500
    }

    onClicked: {
        App.copyText("Qupil " + App.versionText)
        Nav.toast(qsTrId("app.build_copied").arg(App.versionText))
    }

    background: Rectangle {
        radius: height / 2
        color: chip.down ? Theme.segmentActive : Theme.segment
        border.width: chip.visualFocus ? 2 : 0
        border.color: Theme.accentText
    }
    contentItem: Text {
        id: label
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        text: qsTrId("app.build").arg(App.build)
        color: Theme.textMuted
        font.family: "monospace"
        font.pixelSize: 12
        font.weight: Font.DemiBold
    }
}
