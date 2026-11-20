// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// A tiny stand-in for a Nextcloud public share (public WebDAV, Nextcloud 29+):
// GET and PUT of files below /public.php/dav/files/<token>/, Basic auth with the
// share password. Optionally old Nextcloud (no public WebDAV, only /s/<token>/download).

#pragma once

#include <QHash>
#include <QTcpServer>

class FakeNextcloud : public QTcpServer
{
    Q_OBJECT
public:
    explicit FakeNextcloud(const QString &token, const QString &password, QObject *parent = nullptr);

    QString url() const;
    QHash<QString, QByteArray> files;   // name -> content
    bool legacyOnly = false;            // Nextcloud before 29
    bool shareGone = false;             // the teacher ended the share
    int puts = 0;
    int gets = 0;

private:
    void handle(QTcpSocket *socket);
    QString m_token;
    QByteArray m_auth;
};
