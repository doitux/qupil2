// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "Birthday.h"

#include <algorithm>

namespace Qupil::Birthday {

QDate inYear(const QDate &birth, int year)
{
    const QDate d(year, birth.month(), birth.day());
    return d.isValid() ? d : QDate(year, 3, 1);
}

Note noteFor(const QDate &birth, const QDate &today, const QDate &lastNote)
{
    Note note;
    if (!birth.isValid() || !today.isValid() || birth > today)
        return note;
    const QDate thisYear = inYear(birth, today.year());
    const QDate last = thisYear <= today ? thisYear : inYear(birth, today.year() - 1);
    // Since the last lesson, but at least the last week and at most eight weeks
    QDate since = today.addDays(-MinDays);
    if (lastNote.isValid() && lastNote < today)
        since = std::max(std::min(since, lastNote.addDays(1)), today.addDays(-MaxDays));
    if (last == today)
        note.kind = Kind::Today;
    else if (last >= since)
        note.kind = Kind::Past;
    else
        return note;
    note.date = last;
    note.turns = last.year() - birth.year();
    note.days = int(last.daysTo(today));
    return note;
}

} // namespace Qupil::Birthday
