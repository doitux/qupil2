// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Text field (or multi-line area) with a small label above it.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: field

    property string label
    property string text
    property string placeholder
    property int inputMethodHints: Qt.ImhNone
    property int echoMode: TextInput.Normal
    property bool multiline: false
    property var completions: []          // suggestions while typing (e.g. composers)
    //! Marked as needing attention (red frame); the editor says why below the field
    property bool invalid: false
    readonly property bool editing: input.activeFocus || area.activeFocus

    //! The user changed the text (the field does not change "text" itself)
    signal edited(string value)
    //! Return pressed in a single-line field
    signal accepted()

    readonly property var matches: {
        const needle = field.text.trim().toLowerCase()
        if (needle.length < 1 || !input.activeFocus)
            return []
        return field.completions.filter(c => c.toLowerCase().indexOf(needle) >= 0 && c !== field.text).slice(0, 6)
    }

    function forceActiveFocus() {
        (field.multiline ? area : input).forceActiveFocus()
    }

    spacing: 6
    Layout.fillWidth: true

    FieldLabel {
        visible: field.label !== ""
        label: field.label
    }

    TextField {
        id: input
        visible: !field.multiline
        Layout.fillWidth: true
        Layout.preferredHeight: 48
        leftPadding: 14
        rightPadding: 14
        text: field.text
        onTextEdited: field.edited(text)
        onAccepted: field.accepted()
        placeholderText: field.placeholder
        inputMethodHints: field.inputMethodHints
        echoMode: field.echoMode
        font.family: Theme.fontFamily
        font.pixelSize: 16
        color: Theme.text
        placeholderTextColor: Theme.textMuted
        selectByMouse: true
        Accessible.name: field.label
        background: Rectangle {
            radius: 12
            color: Theme.surface
            border.width: input.activeFocus || field.invalid ? 2 : 1
            border.color: field.invalid ? Theme.danger : input.activeFocus ? Theme.accent : Theme.border
        }

        Popup {
            y: input.height + 4
            width: input.width
            padding: 4
            visible: field.matches.length > 0
            closePolicy: Popup.CloseOnEscape
            background: Rectangle {
                radius: 12
                color: Theme.surface
                border.width: 1
                border.color: Theme.border
            }
            contentItem: Column {
                Repeater {
                    model: field.matches
                    delegate: ItemDelegate {
                        required property string modelData
                        width: parent ? parent.width : 200
                        height: 40
                        text: modelData
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        onClicked: {
                            field.edited(modelData)
                            input.forceActiveFocus()
                            input.cursorPosition = input.length
                        }
                    }
                }
            }
        }
    }

    TextArea {
        id: area
        visible: field.multiline
        Layout.fillWidth: true
        Layout.preferredHeight: Math.max(96, implicitHeight)
        text: field.text
        onTextChanged: if (field.multiline && text !== field.text) field.edited(text)
        placeholderText: field.placeholder
        padding: 12
        wrapMode: TextEdit.Wrap
        font.family: Theme.fontFamily
        font.pixelSize: 16
        color: Theme.text
        placeholderTextColor: Theme.textMuted
        selectByMouse: true
        Accessible.name: field.label
        background: Rectangle {
            radius: 12
            color: Theme.surface
            border.width: area.activeFocus || field.invalid ? 2 : 1
            border.color: field.invalid ? Theme.danger : area.activeFocus ? Theme.accent : Theme.border
        }
    }
}
