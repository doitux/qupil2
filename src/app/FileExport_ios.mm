// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// iOS: the system picker "Save to Files" for a copy of a file (a backup).

#include "FileExport.h"

#include <QFileInfo>

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

//! Reports the choice in the picker once
@interface QupilExportDelegate : NSObject <UIDocumentPickerDelegate>
@property (nonatomic, copy) void (^done)(BOOL saved, NSString *name);
@end

@implementation QupilExportDelegate

- (void)documentPicker:(UIDocumentPickerViewController *)controller didPickDocumentsAtURLs:(NSArray<NSURL *> *)urls
{
    (void)controller;
    [self finish:YES name:urls.firstObject.lastPathComponent];
}

- (void)documentPickerWasCancelled:(UIDocumentPickerViewController *)controller
{
    (void)controller;
    [self finish:NO name:nil];
}

- (void)finish:(BOOL)saved name:(NSString *)name
{
    void (^done)(BOOL, NSString *) = self.done;
    self.done = nil;
    if (done)
        done(saved, name);
}

@end

namespace FileExport {

namespace {

// The picker only keeps a weak reference to its delegate
QupilExportDelegate *s_delegate = nil;

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

void exportFile(const QString &path, std::function<void(bool saved, const QString &name)> done)
{
    UIViewController *top = topViewController();
    if (!top || !QFileInfo::exists(path)) {
        if (done)
            done(false, QString());
        return;
    }
    // Exporting a copy: iOS 14 and later (Qt 6.8 needs iOS 16 anyway)
    NSURL *url = [NSURL fileURLWithPath:path.toNSString()];
    UIDocumentPickerViewController *picker = [[UIDocumentPickerViewController alloc] initForExportingURLs:@[ url ]
                                                                                                   asCopy:YES];
    s_delegate = [[QupilExportDelegate alloc] init];
    s_delegate.done = ^(BOOL saved, NSString *name) {
        if (done)
            done(saved, name ? QString::fromNSString(name) : QString());
        // Not while the delegate is still answering the picker
        dispatch_async(dispatch_get_main_queue(), ^{
            s_delegate = nil;
        });
    };
    picker.delegate = s_delegate;
    picker.modalPresentationStyle = UIModalPresentationFormSheet;
    [top presentViewController:picker animated:YES completion:nil];
}

} // namespace FileExport
