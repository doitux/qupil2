// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QByteArray>
#include <QDate>
#include <QNetworkAccessManager>
#include <QString>
#include <QUrl>

#include <optional>

namespace Qupil {

/*!
    Public links in the teacher's Nextcloud (OCS share API), for the students'
    practice books: one folder per student, shared by a link with a password.

    The link lets the student app read the folder and add files (feedback), but not
    change or delete anything (permissions 5 = read + create). A Nextcloud that does
    not allow uploads by link gets a read-only link: then no feedback comes back.

    Blocking calls, like WebDavFolder: create and use it in a worker thread.
*/
class NextcloudShares
{
public:
    NextcloudShares(const QUrl &server, const QString &loginName, const QString &appPassword);

    //! What this Nextcloud allows for links (from its capabilities)
    struct Policy
    {
        bool linksAllowed = false;
        bool uploadAllowed = false;     // "Allow public uploads"
        bool passwordEnforced = false;
        bool expiryEnforced = false;
        int expiryDays = 0;             // maximum when enforced
    };
    std::optional<Policy> policy();

    struct Share
    {
        QString id;
        QString token;
        int permissions = 0;
        QDate expires;                  // invalid: never
        bool canReceive() const { return permissions & 4; }
    };

    /*!
        A link for \a path (relative to the user's files, e.g. "Qupil/Übeheft/x7Kp"),
        protected by \a password. Read + create where allowed, otherwise read only.
        With an enforced expiry the link runs as long as allowed; renew() extends it.
    */
    std::optional<Share> createLink(const QString &path, const QString &password);
    //! Moves the end of an expiring link as far out as allowed again
    bool renew(const Share &share);
    //! Links that exist for \a path (to find one again after a reinstall)
    std::optional<QList<Share>> linksFor(const QString &path);
    //! Ends a link: the student app shows "ended by your teacher"
    bool remove(const QString &id);

    QString lastError() const { return m_error; }

private:
    struct Reply
    {
        int status = 0;
        QByteArray body;
    };
    Reply send(const QByteArray &verb, const QString &endpoint, const QByteArray &form = {});
    std::optional<Share> parseShare(const QJsonObject &data) const;
    bool ok(const Reply &reply, QJsonObject *data = nullptr);

    QNetworkAccessManager m_network;
    QUrl m_server;
    QByteArray m_auth;
    QString m_error;
    std::optional<Policy> m_policy;
};

} // namespace Qupil
