// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

#include "BackKey.h"

#include <QKeyEvent>
#include <QQuickWindow>
#include <QVariant>

#ifdef Q_OS_ANDROID
#include <QCoreApplication>
#include <QJniObject>
#endif

BackKey::BackKey(QQuickWindow *window)
    : QObject(window)
    , m_window(window)
{
    window->installEventFilter(this);
}

bool BackKey::eventFilter(QObject *watched, QEvent *event)
{
    if (watched != m_window || (event->type() != QEvent::KeyPress && event->type() != QEvent::KeyRelease))
        return false;
    auto *key = static_cast<QKeyEvent *>(event);
    if (key->key() != Qt::Key_Back)
        return false;
    if (event->type() == QEvent::KeyPress) {
        // An open popup takes the press as its shortcut and closes: it never gets here
        if (!key->isAutoRepeat())
            m_pressed = true;
        return true;
    }
    // Accepted in any case: an unhandled back key would close the app
    if (!m_pressed)
        return true;
    m_pressed = false;
    QVariant handled;
    QMetaObject::invokeMethod(m_window, "goBack", Q_RETURN_ARG(QVariant, handled));
    if (!handled.toBool()) {
#ifdef Q_OS_ANDROID
        QJniObject activity = QNativeInterface::QAndroidApplication::context();
        if (activity.isValid())
            activity.callMethod<jboolean>("moveTaskToBack", "(Z)Z", jboolean(true));
#endif
    }
    return true;
}
