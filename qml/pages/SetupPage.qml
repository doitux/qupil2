// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// First start: take over Qupil 1.x data, restore a backup or start empty.
// Also shown when the existing data cannot be opened (App.startupError).
// After a take-over the summary replaces the choices until "Continue".

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

Flickable {
    id: page

    clip: true
    contentHeight: column.implicitHeight + 80
    boundsBehavior: Flickable.StopAtBounds
    ScrollBar.vertical: ScrollBar {}

    readonly property bool done: App.lastTakeOver.kind !== undefined

    TakeOverActions { id: actions }

    ColumnLayout {
        id: column
        width: Math.min(page.width - 40, 620)
        x: (page.width - width) / 2
        y: 48
        spacing: 18

        // ------------------------------------------------------------ header
        RowLayout {
            spacing: 8
            Text {
                Layout.fillWidth: true
                text: "Qupil"
                color: Theme.accentText
                font.family: Theme.fontFamily
                font.pixelSize: 30
                font.weight: Font.ExtraBold
            }
            // Which build is installed (test builds)
            BuildChip {}
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: page.done ? (App.lastTakeOver.kind === "backup" ? qsTrId("summary.title.backup")
                                   : App.lastTakeOver.kind === "demo" ? qsTrId("summary.title.demo")
                                   : qsTrId("summary.title.legacy"))
                                : qsTrId("setup.welcome")
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 32
                font.weight: Font.ExtraBold
            }
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: page.done ? qsTrId("summary.check_hint") : qsTrId("setup.question")
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 16
            }
        }

        // ------------------------------------------------------------ unreadable data
        Rectangle {
            visible: App.startupError !== "" && !page.done
            Layout.fillWidth: true
            implicitHeight: errorColumn.implicitHeight + 28
            radius: Theme.radius
            color: Theme.tile.orange.bg

            ColumnLayout {
                id: errorColumn
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 16
                spacing: 6
                RowLayout {
                    spacing: 8
                    Icon {
                        Layout.preferredWidth: 20
                        Layout.preferredHeight: 20
                        name: "warning"
                        color: Theme.tile.orange.fg
                    }
                    Text {
                        Layout.fillWidth: true
                        wrapMode: Text.Wrap
                        text: App.startupError
                        color: Theme.tile.orange.fg
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.Bold
                    }
                }
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: qsTrId("setup.error.kept")
                    color: Theme.tile.orange.fg
                    font.family: Theme.fontFamily
                    font.pixelSize: 14
                }
            }
        }

        // ------------------------------------------------------------ summary after a take-over
        Card {
            visible: page.done
            Layout.fillWidth: true
            TakeOverSummary {
                Layout.fillWidth: true
                result: App.lastTakeOver
            }
        }
        ActionButton {
            visible: page.done
            Layout.alignment: Qt.AlignRight
            kind: "primary"
            text: qsTrId("setup.continue")
            onClicked: App.finishSetup()
        }

        // ------------------------------------------------------------ 1. Qupil 1.x
        Card {
            visible: !page.done
            Layout.fillWidth: true

            ChoiceHeader {
                iconName: "import"
                tile: Theme.tile.plum
                title: qsTrId("setup.legacy.title")
                text: qsTrId("setup.legacy.text")
            }

            // Data folders found on this computer
            Repeater {
                model: App.legacyFolders
                delegate: Rectangle {
                    id: found
                    required property var modelData
                    Layout.fillWidth: true
                    implicitHeight: foundRow.implicitHeight + 20
                    radius: Theme.radius
                    color: Theme.surfaceSunken
                    border.width: 1
                    border.color: Theme.border

                    GridLayout {
                        id: foundRow
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.margins: 12
                        columns: page.width < 520 ? 1 : 3
                        columnSpacing: 12
                        rowSpacing: 10

                        Icon {
                            visible: page.width >= 520
                            Layout.preferredWidth: 22
                            Layout.preferredHeight: 22
                            name: "folder"
                            color: Theme.accentText
                        }
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 1
                            Text {
                                Layout.fillWidth: true
                                elide: Text.ElideMiddle
                                text: qsTrId("setup.legacy.found").arg(found.modelData.path)
                                color: Theme.text
                                font.family: Theme.fontFamily
                                font.pixelSize: 15
                                font.weight: Font.DemiBold
                            }
                            Text {
                                text: qsTrId("summary.students", found.modelData.students)
                                color: Theme.textMuted
                                font.family: Theme.fontFamily
                                font.pixelSize: 13
                            }
                        }
                        ActionButton {
                            Layout.fillWidth: page.width < 520
                            kind: "primary"
                            iconName: "import"
                            text: qsTrId("setup.legacy.action")
                            enabled: !App.busy
                            onClicked: actions.start(found.modelData.url, "legacy")
                        }
                    }
                }
            }

            Text {
                visible: App.legacyFolders.length === 0
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: App.desktop ? qsTrId("setup.legacy.where").arg(Qt.platform.os === "windows" ? "%APPDATA%\\qupil"
                                                                                               : "~/.qupil")
                                  : qsTrId("setup.legacy.mobile_hint")
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 14
            }

            Flow {
                Layout.fillWidth: true
                spacing: 10
                ActionButton {
                    visible: App.desktop
                    iconName: "folder"
                    text: qsTrId("setup.legacy.choose_folder")
                    enabled: !App.busy
                    onClicked: actions.chooseLegacyFolder()
                }
                ActionButton {
                    iconName: "file"
                    text: qsTrId("setup.legacy.choose_file")
                    enabled: !App.busy
                    onClicked: actions.chooseLegacyFile()
                }
            }

            // Notice about the take-over into Qupil 2
            Rectangle {
                Layout.fillWidth: true
                Layout.topMargin: 2
                implicitHeight: hint.implicitHeight + 24
                radius: Theme.radius
                color: Theme.accentSoft

                RowLayout {
                    id: hint
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.margins: 14
                    spacing: 10
                    Icon {
                        Layout.alignment: Qt.AlignTop
                        Layout.preferredWidth: 20
                        Layout.preferredHeight: 20
                        name: "info"
                        color: Theme.accentText
                    }
                    ColumnLayout {
                        Layout.fillWidth: true
                        spacing: 3
                        Text {
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            text: qsTrId("setup.legacy.hint_title")
                            color: Theme.accentText
                            font.family: Theme.fontFamily
                            font.pixelSize: 14
                            font.weight: Font.Bold
                        }
                        Text {
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            text: qsTrId("setup.legacy.hint")
                            color: Theme.textSecondary
                            font.family: Theme.fontFamily
                            font.pixelSize: 14
                            lineHeight: 1.1
                        }
                    }
                }
            }
        }

        // ------------------------------------------------------------ 2. backup
        Card {
            visible: !page.done
            Layout.fillWidth: true
            ChoiceHeader {
                iconName: "restore"
                tile: Theme.tile.blue
                title: qsTrId("backup.restore.title")
                text: qsTrId("setup.backup.text")
            }
            ActionButton {
                iconName: "file"
                text: qsTrId("setup.backup.action")
                enabled: !App.busy
                onClicked: actions.chooseBackup()
            }
        }

        // ------------------------------------------------------------ 3. another device
        Card {
            visible: !page.done
            Layout.fillWidth: true
            ChoiceHeader {
                iconName: "sync"
                tile: Theme.tile.plum
                title: qsTrId("setup.sync.title")
                text: qsTrId("setup.sync.text")
            }
            ActionButton {
                iconName: "cloud"
                text: qsTrId("setup.sync.action")
                enabled: !App.busy
                onClicked: {
                    Nav.openSyncOnStart = true
                    if (!App.startForSync())
                        Nav.openSyncOnStart = false
                }
            }
        }

        // ------------------------------------------------------------ 4. empty
        Card {
            visible: !page.done
            Layout.fillWidth: true
            ChoiceHeader {
                iconName: "plus"
                tile: Theme.tile.green
                title: qsTrId("setup.fresh.title")
                text: qsTrId("setup.fresh.text")
            }
            ActionButton {
                text: qsTrId("setup.fresh.action")
                enabled: !App.busy
                onClicked: App.startFresh()
            }
        }

        // ------------------------------------------------------------ try it out
        ActionButton {
            visible: !page.done
            Layout.alignment: Qt.AlignHCenter
            kind: "quiet"
            text: qsTrId("setup.demo.action")
            enabled: !App.busy
            onClicked: App.loadDemo()
        }


    }

    // Icon tile, title and description of a choice
    component ChoiceHeader: RowLayout {
        id: header
        property string iconName
        property var tile
        property string title
        property string text
        Layout.fillWidth: true
        spacing: 14

        Rectangle {
            Layout.alignment: Qt.AlignTop
            implicitWidth: 42
            implicitHeight: 42
            radius: 13
            color: header.tile.bg
            Icon {
                anchors.centerIn: parent
                width: 22
                height: 22
                name: header.iconName
                color: header.tile.fg
            }
        }
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 3
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: header.title
                color: Theme.text
                font.family: Theme.fontFamily
                font.pixelSize: 18
                font.weight: Font.ExtraBold
            }
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: header.text
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 14
                lineHeight: 1.1
            }
        }
    }
}
