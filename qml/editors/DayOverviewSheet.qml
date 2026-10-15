// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Daily overview as PDF: the lessons of one day with the latest notes and
// room for handwritten ones (Qupil 1.x: "Create daily schedule").

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtCore
import Qupil

Sheet {
    id: sheet

    property var choices: []
    property int weekday: 0
    property int notes: 3
    property int space: 2

    function start() {
        choices = Docs.dayChoices()
        weekday = (new Date().getDay() + 6) % 7
        open()
    }

    title: qsTrId("doc.dayview.create_title")
    acceptText: qsTrId("doc.create")
    onAccepted: {
        const choice = sheet.choices.find(c => c.weekday === sheet.weekday)
        Docs.dayOverview(choice ? choice.date : "", sheet.notes, sheet.space)
    }

    Settings {
        category: "dayOverview"
        property alias notes: sheet.notes
        property alias space: sheet.space
    }

    ChoiceField {
        label: qsTrId("common.label.weekday")
        allowEmpty: false
        options: sheet.choices.map(c => c.label)
        value: (sheet.choices.find(c => c.weekday === sheet.weekday) || {}).label || ""
        onPicked: (value) => {
            const c = sheet.choices.find(x => x.label === value)
            if (c)
                sheet.weekday = c.weekday
        }
    }
    Stepper {
        label: qsTrId("doc.dayview.show_last") + " … " + qsTrId("doc.dayview.notes_per_lesson")
        value: sheet.notes
        from: 0
        to: 20
        onPicked: (value) => sheet.notes = value
    }
    Stepper {
        label: qsTrId("doc.dayview.add_space") + " … " + qsTrId("doc.dayview.space_for_notes")
        value: sheet.space
        from: 0
        to: 30
        suffix: qsTrId("doc.dayview.lines")
        onPicked: (value) => sheet.space = value
    }
}
