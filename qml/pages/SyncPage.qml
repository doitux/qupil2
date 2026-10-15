// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// More › Sync: connect to Nextcloud, choose or enter the sync password,
// what happens to the data already on this device, devices.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page
    title: qsTrId("sync.title")

    property date now: new Date()
    property string serverAddress: Sync.server.replace(/^https:\/\//, "")
    property string password
    property string repeat
    property bool showPassword: false
    property int joinMode: Sync.ReplaceLocal
    property string deviceNameDraft

    readonly property string host: Sync.server.replace(/^https?:\/\//, "")
    //! Data that would be affected when joining an existing folder
    readonly property bool hasLocalData: App.info.awaitingSync !== true
        && ((App.info.students ?? 0) + (App.info.lessons ?? 0) + (App.info.events ?? 0)) > 0
    readonly property bool joining: Sync.folderExists && hasLocalData
    //! A new password is chosen (first device or starting over)
    readonly property bool choosing: !Sync.folderExists || (joining && joinMode === Sync.StartOver)
    readonly property string passwordProblem: !choosing ? ""
        : password.length < 8 ? qsTrId("sync.password.too_short")
        : repeat !== password ? qsTrId("sync.password.mismatch")
        : ""

    function submitPassword() {
        if (Sync.busy || password === "" || passwordProblem !== "")
            return
        Sync.setPassword(password, joining ? joinMode : Sync.Merge)
    }

    Component.onCompleted: deviceNameDraft = Sync.deviceName
    Component.onDestruction: Sync.deviceName = deviceNameDraft

    Timer {
        interval: 30000
        running: true
        repeat: true
        onTriggered: page.now = new Date()
    }
    Connections {
        target: Sync
        function onAccountChanged() {
            if (!Sync.needsPassword) {
                page.password = ""
                page.repeat = ""
            }
        }
    }

    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: qsTrId("sync.intro")
        color: Theme.textSecondary
        font.family: Theme.fontFamily
        font.pixelSize: 15
    }

    Banner {
        visible: App.info.awaitingSync === true
        iconName: "info"
        text: qsTrId("sync.waiting_for_data")
    }

    // ---------------------------------------------------------------- Nextcloud account
    SectionLabel { Layout.topMargin: 6; text: "Nextcloud" }
    Card {
        Layout.fillWidth: true

        // Not connected: server address
        ColumnLayout {
            visible: !Sync.connected && !Sync.loggingIn
            Layout.fillWidth: true
            spacing: 12
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: qsTrId("sync.nextcloud.intro")
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 14
            }
            InputField {
                label: qsTrId("sync.nextcloud.server")
                placeholder: "cloud.example.org"
                text: page.serverAddress
                inputMethodHints: Qt.ImhUrlCharacters | Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                onEdited: (value) => page.serverAddress = value
                onAccepted: if (page.serverAddress.trim() !== "") Sync.connectNextcloud(page.serverAddress)
            }
            Text {
                visible: Sync.error !== ""
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: Sync.error
                color: Theme.danger
                font.family: Theme.fontFamily
                font.pixelSize: 14
            }
            ActionButton {
                kind: "primary"
                iconName: "cloud"
                text: qsTrId("sync.nextcloud.connect")
                enabled: page.serverAddress.trim() !== ""
                onClicked: Sync.connectNextcloud(page.serverAddress)
            }
        }

        // Waiting for the login in the browser
        ColumnLayout {
            visible: Sync.loggingIn
            Layout.fillWidth: true
            spacing: 12
            RowLayout {
                spacing: 12
                BusyIndicator {
                    running: Sync.loggingIn
                    implicitWidth: 32
                    implicitHeight: 32
                }
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: qsTrId("sync.nextcloud.waiting")
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                    font.weight: Font.Bold
                }
            }
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: qsTrId("sync.nextcloud.login_browser")
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 14
            }
            ActionButton {
                text: qsTrId("common.cancel")
                onClicked: Sync.cancelLogin()
            }
        }

        // Connected
        RowLayout {
            visible: Sync.connected
            Layout.fillWidth: true
            spacing: 12
            Tile { iconName: "cloud"; tile: Theme.tile.blue }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 1
                Text {
                    text: "Nextcloud"
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                    font.weight: Font.Bold
                }
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WrapAnywhere
                    text: qsTrId("sync.nextcloud.connected_as").arg(Sync.user).arg(page.host)
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                }
            }
        }
        ActionButton {
            visible: Sync.connected
            Layout.leftMargin: -8
            kind: "quiet"
            text: qsTrId("sync.nextcloud.disconnect")
            onClicked: disconnectDialog.open()
        }
    }

    // ---------------------------------------------------------------- sync password
    Card {
        visible: Sync.connected && Sync.needsPassword
        Layout.fillWidth: true

        RowLayout {
            visible: Sync.checking
            spacing: 12
            BusyIndicator {
                running: Sync.checking
                implicitWidth: 28
                implicitHeight: 28
            }
            Text {
                text: qsTrId("sync.checking")
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 14
            }
        }

        ColumnLayout {
            visible: !Sync.checking
            Layout.fillWidth: true
            spacing: 12

            // The data on this device and the folder: replace, merge or start over
            ColumnLayout {
                visible: page.joining
                Layout.fillWidth: true
                spacing: 8
                Text {
                    text: qsTrId("sync.join.title")
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 16
                    font.weight: Font.ExtraBold
                }
                ChoiceOption {
                    selected: page.joinMode === Sync.ReplaceLocal
                    onClicked: page.joinMode = Sync.ReplaceLocal
                    text: qsTrId("sync.join.replace")
                    hint: qsTrId("sync.join.replace.hint")
                }
                ChoiceOption {
                    selected: page.joinMode === Sync.Merge
                    onClicked: page.joinMode = Sync.Merge
                    text: qsTrId("sync.join.merge")
                    hint: qsTrId("sync.join.merge.hint")
                }
                ChoiceOption {
                    selected: page.joinMode === Sync.StartOver
                    onClicked: page.joinMode = Sync.StartOver
                    text: qsTrId("sync.join.start_over")
                    hint: qsTrId("sync.join.start_over.hint")
                }
            }

            RowLayout {
                Layout.topMargin: page.joining ? 6 : 0
                spacing: 12
                Tile { iconName: "key"; tile: Theme.tile.plum }
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: page.choosing ? qsTrId("sync.password.choose_title") : qsTrId("sync.password.enter_title")
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 16
                    font.weight: Font.ExtraBold
                }
            }
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: page.choosing ? qsTrId("sync.password.choose_text") : qsTrId("sync.password.enter_text")
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 14
            }
            InputField {
                label: qsTrId("sync.password")
                text: page.password
                echoMode: page.showPassword ? TextInput.Normal : TextInput.Password
                inputMethodHints: Qt.ImhSensitiveData | Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
                onEdited: (value) => page.password = value
                onAccepted: page.submitPassword()
            }
            InputField {
                visible: page.choosing
                label: qsTrId("sync.password.repeat")
                text: page.repeat
                echoMode: page.showPassword ? TextInput.Normal : TextInput.Password
                inputMethodHints: Qt.ImhSensitiveData | Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
                onEdited: (value) => page.repeat = value
                onAccepted: page.submitPassword()
            }
            CheckBox {
                Layout.leftMargin: -6
                text: qsTrId("sync.password.show")
                checked: page.showPassword
                onToggled: page.showPassword = checked
                font.family: Theme.fontFamily
                font.pixelSize: 14
            }
            Text {
                visible: page.choosing && page.password !== "" && page.passwordProblem !== ""
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: page.passwordProblem
                color: Theme.danger
                font.family: Theme.fontFamily
                font.pixelSize: 14
            }
            Text {
                visible: Sync.error !== ""
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: Sync.error
                color: Theme.danger
                font.family: Theme.fontFamily
                font.pixelSize: 14
            }
            RowLayout {
                spacing: 12
                ActionButton {
                    kind: "primary"
                    iconName: "sync"
                    text: page.joining && page.joinMode === Sync.StartOver ? qsTrId("sync.join.start_over.action")
                        : page.choosing ? qsTrId("sync.password.set")
                        : qsTrId("sync.password.unlock")
                    enabled: !Sync.busy && page.password !== "" && page.passwordProblem === ""
                    onClicked: page.submitPassword()
                }
                BusyIndicator {
                    visible: Sync.busy
                    running: Sync.busy
                    implicitWidth: 28
                    implicitHeight: 28
                }
            }
        }
    }

    // ---------------------------------------------------------------- status
    Card {
        visible: Sync.connected && !Sync.needsPassword
        Layout.fillWidth: true

        RowLayout {
            Layout.fillWidth: true
            spacing: 12
            Tile {
                iconName: Sync.error !== "" ? "warning" : "sync"
                tile: Sync.error !== "" ? Theme.tile.orange : Theme.tile.green
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 1
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: Sync.busy ? qsTrId("sync.in_progress")
                        : Sync.error !== "" ? qsTrId("sync.status.problem")
                        : Sync.lastSync === "" ? qsTrId("sync.never")
                        : qsTrId("sync.all_synced")
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 15
                    font.weight: Font.Bold
                }
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    visible: text !== ""
                    text: Sync.error !== "" ? Sync.error
                        : Sync.lastSync !== "" ? qsTrId("sync.last").arg(Labels.syncWhen(Sync.lastSync, page.now))
                        : ""
                    color: Sync.error !== "" ? Theme.danger : Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 13
                }
            }
        }
        RowLayout {
            spacing: 12
            ActionButton {
                kind: "primary"
                iconName: "sync"
                text: qsTrId("sync.now")
                enabled: !Sync.busy
                onClicked: Sync.syncNow()
            }
            BusyIndicator {
                visible: Sync.busy
                running: Sync.busy
                implicitWidth: 28
                implicitHeight: 28
            }
        }
        SwitchRow {
            text: qsTrId("sync.auto")
            hint: qsTrId("sync.auto.hint")
            checked: Sync.autoSync
            onToggled: Sync.autoSync = checked
        }
    }

    // ---------------------------------------------------------------- Wi-Fi
    SectionLabel {
        Layout.topMargin: 6
        text: qsTrId("sync.wifi")
    }
    Card {
        Layout.fillWidth: true

        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTrId("wifi.intro")
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 14
        }
        Repeater {
            model: Wifi.peers
            delegate: PeerRow {
                required property var modelData
                name: modelData.name
                syncing: modelData.syncing
                warn: modelData.problem !== "" || modelData.paused
                status: modelData.problem !== "" ? qsTrId("wifi.problem.unknown")
                      : modelData.paused ? qsTrId("wifi.paused")
                      : (modelData.online ? qsTrId("wifi.online") : qsTrId("wifi.offline"))
                        + (modelData.lastSync ? " · " + Labels.syncWhen(modelData.lastSync, page.now) : "")
                canDecide: modelData.paused && modelData.problem === ""
                onDecide: Nav.decideJoin(modelData.id, modelData.name)
                onForget: {
                    forgetDialog.peerId = modelData.id
                    forgetDialog.peerName = modelData.name
                    forgetDialog.open()
                }
            }
        }
        Flow {
            Layout.fillWidth: true
            spacing: 10
            ActionButton {
                kind: Wifi.peers.length === 0 ? "primary" : "secondary"
                iconName: "wifi"
                text: qsTrId("sync.pair")
                onClicked: Nav.pairDevice()
            }
            ActionButton {
                visible: Wifi.peers.length > 0
                iconName: "sync"
                text: qsTrId("sync.now")
                enabled: !Wifi.busy && Wifi.enabled
                onClicked: Wifi.syncNow()
            }
            BusyIndicator {
                visible: Wifi.busy
                running: Wifi.busy
                implicitWidth: 28
                implicitHeight: 28
            }
        }
        SwitchRow {
            visible: Wifi.peers.length > 0
            text: qsTrId("sync.wifi")
            hint: qsTrId("wifi.enabled.hint")
            checked: Wifi.enabled
            onToggled: Wifi.enabled = checked
        }
    }

    AppDialog {
        id: forgetDialog
        property string peerId
        property string peerName
        iconName: "wifi"
        destructive: true
        title: qsTrId("wifi.forget")
        text: qsTrId("wifi.forget.confirm").arg(peerName)
        rejectText: qsTrId("common.cancel")
        acceptText: qsTrId("wifi.forget")
        onAccepted: Wifi.forget(peerId)
    }

    // ---------------------------------------------------------------- this device
    SectionLabel {
        visible: Sync.connected || Wifi.peers.length > 0
        Layout.topMargin: 6
        text: qsTrId("sync.devices")
    }
    Card {
        visible: Sync.connected || Wifi.peers.length > 0
        Layout.fillWidth: true
        spacing: 10

        DeviceRow {
            name: page.deviceNameDraft
            detail: qsTrId("sync.device.this")
        }
        // Devices that sync through the Nextcloud
        Repeater {
            model: Sync.connected ? Sync.devices : []
            delegate: DeviceRow {
                required property string modelData
                name: modelData
                detail: "Nextcloud"
            }
        }
        InputField {
            Layout.topMargin: 4
            label: qsTrId("sync.device_name")
            text: page.deviceNameDraft
            onEdited: (value) => { page.deviceNameDraft = value; nameTimer.restart() }
            onAccepted: Sync.deviceName = page.deviceNameDraft
        }
        Timer {
            id: nameTimer
            interval: 1200
            onTriggered: Sync.deviceName = page.deviceNameDraft
        }
    }

    AppDialog {
        id: disconnectDialog
        iconName: "cloudOff"
        destructive: true
        title: qsTrId("sync.nextcloud.disconnect")
        text: qsTrId("sync.nextcloud.disconnect.confirm")
        rejectText: qsTrId("common.cancel")
        acceptText: qsTrId("sync.nextcloud.disconnect")
        onAccepted: Sync.disconnectNextcloud()
    }

    // ---------------------------------------------------------------- pieces
    component Tile: Rectangle {
        id: tileItem
        property string iconName
        property var tile: Theme.tile.gray
        implicitWidth: 40
        implicitHeight: 40
        radius: 12
        color: tile.bg
        Icon {
            anchors.centerIn: parent
            width: 22
            height: 22
            name: tileItem.iconName
            color: tileItem.tile.fg
        }
    }

    component Banner: Rectangle {
        id: banner
        property string iconName
        property alias text: bannerText.text
        Layout.fillWidth: true
        implicitHeight: bannerRow.implicitHeight + 24
        radius: Theme.radius
        color: Theme.accentSoft
        RowLayout {
            id: bannerRow
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 14
            spacing: 10
            Icon {
                Layout.alignment: Qt.AlignTop
                Layout.preferredWidth: 20
                Layout.preferredHeight: 20
                name: banner.iconName
                color: Theme.accentText
            }
            Text {
                id: bannerText
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                color: Theme.accentText
                font.family: Theme.fontFamily
                font.pixelSize: 14
                font.weight: Font.DemiBold
            }
        }
    }

    component DeviceRow: RowLayout {
        id: deviceRow
        property string name
        property string detail
        Layout.fillWidth: true
        spacing: 12
        Icon {
            Layout.preferredWidth: 22
            Layout.preferredHeight: 22
            name: "device"
            color: Theme.icon
        }
        Text {
            Layout.fillWidth: true
            elide: Text.ElideRight
            text: deviceRow.name
            color: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 15
            font.weight: Font.DemiBold
        }
        Text {
            visible: deviceRow.detail !== ""
            text: deviceRow.detail
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 13
        }
    }

    component PeerRow: RowLayout {
        id: peerRow
        property string name
        property string status
        property bool warn
        property bool syncing
        property bool canDecide
        signal decide()
        signal forget()
        Layout.fillWidth: true
        spacing: 12
        Icon {
            Layout.preferredWidth: 22
            Layout.preferredHeight: 22
            name: "device"
            color: peerRow.warn ? Theme.tile.orange.fg : Theme.icon
        }
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 1
            Text {
                Layout.fillWidth: true
                elide: Text.ElideRight
                text: peerRow.name
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 15
                font.weight: Font.DemiBold
            }
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: peerRow.status
                color: peerRow.warn ? Theme.tile.orange.fg : Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 13
            }
        }
        BusyIndicator {
            visible: peerRow.syncing
            running: peerRow.syncing
            implicitWidth: 24
            implicitHeight: 24
        }
        ActionButton {
            visible: peerRow.canDecide
            text: qsTrId("wifi.decide")
            onClicked: peerRow.decide()
        }
        IconButton {
            iconName: "trash"
            text: qsTrId("wifi.forget")
            size: 36
            tint: Theme.icon
            onClicked: peerRow.forget()
        }
    }
}
