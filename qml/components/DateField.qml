// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Date as a button that opens a month calendar. date is ISO (yyyy-MM-dd) or "".

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qupil

ColumnLayout {
    id: field

    property string label
    property string date
    property bool allowEmpty: false

    //! The user picked a date ("" = cleared); the field does not change "date" itself
    signal picked(string value)

    spacing: 6
    Layout.fillWidth: true

    function parse(iso) {
        const m = /^(\d{4})-(\d{2})-(\d{2})$/.exec(iso || "")
        return m ? new Date(Number(m[1]), Number(m[2]) - 1, Number(m[3])) : new Date()
    }
    function iso(d) {
        const p = n => (n < 10 ? "0" : "") + n
        return d.getFullYear() + "-" + p(d.getMonth() + 1) + "-" + p(d.getDate())
    }

    FieldLabel {
        visible: field.label !== ""
        label: field.label
    }

    AbstractButton {
        id: button
        Layout.fillWidth: true
        Layout.preferredHeight: 48
        focusPolicy: Qt.StrongFocus
        Accessible.role: Accessible.Button
        Accessible.name: field.label + " " + display.text
        onClicked: {
            const d = field.parse(field.date)
            calendar.month = d.getMonth()
            calendar.year = d.getFullYear()
            popup.open()
        }
        background: Rectangle {
            radius: 12
            color: Theme.surface
            border.width: button.visualFocus || popup.opened ? 2 : 1
            border.color: button.visualFocus || popup.opened ? Theme.accent : Theme.border
        }
        contentItem: RowLayout {
            spacing: 10
            Icon {
                Layout.leftMargin: 4
                Layout.preferredWidth: 20
                Layout.preferredHeight: 20
                name: "calendar"
                color: Theme.accentText
            }
            Text {
                id: display
                Layout.fillWidth: true
                text: {
                    if (Data.revision < 0) return undefined
                    if (LanguageManager.effectiveLanguage === "") return ""
                    return field.date !== "" ? Data.formatDate(field.date, true) : "–"
                }
                color: field.date !== "" ? Theme.text : Theme.textMuted
                elide: Text.ElideRight
                font.family: Theme.fontFamily
                font.pixelSize: 16
                font.weight: Font.DemiBold
            }
        }
        leftPadding: 10
        rightPadding: 14

        Popup {
            id: popup
            y: button.height + 6
            width: Math.max(300, Math.min(button.width, 360))
            padding: 12
            modal: false
            focus: true
            background: Rectangle {
                radius: 16
                color: Theme.surface
                border.width: 1
                border.color: Theme.border
            }
            contentItem: ColumnLayout {
                id: calendar
                property int month: 0
                property int year: 2026
                spacing: 8

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 2
                    IconButton {
                        size: 36
                        iconName: "chevron"
                        rotation: 180
                        text: qsTrId("calendar.previous_month")
                        onClicked: {
                            if (calendar.month === 0) { calendar.month = 11; calendar.year-- } else calendar.month--
                        }
                    }
                    Text {
                        Layout.fillWidth: true
                        horizontalAlignment: Text.AlignHCenter
                        text: {
                            if (LanguageManager.effectiveLanguage === "") return ""
                            return Qt.locale().standaloneMonthName(calendar.month, Locale.LongFormat)
                        }
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.Bold
                    }
                    IconButton {
                        size: 36
                        iconName: "chevron"
                        text: qsTrId("calendar.next_month")
                        onClicked: {
                            if (calendar.month === 11) { calendar.month = 0; calendar.year++ } else calendar.month++
                        }
                    }
                    Item { implicitWidth: 6 }
                    IconButton {
                        size: 36
                        iconName: "chevron"
                        rotation: 180
                        text: qsTrId("calendar.previous_year")
                        onClicked: calendar.year--
                    }
                    Text {
                        horizontalAlignment: Text.AlignHCenter
                        Layout.preferredWidth: 46
                        text: calendar.year
                        color: Theme.text
                        font.family: Theme.fontFamily
                        font.pixelSize: 15
                        font.weight: Font.Bold
                    }
                    IconButton {
                        size: 36
                        iconName: "chevron"
                        text: qsTrId("calendar.next_year")
                        onClicked: calendar.year++
                    }
                }

                DayOfWeekRow {
                    Layout.fillWidth: true
                    locale: Qt.locale(LanguageManager.effectiveLanguage)
                    delegate: Text {
                        required property string shortName
                        text: shortName
                        horizontalAlignment: Text.AlignHCenter
                        color: Theme.textMuted
                        font.family: Theme.fontFamily
                        font.pixelSize: 12
                        font.weight: Font.Bold
                    }
                }

                MonthGrid {
                    id: grid
                    Layout.fillWidth: true
                    month: calendar.month
                    year: calendar.year
                    locale: Qt.locale(LanguageManager.effectiveLanguage)
                    delegate: AbstractButton {
                        id: day
                        required property var model
                        readonly property bool selected: field.date === field.iso(model.date)
                        implicitHeight: 38
                        opacity: model.month === calendar.month ? 1 : 0.35
                        onClicked: {
                            field.picked(field.iso(model.date))
                            popup.close()
                        }
                        background: Rectangle {
                            anchors.centerIn: parent
                            width: Math.min(parent.width, parent.height)
                            height: width
                            radius: width / 2
                            color: day.selected ? Theme.accent : day.model.today ? Theme.accentSoft : "transparent"
                        }
                        contentItem: Text {
                            text: day.model.day
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: day.selected ? Theme.textOnAccent : Theme.text
                            font.family: Theme.fontFamily
                            font.pixelSize: 14
                            font.weight: day.model.today || day.selected ? Font.Bold : Font.Normal
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    ActionButton {
                        kind: "quiet"
                        text: qsTrId("nav.today")
                        onClicked: {
                            field.picked(field.iso(new Date()))
                            popup.close()
                        }
                    }
                    Item { Layout.fillWidth: true }
                    ActionButton {
                        visible: field.allowEmpty && field.date !== ""
                        kind: "quiet"
                        text: qsTrId("calendar.clear")
                        onClicked: {
                            field.picked("")
                            popup.close()
                        }
                    }
                }
            }
        }
    }
}
