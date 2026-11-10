// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "Performers.h"

#include <QSet>
#include <QStringList>

using namespace Qt::StringLiterals;

namespace Qupil::Performers {

QString line(const QList<Person> &people)
{
    QSet<QString> instruments;
    for (const Person &p : people)
        instruments.insert(p.instrument.trimmed());
    QStringList parts;
    // Everyone the same, known instrument: once at the end
    if (instruments.size() == 1 && !instruments.cbegin()->isEmpty()) {
        for (const Person &p : people)
            parts << p.name;
        return parts.join(u" · "_s) + u" – "_s + *instruments.cbegin();
    }
    // Otherwise behind each name that has one
    for (const Person &p : people) {
        const QString instrument = p.instrument.trimmed();
        parts << (instrument.isEmpty() ? p.name : p.name + u", "_s + instrument);
    }
    return parts.join(u" · "_s);
}

} // namespace Qupil::Performers
