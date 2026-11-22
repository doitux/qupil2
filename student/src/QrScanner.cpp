// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

#include "QrScanner.h"

#include <QCoreApplication>
#include <QMetaObject>
#include <QTimer>

#include <optional>

namespace QrScanner {

namespace {
std::optional<Result> s_pretended;
bool s_open = false;
} // namespace

bool available()
{
    return s_pretended.has_value() || nativeAvailable();
}

void pretend(const Result &result)
{
    s_pretended = result;
}

void scan(const Texts &texts, std::function<void(const Result &)> done)
{
    if (s_open || !qApp)
        return;
    s_open = true;
    // Back on Qt's thread, whatever thread the platform answers on
    auto finish = [done = std::move(done)](const Result &result) {
        QMetaObject::invokeMethod(qApp, [done, result] {
            s_open = false;
            done(result);
        }, Qt::QueuedConnection);
    };
    if (s_pretended) {
        finish(*s_pretended);
        return;
    }
    if (!nativeAvailable()) {
        finish({});
        return;
    }
    nativeScan(texts, finish);
}

} // namespace QrScanner
