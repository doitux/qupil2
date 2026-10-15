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
    Hybrid logical clock. A timestamp is "<13-digit ms>.<4-hex counter>.<device id>"
    and compares correctly as a plain string, which keeps "last writer wins"
    decisions identical on every device, even with slightly wrong device clocks.
*/
class HybridClock
{
public:
    explicit HybridClock(QString deviceId = {});

    void setDeviceId(const QString &deviceId) { m_device = deviceId; }
    QString deviceId() const { return m_device; }

    //! Timestamp for a local change
    QString now();
    //! Merge a timestamp received from another device (keeps the clock ahead of it)
    void observe(const QString &remoteTimestamp);

    //! Restore/persist the clock state (last physical ms and counter)
    void restore(qint64 lastMs, int counter);
    qint64 lastMs() const { return m_lastMs; }
    int counter() const { return m_counter; }

    static QString format(qint64 ms, int counter, const QString &device);
    static bool parse(const QString &ts, qint64 *ms, int *counter, QString *device = nullptr);

    //! For tests: replace the wall clock
    void setWallClock(qint64 (*fn)()) { m_wall = fn; }

private:
    QString m_device;
    qint64 m_lastMs = 0;
    int m_counter = 0;
    qint64 (*m_wall)() = nullptr;
};

} // namespace Qupil
