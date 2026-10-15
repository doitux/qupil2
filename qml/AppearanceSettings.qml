// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Settings section "Appearance": colour scheme (System / Light / Dark) and UI language.
// Lives in the same QML module as Theme.qml and the LanguageManager singleton.
// Both values are stored per device and are not synchronised.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

ColumnLayout {
    id: root
    spacing: 10

    Label {
        text: qsTrId("settings.appearance")
        color: Theme.text
        font.family: Theme.fontFamily
        font.pixelSize: 15
        font.weight: Font.DemiBold
    }

    // Segmented control: track + three equal segments
    Rectangle {
        Layout.fillWidth: true
        implicitHeight: 48
        radius: 12
        color: Theme.segment

        RowLayout {
            anchors.fill: parent
            anchors.margins: 4
            spacing: 4

            Repeater {
                model: ["system", "light", "dark"]

                delegate: AbstractButton {
                    id: segment
                    required property string modelData
                    readonly property bool selected: Theme.mode === modelData

                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    checkable: true
                    checked: selected
                    text: root.modeLabel(modelData)
                    Accessible.role: Accessible.RadioButton
                    Accessible.checked: selected
                    onClicked: Theme.mode = modelData

                    background: Rectangle {
                        radius: 9
                        color: segment.selected ? Theme.segmentActive : "transparent"
                    }
                    contentItem: Text {
                        text: segment.text
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                        color: segment.selected ? Theme.accentText : Theme.textMuted
                        font.family: Theme.fontFamily
                        font.pixelSize: 14
                        font.weight: Font.Bold
                    }
                }
            }
        }
    }

    Label {
        Layout.topMargin: 8
        text: qsTrId("settings.language").replace(/\s*:\s*$/, "")   // 1.4.1 wording ends with a colon
        color: Theme.text
        font.family: Theme.fontFamily
        font.pixelSize: 15
        font.weight: Font.DemiBold
    }

    ComboBox {
        id: languageBox
        Layout.fillWidth: true
        Layout.preferredHeight: Theme.touchTarget
        textRole: "label"
        valueRole: "code"
        // "System language (Deutsch)", then every language in its own name
        model: LanguageManager.languages.map(l => ({
            code: l.code,
            label: l.code === "system"
                   ? qsTrId("language.system") + " (" + root.nativeName(LanguageManager.effectiveLanguage) + ")"
                   : l.nativeName
        }))
        currentIndex: LanguageManager.languages.findIndex(l => l.code === LanguageManager.language)
        onActivated: LanguageManager.language = currentValue
        Accessible.name: qsTrId("settings.language")
    }

    function modeLabel(mode) {
        switch (mode) {
        case "light": return qsTrId("appearance.light")
        case "dark": return qsTrId("appearance.dark")
        default: return qsTrId("appearance.system")
        }
    }

    function nativeName(code) {
        const hit = LanguageManager.languages.find(l => l.code === code)
        return hit ? hit.nativeName : code
    }
}
