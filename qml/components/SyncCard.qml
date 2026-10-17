// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Sync status (Nextcloud and Wi-Fi) with "Sync now" and "Pair device": at the top
// of the settings. compact: only the status line (on "More" when there is a problem).

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Rectangle {
    id: card

    property bool compact: false
    property date now: new Date()
    signal openSync()

    // Title and detail line of the sync card
    readonly property bool wifiUsed: Wifi.peers.length > 0
    readonly property bool syncUsed: Sync.connected || wifiUsed
    readonly property bool syncSetUp: (Sync.connected && !Sync.needsPassword) || wifiUsed
    readonly property bool wifiPaused: Wifi.peers.some(p => p.paused || p.problem !== "")
    readonly property bool syncWarn: (Sync.connected && (Sync.needsPassword || Sync.error !== "")) || wifiPaused
    readonly property string syncTitle: {
        if (Sync.loggingIn || !syncUsed) return qsTrId("sync.title")
        if (Sync.connected && Sync.needsPassword) return Sync.folderExists ? qsTrId("sync.password.enter_title") : qsTrId("sync.password.choose_title")
        if (Sync.busy || Wifi.busy) return qsTrId("sync.in_progress")
        if ((Sync.connected && Sync.error !== "") || wifiPaused) return qsTrId("sync.status.problem")
        if (Sync.lastSync === "" && Wifi.lastSync === "") return qsTrId("sync.never")
        return qsTrId("sync.all_synced")
    }
    readonly property string syncDetail: {
        if (Sync.loggingIn) return qsTrId("sync.nextcloud.waiting")
        if (!syncUsed) return qsTrId("sync.status.off")
        if (Sync.connected && Sync.needsPassword) return qsTrId("sync.nextcloud.connected_as").arg(Sync.user).arg(Sync.server.replace(/^https?:\/\//, ""))
        if (Sync.connected && Sync.error !== "") return Sync.error
        const parts = []
        if (Sync.connected)
            parts.push(qsTrId("sync.nextcloud.when").arg(Labels.syncWhen(Sync.lastSync, card.now)))
        if (wifiPaused)
            parts.push(qsTrId("wifi.paused"))
        else if (Wifi.lastPeer !== "")
            parts.push(qsTrId("sync.wifi.when").arg(Wifi.lastPeer).arg(Labels.syncWhen(Wifi.lastSync, card.now)))
        return parts.join(" · ")
    }
    readonly property bool warn: syncWarn
    readonly property var tile: !syncUsed ? Theme.tile.gray : syncWarn ? Theme.tile.orange : Theme.tile.green

    Timer {
        interval: 30000
        running: card.visible
        repeat: true
        onTriggered: card.now = new Date()
    }

    implicitHeight: syncColumn.implicitHeight + 28
    radius: Theme.radiusLarge - 2
    color: Theme.surface

    ColumnLayout {
        id: syncColumn
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 14
        spacing: 12

        AbstractButton {
            id: syncHeader
            Layout.fillWidth: true
            implicitHeight: syncHeaderRow.implicitHeight
            focusPolicy: Qt.StrongFocus
            Accessible.role: Accessible.Button
            Accessible.name: card.syncTitle + ", " + card.syncDetail
            onClicked: card.openSync()
            background: Rectangle {
                radius: 10
                color: "transparent"
                border.width: syncHeader.visualFocus ? 2 : 0
                border.color: Theme.accentText
            }
            contentItem: RowLayout {
                id: syncHeaderRow
                spacing: 12
                Rectangle {
                    implicitWidth: 44
                    implicitHeight: 44
                    radius: 14
                    color: card.tile.bg
                    Icon {
                        anchors.centerIn: parent
                        width: 24
                        height: 24
                        name: !card.syncUsed ? "cloud" : card.warn ? "cloudOff" : "sync"
                        color: card.tile.fg
                    }
                }
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 2
                    Text {
                        Layout.fillWidth: true
                        elide: Text.ElideRight
                        text: card.syncTitle
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 16
                        font.weight: Font.ExtraBold
                    }
                    Text {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        maximumLineCount: 2
                        elide: Text.ElideRight
                        text: card.syncDetail
                        color: Theme.textMuted
                        font.family: Theme.fontFamily
                        font.pixelSize: 13
                    }
                }
                Icon {
                    Layout.preferredWidth: 18
                    Layout.preferredHeight: 18
                    name: "chevron"
                    color: Theme.icon
                }
            }
        }

        RowLayout {
            visible: !card.compact
            Layout.fillWidth: true
            spacing: 8
            ActionButton {
                Layout.fillWidth: true
                Layout.preferredWidth: 1
                kind: "primary"
                text: card.syncSetUp ? qsTrId("sync.now") : qsTrId("sync.setup_short")
                enabled: !card.syncSetUp || !(Sync.busy || Wifi.busy)
                onClicked: {
                    if (!card.syncSetUp) {
                        card.openSync()
                        return
                    }
                    Sync.syncNow()
                    Wifi.syncNow()
                }
            }
            ActionButton {
                Layout.fillWidth: true
                Layout.preferredWidth: 1
                text: qsTrId("sync.pair")
                onClicked: Nav.pairDevice()
            }
        }
    }
}
