// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Android: StudentActivity.keyboardHeightChanged(px) comes here (Android's main thread) and
// goes on to Qt's thread in device-independent pixels.

#include "KeyboardHeight.h"

#include <QGuiApplication>
#include <QJniEnvironment>
#include <QJniObject>
#include <QMetaObject>
#include <QScreen>

namespace {

std::function<void(qreal)> s_changed;

void JNICALL keyboardHeightChanged(JNIEnv *, jclass, jint px)
{
    if (!qApp)
        return;
    QMetaObject::invokeMethod(qApp, [px] {
        const QScreen *screen = QGuiApplication::primaryScreen();
        const qreal ratio = screen && screen->devicePixelRatio() > 0 ? screen->devicePixelRatio() : 1.0;
        if (s_changed)
            s_changed(px / ratio);
    }, Qt::QueuedConnection);
}

} // namespace

namespace KeyboardHeight {

void watch(std::function<void(qreal)> changed)
{
    s_changed = std::move(changed);
    const JNINativeMethod methods[] = {
        { "keyboardHeightChanged", "(I)V", reinterpret_cast<void *>(keyboardHeightChanged) },
    };
    QJniEnvironment env;
    if (!env.registerNativeMethods("de/qupil/student/StudentActivity", methods, 1)) {
        qWarning("Keyboard height: StudentActivity not found");
        return;
    }
    QJniObject::callStaticMethod<void>("de/qupil/student/StudentActivity", "keyboardWatched");
}

} // namespace KeyboardHeight
