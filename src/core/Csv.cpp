// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "Csv.h"

#include <QDate>
#include <QRegularExpression>

#include <algorithm>

using namespace Qt::StringLiterals;

namespace Qupil::Csv {

std::optional<Table> parse(const QByteArray &data, bool utf8, Error *error)
{
    QString text = utf8 ? QString::fromUtf8(data) : QString::fromLatin1(data);
    if (text.startsWith(QChar(0xFEFF)))
        text.remove(0, 1);   // byte order mark
    text.replace(u"\r\n"_s, u"\n"_s);
    text.replace(u'\r', u'\n');

    const QString firstLine = text.section(u'\n', 0, 0);
    const qsizetype commas = firstLine.count(u',');
    const qsizetype semicolons = firstLine.count(u';');
    if (commas == semicolons) {
        if (error)
            *error = Error::NoSeparator;
        return std::nullopt;
    }
    Table table;
    table.separator = commas > semicolons ? u',' : u';';

    QList<QStringList> lines;
    QStringList fields;
    QString field;
    bool quoted = false;
    for (qsizetype i = 0; i < text.size(); ++i) {
        const QChar c = text.at(i);
        if (quoted) {
            if (c == u'"') {
                if (i + 1 < text.size() && text.at(i + 1) == u'"') {
                    field += u'"';   // "" inside quotes
                    ++i;
                } else {
                    quoted = false;
                }
            } else {
                field += c;
            }
        } else if (c == u'"') {
            quoted = true;
        } else if (c == table.separator) {
            fields << field.trimmed();
            field.clear();
        } else if (c == u'\n') {
            fields << field.trimmed();
            field.clear();
            lines << fields;
            fields.clear();
        } else {
            field += c;
        }
    }
    if (!field.isEmpty() || !fields.isEmpty()) {
        fields << field.trimmed();
        lines << fields;
    }
    // Lines without any content (e.g. at the end) are left out
    for (const QStringList &l : std::as_const(lines)) {
        const bool empty = std::all_of(l.cbegin(), l.cend(), [](const QString &f) { return f.isEmpty(); });
        if (empty)
            continue;
        if (table.headers.isEmpty())
            table.headers = l;
        else
            table.rows << l;
    }
    if (error)
        *error = Error::None;
    return table;
}

QString isoDate(const QString &text)
{
    const QString t = text.trimmed();
    if (t.isEmpty())
        return {};
    static const QRegularExpression iso(u"^(\\d{4})-(\\d{1,2})-(\\d{1,2})"_s);
    static const QRegularExpression dotted(u"^(\\d{1,2})\\.(\\d{1,2})\\.(\\d{2,4})$"_s);
    static const QRegularExpression slashed(u"^(\\d{1,2})/(\\d{1,2})/(\\d{2,4})$"_s);
    QDate d;
    auto year = [](int y) { return y < 100 ? (y > QDate::currentDate().year() % 100 ? 1900 + y : 2000 + y) : y; };
    if (const auto m = iso.match(t); m.hasMatch()) {
        d = QDate(m.captured(1).toInt(), m.captured(2).toInt(), m.captured(3).toInt());
    } else if (const auto m = dotted.match(t); m.hasMatch()) {
        d = QDate(year(m.captured(3).toInt()), m.captured(2).toInt(), m.captured(1).toInt());
    } else if (const auto m = slashed.match(t); m.hasMatch()) {
        // US month/day, unless the first number cannot be a month
        const int a = m.captured(1).toInt();
        const int b = m.captured(2).toInt();
        d = a > 12 ? QDate(year(m.captured(3).toInt()), b, a) : QDate(year(m.captured(3).toInt()), a, b);
    }
    return d.isValid() ? d.toString(Qt::ISODate) : QString();
}

} // namespace Qupil::Csv
