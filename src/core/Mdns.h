// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QByteArray>
#include <QList>
#include <QString>

#include <optional>

/*!
    Multicast DNS messages (RFC 6762) for finding other Qupil devices in the
    same Wi-Fi with DNS service discovery (RFC 6763), the way Bonjour does it.
    Only what Qupil needs: questions and PTR, SRV, TXT, A and AAAA records.
*/
namespace Qupil::Mdns {

enum Type : quint16 {
    A = 1,
    PTR = 12,
    TXT = 16,
    AAAA = 28,
    SRV = 33,
    ANY = 255
};

struct Question
{
    QString name;
    quint16 type = PTR;
    bool unicastResponse = false;   //!< "QU" bit
};

struct Record
{
    QString name;
    quint16 type = PTR;
    bool cacheFlush = false;
    quint32 ttl = 120;
    QString target;               //!< PTR: instance name; SRV: host name
    quint16 port = 0;             //!< SRV
    QList<QByteArray> txt;        //!< TXT: "key=value" strings
    QByteArray address;           //!< A: 4 bytes, AAAA: 16 bytes (network order)
};

struct Message
{
    quint16 id = 0;
    bool response = false;
    QList<Question> questions;
    QList<Record> answers;
    QList<Record> additional;     //!< authority records are read into here as well
};

QByteArray encode(const Message &message);
//! nullopt for packets that are not valid DNS messages
std::optional<Message> decode(const QByteArray &packet);

//! Names compare without regard to case
bool sameName(const QString &a, const QString &b);
//! "key=value" from TXT strings
QString txtValue(const QList<QByteArray> &txt, const QByteArray &key);

} // namespace Qupil::Mdns
