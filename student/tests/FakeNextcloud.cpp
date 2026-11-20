// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

#include "FakeNextcloud.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QTcpSocket>
#include <QUrl>
#include <QUrlQuery>

using namespace Qt::StringLiterals;

namespace {
const QString kDefaultPath = u"/default"_s;

void reply(QTcpSocket *socket, int status, const QByteArray &content = {})
{
    socket->write("HTTP/1.1 " + QByteArray::number(status) + " X\r\nContent-Length: "
                  + QByteArray::number(content.size()) + "\r\nConnection: keep-alive\r\n\r\n" + content);
}

QByteArray ocsBody(int status, const QJsonValue &data, const QString &message = {})
{
    return QJsonDocument(QJsonObject{{u"ocs"_s, QJsonObject{
        {u"meta"_s, QJsonObject{{u"status"_s, status == 200 ? u"ok"_s : u"failure"_s},
                                 {u"statuscode"_s, status}, {u"message"_s, message}}},
        {u"data"_s, data}}}}).toJson(QJsonDocument::Compact);
}

QByteArray basic(const QString &user, const QString &password)
{
    return "Basic " + (user + u':' + password).toUtf8().toBase64();
}
} // namespace

FakeNextcloud::FakeNextcloud(const QString &token, const QString &password, QObject *parent)
    : QTcpServer(parent)
{
    if (!token.isEmpty())
        links.insert(token, Link{u"1"_s, kDefaultPath, password, 5, {}});
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
                buffer->remove(0, end + 4 + length);
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
    bool ocsHeader = false;
    for (const QByteArray &line : lines) {
        const QByteArray lower = line.toLower();
        if (lower.startsWith("authorization:"))
            auth = line.mid(14).trimmed();
        if (lower.startsWith("x-requested-with:"))
            ajax = true;
        if (lower.startsWith("ocs-apirequest:"))
            ocsHeader = true;
    }

    // ---- teacher: OCS
    if (url.path().startsWith(u"/ocs/"_s)) {
        if (!ocsHeader)
            return reply(socket, 412);
        if (auth != basic(teacherLogin, teacherPassword))
            return reply(socket, 401, ocsBody(997, QJsonObject{}, u"Unauthorised"_s));
        return ocs(socket, method, url, body);
    }

    // ---- student: public link
    const QString prefix = u"/public.php/dav/files/"_s;
    QString token, name;
    bool legacy = false;
    if (url.path().startsWith(prefix)) {
        const QString rest = url.path().mid(prefix.size());
        token = rest.section(u'/', 0, 0);
        name = rest.section(u'/', 1);
    } else if (url.path().startsWith(u"/public.php/webdav/"_s)) {
        // Old public WebDAV: user name is the token
        const QByteArray decoded = QByteArray::fromBase64(auth.mid(6));
        token = QString::fromUtf8(decoded.left(decoded.indexOf(':')));
        name = url.path().mid(19);
        legacy = true;
        auth = basic(u"anonymous"_s, QString::fromUtf8(decoded.mid(decoded.indexOf(':') + 1)));
    } else {
        return reply(socket, 404);
    }
    if (shareGone || !links.contains(token))
        return reply(socket, 404);
    const Link link = links.value(token);
    if (link.expires.isValid() && link.expires < QDate::currentDate())
        return reply(socket, 404);
    if (auth != basic(u"anonymous"_s, link.password))
        return reply(socket, 401);
    if (!legacy && legacyOnly)
        return reply(socket, 404);
    QHash<QString, QByteArray> &folder = folders[link.path];

    if (method == "GET") {
        ++gets;
        return folder.contains(name) ? reply(socket, 200, folder.value(name)) : reply(socket, 404);
    }
    if (method == "PUT") {
        if (!ajax)                                  // CSRF check of Nextcloud
            return reply(socket, 412);
        if (!(link.permissions & 4))                // no "create"
            return reply(socket, 403);
        if (folder.contains(name) && !(link.permissions & 2))   // no "update"
            return reply(socket, 403);
        ++puts;
        folder.insert(name, body);
        return reply(socket, 201);
    }
    if (method == "DELETE") {
        if (!(link.permissions & 8))
            return reply(socket, 403);
        folder.remove(name);
        return reply(socket, 204);
    }
    reply(socket, 405);
}

void FakeNextcloud::ocs(QTcpSocket *socket, const QByteArray &method, const QUrl &url, const QByteArray &body)
{
    const QString path = url.path();
    const auto shareJson = [](const QString &token, const Link &l) {
        return QJsonObject{{u"id"_s, l.id}, {u"share_type"_s, 3}, {u"token"_s, token}, {u"path"_s, l.path},
                           {u"permissions"_s, l.permissions},
                           {u"expiration"_s, l.expires.isValid() ? QJsonValue(l.expires.toString(Qt::ISODate) + u" 00:00:00"_s)
                                                                 : QJsonValue()}};
    };

    if (path == u"/ocs/v2.php/cloud/capabilities"_s) {
        const QJsonObject pub{{u"enabled"_s, true}, {u"upload"_s, uploadAllowed},
                              {u"password"_s, QJsonObject{{u"enforced"_s, false}}},
                              {u"expire_date"_s, QJsonObject{{u"enabled"_s, expiryEnforced}, {u"enforced"_s, expiryEnforced},
                                                             {u"days"_s, QString::number(expiryDays)}}}};
        return reply(socket, 200, ocsBody(200, QJsonObject{{u"capabilities"_s, QJsonObject{
                                                  {u"files_sharing"_s, QJsonObject{{u"public"_s, pub}}}}}}));
    }
    const QString shares = u"/ocs/v2.php/apps/files_sharing/api/v1/shares"_s;
    const QUrlQuery form(QString::fromUtf8(body));
    if (path == shares && method == "POST") {
        Link l;
        l.id = QString::number(m_nextId++);
        l.path = form.queryItemValue(u"path"_s, QUrl::FullyDecoded);
        l.password = form.queryItemValue(u"password"_s, QUrl::FullyDecoded);
        l.permissions = form.queryItemValue(u"permissions"_s).toInt();
        if (form.hasQueryItem(u"expireDate"_s))
            l.expires = QDate::fromString(form.queryItemValue(u"expireDate"_s), Qt::ISODate);
        if (expiryEnforced && !l.expires.isValid())
            return reply(socket, 404, ocsBody(404, QJsonObject{}, u"Expiration date is enforced"_s));
        if ((l.permissions & 4) && !uploadAllowed)
            return reply(socket, 403, ocsBody(403, QJsonObject{}, u"Public upload disabled by the administrator"_s));
        if (l.password.isEmpty())
            return reply(socket, 400, ocsBody(400, QJsonObject{}, u"Password needed"_s));
        QString token;
        for (int i = 0; i < 15; ++i)
            token += QChar(u"abcdefghijkmnpqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ23456789"_s
                               .at(QRandomGenerator::global()->bounded(56)));
        links.insert(token, l);
        return reply(socket, 200, ocsBody(200, shareJson(token, l)));
    }
    if (path == shares && method == "GET") {
        const QString wanted = QUrlQuery(url).queryItemValue(u"path"_s, QUrl::FullyDecoded);
        QJsonArray list;
        for (auto it = links.constBegin(); it != links.constEnd(); ++it)
            if (it->path == wanted)
                list.append(shareJson(it.key(), *it));
        return reply(socket, 200, ocsBody(200, list));
    }
    if (path.startsWith(shares + u'/')) {
        const QString id = path.mid(shares.size() + 1);
        for (auto it = links.begin(); it != links.end(); ++it) {
            if (it->id != id)
                continue;
            if (method == "DELETE") {
                links.erase(it);
                return reply(socket, 200, ocsBody(200, QJsonArray{}));
            }
            if (method == "PUT") {
                if (form.hasQueryItem(u"expireDate"_s))
                    it->expires = QDate::fromString(form.queryItemValue(u"expireDate"_s), Qt::ISODate);
                return reply(socket, 200, ocsBody(200, shareJson(it.key(), *it)));
            }
        }
        return reply(socket, 404, ocsBody(404, QJsonObject{}, u"Wrong share ID"_s));
    }
    reply(socket, 404, ocsBody(404, QJsonObject{}));
}
