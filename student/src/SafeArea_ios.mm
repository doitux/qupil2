// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// iOS: the safe area of the UIWindow the app's window is in. The window covers the whole
// UIWindow (Qt::MaximizeUsingFullscreenGeometryHint), so the UIWindow's insets are the
// window's. They are asked for again whenever they can have changed: UIKit has no
// notification of its own for them outside a view's subclass.

#include "SafeArea.h"

#include <QGuiApplication>
#include <QPointer>
#include <QScreen>
#include <QTimer>
#include <QWindow>

#import <UIKit/UIKit.h>

namespace SafeArea {

void watch(QWindow *window, std::function<void(qreal, qreal)> changed)
{
    if (!window)
        return;
    const QPointer<QWindow> guarded(window);
    const auto read = [guarded, changed] {
        if (!guarded || !guarded->handle())
            return;
        UIView *view = (__bridge UIView *)reinterpret_cast<void *>(guarded->winId());
        // Before the scene is connected (in main()) there is no UIWindow yet: nothing is
        // on the screen then either
        UIWindow *uiWindow = view.window;
        if (!uiWindow)
            return;
        const UIEdgeInsets insets = uiWindow.safeAreaInsets;
        changed(insets.top, insets.bottom);
    };
    // Now, and once more when UIKit has laid out (after turning the device, the insets
    // follow the size)
    const auto update = [guarded, read] {
        read();
        if (guarded)
            QTimer::singleShot(300, guarded, read);
    };

    QObject::connect(window, &QWindow::widthChanged, window, update);
    QObject::connect(window, &QWindow::heightChanged, window, update);
    QObject::connect(window, &QWindow::visibleChanged, window, update);
    QObject::connect(qGuiApp, &QGuiApplication::applicationStateChanged, window, update);
    if (QScreen *screen = window->screen())
        QObject::connect(screen, &QScreen::geometryChanged, window, update);

    // The first picture: the UIWindow shows before Qt's signals above come
    NSNotificationCenter *center = NSNotificationCenter.defaultCenter;
    for (NSNotificationName name in @[UIWindowDidBecomeVisibleNotification, UIWindowDidBecomeKeyNotification,
                                      UISceneDidActivateNotification, UIApplicationDidBecomeActiveNotification]) {
        [center addObserverForName:name object:nil queue:NSOperationQueue.mainQueue
                        usingBlock:^(NSNotification *) { update(); }];
    }
    update();
}

} // namespace SafeArea
