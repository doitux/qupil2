// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Label + drop-down of strings. editable: free text with the list as suggestions.
// A value that is not in the list (e.g. an old location) is shown and kept.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: field

    property string label
    property var options: []
    property string value
    property bool editable: false
    property bool allowEmpty: true

    //! The user picked or typed a value (the field does not change "value" itself)
    signal picked(string value)

    readonly property var items: {
        let list = field.allowEmpty ? [""] : []
        for (const o of field.options)
            if (list.indexOf(o) < 0)
                list.push(o)
        if (field.value !== "" && list.indexOf(field.value) < 0)
            list.push(field.value)
        return list
    }

    spacing: 6
    Layout.fillWidth: true

    FieldLabel {
        visible: field.label !== ""
        label: field.label
    }

    ComboBox {
        id: box
        Layout.fillWidth: true
        Layout.preferredHeight: 48
        model: field.items
        editable: field.editable
        currentIndex: Math.max(0, field.items.indexOf(field.value))
        font.family: Theme.fontFamily
        font.pixelSize: 16
        Accessible.name: field.label
        onActivated: field.picked(currentText)
        onAccepted: field.picked(editText)
        onEditTextChanged: if (field.editable && activeFocus && editText !== field.value) field.picked(editText)
        background: Rectangle {
            radius: 12
            color: Theme.surface
            border.width: box.activeFocus ? 2 : 1
            border.color: box.activeFocus ? Theme.accent : Theme.border
        }
        delegate: ItemDelegate {
            required property string modelData
            required property int index
            width: ListView.view ? ListView.view.width : 200
            height: 44
            text: modelData === "" ? "–" : modelData
            highlighted: box.highlightedIndex === index
            font.family: Theme.fontFamily
            font.pixelSize: 15
        }
    }
}
