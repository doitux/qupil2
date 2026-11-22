// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QUrl>

#include <functional>

class QObject;

/*!
    The links the system hands to the app: https://qupil.de/u#… (the QR code read by the
    phone's camera, a link in a message) and qupil-student:…

    iOS and desktop: Qt's URL handlers (QDesktopServices). Android: StudentActivity, because
    Qt's way did not deliver them there. \a received may be called on any thread.
*/
namespace IncomingLinks {

void watch(QObject *receiver, std::function<void(const QUrl &)> received);
void unwatch();

} // namespace IncomingLinks
