// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Editable list of short texts (lesson locations, styles, sizes) as chips:
// tap removes one, press and hold and drag changes the order (a mark shows where
// it goes), Ctrl or Alt + arrow keys move the focused one.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    id: editor

    property string label
    property var items: []
    property string entry
    //! The line about tapping and dragging (once is enough where several lists follow)
    property bool hint: true

    signal changed(var items)

    function add() {
        const v = entry.trim()
        if (v === "" || items.indexOf(v) >= 0)
            return
        changed(items.concat([v]))
        entry = ""
    }
    function moveTo(from, to) {
        if (from === to || to < 0 || to >= items.length)
            return
        const list = items.slice()
        const [v] = list.splice(from, 1)
        list.splice(to, 0, v)
        changed(list)
    }

    // ---------------------------------------------------------------- drag to reorder
    property int dragIndex: -1
    property int insertBefore: -1      // chip index the dragged one goes in front of (items.length: at the end)
    property point dragPoint           // finger in the flow
    property point grabOffset          // finger within the chip
    property Item page: null           // the page's Flickable: it does not scroll meanwhile

    readonly property int targetIndex: dragIndex < 0 || insertBefore < 0 ? -1
                                       : insertBefore > dragIndex ? insertBefore - 1 : insertBefore

    function startDrag(index, chip, scenePoint) {
        if (dragIndex >= 0 || items.length < 2)
            return
        const local = chip.mapFromItem(null, scenePoint.x, scenePoint.y)
        grabOffset = Qt.point(local.x, local.y)
        dragIndex = index
        dragTo(scenePoint)
        for (let p = editor.parent; p; p = p.parent) {
            if (typeof p.flick === "function" && p.interactive !== undefined) {
                page = p
                page.interactive = false
                break
            }
        }
    }
    function dragTo(scenePoint) {
        if (dragIndex < 0)
            return
        const p = flow.mapFromItem(null, scenePoint.x, scenePoint.y)
        dragPoint = Qt.point(p.x, p.y)
        // In front of the first chip that is on a later line, or on this line right of the finger
        let before = items.length
        for (let i = 0; i < chips.count; ++i) {
            const c = chips.itemAt(i)
            if (i === dragIndex || !c)
                continue
            if (p.y < c.y || (p.y < c.y + c.height && p.x < c.x + c.width / 2)) {
                before = i
                break
            }
        }
        insertBefore = before
    }
    function drop() {
        if (dragIndex < 0)
            return
        const from = dragIndex
        const to = targetIndex
        endDrag()
        // After the finger's handler returns: the new order rebuilds the chips
        Qt.callLater(moveTo, from, to)
    }
    function endDrag() {
        if (page)
            page.interactive = true
        page = null
        dragIndex = -1
        insertBefore = -1
    }
    onItemsChanged: endDrag()

    spacing: 8
    Layout.fillWidth: true

    FieldLabel {
        visible: editor.label !== ""
        label: editor.label
    }
    Item {
        Layout.fillWidth: true
        implicitHeight: flow.implicitHeight
        z: 2
        Flow {
            id: flow
            width: parent.width
            spacing: 6
            Repeater {
                id: chips
                model: editor.items
                delegate: AbstractButton {
                    id: chip
                    required property string modelData
                    required property int index
                    readonly property bool dragged: editor.dragIndex === index
                    implicitHeight: 34
                    implicitWidth: chipRow.implicitWidth + 24
                    focusPolicy: Qt.StrongFocus
                    opacity: dragged ? 0.35 : 1
                    Accessible.role: Accessible.Button
                    Accessible.name: qsTrId("common.remove") + " " + modelData
                    Accessible.onIncreaseAction: editor.moveTo(index, index + 1)
                    Accessible.onDecreaseAction: editor.moveTo(index, index - 1)
                    Keys.onPressed: (event) => {
                        if (!(event.modifiers & (Qt.ControlModifier | Qt.AltModifier)))
                            return
                        if (event.key === Qt.Key_Left || event.key === Qt.Key_Up)
                            editor.moveTo(index, index - 1)
                        else if (event.key === Qt.Key_Right || event.key === Qt.Key_Down)
                            editor.moveTo(index, index + 1)
                        else
                            return
                        event.accepted = true
                    }
                    onClicked: {
                        const list = editor.items.slice()
                        list.splice(index, 1)
                        editor.changed(list)
                    }
                    // Holding lifts it (no removal then); the finger moves it from there
                    onPressAndHold: editor.startDrag(index, chip, tracker.point.scenePosition)
                    PointHandler {
                        id: tracker
                        onPointChanged: if (active && chip.dragged) editor.dragTo(point.scenePosition)
                        onActiveChanged: if (!active && chip.dragged) editor.drop()
                    }
                    background: Rectangle {
                        radius: height / 2
                        color: Theme.surfaceAlt
                        border.width: chip.visualFocus ? 2 : 1
                        border.color: chip.visualFocus ? Theme.accent : Theme.border
                    }
                    contentItem: Item {
                        RowLayout {
                            id: chipRow
                            anchors.centerIn: parent
                            spacing: 6
                            Text {
                                text: chip.modelData
                                color: Theme.text
                                font.family: Theme.fontFamily
                                font.pixelSize: 14
                                font.weight: Font.DemiBold
                            }
                            Icon {
                                Layout.preferredWidth: 14
                                Layout.preferredHeight: 14
                                name: "close"
                                color: Theme.textMuted
                                strokeWidth: 2.4
                            }
                        }
                    }
                }
            }
        }

        // Where the dragged chip goes (over the lifted one, which the finger holds)
        Rectangle {
            id: mark
            z: 2
            readonly property Item next: editor.insertBefore >= 0 && editor.insertBefore < chips.count
                                         ? chips.itemAt(editor.insertBefore) : null
            readonly property Item last: chips.count > 0
                                         ? chips.itemAt(editor.dragIndex === chips.count - 1 ? chips.count - 2 : chips.count - 1) : null
            visible: editor.dragIndex >= 0 && editor.targetIndex !== editor.dragIndex && (next !== null || last !== null)
            x: next ? next.x - flow.spacing / 2 - width / 2 : last ? last.x + last.width + flow.spacing / 2 - width / 2 : 0
            y: next ? next.y : last ? last.y : 0
            width: 3
            height: 34
            radius: 1.5
            color: Theme.accent
        }

        // The lifted chip under the finger
        Rectangle {
            visible: editor.dragIndex >= 0
            x: editor.dragPoint.x - editor.grabOffset.x
            y: editor.dragPoint.y - editor.grabOffset.y
            width: liftedLabel.implicitWidth + 24
            height: 34
            radius: height / 2
            color: Theme.dark ? Theme.surfaceAlt : Theme.surface
            border.width: 1
            border.color: Theme.accent
            scale: 1.06
            Rectangle {
                z: -1
                anchors.fill: parent
                anchors.topMargin: 4
                anchors.bottomMargin: -4
                radius: parent.radius
                color: Theme.shadow
            }
            Text {
                id: liftedLabel
                anchors.centerIn: parent
                text: editor.dragIndex >= 0 && editor.dragIndex < editor.items.length ? editor.items[editor.dragIndex] : ""
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 14
                font.weight: Font.DemiBold
            }
        }
    }
    Text {
        visible: editor.hint && editor.items.length > 0
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: editor.items.length > 1 ? qsTrId("listeditor.hint_reorder") : qsTrId("listeditor.hint_remove")
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 12
    }
    RowLayout {
        Layout.fillWidth: true
        spacing: 8
        InputField {
            Layout.fillWidth: true
            placeholder: qsTrId("common.add_ellipsis")
            text: editor.entry
            onEdited: (value) => editor.entry = value
            onAccepted: editor.add()
        }
        ActionButton {
            Layout.alignment: Qt.AlignBottom
            iconName: "plus"
            text: qsTrId("common.add")
            // A tap leaves the focus (and the keyboard) in the field for the next entry
            focusPolicy: Qt.TabFocus
            enabled: editor.entry.trim() !== ""
            onClicked: editor.add()
        }
    }
}
