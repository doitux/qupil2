// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Android: QupilActivity.setSystemBars sets the colours on Android's main thread.

#include "SystemBars.h"

#include <QJniObject>
#include <QtCore/qcoreapplication_platform.h>

namespace SystemBars {

void set(const QColor &top, const QColor &bottom, bool dark)
{
    const QJniObject activity = QNativeInterface::QAndroidApplication::context();
    if (!activity.isValid())
        return;
    // QRgb is ARGB like Android's colour ints
    activity.callMethod<void>("setSystemBars", "(IIZ)V", jint(top.rgba()), jint(bottom.rgba()), jboolean(dark));
}

} // namespace SystemBars
