// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// iOS: every alert time is a local notification that repeats weekly, so it
// comes also while Qupil is in the background or not running.

#include "SystemAlerts.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QPointer>
#include <QStringList>

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <UserNotifications/UserNotifications.h>

using namespace Qt::StringLiterals;

//! Shows lesson-end notifications (banner and sound) also while Qupil is in front
@interface QupilNotificationDelegate : NSObject <UNUserNotificationCenterDelegate>
@end

@implementation QupilNotificationDelegate

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
       willPresentNotification:(UNNotification *)notification
         withCompletionHandler:(void (^)(UNNotificationPresentationOptions options))completionHandler
{
    (void)center;
    // Reminders: Qupil shows them itself while it is in front (with "Done?")
    if ([notification.request.content.userInfo[@"kind"] isEqual:@"reminder"]) {
        completionHandler(UNNotificationPresentationOptionNone);
        return;
    }
    if (@available(iOS 14.0, *)) {
        completionHandler(UNNotificationPresentationOptionBanner | UNNotificationPresentationOptionList
                          | UNNotificationPresentationOptionSound);
    } else {
        completionHandler(UNNotificationPresentationOptionAlert | UNNotificationPresentationOptionSound);
    }
}

- (void)userNotificationCenter:(UNUserNotificationCenter *)center
    didReceiveNotificationResponse:(UNNotificationResponse *)response
             withCompletionHandler:(void (^)(void))completionHandler
{
    (void)center;
    (void)response;
    completionHandler();
}

@end

namespace SystemAlerts {

namespace {

constexpr int kMaxPending = 60;   // iOS keeps at most 64 pending notifications per app
QupilNotificationDelegate *s_delegate = nil;   // the notification center only keeps a weak reference

NSString *identifier(int index)
{
    return [NSString stringWithFormat:@"qupil.alert.%d", index];
}

NSArray<NSString *> *allIdentifiers()
{
    NSMutableArray<NSString *> *ids = [NSMutableArray arrayWithCapacity:128];
    for (int i = 0; i < 64; ++i) {
        [ids addObject:identifier(i)];
        [ids addObject:[NSString stringWithFormat:@"qupil.lesson-end.%d", i]];   // test builds before
    }
    return ids;
}

//! Library/Sounds: where iOS looks for the sounds of notifications
QString soundsFolder()
{
    NSArray<NSString *> *paths = NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSUserDomainMask, YES);
    return QString::fromNSString(paths.firstObject) + u"/Sounds"_s;
}

Permission toPermission(UNAuthorizationStatus status)
{
    switch (status) {
    case UNAuthorizationStatusNotDetermined:
        return Permission::NotAsked;
    case UNAuthorizationStatusDenied:
        return Permission::Denied;
    default:
        return Permission::Granted;   // authorized, provisional, ephemeral
    }
}

//! Calls back on the main thread, unless context is gone
void later(const QPointer<QObject> &guard, std::function<void()> call)
{
    QMetaObject::invokeMethod(qApp, [guard, call] {
        if (guard)
            call();
    }, Qt::QueuedConnection);
}

} // namespace

bool available()
{
    return true;
}

void setUp()
{
    if (s_delegate)
        return;
    s_delegate = [[QupilNotificationDelegate alloc] init];
    UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
    center.delegate = s_delegate;
    // Old lesson-end messages in the notification centre are of no use any more
    [center removeDeliveredNotificationsWithIdentifiers:allIdentifiers()];
}

void queryState(QObject *context, std::function<void(State)> done)
{
    const QPointer<QObject> guard(context);
    [[UNUserNotificationCenter currentNotificationCenter]
        getNotificationSettingsWithCompletionHandler:^(UNNotificationSettings *settings) {
            State state;
            state.permission = toPermission(settings.authorizationStatus);
            later(guard, [done, state] { done(state); });
        }];
}

void requestPermission(QObject *context, std::function<void()> done)
{
    const QPointer<QObject> guard(context);
    UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
    [center getNotificationSettingsWithCompletionHandler:^(UNNotificationSettings *settings) {
        if (settings.authorizationStatus == UNAuthorizationStatusDenied) {
            // iOS asks only once: from then on only the settings can change it
            later(guard, [done] {
                openSettings(false);
                done();
            });
            return;
        }
        [center requestAuthorizationWithOptions:(UNAuthorizationOptionAlert | UNAuthorizationOptionSound)
                              completionHandler:^(BOOL granted, NSError *error) {
                                  (void)granted;
                                  (void)error;
                                  later(guard, done);
                              }];
    }];
}

void openSettings(bool)
{
    NSString *address = UIApplicationOpenSettingsURLString;
    if (@available(iOS 16.0, *))
        address = UIApplicationOpenNotificationSettingsURLString;
    [[UIApplication sharedApplication] openURL:[NSURL URLWithString:address] options:@{} completionHandler:nil];
}

namespace Platform {

void schedule(const QList<Alert> &alerts, const QMap<QString, QString> &)
{
    // The sounds have to be in Library/Sounds. Their names change with sound and volume
    // (iOS keeps sounds by name); the ones no longer used are removed.
    const QString folder = soundsFolder();
    QDir().mkpath(folder);
    QStringList used;
    for (const Alert &alert : alerts) {
        if (alert.sound.isEmpty())
            continue;
        const QString name = QFileInfo(alert.sound).fileName();
        const QString target = folder + u'/' + name;
        if (!used.contains(name) && (QFileInfo::exists(target) || QFile::copy(alert.sound, target)))
            used << name;
    }
    for (const QString &old : QDir(folder).entryList({ u"*.wav"_s }, QDir::Files)) {
        if (!used.contains(old))
            QFile::remove(folder + u'/' + old);
    }

    UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
    [center removePendingNotificationRequestsWithIdentifiers:allIdentifiers()];
    NSCalendar *gregorian = [NSCalendar calendarWithIdentifier:NSCalendarIdentifierGregorian];
    int index = 0;
    for (const Alert &alert : alerts) {
        if (index >= kMaxPending)
            break;   // the next ones are added when Qupil is opened again
        UNMutableNotificationContent *content = [[UNMutableNotificationContent alloc] init];
        content.title = alert.title.toNSString();
        content.body = alert.text.toNSString();
        const QString soundName = QFileInfo(alert.sound).fileName();
        if (!alert.sound.isEmpty() && used.contains(soundName))
            content.sound = [UNNotificationSound soundNamed:soundName.toNSString()];
        content.threadIdentifier = (u"qupil."_s + alert.kind).toNSString();
        content.userInfo = @{ @"kind": alert.kind.toNSString() };

        NSDateComponents *when = [[NSDateComponents alloc] init];
        when.calendar = gregorian;
        // Ours: 0 = Monday; Gregorian weekday: 1 = Sunday … 7 = Saturday
        when.weekday = (alert.at / (24 * 60) + 1) % 7 + 1;
        when.hour = (alert.at % (24 * 60)) / 60;
        when.minute = alert.at % 60;
        UNCalendarNotificationTrigger *trigger = [UNCalendarNotificationTrigger triggerWithDateMatchingComponents:when
                                                                                                         repeats:YES];
        UNNotificationRequest *request = [UNNotificationRequest requestWithIdentifier:identifier(index++)
                                                                              content:content
                                                                              trigger:trigger];
        [center addNotificationRequest:request withCompletionHandler:nil];
    }
}

void cancel()
{
    UNUserNotificationCenter *center = [UNUserNotificationCenter currentNotificationCenter];
    [center removePendingNotificationRequestsWithIdentifiers:allIdentifiers()];
    [center removeDeliveredNotificationsWithIdentifiers:allIdentifiers()];
}

} // namespace Platform

} // namespace SystemAlerts
