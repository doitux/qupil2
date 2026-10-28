// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QDate>
#include <QDateTime>
#include <QTime>

/*!
    Today's date as Qupil shows it: normally the real date. The screenshots
    (and --date for trying things out) pretend another day, so that "today" is
    a weekday with lessons whenever the pictures are made.

    Only what is shown uses it (schedule, sample data, notices, documents).
    Alerts, sync and backups keep the real time.
*/
namespace Qupil::Clock {

//! Pretend that today is \a date. A valid \a time stops the clock at that time
//! of day; otherwise it keeps running. An invalid \a date returns to the real date.
void pretend(const QDate &date, const QTime &time = {});

//! Today's date (pretended or real)
QDate today();

//! Date and time (pretended or real)
QDateTime now();

//! The most recent \a weekday (1 = Monday … 7 = Sunday) on or before \a date
QDate lastWeekday(const QDate &date, int weekday);

} // namespace Qupil::Clock
