// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// iOS: the share sheet (UIActivityViewController) with the text.

#include "TextShare.h"

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

//! Gives the text to the apps, and the title to those that use a subject (Mail)
@interface QupilShareText : NSObject <UIActivityItemSource>
@property (nonatomic, copy) NSString *text;
@property (nonatomic, copy) NSString *title;
@end

@implementation QupilShareText

- (id)activityViewControllerPlaceholderItem:(UIActivityViewController *)controller
{
    (void)controller;
    return self.text;
}

- (id)activityViewController:(UIActivityViewController *)controller itemForActivityType:(UIActivityType)type
{
    (void)controller;
    (void)type;
    return self.text;
}

- (NSString *)activityViewController:(UIActivityViewController *)controller subjectForActivityType:(UIActivityType)type
{
    (void)controller;
    (void)type;
    return self.title;
}

@end

namespace TextShare {

namespace {

UIViewController *topViewController()
{
    UIViewController *top = nil;
    for (UIScene *scene in UIApplication.sharedApplication.connectedScenes) {
        if (![scene isKindOfClass:UIWindowScene.class])
            continue;
        for (UIWindow *window in ((UIWindowScene *)scene).windows) {
            if (window.isKeyWindow || !top)
                top = window.rootViewController;
        }
    }
    while (top.presentedViewController)
        top = top.presentedViewController;
    return top;
}

} // namespace

bool available()
{
    return true;
}

bool share(const QString &text, const QString &title)
{
    UIViewController *top = topViewController();
    if (!top)
        return false;
    QupilShareText *item = [[QupilShareText alloc] init];
    item.text = text.toNSString();
    item.title = title.toNSString();
    UIActivityViewController *sheet = [[UIActivityViewController alloc] initWithActivityItems:@[ item ]
                                                                         applicationActivities:nil];
    // iPad: a popover in the middle of the window, without an arrow
    if (UIPopoverPresentationController *popover = sheet.popoverPresentationController) {
        popover.sourceView = top.view;
        popover.sourceRect = CGRectMake(CGRectGetMidX(top.view.bounds), CGRectGetMidY(top.view.bounds), 0, 0);
        popover.permittedArrowDirections = 0;
    }
    [top presentViewController:sheet animated:YES completion:nil];
    return true;
}

} // namespace TextShare
