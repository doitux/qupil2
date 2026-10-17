// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// One event: details, programme in order, playing time. Setting it to "done"
// can enter it as an activity for every performer and finish the pieces.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Item {
    id: detail

    property string eventId
    property bool showBack: true
    property string backText: qsTrId("common.back")
    readonly property var event: { if (Data.revision < 0) return undefined; return Data.event(eventId) }
    readonly property var programme: event.programme || []

    signal back()

    function move(index, delta) {
        moveTo(index, index + delta)
    }
    function moveTo(from, to) {
        const ids = programme.map(p => p.itemId)
        if (from === to || to < 0 || to >= ids.length)
            return false
        const [item] = ids.splice(from, 1)
        ids.splice(to, 0, item)
        return Data.reorderProgramme(eventId, ids)
    }

    // ---------------------------------------------------------------- drag to reorder
    // The lifted row follows the finger, the others move aside (no model change
    // until it is dropped). Near the top or bottom edge the page scrolls along.
    property int dragIndex: -1
    property int dropIndex: -1
    property real dragOffset: 0
    property bool settling: false
    property real dragStartY: 0       // scene y where it was lifted
    property real dragStartContentY: 0
    property real dragPointerY: 0     // scene y of the finger now
    property real dragHeight: 0

    function startDrag(index, sceneY) {
        if (dragIndex >= 0 || programme.length < 2)
            return
        const row = programmeRows.itemAt(index)
        if (!row)
            return
        dragHeight = row.height
        dragStartY = dragPointerY = sceneY
        dragStartContentY = flick.contentY
        dragOffset = 0
        dropIndex = index
        settling = false
        dragIndex = index
        flick.interactive = false   // the finger moves the row now, not the page
    }
    function dragTo(sceneY) {
        if (dragIndex < 0 || settling)
            return
        dragPointerY = sceneY
        updateDrag()
    }
    function updateDrag() {
        const row = programmeRows.itemAt(dragIndex)
        if (!row)
            return
        const offset = dragPointerY - dragStartY + flick.contentY - dragStartContentY
        dragOffset = Math.max(-row.y, Math.min(programmeColumn.height - row.y - row.height, offset))
        // New place: a row below is passed once the lower edge crosses its middle,
        // a row above once the upper edge does
        const top = row.y + dragOffset
        const bottom = top + row.height
        let place = 0
        for (let i = 0; i < programme.length; ++i) {
            const other = programmeRows.itemAt(i)
            if (i === dragIndex || !other)
                continue
            const middle = other.y + other.height / 2
            if (i < dragIndex ? top >= middle : bottom > middle)
                ++place
        }
        dropIndex = place
    }
    function shiftFor(index) {
        if (dragIndex < 0 || index === dragIndex)
            return 0
        if (index > dragIndex && index <= dropIndex)
            return -dragHeight
        if (index < dragIndex && index >= dropIndex)
            return dragHeight
        return 0
    }
    function drop() {
        if (dragIndex < 0 || settling)
            return
        flick.interactive = true
        // Glide into the gap, then store the order
        let target = 0
        if (dropIndex > dragIndex)
            for (let i = dragIndex + 1; i <= dropIndex; ++i) target += programmeRows.itemAt(i).height
        else
            for (let j = dropIndex; j < dragIndex; ++j) target -= programmeRows.itemAt(j).height
        settling = true
        settle.to = target
        settle.restart()
    }
    function finishDrag() {
        const from = dragIndex
        const to = dropIndex
        // The rows stay as they are until the new order arrives (no flash of the old one)
        if (!moveTo(from, to))
            resetDrag()
        else
            resetSoon.restart()
    }
    function resetDrag() {
        resetSoon.stop()
        settle.stop()
        flick.interactive = true
        dragIndex = -1
        dropIndex = -1
        dragOffset = 0
        settling = false
    }
    onProgrammeChanged: resetDrag()

    NumberAnimation {
        id: settle
        target: detail
        property: "dragOffset"
        duration: 140
        easing.type: Easing.OutCubic
        onFinished: detail.finishDrag()
    }
    Timer {
        id: resetSoon   // in case the new order never arrives
        interval: 600
        onTriggered: detail.resetDrag()
    }
    Timer {
        // Scrolls along near the top or bottom edge
        interval: 16
        repeat: true
        running: detail.dragIndex >= 0 && !detail.settling
        onTriggered: {
            const top = flick.mapToItem(null, 0, 0).y
            const edge = 56
            let step = 0
            if (detail.dragPointerY < top + edge)
                step = -Math.ceil((top + edge - detail.dragPointerY) / 6)
            else if (detail.dragPointerY > top + flick.height - edge)
                step = Math.ceil((detail.dragPointerY - (top + flick.height - edge)) / 6)
            if (step === 0)
                return
            const y = Math.max(0, Math.min(flick.contentHeight - flick.height, flick.contentY + step))
            if (y !== flick.contentY) {
                flick.contentY = y
                detail.updateDrag()
            }
        }
    }

    Flickable {
        id: flick
        anchors.fill: parent
        clip: true
        contentHeight: column.implicitHeight + 60
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: ScrollBar {}

        ColumnLayout {
            id: column
            x: 20
            y: detail.showBack ? 12 : 28
            width: flick.width - 40
            spacing: 16

            PageHeader {
                visible: detail.showBack
                Layout.leftMargin: -8
                backText: detail.backText
                onBack: detail.back()
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: 8
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: detail.event.title || ""
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 26
                    font.weight: Font.ExtraBold
                }
                IconButton {
                    iconName: "edit"
                    text: qsTrId("event.edit")
                    onClicked: editor.edit(detail.eventId)
                }
                IconButton {
                    id: moreButton
                    iconName: "more"
                    text: qsTrId("common.more_actions")
                    onClicked: eventMenu.popup(moreButton, moreButton.width - eventMenu.width, moreButton.height + 4)
                }
            }

            Segmented {
                Layout.maximumWidth: 360
                options: [0, 1]
                value: detail.event.state || 0
                labelFor: (v) => v === 0 ? qsTrId("event.state.planned") : qsTrId("event.state.done")
                onActivated: (v) => {
                    if (v === 1 && detail.programme.length > 0)
                        finish.open()
                    else
                        Data.setEventState(detail.eventId, v, false, false)
                }
            }

            // ------------------------------------------------------------ details
            Card {
                Layout.fillWidth: true
                GridLayout {
                    Layout.fillWidth: true
                    columns: column.width > 520 ? 3 : 2
                    columnSpacing: 16
                    rowSpacing: 14
                    Info { label: qsTrId("common.date"); value: Data.formatDate(detail.event.date || "", true) }
                    Info { label: qsTrId("common.time"); value: detail.event.time || "" }
                    Info { label: qsTrId("common.location"); value: detail.event.location || "" }
                    Info { label: qsTrId("event.organizer"); value: detail.event.organizer || "" }
                    Info { label: qsTrId("event.accompanist"); value: detail.event.accompanist || "" }
                    Info {
                        label: qsTrId("event.label.contributions")
                        value: qsTrId("event.contributions", detail.programme.length)
                               + (detail.event.externalCount > 0 ? " (" + qsTrId("event.external_count", detail.event.externalCount) + ")" : "")
                    }
                }
            }

            // ------------------------------------------------------------ programme
            RowLayout {
                Layout.fillWidth: true
                Text {
                    Layout.fillWidth: true
                    text: qsTrId("event.programme")
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 20
                    font.weight: Font.ExtraBold
                }
            }

            EmptyHint {
                visible: detail.programme.length === 0
                text: qsTrId("event.programme_empty")
            }

            Text {
                visible: detail.programme.length > 1
                Layout.fillWidth: true
                Layout.topMargin: -8
                wrapMode: Text.WordWrap
                text: qsTrId("event.reorder_hint")
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 13
            }

            Rectangle {
                visible: detail.programme.length > 0
                Layout.fillWidth: true
                implicitHeight: programmeColumn.implicitHeight
                radius: Theme.radius
                color: Theme.surface
                ColumnLayout {
                    id: programmeColumn
                    width: parent.width
                    spacing: 0
                    Repeater {
                        id: programmeRows
                        model: detail.programme
                        // Press and hold (or grab the handle) and drag: the others make room
                        delegate: Item {
                            id: item
                            required property var modelData
                            required property int index
                            readonly property bool dragged: detail.dragIndex === index
                            // Place while dragging (numbering and divider follow it)
                            readonly property int visualIndex: dragged ? detail.dropIndex : index + Math.sign(detail.shiftFor(index))
                            Layout.fillWidth: true
                            implicitHeight: itemRow.implicitHeight + 24
                            z: dragged ? 10 : 0
                            scale: dragged && !detail.settling ? 1.02 : 1
                            Behavior on scale { NumberAnimation { duration: 120 } }
                            transform: Translate {
                                id: shift
                                y: item.dragged ? detail.dragOffset : detail.shiftFor(item.index)
                                Behavior on y {
                                    enabled: !item.dragged
                                    NumberAnimation { duration: 160; easing.type: Easing.OutCubic }
                                }
                            }
                            Accessible.role: Accessible.ListItem
                            Accessible.name: (index + 1) + ". " + (modelData.composer ? modelData.composer + " – " : "") + modelData.title

                            // Lifted card while dragging
                            Item {
                                anchors.fill: parent
                                opacity: item.dragged ? 1 : 0
                                visible: opacity > 0
                                Behavior on opacity { NumberAnimation { duration: 120 } }
                                Repeater {
                                    model: [{ grow: 1, y: 2, alpha: 0.5 }, { grow: 5, y: 6, alpha: 0.22 }]
                                    delegate: Rectangle {
                                        required property var modelData
                                        x: -modelData.grow
                                        y: modelData.y - modelData.grow
                                        width: parent.width + 2 * modelData.grow
                                        height: parent.height + 2 * modelData.grow
                                        radius: Theme.radiusSmall + modelData.grow
                                        color: Theme.shadow
                                        opacity: modelData.alpha
                                    }
                                }
                                Rectangle {
                                    anchors.fill: parent
                                    radius: Theme.radiusSmall
                                    color: Theme.dark ? Theme.surfaceAlt : Theme.surface
                                    border.width: 1
                                    border.color: Theme.border
                                }
                            }
                            Rectangle {
                                visible: item.visualIndex > 0 && !item.dragged
                                width: parent.width
                                height: 1
                                color: Theme.divider
                            }

                            TapHandler {
                                // Holding still lifts it; moving first scrolls the page as usual
                                onLongPressed: detail.startDrag(item.index, tracker.point.scenePosition.y)
                            }
                            // Only watches (passive): follows the finger once it is lifted
                            PointHandler {
                                id: tracker
                                onPointChanged: if (active && item.dragged) detail.dragTo(point.scenePosition.y)
                                onActiveChanged: if (!active && item.dragged) detail.drop()
                            }

                            RowLayout {
                                id: itemRow
                                anchors.left: parent.left
                                anchors.right: parent.right
                                anchors.verticalCenter: parent.verticalCenter
                                anchors.leftMargin: 14
                                anchors.rightMargin: 4
                                spacing: 10
                                Text {
                                    Layout.preferredWidth: 22
                                    text: item.visualIndex + 1
                                    color: Theme.accentText
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 15
                                    font.weight: Font.ExtraBold
                                }
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 2
                                    RowLayout {
                                        Layout.fillWidth: true
                                        spacing: 6
                                        Text {
                                            Layout.fillWidth: true
                                            wrapMode: Text.WordWrap
                                            text: (item.modelData.composer ? item.modelData.composer + " – " : "") + item.modelData.title
                                            color: Theme.text
                                            font.family: Theme.fontFamily
                                            font.pixelSize: 15
                                            font.weight: Font.Bold
                                        }
                                        Rectangle {
                                            visible: item.modelData.external
                                            implicitWidth: externalLabel.implicitWidth + 12
                                            implicitHeight: externalLabel.implicitHeight + 4
                                            radius: 6
                                            color: Theme.tile.yellow.bg
                                            Text {
                                                id: externalLabel
                                                anchors.centerIn: parent
                                                text: qsTrId("event.external_badge")
                                                color: Theme.tile.yellow.fg
                                                font.family: Theme.fontFamily
                                                font.pixelSize: 11
                                                font.weight: Font.ExtraBold
                                            }
                                        }
                                    }
                                    Text {
                                        Layout.fillWidth: true
                                        wrapMode: Text.WordWrap
                                        text: item.modelData.performers
                                        color: Theme.textMuted
                                        font.family: Theme.fontFamily
                                        font.pixelSize: 13
                                    }
                                }
                                Text {
                                    text: item.modelData.duration > 0 ? item.modelData.duration + " " + qsTrId("common.unit.min") : ""
                                    color: Theme.textSecondary
                                    font.family: Theme.fontFamily
                                    font.pixelSize: 14
                                    font.weight: Font.DemiBold
                                }
                                IconButton {
                                    size: 36
                                    iconName: "close"
                                    fill: "transparent"
                                    tint: Theme.textMuted
                                    text: qsTrId("common.remove")
                                    onClicked: Data.removeFromProgramme(item.modelData.itemId)
                                }
                                // Handle: drags at once (mouse and finger); arrow keys move it too
                                Item {
                                    id: grip
                                    visible: detail.programme.length > 1
                                    implicitWidth: 36
                                    implicitHeight: 44
                                    activeFocusOnTab: true
                                    Accessible.role: Accessible.Button
                                    Accessible.name: qsTrId("event.reorder")
                                    Accessible.onIncreaseAction: detail.move(item.index, 1)
                                    Accessible.onDecreaseAction: detail.move(item.index, -1)
                                    Keys.onUpPressed: detail.move(item.index, -1)
                                    Keys.onDownPressed: detail.move(item.index, 1)
                                    HoverHandler { cursorShape: item.dragged ? Qt.ClosedHandCursor : Qt.OpenHandCursor }
                                    TapHandler {
                                        // Passive, so the row's tracker sees the finger as well
                                        onPressedChanged: {
                                            if (pressed)
                                                detail.startDrag(item.index, point.scenePosition.y)
                                            else if (item.dragged && !tracker.active)
                                                detail.drop()
                                        }
                                    }
                                    Rectangle {
                                        anchors.centerIn: parent
                                        width: 32
                                        height: 32
                                        radius: 8
                                        color: Theme.text
                                        opacity: item.dragged ? 0.1 : 0
                                    }
                                    Rectangle {
                                        anchors.centerIn: parent
                                        width: 32
                                        height: 32
                                        radius: 8
                                        color: "transparent"
                                        border.width: grip.activeFocus ? 2 : 0
                                        border.color: Theme.accentText
                                    }
                                    Icon {
                                        anchors.centerIn: parent
                                        width: 20
                                        height: 20
                                        name: "grip"
                                        color: item.dragged ? Theme.accentText : Theme.icon
                                    }
                                }
                            }
                        }
                    }
                }
            }

            // Totals
            Card {
                visible: detail.programme.length > 0
                Layout.fillWidth: true
                spacing: 6
                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        Layout.fillWidth: true
                        text: qsTrId("event.net_playing_time")
                        color: Theme.textSecondary
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                    }
                    Text {
                        text: (detail.event.playingMinutes || 0) + " " + qsTrId("common.unit.min")
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.Bold
                    }
                }
                RowLayout {
                    Layout.fillWidth: true
                    Text {
                        Layout.fillWidth: true
                        text: qsTrId("event.estimated_total")
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 16
                        font.weight: Font.Bold
                    }
                    Text {
                        text: (detail.event.totalMinutes || 0) + " " + qsTrId("common.unit.min")
                        color: Theme.accentText
                        font.family: Theme.fontFamily
                        font.pixelSize: 18
                        font.weight: Font.ExtraBold
                    }
                }
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: qsTrId("event.duration_breakdown").arg(detail.event.moderationMinutes || 0).arg(detail.event.betweenMinutes || 0)
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                }
            }

            Flow {
                Layout.fillWidth: true
                spacing: 10
                ActionButton {
                    kind: "primary"
                    iconName: "plus"
                    text: qsTrId("event.add_ready")
                    onClicked: picker.start(detail.eventId)
                }
                ActionButton {
                    iconName: "plus"
                    text: qsTrId("event.add_external")
                    onClicked: external.start(detail.eventId)
                }
            }
        }
    }

    AppMenu {
        id: eventMenu
        AppMenuItem {
            iconName: "document"
            text: qsTrId("event.export_program")
            onTriggered: Docs.programme(detail.eventId)
        }
        AppMenuItem {
            iconName: "archive"
            text: qsTrId("event.archive")
            onTriggered: {
                Docs.archiveEvent(detail.eventId)
                Nav.toast(qsTrId("archive.saved").arg(detail.event.title || ""))
            }
        }
        AppMenuSeparator {}
        AppMenuItem {
            iconName: "archive"
            destructive: true
            text: qsTrId("event.archive_delete")
            onTriggered: archiveAndDelete.open()
        }
        AppMenuItem {
            iconName: "trash"
            destructive: true
            text: qsTrId("event.delete.title")
            onTriggered: confirmDelete.open()
        }
    }

    AppDialog {
        id: archiveAndDelete
        iconName: "archive"
        destructive: true
        title: qsTrId("event.archive_delete")
        text: qsTrId("event.delete.confirm").arg(detail.event.title || "")
        rejectText: qsTrId("common.cancel")
        acceptText: qsTrId("event.archive_delete")
        onAccepted: {
            const title = detail.event.title || ""
            Docs.archiveEvent(detail.eventId)
            Data.deleteEvent(detail.eventId)
            Nav.toast(qsTrId("archive.saved").arg(title))
            detail.back()
        }
    }

    EventEditor { id: editor }
    ReadyPiecesPicker { id: picker }
    ExternalEditor { id: external }

    AppDialog {
        id: confirmDelete
        iconName: "trash"
        destructive: true
        title: qsTrId("event.delete.title")
        text: qsTrId("event.delete.confirm").arg(detail.event.title || "")
        rejectText: qsTrId("common.cancel")
        acceptText: qsTrId("common.delete")
        onAccepted: {
            Data.deleteEvent(detail.eventId)
            detail.back()
        }
    }

    AppDialog {
        id: finish
        iconName: "check"
        title: qsTrId("event.finish.title")
        text: qsTrId("event.finish.intro")
        rejectText: qsTrId("common.cancel")
        acceptText: qsTrId("common.ok")
        onAccepted: Data.setEventState(detail.eventId, 1, activitiesSwitch.checked, piecesSwitch.checked)
        SwitchRow {
            id: activitiesSwitch
            text: qsTrId("common.activities")
            hint: qsTrId("event.finish.activities")
            checked: true
        }
        SwitchRow {
            id: piecesSwitch
            text: qsTrId("common.pieces")
            hint: qsTrId("event.finish.pieces")
            checked: true
        }
    }

    component Info: ColumnLayout {
        property string label
        property string value
        Layout.fillWidth: true
        spacing: 2
        FieldLabel { label: parent.label }
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: parent.value !== "" ? parent.value : "–"
            color: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 15
            font.weight: Font.DemiBold
        }
    }
}
