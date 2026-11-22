// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// iOS: Qt's URL handlers, plus the link the app is started with.
//
// Qt 6.8 hands a link to the handlers when it comes for the running app. At the app's
// start it passes on only qupil-student:… (and only if the handlers are set by then);
// a universal link (https://qupil.de/u#…, the QR code read by the camera) that starts
// the app is not passed on at all: the app opened and nothing happened. So the links of
// the start are picked up where the window scene connects (Qt's scene delegate), kept
// until the app takes links, and handed on then. The same link arriving twice is the
// app's business (StudentApp::openUrl ignores the repeat).

#include "IncomingLinks.h"

#include <QDesktopServices>

#import <UIKit/UIKit.h>
#import <objc/runtime.h>

using namespace Qt::StringLiterals;

namespace {

std::function<void(const QUrl &)> s_received;
NSMutableArray<NSURL *> *s_atStart = nil;

typedef void (*WillConnect)(id, SEL, UIScene *, UISceneSession *, UISceneConnectionOptions *);
WillConnect s_qtWillConnect = nullptr;

void willConnect(id self, SEL command, UIScene *scene, UISceneSession *session, UISceneConnectionOptions *options)
{
    NSMutableArray<NSURL *> *urls = [NSMutableArray array];
    for (NSUserActivity *activity in options.userActivities) {
        if ([activity.activityType isEqualToString:NSUserActivityTypeBrowsingWeb] && activity.webpageURL)
            [urls addObject:activity.webpageURL];
    }
    for (UIOpenURLContext *context in options.URLContexts) {
        if (context.URL)
            [urls addObject:context.URL];
    }
    if (s_qtWillConnect)
        s_qtWillConnect(self, command, scene, session, options);
    if (urls.count == 0)
        return;
    NSLog(@"Qupil student: %lu link(s) with the start", (unsigned long)urls.count);
    if (s_received) {
        for (NSURL *url in urls)
            s_received(QUrl::fromNSURL(url));
    } else {
        if (!s_atStart)
            s_atStart = [NSMutableArray array];
        [s_atStart addObjectsFromArray:urls];
    }
}

} // namespace

@interface QupilStartLinks : NSObject
@end

@implementation QupilStartLinks

+ (void)load
{
    // Qt's scene delegate (a private class of Qt's iOS plugin; another Qt may not have
    // it: then the start's links come as Qt hands them on)
    Class delegate = NSClassFromString(@"QIOSWindowSceneDelegate");
    Method method = delegate ? class_getInstanceMethod(delegate, @selector(scene:willConnectToSession:options:)) : nullptr;
    if (method)
        s_qtWillConnect = reinterpret_cast<WillConnect>(method_setImplementation(method, reinterpret_cast<IMP>(willConnect)));
}

@end

namespace IncomingLinks {

void watch(QObject *receiver, std::function<void(const QUrl &)> received)
{
    QDesktopServices::setUrlHandler(u"https"_s, receiver, "openUrl");
    QDesktopServices::setUrlHandler(u"qupil-student"_s, receiver, "openUrl");
    s_received = std::move(received);
    NSArray<NSURL *> *urls = [s_atStart copy];
    s_atStart = nil;
    for (NSURL *url in urls)
        s_received(QUrl::fromNSURL(url));
}

void unwatch()
{
    QDesktopServices::unsetUrlHandler(u"https"_s);
    QDesktopServices::unsetUrlHandler(u"qupil-student"_s);
    s_received = nullptr;
}

} // namespace IncomingLinks
