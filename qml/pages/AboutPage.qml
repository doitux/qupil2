// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// More › About Qupil: version and build, what Qupil does, author, website,
// source code and license (as in the 1.x about dialog, without translators).

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page
    title: qsTrId("about.title")

    signal showLicense()

    readonly property string website: "https://qupil.de"
    readonly property string email: "fhammer@qupil.de"
    readonly property string sourceCode: "https://github.com/doitux/qupil2"

    // ------------------------------------------------------------ name, version, what it does
    Card {
        Layout.fillWidth: true
        spacing: 16
        RowLayout {
            Layout.fillWidth: true
            spacing: 16
            Image {
                Layout.preferredWidth: 72
                Layout.preferredHeight: 72
                source: "assets/icons/qupil.png"
                sourceSize.width: 144
                sourceSize.height: 144
                fillMode: Image.PreserveAspectFit
                mipmap: true
            }
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                Text {
                    text: "Qupil"
                    color: Theme.text
                    font.family: Theme.fontFamily
                    font.pixelSize: 26
                    font.weight: Font.ExtraBold
                }
                AbstractButton {
                    id: version
                    Layout.fillWidth: true
                    implicitHeight: implicitContentHeight + topPadding + bottomPadding
                    topPadding: 2
                    bottomPadding: 2
                    focusPolicy: Qt.StrongFocus
                    Accessible.role: Accessible.Button
                    Accessible.name: versionText.text
                    // A tap copies version and commit (for bug reports)
                    onClicked: {
                        App.copyText("Qupil " + App.versionText)
                        Nav.toast(qsTrId("app.build_copied").arg(App.versionText))
                    }
                    contentItem: Text {
                        id: versionText
                        wrapMode: Text.WordWrap
                        text: qsTrId("about.version").arg(App.versionText)
                        color: version.visualFocus ? Theme.accentText : Theme.textSecondary
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.DemiBold
                    }
                    background: null
                }
                Text {
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                    text: "© 2006–" + new Date().getFullYear() + " Felix Hammer"
                    color: Theme.textMuted
                    font.family: Theme.fontFamily
                    font.pixelSize: 14
                }
            }
        }
        ColumnLayout {
            Layout.fillWidth: true
            spacing: 6
            Repeater {
                // "- Student and lesson management\n- Event manager\n…" (Qupil 1.x text)
                model: qsTrId("about.features").split("\n").map(line => line.replace(/^\s*-\s*/, "")).filter(Boolean)
                delegate: RowLayout {
                    required property string modelData
                    Layout.fillWidth: true
                    spacing: 10
                    Rectangle {
                        Layout.alignment: Qt.AlignTop
                        Layout.topMargin: 8
                        implicitWidth: 6
                        implicitHeight: 6
                        radius: 3
                        color: Theme.accent
                    }
                    Text {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: modelData
                        color: Theme.textSecondary
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                    }
                }
            }
        }
    }

    // ------------------------------------------------------------ project
    SectionLabel {
        Layout.topMargin: 6
        text: qsTrId("about.tab.project")
    }
    Rows {
        MenuRow {
            iconName: "globe"
            tile: Theme.tile.plum
            text: qsTrId("about.website")
            value: "qupil.de"
            onClicked: Qt.openUrlExternally(page.website)
        }
        MenuRow {
            divider: true
            iconName: "mail"
            tile: Theme.tile.blue
            text: qsTrId("about.author")
            value: "Felix Hammer"
            onClicked: Qt.openUrlExternally("mailto:" + page.email)
        }
        MenuRow {
            divider: true
            iconName: "code"
            tile: Theme.tile.gray
            text: qsTrId("about.source")
            value: "GitHub"
            onClicked: Qt.openUrlExternally(page.sourceCode)
        }
    }

    // ------------------------------------------------------------ license
    SectionLabel {
        Layout.topMargin: 6
        text: qsTrId("about.tab.license")
    }
    Card {
        Layout.fillWidth: true
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTrId("about.license.summary")
            lineHeight: 1.15
            color: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 15
        }
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTrId("about.components")
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 13
        }
    }
    Rows {
        MenuRow {
            iconName: "document"
            tile: Theme.tile.gray
            text: qsTrId("about.license.show")
            value: "GPL 3.0"
            onClicked: page.showLicense()
        }
    }

    component Rows: Rectangle {
        default property alias rows: rowsColumn.data
        Layout.fillWidth: true
        implicitHeight: rowsColumn.implicitHeight
        radius: Theme.radius + 2
        color: Theme.surface
        clip: true
        ColumnLayout {
            id: rowsColumn
            width: parent.width
            spacing: 0
        }
    }
}
