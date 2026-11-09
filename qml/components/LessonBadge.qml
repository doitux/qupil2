// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// The small sign in front of a lesson in LessonPicker: the student's initials for an
// individual lesson, a square with the students symbol for groups and ensembles.

import QtQuick
import QtQuick.Layouts

Item {
    id: badge
    property var lesson: null
    readonly property bool single: !!lesson && lesson.kind === 1 && (lesson.initials || "") !== ""
    implicitWidth: 34
    implicitHeight: 34
    Layout.preferredWidth: 34
    Layout.preferredHeight: 34

    Avatar {
        visible: badge.single
        anchors.centerIn: parent
        size: 34
        soft: true
        text: badge.lesson ? (badge.lesson.initials || "") : ""
        seed: badge.lesson ? (badge.lesson.title || "") : ""
    }
    Rectangle {
        visible: !badge.single
        anchors.fill: parent
        radius: 10
        color: Theme.surfaceAlt
        Icon {
            anchors.centerIn: parent
            width: 18
            height: 18
            name: "students"
            color: Theme.textSecondary
        }
    }
}
