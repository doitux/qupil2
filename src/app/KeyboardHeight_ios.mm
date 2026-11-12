// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// iOS: the keyboard's height as soon as it starts to come or go, and holding Qt's view in
// place while an editor is open.
//
// Height: Qt's keyboardRectangle is the keyboard's start frame until the keyboard has
// arrived (UIKeyboardDidShow), so an editor made room only then, in one jump, after the
// keyboard had slid in. UIKeyboardWillChangeFrame has the end frame at the start: the editor
// makes room while the keyboard comes (Nav.keyboardInset, animated like the keyboard).
//
// View: Qt moves its whole view up when the keyboard would cover the cursor (a translation
// of the root view's sublayers, and its windows raised over the status bar meanwhile).
// While an editor is open, Qupil makes room itself; this undoes Qt's move. Our observers are
// added after Qt's (Qt's input context exists from the start), so they run after it. The
// undo must not animate: the keyboard's notifications come inside its animation, and there
// setting the transform back slid the whole view down from Qt's position (it jumped up,
// then slid down while the keyboard came).

#include "KeyboardHeight.h"

#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>

namespace {

bool s_hold = false;
std::function<void(qreal)> s_changed;
qreal s_height = -1;

bool isQtWindow(UIWindow *window)
{
    return window.rootViewController
        && [NSStringFromClass([window.rootViewController class]) containsString:@"QIOSViewController"];
}

NSArray<UIWindow *> *appWindows()
{
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
    return [UIApplication sharedApplication].windows;
#pragma clang diagnostic pop
}

UIView *qtRootView()
{
    for (UIWindow *window in appWindows())
        if (isQtWindow(window) && !window.hidden)
            return window.rootViewController.view;
    return nil;
}

void undoMove()
{
    if (!s_hold)
        return;
    [UIView performWithoutAnimation:^{
        [CATransaction begin];
        [CATransaction setDisableActions:YES];
        for (UIWindow *window in appWindows()) {
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
        [CATransaction commit];
    }];
}

//! The part of Qupil's view the keyboard will cover at the end of its move: 0 when it goes,
//! floats (iPad) or is split
qreal coveredHeight(NSNotification *notification)
{
    UIView *view = qtRootView();
    NSValue *value = notification.userInfo[UIKeyboardFrameEndUserInfoKey];
    if (!view || !value || !view.window.screen)
        return 0;
    const CGRect end = [view convertRect:value.CGRectValue fromCoordinateSpace:view.window.screen.coordinateSpace];
    const CGRect bounds = view.bounds;
    const bool docked = end.size.width >= bounds.size.width - 1 && CGRectGetMaxY(end) >= CGRectGetMaxY(bounds) - 1;
    if (!docked)
        return 0;
    const CGRect covered = CGRectIntersection(bounds, end);
    return CGRectIsNull(covered) ? 0 : covered.size.height;
}

void report(NSNotification *notification)
{
    const qreal height = coveredHeight(notification);
    if (qAbs(height - s_height) < 0.5)
        return;
    s_height = height;
    if (s_changed)
        s_changed(height);
}

} // namespace

namespace KeyboardHeight {

void watch(std::function<void(qreal)> changed)
{
    s_changed = std::move(changed);
    NSNotificationCenter *center = [NSNotificationCenter defaultCenter];
    // Will…: at the start of the keyboard's move; Did…: settles it (rotation, a floating
    // keyboard docked by hand)
    for (NSNotificationName name in @[ UIKeyboardWillChangeFrameNotification, UIKeyboardDidChangeFrameNotification,
                                       UIKeyboardWillHideNotification ]) {
        [center addObserverForName:name object:nil queue:nil usingBlock:^(NSNotification *notification) {
            if ([notification.name isEqualToString:UIKeyboardWillHideNotification]) {
                if (s_height != 0) {
                    s_height = 0;
                    if (s_changed)
                        s_changed(0);
                }
                return;
            }
            report(notification);
        }];
    }
}

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
    UIView *view = qtRootView();
    if (!view)
        return 0;
    CALayer *shown = view.layer.presentationLayer ?: view.layer;
    return shown.sublayerTransform.m42;
}

} // namespace KeyboardHeight
