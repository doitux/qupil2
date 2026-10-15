// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// More › Instruments: rental instruments, who needs the next size, how many
// play which instrument and size, and the instruments offered for students.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page
    title: qsTrId("common.instruments")

    readonly property var overview: { if (Data.revision < 0) return undefined; return Data.instrumentOverview() }
    readonly property var rentals: page.overview.students.filter(s => s.rental === 1)
    readonly property var nextSize: page.overview.students.filter(s => s.next_size === 1)

    function details(s) {
        return [s.instrument, s.instrument_size].filter(x => x).join(" · ")
    }

    actions: IconButton {
        iconName: "document"
        text: qsTrId("instruments.inventory_list")
        onClicked: Docs.rentalList()
    }

    // ---------------------------------------------------------------- numbers
    RowLayout {
        Layout.fillWidth: true
        spacing: 12
        Figure {
            value: page.overview.rentalCount
            text: qsTrId("instruments.rentals")
        }
        Figure {
            value: page.overview.nextSizeCount
            text: qsTrId("instruments.next_size")
        }
    }

    // ---------------------------------------------------------------- rental instruments
    SectionLabel { Layout.topMargin: 6; text: qsTrId("instruments.rentals") }
    EmptyHint {
        visible: page.rentals.length === 0
        text: qsTrId("instruments.empty")
    }
    Repeater {
        model: page.rentals
        delegate: ListRow {
            required property var modelData
            avatar: modelData.initials
            title: modelData.name
            subtitle: [page.details(modelData), modelData.rental_desc,
                       modelData.rental_since ? qsTrId("student.rented_since") + " " + Data.formatDate(modelData.rental_since) : ""]
                      .filter(x => x).join(" · ")
            trailing: modelData.next_size === 1 ? "↑" : ""
            onClicked: Nav.showStudent(modelData.id, "")
        }
    }

    // ---------------------------------------------------------------- next size
    SectionLabel {
        visible: page.nextSize.length > 0
        Layout.topMargin: 6
        text: qsTrId("instruments.next_size")
    }
    Repeater {
        model: page.nextSize
        delegate: ListRow {
            required property var modelData
            avatar: modelData.initials
            title: modelData.name
            subtitle: page.details(modelData)
            onClicked: Nav.showStudent(modelData.id, "")
        }
    }

    // ---------------------------------------------------------------- counts
    SectionLabel {
        visible: page.overview.byInstrument.length > 0
        Layout.topMargin: 6
        text: qsTrId("instruments.tab.instrument_count")
    }
    Card {
        visible: page.overview.byInstrument.length > 0
        Layout.fillWidth: true
        spacing: 6
        Repeater {
            model: page.overview.byInstrument
            delegate: CountRow {
                required property var modelData
                name: modelData.name
                count: modelData.count
            }
        }
    }
    SectionLabel {
        visible: page.overview.bySize.length > 0
        Layout.topMargin: 6
        text: qsTrId("instruments.tab.size_count")
    }
    Card {
        visible: page.overview.bySize.length > 0
        Layout.fillWidth: true
        spacing: 6
        Repeater {
            model: page.overview.bySize
            delegate: CountRow {
                required property var modelData
                name: modelData.name
                count: modelData.count
            }
        }
    }

    // ---------------------------------------------------------------- choices
    SectionLabel { Layout.topMargin: 6; text: qsTrId("instruments.choices") }
    Card {
        Layout.fillWidth: true
        ListEditor {
            label: ""
            items: { if (Data.revision < 0) return undefined; return Data.instrumentChoices() }
            onChanged: (items) => Data.setInstrumentChoices(items)
        }
    }

    component Figure: Rectangle {
        id: number
        property int value
        property string text
        Layout.fillWidth: true
        implicitHeight: numberColumn.implicitHeight + 24
        radius: Theme.radius
        color: Theme.surface
        ColumnLayout {
            id: numberColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 14
            spacing: 0
            Text {
                text: String(number.value)
                color: Theme.accentText
                font.family: Theme.fontFamily
                font.pixelSize: 28
                font.weight: Font.ExtraBold
            }
            Text {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                text: number.text
                color: Theme.textMuted
                font.family: Theme.fontFamily
                font.pixelSize: 13
            }
        }
    }

    component CountRow: RowLayout {
        id: countRow
        property string name
        property int count
        Layout.fillWidth: true
        Text {
            Layout.fillWidth: true
            elide: Text.ElideRight
            text: countRow.name
            color: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 15
        }
        Text {
            text: String(countRow.count)
            color: Theme.textMuted
            font.family: Theme.fontFamily
            font.pixelSize: 15
            font.weight: Font.Bold
        }
    }
}
