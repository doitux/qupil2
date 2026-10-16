// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Settings section "Appearance": light / dark, your two colours and the UI language.
// Lives in the same QML module as Theme.qml and the LanguageManager singleton.
// Both values are stored per device and are not synchronised.

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
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

    // ------------------------------------------------------------ the two colours
    Label {
        Layout.topMargin: 8
        text: qsTrId("settings.colors")
        color: Theme.text
        font.family: Theme.fontFamily
        font.pixelSize: 15
        font.weight: Font.DemiBold
    }
    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: qsTrId("settings.colors.hint")
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 13
    }
    ColorRow {
        label: qsTrId("settings.colors.accent")
        // dark enough for white writing
        colors: ["#6E3C5A", "#A0305F", "#8C1C3A", "#7A4B1E", "#2F6B3A", "#0F6B6B", "#1F4E8C", "#5B3FA0", "#3F4A5A"]
        current: Theme.accentBase
        onPicked: (c) => Theme.accentChoice = Qt.colorEqual(c, Theme.defaultAccent) ? "" : c
        onCustom: colorDialog.choose("accent", Theme.accentBase)
    }
    ColorRow {
        label: qsTrId("settings.colors.highlight")
        // bright, for "now" and the main quick action
        colors: ["#C9F36B", "#FFE066", "#FFB680", "#FF8C7A", "#FFADD2", "#CDB8FF", "#9FD8FF", "#9FF0C8", "#E3DED3"]
        current: Theme.highlightBase
        onPicked: (c) => Theme.highlightChoice = Qt.colorEqual(c, Theme.defaultHighlight) ? "" : c
        onCustom: colorDialog.choose("highlight", Theme.highlightBase)
    }
    ColorRow {
        label: qsTrId("settings.colors.mark")
        // marked text in lesson notes
        colors: ["#B3261E", "#D84315", "#B8860B", "#2E7D32", "#00838F", "#1565C0", "#6A1B9A", "#AD1457"]
        current: Theme.customMark ? Theme.markChoice : Theme.defaultMark
        onPicked: (c) => Theme.markChoice = Qt.colorEqual(c, Theme.defaultMark) ? "" : c
        onCustom: colorDialog.choose("mark", Theme.customMark ? Theme.markChoice : Theme.defaultMark)
    }
    ActionButton {
        visible: Theme.accentChoice !== "" || Theme.highlightChoice !== "" || Theme.markChoice !== ""
        kind: "quiet"
        iconName: "restore"
        text: qsTrId("settings.colors.reset")
        onClicked: {
            Theme.accentChoice = ""
            Theme.highlightChoice = ""
            Theme.markChoice = ""
        }
    }

    ColorDialog {
        id: colorDialog
        property string target
        function choose(which, current) {
            target = which
            selectedColor = current
            open()
        }
        title: target === "accent" ? qsTrId("settings.colors.accent")
             : target === "mark" ? qsTrId("settings.colors.mark") : qsTrId("settings.colors.highlight")
        onAccepted: {
            const c = selectedColor.toString()
            if (target === "accent")
                Theme.accentChoice = Qt.colorEqual(c, Theme.defaultAccent) ? "" : c
            else if (target === "mark")
                Theme.markChoice = Qt.colorEqual(c, Theme.defaultMark) ? "" : c
            else
                Theme.highlightChoice = Qt.colorEqual(c, Theme.defaultHighlight) ? "" : c
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

    // A row of colour circles and one for a colour of your own
    component ColorRow: ColumnLayout {
        id: colorRow
        property string label
        property var colors: []
        property color current
        readonly property bool own: !colors.some(c => Qt.colorEqual(c, current))
        signal picked(string color)
        signal custom()

        Layout.fillWidth: true
        Layout.topMargin: 4
        spacing: 8

        FieldLabel { label: colorRow.label }
        Flow {
            Layout.fillWidth: true
            spacing: 10
            Repeater {
                model: colorRow.colors
                delegate: Swatch {
                    required property string modelData
                    fill: modelData
                    selected: Qt.colorEqual(colorRow.current, modelData)
                    onClicked: colorRow.picked(modelData)
                }
            }
            Swatch {
                // Your own colour (colour dialog); shows it once chosen
                fill: colorRow.own ? colorRow.current : Theme.surfaceAlt
                selected: colorRow.own
                iconName: colorRow.own ? "check" : "plus"
                text: qsTrId("settings.colors.custom")
                onClicked: colorRow.custom()
            }
        }
    }

    component Swatch: AbstractButton {
        id: swatch
        property color fill
        property bool selected
        property string iconName: "check"
        implicitWidth: 40
        implicitHeight: 40
        focusPolicy: Qt.StrongFocus
        hoverEnabled: true
        Accessible.role: Accessible.RadioButton
        Accessible.checked: selected
        Accessible.name: text !== "" ? text : fill.toString()
        ToolTip.visible: hovered && text !== ""
        ToolTip.text: text
        background: Rectangle {
            radius: width / 2
            color: swatch.fill
            border.width: swatch.selected || swatch.visualFocus ? 3 : 1
            border.color: swatch.selected || swatch.visualFocus ? Theme.text : Theme.border
        }
        contentItem: Item {
            Icon {
                anchors.centerIn: parent
                visible: swatch.selected || swatch.iconName === "plus"
                width: 20
                height: 20
                name: swatch.iconName
                color: swatch.iconName === "plus" ? Theme.textMuted : Theme.textOn(swatch.fill)
                strokeWidth: 2.4
            }
        }
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
