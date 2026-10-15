// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QNetworkAccessManager>
#include <QUrl>

#include "FolderSync.h"

/*!
    A folder in the user's Nextcloud (WebDAV), with blocking calls for the
    sync worker thread. Create and use it in that thread.
*/
class WebDavFolder : public Qupil::RemoteFolder
{
public:
    /*!
        server like https://cloud.example.org. Nextcloud signs in with the login
        name (may be an e-mail address); the files live under the user id.
    */
    WebDavFolder(const QUrl &server, const QString &loginName, const QString &userId, const QString &appPassword);

    bool makeFolder(const QString &path) override;
    std::optional<QStringList> list(const QString &path) override;
    std::optional<QByteArray> read(const QString &path) override;
    bool write(const QString &path, const QByteArray &data) override;
    bool move(const QString &from, const QString &to) override;
    QString lastError() const override { return m_error; }
    bool lastErrorWasNotFound() const override { return m_notFound; }
    //! HTTP status of the last request (401: the app password was revoked)
    int lastStatus() const { return m_lastStatus; }

    //! The files URL of a user, e.g. https://cloud/remote.php/dav/files/anna/
    static QUrl filesUrl(const QUrl &server, const QString &user);

private:
    struct Reply
    {
        int status = 0;
        QByteArray body;
        QString error;
    };
    Reply send(const QByteArray &verb, const QString &path, const QByteArray &body = {},
               const QByteArray &contentType = {}, int depth = -1, const QByteArray &destination = {});
    QUrl url(const QString &path) const;

    QNetworkAccessManager m_network;
    QUrl m_base;
    QByteArray m_auth;
    QString m_error;
    bool m_notFound = false;
    int m_lastStatus = 0;
};
