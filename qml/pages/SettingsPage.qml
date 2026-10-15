// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Settings that are shared by all your devices (stored with the data).
// Appearance and language are per device and live on the More page.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

SubPage {
    id: page
    title: qsTrId("settings.title")

    function value(key, fallback) {
        if (Data.revision < 0) return undefined
        return Data.setting(key, fallback)
    }

    SectionLabel { text: qsTrId("settings.this_device") }
    Card {
        Layout.fillWidth: true
        SwitchRow {
            text: qsTrId("settings.lesson_end.enable")
            hint: qsTrId("settings.lesson_end.hint")
            checked: Prefs.lessonEndAlert
            onToggled: Prefs.lessonEndAlert = checked
        }
        Stepper {
            visible: Prefs.lessonEndAlert
            label: qsTrId("settings.lesson_end.before")
            from: 1
            to: 30
            value: Prefs.lessonEndMinutes
            suffix: qsTrId("common.unit.min")
            onPicked: (value) => Prefs.lessonEndMinutes = value
        }
    }

    SectionLabel { Layout.topMargin: 6; text: qsTrId("common.lesson") }
    Card {
        Layout.fillWidth: true
        SwitchRow {
            text: qsTrId("settings.save_for_all")
            checked: page.value("save_for_all_students", 1) !== 0
            onToggled: Data.setSetting("save_for_all_students", checked ? 1 : 0)
        }
        SwitchRow {
            text: qsTrId("settings.birthday_reminder")
            checked: page.value("birthday_reminder", 1) !== 0
            onToggled: Data.setSetting("birthday_reminder", checked ? 1 : 0)
        }
        SwitchRow {
            text: qsTrId("settings.limit_notes")
            checked: page.value("limit_notes", 0) !== 0
            onToggled: Data.setSetting("limit_notes", checked ? 1 : 0)
        }
        Stepper {
            visible: page.value("limit_notes", 0) !== 0
            label: qsTrId("settings.limit_notes.count")
            from: 1
            value: page.value("limit_notes_count", 30)
            onPicked: (value) => Data.setSetting("limit_notes_count", value)
        }
        SwitchRow {
            text: qsTrId("settings.limit_pieces")
            checked: page.value("limit_pieces", 0) !== 0
            onToggled: Data.setSetting("limit_pieces", checked ? 1 : 0)
        }
        Stepper {
            visible: page.value("limit_pieces", 0) !== 0
            label: qsTrId("settings.limit_pieces.count")
            from: 1
            value: page.value("limit_pieces_count", 20)
            onPicked: (value) => Data.setSetting("limit_pieces_count", value)
        }
    }

    SectionLabel { Layout.topMargin: 6; text: qsTrId("nav.events") }
    Card {
        Layout.fillWidth: true
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTrId("settings.event_duration")
            color: Theme.text
            font.family: Theme.fontFamily
            font.pixelSize: 15
            font.weight: Font.Bold
        }
        Stepper {
            label: qsTrId("settings.event.moderation")
            value: page.value("event_moderation_minutes", 4)
            suffix: qsTrId("common.unit.min")
            onPicked: (value) => Data.setSetting("event_moderation_minutes", value)
        }
        Stepper {
            label: qsTrId("settings.event.changeover")
            value: page.value("event_between_minutes", 2)
            suffix: qsTrId("common.unit.min")
            onPicked: (value) => Data.setSetting("event_between_minutes", value)
        }
    }

    SectionLabel { Layout.topMargin: 6; text: qsTrId("candidates.recital.title") }
    Card {
        Layout.fillWidth: true
        Text {
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
            text: qsTrId("settings.recital_check")
            color: Theme.textSecondary
            font.family: Theme.fontFamily
            font.pixelSize: 14
        }
        Repeater {
            model: [1, 0]
            delegate: RadioDelegate {
                required property int modelData
                Layout.fillWidth: true
                text: modelData === 1 ? qsTrId("settings.recital_check.solo") : qsTrId("settings.recital_check.solo_ensemble")
                checked: page.value("recital_check_solo_only", 1) === modelData
                font.family: Theme.fontFamily
                font.pixelSize: 15
                onClicked: Data.setSetting("recital_check_solo_only", modelData)
            }
        }
    }

    SectionLabel { Layout.topMargin: 6; text: qsTrId("settings.lists") }
    Card {
        Layout.fillWidth: true
        spacing: 18
        ListEditor {
            label: qsTrId("settings.locations")
            items: { if (Data.revision < 0) return undefined; return Data.listSetting("locations") }
            onChanged: (items) => Data.setSetting("locations", items)
        }
        ListEditor {
            label: qsTrId("settings.genres")
            items: { if (Data.revision < 0) return undefined; return Data.listSetting("genres") }
            onChanged: (items) => Data.setSetting("genres", items)
        }
        ListEditor {
            label: qsTrId("settings.sizes")
            items: { if (Data.revision < 0) return undefined; return Data.listSetting("instrument_sizes") }
            onChanged: (items) => Data.setSetting("instrument_sizes", items)
        }
    }
}
