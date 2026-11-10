// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Time of day "hh:mm".

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: field

    property string label
    property string time: "14:00"

    //! The field Tab and Return go to (another TimeField: its time is ready to be typed over)
    property Item nextField: null

    //! The user entered a time; the field does not change "time" itself
    signal picked(string value)

    //! Focus with the cursor at the start: four digits replace the time
    function focusField() {
        input.forceActiveFocus(Qt.TabFocusReason)
        input.cursorPosition = 0
    }
    function goNext(event) {
        // Without a next field Tab and Return do what they always do
        event.accepted = !!field.nextField
        if (!field.nextField)
            return
        if (field.nextField.focusField)
            field.nextField.focusField()
        else
            field.nextField.forceActiveFocus(Qt.TabFocusReason)
    }

    spacing: 6
    Layout.fillWidth: true

    function normalize(t) {
        const m = /^(\d{1,2}):?(\d{0,2})$/.exec(t.trim())
        if (!m)
            return field.time
        const h = Math.min(23, Number(m[1]))
        const min = Math.min(59, Number(m[2] || 0))
        return (h < 10 ? "0" : "") + h + ":" + (min < 10 ? "0" : "") + min
    }

    FieldLabel {
        visible: field.label !== ""
        label: field.label
    }

    TextField {
        id: input
        Layout.fillWidth: true
        Layout.preferredHeight: 48
        horizontalAlignment: Text.AlignHCenter
        text: field.time
        inputMask: "99:99"
        inputMethodHints: Qt.ImhPreferNumbers | Qt.ImhNoPredictiveText
        font.family: Theme.fontFamily
        font.pixelSize: 17
        font.weight: Font.DemiBold
        color: Theme.text
        selectByMouse: true
        Accessible.name: field.label
        // Start → end with Tab (physical keyboard) or the Return key of the on-screen one
        EnterKey.type: field.nextField ? Qt.EnterKeyNext : Qt.EnterKeyDefault
        Keys.onTabPressed: (event) => field.goNext(event)
        Keys.onReturnPressed: (event) => field.goNext(event)
        Keys.onEnterPressed: (event) => field.goNext(event)
        // Every complete time counts at once: "Speichern" may be tapped while the field
        // still has the focus (iPad, iPhone: the keyboard stays open, no Return), and then
        // editingFinished comes too late or not at all
        onTextEdited: {
            if (!acceptableInput)
                return
            const value = field.normalize(text)
            if (value !== field.time)
                field.picked(value)
        }
        onEditingFinished: {
            const value = field.normalize(text)
            text = Qt.binding(() => field.time)
            if (value !== field.time)
                field.picked(value)
        }
        background: Rectangle {
            radius: 12
            color: Theme.surface
            border.width: input.activeFocus ? 2 : 1
            border.color: input.activeFocus ? Theme.accent : Theme.border
        }
    }
}
