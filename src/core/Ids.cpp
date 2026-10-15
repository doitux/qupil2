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

HybridClock::HybridClock(QString deviceId)
    : m_device(std::move(deviceId))
{
}

static qint64 systemMs()
{
    return QDateTime::currentMSecsSinceEpoch();
}

QString HybridClock::now()
{
    const qint64 wall = m_wall ? m_wall() : systemMs();
    if (wall > m_lastMs) {
        m_lastMs = wall;
        m_counter = 0;
    } else {
        ++m_counter;
        if (m_counter > 0xFFFF) {   // extremely unlikely: borrow from the future
            ++m_lastMs;
            m_counter = 0;
        }
    }
    return format(m_lastMs, m_counter, m_device);
}

void HybridClock::observe(const QString &remoteTimestamp)
{
    qint64 ms = 0;
    int counter = 0;
    if (!parse(remoteTimestamp, &ms, &counter))
        return;
    // Never fall behind a timestamp we have seen: now() then issues a later one,
    // also when the remote time equals our wall clock (same millisecond, or a
    // coarse system timer like the 16 ms on Windows)
    if (ms > m_lastMs) {
        m_lastMs = ms;
        m_counter = counter;
    } else if (ms == m_lastMs) {
        m_counter = qMax(m_counter, counter);
    }
}

void HybridClock::restore(qint64 lastMs, int counter)
{
    m_lastMs = lastMs;
    m_counter = counter;
}

QString HybridClock::format(qint64 ms, int counter, const QString &device)
{
    return QStringLiteral("%1.%2.%3")
        .arg(ms, 13, 10, QLatin1Char('0'))
        .arg(counter, 4, 16, QLatin1Char('0'))
        .arg(device);
}

bool HybridClock::parse(const QString &ts, qint64 *ms, int *counter, QString *device)
{
    const QStringList parts = ts.split(QLatin1Char('.'));
    if (parts.size() < 3)
        return false;
    bool ok1 = false, ok2 = false;
    const qint64 m = parts.at(0).toLongLong(&ok1);
    const int c = parts.at(1).toInt(&ok2, 16);
    if (!ok1 || !ok2)
        return false;
    if (ms)
        *ms = m;
    if (counter)
        *counter = c;
    if (device)
        *device = parts.mid(2).join(QLatin1Char('.'));
    return true;
}

} // namespace Qupil
