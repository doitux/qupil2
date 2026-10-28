// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "Clock.h"

namespace Qupil::Clock {

namespace {
QDate s_date;
QTime s_time;
} // namespace

void pretend(const QDate &date, const QTime &time)
{
    s_date = date;
    s_time = date.isValid() ? time : QTime();
}

QDate today()
{
    return s_date.isValid() ? s_date : QDate::currentDate();
}

QDateTime now()
{
    if (!s_date.isValid())
        return QDateTime::currentDateTime();
    return QDateTime(s_date, s_time.isValid() ? s_time : QTime::currentTime());
}

QDate lastWeekday(const QDate &date, int weekday)
{
    return date.addDays(-((date.dayOfWeek() - weekday + 7) % 7));
}

} // namespace Qupil::Clock
