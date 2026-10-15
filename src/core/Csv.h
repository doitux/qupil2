// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QByteArray>
#include <QList>
#include <QStringList>

#include <optional>

/*!
    Reading address books exported as CSV (Outlook, Google, Thunderbird, …)
    for importing students.
*/
namespace Qupil::Csv {

struct Table
{
    QChar separator;
    QStringList headers;
    QList<QStringList> rows;
};

enum class Error {
    None,
    NoSeparator    //!< neither comma nor semicolon in the first line
};

/*!
    The separator is the one of comma and semicolon that the first line has
    more of (as in Qupil 1.x). Quoted fields may contain separators, line
    breaks and doubled quotes. utf8 false: Latin-1.
*/
std::optional<Table> parse(const QByteArray &data, bool utf8, Error *error = nullptr);

//! "31.12.2010", "2010-12-31", "12/31/2010", "31/12/2010" → ISO date, or empty
QString isoDate(const QString &text);

} // namespace Qupil::Csv
