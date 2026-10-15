// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Desktop systems: Qupil shows the alerts itself while it runs.

#include "SystemAlerts.h"

namespace SystemAlerts {

bool available()
{
    return false;
}

void setUp()
{
}

void queryState(QObject *, std::function<void(State)> done)
{
    done(State {});
}

void requestPermission(QObject *, std::function<void()> done)
{
    done();
}

void openSettings(bool)
{
}

namespace Platform {

void schedule(const QList<Alert> &, const QMap<QString, QString> &)
{
}

void cancel()
{
}

} // namespace Platform

} // namespace SystemAlerts
