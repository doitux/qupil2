// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Collects the alerts of all sources and hands them to the system (Platform::…
// in SystemAlerts_ios.mm, SystemAlerts_android.cpp, SystemAlerts_none.cpp).

#include "SystemAlerts.h"

#include <QDateTime>

#include <algorithm>

using namespace Qt::StringLiterals;

namespace SystemAlerts {

namespace {

constexpr int kMinutesPerWeek = 7 * 24 * 60;

struct Source
{
    QString channel;
    QList<Alert> alerts;
};

QMap<QString, Source> &sources()
{
    static QMap<QString, Source> s;
    return s;
}

QByteArray &scheduled()
{
    static QByteArray s;
    return s;
}

void apply(bool force)
{
    if (!available())
        return;
    QList<Alert> all;
    QMap<QString, QString> channels;
    QByteArray signature;
    for (auto it = sources().cbegin(); it != sources().cend(); ++it) {
        channels.insert(it.key(), it->channel);
        signature += it.key().toUtf8() + '=' + it->channel.toUtf8() + '\n';
        for (const Alert &a : it->alerts) {
            all.append(a);
            signature += QByteArray::number(a.at) + ' ' + QByteArray::number(a.lead) + ' ' + a.title.toUtf8() + '|'
                         + a.text.toUtf8() + '|' + a.sound.toUtf8() + '\n';
        }
    }
    if (!force && signature == scheduled())
        return;   // every edit of the data comes here: only real changes go to the system
    scheduled() = signature;
    if (all.isEmpty()) {
        Platform::cancel();
        return;
    }
    // In the order they come; one in the current minute may have passed already: last
    const QDateTime now = QDateTime::currentDateTime();
    const int nowMinute = (now.date().dayOfWeek() - 1) * 24 * 60 + now.time().hour() * 60 + now.time().minute();
    const auto distance = [nowMinute](const Alert &a) {
        return (a.at - nowMinute - 1 + 2 * kMinutesPerWeek) % kMinutesPerWeek;
    };
    std::stable_sort(all.begin(), all.end(),
                     [&distance](const Alert &a, const Alert &b) { return distance(a) < distance(b); });
    Platform::schedule(all, channels);
}

} // namespace

void setAlerts(const QString &kind, const QString &channelName, const QList<Alert> &alerts)
{
    Source &source = sources()[kind];
    source.channel = channelName;
    source.alerts = alerts;
    apply(false);
}

void refresh()
{
    apply(true);
}

} // namespace SystemAlerts
