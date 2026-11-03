// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Android: the window of the activity keeps the screen on (set on Android's main thread,
// which owns the window).

#include "ScreenAwake.h"

#include <QCoreApplication>
#include <QJniObject>
#include <QVariant>
#include <QtCore/qcoreapplication_platform.h>

namespace ScreenAwake {

void set(bool awake)
{
    QNativeInterface::QAndroidApplication::runOnAndroidMainThread([awake]() -> QVariant {
        const QJniObject activity = QNativeInterface::QAndroidApplication::context();
        if (!activity.isValid())
            return {};
        const QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
        if (!window.isValid())
            return {};
        const jint keepScreenOn = 0x00000080;   // WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON
        window.callMethod<void>(awake ? "addFlags" : "clearFlags", "(I)V", keepScreenOn);
        return {};
    });
}

} // namespace ScreenAwake
