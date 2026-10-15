// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Desktop systems: Qupil plays the lesson-end alert itself while it runs.

#include "SystemAlerts.h"

namespace SystemAlerts {

bool available()
{
    return false;
}

void setUp()
{
}

void schedule(const QList<Alert> &, const QString &, const QString &)
{
}

void cancel()
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

} // namespace SystemAlerts
