// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// "Pair device": shows this device's code and the devices nearby that show
// one. Entering the code of the other device pairs both. The same sheet is
// open on both devices, so it does not matter which one enters the code.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Sheet {
    id: sheet
    title: qsTrId("sync.pair")
    showAccept: false
    rejectText: qsTrId("common.done")

    property string selectedId
    property string selectedName
    property string code
    property bool byAddress: false
    property string address

    readonly property bool codeComplete: code.replace(/[^0-9]/g, "").length === 6
    readonly property bool canPair: codeComplete && !Wifi.connecting
                                    && (byAddress ? address.trim() !== "" : selectedId !== "")

    function start() {
        selectedId = ""
        selectedName = ""
        code = ""
        byAddress = false
        address = ""
        Wifi.startPairing()
        open()
    }
    function pair() {
        if (!canPair)
            return
        if (byAddress)
            Wifi.pairWithAddress(address, code)
        else
            Wifi.pairWith(selectedId, code)
    }

    onClosed: Wifi.stopPairing()

    Connections {
        target: Wifi
        // Paired (here or on the other device) or timed out
        function onPairingChanged() {
            if (!Wifi.pairing && sheet.opened)
                sheet.close()
        }
    }

    // ---------------------------------------------------------------- our code
    Rectangle {
        Layout.fillWidth: true
        implicitHeight: codeColumn.implicitHeight + 32
        radius: Theme.radiusLarge
        color: Theme.accentSoft

        ColumnLayout {
            id: codeColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 16
            spacing: 6
            Text {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: qsTrId("wifi.code.title")
                color: Theme.accentText
                font.family: Theme.fontFamily
                font.pixelSize: 14
                font.weight: Font.Bold
            }
            Text {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                text: Wifi.pairingCode.length === 6 ? Wifi.pairingCode.slice(0, 3) + " " + Wifi.pairingCode.slice(3) : "– – –"
                color: Theme.accentText
                font.family: Theme.fontFamily
                font.pixelSize: 44
                font.weight: Font.ExtraBold
                font.letterSpacing: 4
                Accessible.name: Wifi.pairingCode.split("").join(" ")
            }
            Text {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                text: qsTrId("wifi.code.hint")
                color: Theme.textSecondary
                font.family: Theme.fontFamily
                font.pixelSize: 14
            }
            Text {
                visible: Wifi.addresses.length > 0
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                text: qsTrId("wifi.own_address").arg(Wifi.addresses.join(", "))
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 12
            }
        }
    }

    // ---------------------------------------------------------------- devices nearby
    SectionLabel {
        Layout.topMargin: 8
        text: qsTrId("wifi.nearby")
    }
    RowLayout {
        visible: Wifi.nearby.length === 0 && !sheet.byAddress
        Layout.fillWidth: true
        spacing: 12
        BusyIndicator {
            running: parent.visible && sheet.opened
            implicitWidth: 28
            implicitHeight: 28
        }
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTrId("wifi.nearby.searching")
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 14
        }
    }
    Repeater {
        model: sheet.byAddress ? [] : Wifi.nearby
        delegate: ChoiceOption {
            required property var modelData
            text: modelData.name || "Qupil"
            selected: sheet.selectedId === modelData.id
            onClicked: {
                sheet.selectedId = modelData.id
                sheet.selectedName = modelData.name
                sheet.code = ""
                codeField.forceActiveFocus()
            }
        }
    }

    // ---------------------------------------------------------------- the other device's code
    ColumnLayout {
        visible: sheet.selectedId !== "" || sheet.byAddress
        Layout.fillWidth: true
        Layout.topMargin: 4
        spacing: 12

        InputField {
            visible: sheet.byAddress
            label: qsTrId("wifi.address")
            placeholder: "192.168.1.23"
            text: sheet.address
            inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhUrlCharacters
            onEdited: (value) => sheet.address = value
        }
        InputField {
            id: codeField
            label: sheet.byAddress ? qsTrId("wifi.code") : qsTrId("wifi.enter_code").arg(sheet.selectedName)
            placeholder: "123 456"
            text: sheet.code
            inputMethodHints: Qt.ImhDigitsOnly
            onEdited: (value) => sheet.code = value
            onAccepted: sheet.pair()
        }
        RowLayout {
            spacing: 12
            ActionButton {
                kind: "primary"
                iconName: "wifi"
                text: qsTrId("wifi.pair")
                enabled: sheet.canPair
                onClicked: sheet.pair()
            }
            BusyIndicator {
                visible: Wifi.connecting
                running: Wifi.connecting
                implicitWidth: 28
                implicitHeight: 28
            }
            Text {
                visible: Wifi.connecting
                text: qsTrId("wifi.connecting")
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 14
            }
        }
    }

    ActionButton {
        visible: !sheet.byAddress
        Layout.topMargin: 4
        kind: "quiet"
        text: qsTrId("wifi.by_address")
        onClicked: {
            sheet.byAddress = true
            sheet.selectedId = ""
        }
    }
}
