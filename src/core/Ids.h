// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QDateTime>
#include <QString>

namespace Qupil {

//! Time-ordered UUID (RFC 9562, version 7) as lower-case string with dashes.
//! Every record gets one, so records created on different devices never collide.
QString newId();

/*!
    Hybrid logical clock. A time is one 64-bit number: milliseconds since 1970
    times 65536 plus a counter for changes within the same millisecond. Together
    with the id of the device that made a change it orders all changes the same
    way on every device ("last writer wins"), even with slightly wrong device
    clocks: a device never issues a time earlier than one it has already seen.
*/
class HybridClock
{
public:
    //! Time of a local change
    qint64 now();
    //! Merge a time received from another device (keeps the clock ahead of it)
    void observe(qint64 time);

    //! Restore/persist the clock state (last physical ms and counter)
    void restore(qint64 lastMs, int counter);
    qint64 lastMs() const { return m_last >> 16; }
    int counter() const { return int(m_last & 0xFFFF); }
    qint64 last() const { return m_last; }

    static qint64 make(qint64 ms, int counter) { return (ms << 16) + (counter & 0xFFFF); }
    //! "<13-digit ms>.<4-hex counter>.<device id>": the text form of sync format 1
    static QString format(qint64 time, const QString &device);
    static bool parse(const QString &text, qint64 *time, QString *device = nullptr);

    //! For tests: replace the wall clock
    void setWallClock(qint64 (*fn)()) { m_wall = fn; }

private:
    qint64 m_last = 0;
    qint64 (*m_wall)() = nullptr;
};

} // namespace Qupil
