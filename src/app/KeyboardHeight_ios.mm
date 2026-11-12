// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// iOS: Qt moves its whole view up when the keyboard would cover the cursor (a translation
// of the root view's sublayers, and its windows raised over the status bar meanwhile).
// While an editor is open, Qupil makes room itself; this undoes Qt's move. Our observers are
// added after Qt's (Qt's input context exists from the start), so they run after it.

#include "KeyboardHeight.h"

#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>

namespace {

bool s_hold = false;

bool isQtWindow(UIWindow *window)
{
    return window.rootViewController
        && [NSStringFromClass([window.rootViewController class]) containsString:@"QIOSViewController"];
}

void undoMove()
{
    if (!s_hold)
        return;
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    NSArray<UIWindow *> *windows = [UIApplication sharedApplication].windows;
#pragma clang diagnostic pop
    for (UIWindow *window in windows) {
        if (!isQtWindow(window))
            continue;
        CALayer *layer = window.rootViewController.view.layer;
        if ([layer animationForKey:@"AnimateSubLayerTransform"] || !CATransform3DIsIdentity(layer.sublayerTransform)) {
            [layer removeAnimationForKey:@"AnimateSubLayerTransform"];
            layer.sublayerTransform = CATransform3DIdentity;
        }
        // Qt raised its windows over the status bar while moved
        if (window.windowLevel >= UIWindowLevelStatusBar)
            window.windowLevel = window.windowLevel - UIWindowLevelStatusBar;
    }
}

} // namespace

namespace KeyboardHeight {

void holdView(bool hold)
{
    s_hold = hold;
    static bool observing = false;
    if (hold && !observing) {
        observing = true;
        NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
        for (NSNotificationName name in @[ UIKeyboardWillShowNotification, UIKeyboardDidShowNotification,
                                           UIKeyboardWillChangeFrameNotification, UIKeyboardDidChangeFrameNotification ]) {
            [center addObserverForName:name object:nil queue:nil usingBlock:^(NSNotification *) {
                undoMove();
            }];
        }
    }
    undoMove();
}

void keepViewInPlace()
{
    undoMove();
}

qreal viewOffset()
{
    for (UIWindow *window in [UIApplication sharedApplication].windows)
        if (isQtWindow(window) && !window.hidden) {
            CALayer *layer = window.rootViewController.view.layer;
            return (layer.presentationLayer ?: layer).sublayerTransform.m42;
        }
    return 0;
}

} // namespace KeyboardHeight
