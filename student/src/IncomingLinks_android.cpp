// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Android: StudentActivity hands over the links that open the app (linkReceived, on
// Android's main thread) and keeps the one the app was started with (takePendingLink).

#include "IncomingLinks.h"

#include <QJniEnvironment>
#include <QJniObject>

namespace IncomingLinks {

namespace {

std::function<void(const QUrl &)> s_received;

void JNICALL linkReceived(JNIEnv *, jclass, jstring link)
{
    const QString text = QJniObject(link).toString();
    if (s_received && !text.isEmpty())
        s_received(QUrl(text));
}

} // namespace

void watch(QObject *, std::function<void(const QUrl &)> received)
{
    s_received = std::move(received);
    const JNINativeMethod methods[] = {
        { "linkReceived", "(Ljava/lang/String;)V", reinterpret_cast<void *>(linkReceived) },
    };
    QJniEnvironment env;
    if (!env.registerNativeMethods("de/qupil/student/StudentActivity", methods, 1)) {
        qWarning("Incoming links: StudentActivity not found");
        return;
    }
    const QString pending = QJniObject::callStaticObjectMethod("de/qupil/student/StudentActivity", "takePendingLink",
                                                               "()Ljava/lang/String;").toString();
    if (!pending.isEmpty())
        s_received(QUrl(pending));
}

void unwatch()
{
    s_received = nullptr;
}

} // namespace IncomingLinks
