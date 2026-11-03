// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QDate>

/*!
    A student's birthday as it matters for the lesson: today, or "had a birthday"
    since the last lesson. The last lesson is the day of the student's last note, so
    holidays need no holiday calendar: after two weeks of autumn holidays the first
    lesson still shows a birthday in the holidays. Always at least the last week
    (no note last week, two lessons a week) and at most eight weeks back (summer
    holidays; an old last note does not bring up birthdays of months ago).
*/
namespace Qupil::Birthday {

enum class Kind { None, Today, Past };

struct Note
{
    Kind kind = Kind::None;
    QDate date;      //!< the birthday (this year or last)
    int turns = 0;   //!< age reached on that day
    int days = 0;    //!< days since then (0 = today)
};

//! How far back "had a birthday" goes: at least a week, at most eight weeks
constexpr int MinDays = 7;
constexpr int MaxDays = 56;

//! The birthday of someone born on \a birth for \a today; \a lastNote: their last note
//! before today (invalid if none)
Note noteFor(const QDate &birth, const QDate &today, const QDate &lastNote);

//! The birthday in \a year (29 February: 1 March in other years)
QDate inYear(const QDate &birth, int year);

} // namespace Qupil::Birthday
