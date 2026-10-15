// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Address book import (CSV): pick the file, assign its columns to the
// student fields (suggested from the column titles), import.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qupil

Sheet {
    id: sheet

    property url file
    property bool utf8: true
    property var info: ({})
    property var mapping: ({})
    readonly property var fields: ["forename", "surname", "birthday", "street", "zip", "city", "email", "phone", "mobile"]
    readonly property var headers: info.headers || []

    function fieldLabel(field) {
        switch (field) {
        case "forename": return qsTrId("common.first_name")
        case "surname": return qsTrId("common.last_name")
        case "birthday": return qsTrId("common.birthday")
        case "street": return qsTrId("common.street")
        case "zip": return qsTrId("common.zip")
        case "city": return qsTrId("common.city")
        case "email": return qsTrId("common.email")
        case "phone": return qsTrId("common.phone")
        default: return qsTrId("common.mobile")
        }
    }
    function start() {
        picker.open()
    }
    function load() {
        info = Data.readCsv(file, utf8)
        if (info.error === "open") {
            Nav.toast(qsTrId("import.csv.open_failed"))
            return false
        }
        if (info.error === "separator") {
            Nav.toast(qsTrId("import.csv.no_separator"))
            return false
        }
        mapping = Object.assign({}, info.suggested)
        return true
    }

    title: qsTrId("import.csv.title")
    acceptText: qsTrId("import.csv.action")
    canAccept: (mapping.forename ?? -1) >= 0 || (mapping.surname ?? -1) >= 0
    onAccepted: {
        const count = Data.importCsv(file, utf8, mapping)
        Nav.toast(qsTrId("import.csv.done", count))
        Nav.showPage(1)
    }

    FileDialog {
        id: picker
        title: qsTrId("import.open_file")
        fileMode: FileDialog.OpenFile
        nameFilters: App.desktop ? [qsTrId("import.filter_csv"), qsTrId("common.filter_all")] : []
        onAccepted: {
            sheet.file = selectedFile
            sheet.utf8 = true
            if (sheet.load())
                sheet.open()
        }
    }

    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: qsTrId("import.csv.rows", sheet.info.count || 0) + " · " + App.displayPath(sheet.file)
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 13
    }
    Segmented {
        options: [true, false]
        value: sheet.utf8
        labelFor: function(v) { return v ? "UTF-8" : "Latin-1" }
        onActivated: (v) => {
            sheet.utf8 = v
            sheet.load()
        }
    }
    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: qsTrId("import.csv.assign")
        color: Theme.textSecondary
        font.family: Theme.fontFamily
        font.pixelSize: 14
    }
    Repeater {
        model: sheet.fields
        delegate: ChoiceField {
            required property string modelData
            label: sheet.fieldLabel(modelData)
            allowEmpty: false
            options: [qsTrId("import.csv.skip_field")].concat(sheet.headers)
            value: (sheet.mapping[modelData] ?? -1) >= 0 ? sheet.headers[sheet.mapping[modelData]] : qsTrId("import.csv.skip_field")
            onPicked: (value) => {
                const m = Object.assign({}, sheet.mapping)
                m[modelData] = sheet.headers.indexOf(value)
                sheet.mapping = m
            }
        }
    }
    Text {
        Layout.fillWidth: true
        wrapMode: Text.WordWrap
        text: qsTrId("import.csv.hint")
        color: Theme.textMuted
        font.family: Theme.fontFamily
        font.pixelSize: 13
    }
}
