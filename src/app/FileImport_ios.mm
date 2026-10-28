// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// iOS: the system picker for opening a file, as a copy. iOS fetches the file
// from its provider (Nextcloud, iCloud Drive, …) and hands over a copy that
// belongs to Qupil, so no access rights to the other app's storage are needed.

#include "FileImport.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

//! Reports the choice in the picker once
@interface QupilImportDelegate : NSObject <UIDocumentPickerDelegate>
@property (nonatomic, copy) void (^done)(NSURL *url);
@end

@implementation QupilImportDelegate

- (void)documentPicker:(UIDocumentPickerViewController *)controller didPickDocumentsAtURLs:(NSArray<NSURL *> *)urls
{
    (void)controller;
    [self finish:urls.firstObject];
}

- (void)documentPickerWasCancelled:(UIDocumentPickerViewController *)controller
{
    (void)controller;
    [self finish:nil];
}

- (void)finish:(NSURL *)url
{
    void (^done)(NSURL *) = self.done;
    self.done = nil;
    if (done)
        done(url);
}

@end

namespace FileImport {

namespace {

// The picker only keeps a weak reference to its delegate
QupilImportDelegate *s_delegate = nil;

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

//! Moves the picker's copy (in the app's temporary inbox) to the folder \a into
QString takeCopy(NSURL *url, const QString &into, QString *error)
{
    // A copy is ours; should iOS ever hand over the original, it is ours to read while we copy it
    const bool scoped = [url startAccessingSecurityScopedResource];
    const QString from = QString::fromNSString(url.path);
    const QString to = QDir(into).filePath(QFileInfo(from).fileName());
    QFile::remove(to);
    bool ok = QFile::rename(from, to);
    if (!ok) {
        QFile in(from);
        ok = in.open(QIODevice::ReadOnly);
        if (ok) {
            QFile out(to);
            ok = out.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QByteArray chunk;
            while (ok && !(chunk = in.read(1 << 20)).isEmpty())
                ok = out.write(chunk) == chunk.size();
            if (!ok)
                *error = out.error() != QFileDevice::NoError ? out.errorString() : in.errorString();
        } else {
            *error = in.errorString();
        }
        QFile::remove(from);
    }
    if (scoped)
        [url stopAccessingSecurityScopedResource];
    return ok ? to : QString();
}

} // namespace

bool available()
{
    return true;
}

void pickFile(const QString &into,
              std::function<void(const QString &path, const QString &name, const QString &error)> done)
{
    UIViewController *top = topViewController();
    if (!top) {
        if (done)
            done(QString(), QString(), QString());
        return;
    }
    // A copy for the block below: "into" may refer to a temporary of the caller
    const QString folder = into;
    QDir(folder).removeRecursively();
    QDir().mkpath(folder);
    // Any file: backups (*.db) have no type iOS knows
    UIDocumentPickerViewController *picker = [[UIDocumentPickerViewController alloc] initForOpeningContentTypes:@[ UTTypeItem ]
                                                                                                         asCopy:YES];
    picker.allowsMultipleSelection = NO;
    s_delegate = [[QupilImportDelegate alloc] init];
    s_delegate.done = ^(NSURL *url) {
        QString path;
        QString error;
        if (url)
            path = takeCopy(url, folder, &error);
        if (done)
            done(path, url ? QString::fromNSString(url.lastPathComponent) : QString(), error);
        // Not while the delegate is still answering the picker
        dispatch_async(dispatch_get_main_queue(), ^{
            s_delegate = nil;
        });
    };
    picker.delegate = s_delegate;
    picker.modalPresentationStyle = UIModalPresentationFormSheet;
    [top presentViewController:picker animated:YES completion:nil];
}

} // namespace FileImport
