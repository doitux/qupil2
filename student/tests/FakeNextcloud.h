// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// A tiny stand-in for Nextcloud, enough for the practice book:
// - public links: GET and PUT below /public.php/dav/files/<token>/ (Nextcloud 29+) with
//   the link password, or the old /public.php/webdav/ (token and password; Nextcloud
//   before 29); permissions as set
// - the teacher's side: capabilities and the OCS share API (create, list, renew, delete)

#pragma once

#include <QDate>
#include <QHash>
#include <QMap>
#include <QTcpServer>

class FakeNextcloud : public QTcpServer
{
    Q_OBJECT
public:
    //! With a ready link (\a token, \a password) for the student app's tests
    explicit FakeNextcloud(const QString &token = {}, const QString &password = {}, QObject *parent = nullptr);

    QString url() const;

    struct Link
    {
        QString id, path, password;
        int permissions = 1;
        QDate expires;
    };
    QMap<QString, Link> links;                    // token -> link
    QHash<QString, QHash<QString, QByteArray>> folders;   // path -> name -> content

    //! The folder of the constructor's link (student app tests)
    QHash<QString, QByteArray> &files() { return folders[QStringLiteral("/default")]; }

    // Behaviour
    bool legacyOnly = false;        // Nextcloud before 29
    bool shareGone = false;         // every link answers 404
    bool down = false;              // the cloud is not reachable right now: 503
    bool uploadAllowed = true;      // "Allow public uploads"
    bool expiryEnforced = false;
    int expiryDays = 7;
    QString teacherLogin = QStringLiteral("teacher");
    QString teacherPassword = QStringLiteral("app-password-for-tests");

    int puts = 0;
    int gets = 0;

private:
    void handle(class QTcpSocket *socket);
    void ocs(class QTcpSocket *socket, const QByteArray &method, const class QUrl &url, const QByteArray &body);
    int m_nextId = 100;
};
