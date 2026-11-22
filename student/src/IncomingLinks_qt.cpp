// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// iOS and desktop: Qt's URL handlers; the receiver has the slot openUrl(QUrl).

#include "IncomingLinks.h"

#include <QDesktopServices>

using namespace Qt::StringLiterals;

namespace IncomingLinks {

void watch(QObject *receiver, std::function<void(const QUrl &)>)
{
    QDesktopServices::setUrlHandler(u"https"_s, receiver, "openUrl");
    QDesktopServices::setUrlHandler(u"qupil-student"_s, receiver, "openUrl");
}

void unwatch()
{
    QDesktopServices::unsetUrlHandler(u"https"_s);
    QDesktopServices::unsetUrlHandler(u"qupil-student"_s);
}

} // namespace IncomingLinks
