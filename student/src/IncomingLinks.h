// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QUrl>

#include <functional>

class QObject;

/*!
    The links the system hands to the app: https://qupil.de/u#… (the QR code read by the
    phone's camera, a link in a message) and qupil-student:…

    Desktop: Qt's URL handlers (QDesktopServices; the receiver's slot openUrl(QUrl)).
    iOS: the same, plus the link the app is started with (IncomingLinks_ios.mm).
    Android: StudentActivity, because Qt's way did not deliver the links there.
    \a received may be called on any thread, and the same link may come twice.
*/
namespace IncomingLinks {

void watch(QObject *receiver, std::function<void(const QUrl &)> received);
void unwatch();

} // namespace IncomingLinks
