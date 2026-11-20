// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

#include "FakeNextcloud.h"

#include <QTcpSocket>
#include <QUrl>
#include <QUrlQuery>

using namespace Qt::StringLiterals;

FakeNextcloud::FakeNextcloud(const QString &token, const QString &password, QObject *parent)
    : QTcpServer(parent), m_token(token), m_auth("Basic " + ("anonymous:" + password.toUtf8()).toBase64())
{
    listen(QHostAddress::LocalHost);
    connect(this, &QTcpServer::newConnection, this, [this] {
        while (QTcpSocket *socket = nextPendingConnection()) {
            auto *buffer = new QByteArray;
            connect(socket, &QTcpSocket::readyRead, socket, [this, socket, buffer] {
                buffer->append(socket->readAll());
                const qsizetype end = buffer->indexOf("\r\n\r\n");
                if (end < 0)
                    return;
                const QByteArray head = buffer->left(end);
                qsizetype length = 0;
                for (const QByteArray &line : head.split('\n'))
                    if (line.toLower().startsWith("content-length:"))
                        length = line.mid(15).trimmed().toLongLong();
                if (buffer->size() < end + 4 + length)
                    return;
                socket->setProperty("head", head);
                socket->setProperty("body", buffer->mid(end + 4, length));
                buffer->clear();
                handle(socket);
            });
            connect(socket, &QTcpSocket::disconnected, socket, [socket, buffer] {
                delete buffer;
                socket->deleteLater();
            });
        }
    });
}

QString FakeNextcloud::url() const
{
    return u"http://127.0.0.1:%1"_s.arg(serverPort());
}

void FakeNextcloud::handle(QTcpSocket *socket)
{
    const QByteArray head = socket->property("head").toByteArray();
    const QByteArray body = socket->property("body").toByteArray();
    const QList<QByteArray> lines = head.split('\n');
    const QList<QByteArray> request = lines.value(0).trimmed().split(' ');
    const QByteArray method = request.value(0);
    const QUrl url(QString::fromLatin1(request.value(1)));
    QByteArray auth;
    bool ajax = false;
    for (const QByteArray &line : lines) {
        if (line.toLower().startsWith("authorization:"))
            auth = line.mid(14).trimmed();
        if (line.toLower().startsWith("x-requested-with:"))
            ajax = true;
    }

    auto reply = [socket](int status, const QByteArray &content = {}) {
        socket->write("HTTP/1.1 " + QByteArray::number(status) + " X\r\nContent-Length: "
                      + QByteArray::number(content.size()) + "\r\nConnection: keep-alive\r\n\r\n" + content);
    };

    if (shareGone)
        return reply(404);
    if (auth != m_auth)
        return reply(401);

    const QString dav = u"/public.php/dav/files/%1/"_s.arg(m_token);
    if (url.path().startsWith(dav)) {
        if (legacyOnly)
            return reply(404);
        const QString name = url.path().mid(dav.size());
        if (method == "GET") {
            ++gets;
            return files.contains(name) ? reply(200, files.value(name)) : reply(404);
        }
        if (method == "PUT") {
            if (!ajax)   // Nextcloud refuses writes without it (CSRF check)
                return reply(412);
            ++puts;
            files.insert(name, body);
            return reply(201);
        }
        return reply(405);
    }
    if (url.path() == u"/s/%1/download"_s.arg(m_token) && method == "GET") {
        ++gets;
        const QString name = QUrlQuery(url).queryItemValue(u"files"_s);
        return files.contains(name) ? reply(200, files.value(name)) : reply(404);
    }
    reply(404);
}
