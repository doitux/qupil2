// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// More › About Qupil › License text: the GPL with Qupil's app store
// permission, and the licenses of the parts that come with Qupil.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page
    title: qsTrId("about.license.show")
    backText: qsTrId("about.title")

    Card {
        Layout.fillWidth: true
        TextEdit {
            Layout.fillWidth: true
            readOnly: true
            selectByMouse: true
            wrapMode: TextEdit.Wrap
            textFormat: TextEdit.PlainText
            text: App.licenseText()
            color: Theme.textSecondary
            selectionColor: Theme.accentSoft
            selectedTextColor: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 13
        }
    }
}
