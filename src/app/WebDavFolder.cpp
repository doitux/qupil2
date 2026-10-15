// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "WebDavFolder.h"

#include <QBuffer>
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QXmlStreamReader>

using namespace Qt::StringLiterals;

namespace {
constexpr int kTimeoutMs = 60000;
}

QUrl WebDavFolder::filesUrl(const QUrl &server, const QString &user)
{
    QUrl url = server;
    QString path = url.path();
    if (!path.endsWith(u'/'))
        path += u'/';
    url.setPath(path + u"remote.php/dav/files/"_s + user + u'/');
    return url;
}

WebDavFolder::WebDavFolder(const QUrl &server, const QString &loginName, const QString &userId,
                           const QString &appPassword)
    : m_base(filesUrl(server, userId))
    , m_auth("Basic " + (loginName + u':' + appPassword).toUtf8().toBase64())
{
}

QUrl WebDavFolder::url(const QString &path) const
{
    QUrl u = m_base;
    // Every segment percent-encoded (device ids are safe, but stay correct)
    QStringList segments;
    for (const QString &s : path.split(u'/', Qt::SkipEmptyParts))
        segments << QString::fromLatin1(QUrl::toPercentEncoding(s));
    u.setPath(m_base.path(QUrl::FullyEncoded) + segments.join(u'/'), QUrl::StrictMode);
    return u;
}

WebDavFolder::Reply WebDavFolder::send(const QByteArray &verb, const QString &path, const QByteArray &body,
                                       const QByteArray &contentType, int depth, const QByteArray &destination)
{
    QNetworkRequest request(url(path));
    request.setRawHeader("Authorization", m_auth);
    request.setRawHeader("User-Agent", "Qupil");
    request.setRawHeader("OCS-APIRequest", "true");
    if (!contentType.isEmpty())
        request.setHeader(QNetworkRequest::ContentTypeHeader, contentType);
    if (depth >= 0)
        request.setRawHeader("Depth", QByteArray::number(depth));
    if (!destination.isEmpty()) {
        request.setRawHeader("Destination", destination);
        request.setRawHeader("Overwrite", "F");
    }
    request.setTransferTimeout(kTimeoutMs);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    QBuffer *buffer = nullptr;
    if (!body.isEmpty()) {
        buffer = new QBuffer;
        buffer->setData(body);
        buffer->open(QIODevice::ReadOnly);
    }
    QNetworkReply *reply = m_network.sendCustomRequest(request, verb, buffer);
    if (buffer)
        buffer->setParent(reply);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    Reply r;
    r.status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    m_lastStatus = r.status;
    r.body = reply->readAll();
    if (reply->error() != QNetworkReply::NoError && r.status == 0)
        r.error = reply->errorString();
    reply->deleteLater();
    return r;
}

bool WebDavFolder::makeFolder(const QString &path)
{
    m_notFound = false;
    const Reply r = send("MKCOL", path);
    // 201 created, 405 already there
    if (r.status == 201 || r.status == 405)
        return true;
    m_error = !r.error.isEmpty() ? r.error : u"MKCOL %1: HTTP %2"_s.arg(path).arg(r.status);
    return false;
}

std::optional<QStringList> WebDavFolder::list(const QString &path)
{
    m_notFound = false;
    const QByteArray body =
        "<?xml version=\"1.0\"?><d:propfind xmlns:d=\"DAV:\"><d:prop><d:resourcetype/></d:prop></d:propfind>";
    const Reply r = send("PROPFIND", path, body, "application/xml; charset=utf-8", 1);
    if (r.status == 404)
        return QStringList();
    if (r.status != 207) {
        m_error = !r.error.isEmpty() ? r.error : u"PROPFIND %1: HTTP %2"_s.arg(path).arg(r.status);
        return std::nullopt;
    }

    QStringList names;
    const QString self = url(path).path(QUrl::FullyDecoded);
    QXmlStreamReader xml(r.body);
    QString href;
    bool collection = false;
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            if (xml.name() == u"response")
                href.clear(), collection = false;
            else if (xml.name() == u"href")
                href = QUrl::fromPercentEncoding(xml.readElementText().toUtf8());
            else if (xml.name() == u"collection")
                collection = true;
        } else if (xml.isEndElement() && xml.name() == u"response") {
            QString p = href;
            if (p.endsWith(u'/'))
                p.chop(1);
            QString selfPath = self;
            if (selfPath.endsWith(u'/'))
                selfPath.chop(1);
            if (p == selfPath || p.isEmpty())
                continue;   // the folder itself
            const QString name = p.section(u'/', -1);
            names << (collection ? name + u'/' : name);
        }
    }
    if (xml.hasError()) {
        m_error = xml.errorString();
        return std::nullopt;
    }
    return names;
}

std::optional<QByteArray> WebDavFolder::read(const QString &path)
{
    const Reply r = send("GET", path);
    m_notFound = r.status == 404;
    if (r.status == 200)
        return r.body;
    m_error = !r.error.isEmpty() ? r.error : u"GET %1: HTTP %2"_s.arg(path).arg(r.status);
    return std::nullopt;
}

bool WebDavFolder::write(const QString &path, const QByteArray &data)
{
    m_notFound = false;
    const Reply r = send("PUT", path, data.isEmpty() ? QByteArray(" ") : data, "application/octet-stream");
    if (r.status == 200 || r.status == 201 || r.status == 204)
        return true;
    m_error = !r.error.isEmpty() ? r.error : u"PUT %1: HTTP %2"_s.arg(path).arg(r.status);
    return false;
}

bool WebDavFolder::move(const QString &from, const QString &to)
{
    m_notFound = false;
    const Reply r = send("MOVE", from, {}, {}, -1, url(to).toEncoded());
    m_notFound = r.status == 404;
    if (r.status == 201 || r.status == 204)
        return true;
    m_error = !r.error.isEmpty() ? r.error : u"MOVE %1: HTTP %2"_s.arg(from).arg(r.status);
    return false;
}
