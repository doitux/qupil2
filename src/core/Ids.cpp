// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "Ids.h"

#include <QRandomGenerator>
#include <QStringList>

namespace Qupil {

QString newId()
{
    const quint64 ms = quint64(QDateTime::currentMSecsSinceEpoch());
    quint8 b[16];
    for (int i = 0; i < 6; ++i)
        b[i] = quint8(ms >> (8 * (5 - i)));
    QRandomGenerator *rng = QRandomGenerator::system();
    for (int i = 6; i < 16; ++i)
        b[i] = quint8(rng->bounded(256));
    b[6] = quint8(0x70 | (b[6] & 0x0F));   // version 7
    b[8] = quint8(0x80 | (b[8] & 0x3F));   // variant 10

    static const char hex[] = "0123456789abcdef";
    QString s;
    s.reserve(36);
    for (int i = 0; i < 16; ++i) {
        if (i == 4 || i == 6 || i == 8 || i == 10)
            s += QLatin1Char('-');
        s += QLatin1Char(hex[b[i] >> 4]);
        s += QLatin1Char(hex[b[i] & 0x0F]);
    }
    return s;
}

static qint64 systemMs()
{
    return QDateTime::currentMSecsSinceEpoch();
}

qint64 HybridClock::now()
{
    // Never earlier than a time we have seen: within the same millisecond (or
    // a coarse system timer like the 16 ms on Windows) the counter goes up, and
    // after more than 65535 changes it borrows from the next millisecond
    const qint64 wall = make(m_wall ? m_wall() : systemMs(), 0);
    m_last = wall > m_last ? wall : m_last + 1;
    return m_last;
}

void HybridClock::observe(qint64 time)
{
    if (time > m_last)
        m_last = time;
}

void HybridClock::restore(qint64 lastMs, int counter)
{
    m_last = make(lastMs, counter);
}

QString HybridClock::format(qint64 time, const QString &device)
{
    return QStringLiteral("%1.%2.%3")
        .arg(time >> 16, 13, 10, QLatin1Char('0'))
        .arg(int(time & 0xFFFF), 4, 16, QLatin1Char('0'))
        .arg(device);
}

bool HybridClock::parse(const QString &text, qint64 *time, QString *device)
{
    const QStringList parts = text.split(QLatin1Char('.'));
    if (parts.size() < 3)
        return false;
    bool ok1 = false, ok2 = false;
    const qint64 ms = parts.at(0).toLongLong(&ok1);
    const int counter = parts.at(1).toInt(&ok2, 16);
    if (!ok1 || !ok2 || ms < 0 || counter < 0 || counter > 0xFFFF)
        return false;
    if (time)
        *time = make(ms, counter);
    if (device)
        *device = parts.mid(2).join(QLatin1Char('.'));
    return true;
}

} // namespace Qupil
