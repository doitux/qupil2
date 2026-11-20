// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

#include "NextcloudShares.h"

#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QUrlQuery>

using namespace Qt::StringLiterals;

namespace Qupil {

namespace {
constexpr int kTimeoutMs = 30000;
constexpr int kRead = 1;
constexpr int kCreate = 4;
const QString kShares = u"/ocs/v2.php/apps/files_sharing/api/v1/shares"_s;
} // namespace

NextcloudShares::NextcloudShares(const QUrl &server, const QString &loginName, const QString &appPassword)
    : m_server(server)
    , m_auth("Basic " + (loginName + u':' + appPassword).toUtf8().toBase64())
{
    QString path = m_server.path();
    while (path.endsWith(u'/'))
        path.chop(1);
    m_server.setPath(path);
}

NextcloudShares::Reply NextcloudShares::send(const QByteArray &verb, const QString &endpoint, const QByteArray &form)
{
    QUrl url = m_server;
    const qsizetype query = endpoint.indexOf(u'?');
    url.setPath(m_server.path() + endpoint.left(query));
    QUrlQuery q(query >= 0 ? endpoint.mid(query + 1) : QString());
    q.addQueryItem(u"format"_s, u"json"_s);
    url.setQuery(q);

    QNetworkRequest request(url);
    request.setRawHeader("Authorization", m_auth);
    request.setRawHeader("OCS-APIRequest", "true");
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("User-Agent", "Qupil");
    if (!form.isEmpty())
        request.setHeader(QNetworkRequest::ContentTypeHeader, u"application/x-www-form-urlencoded"_s);
    request.setTransferTimeout(kTimeoutMs);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);

    QNetworkReply *reply = m_network.sendCustomRequest(request, verb, form);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    Reply r;
    r.status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    r.body = reply->readAll();
    if (r.status == 0)
        m_error = reply->errorString();
    reply->deleteLater();
    return r;
}

bool NextcloudShares::ok(const Reply &reply, QJsonObject *data)
{
    const QJsonObject ocs = QJsonDocument::fromJson(reply.body).object().value(u"ocs"_s).toObject();
    const QJsonObject meta = ocs.value(u"meta"_s).toObject();
    const int code = meta.value(u"statuscode"_s).toInt(reply.status);
    if (reply.status >= 200 && reply.status < 300 && (code == 100 || code == 200)) {
        if (data)
            *data = ocs.value(u"data"_s).toObject();
        m_error.clear();
        return true;
    }
    if (reply.status != 0) {
        const QString message = meta.value(u"message"_s).toString();
        m_error = u"HTTP %1%2"_s.arg(reply.status).arg(message.isEmpty() ? QString() : u": "_s + message);
    }
    return false;
}

std::optional<NextcloudShares::Policy> NextcloudShares::policy()
{
    if (m_policy)
        return m_policy;
    QJsonObject data;
    if (!ok(send("GET", u"/ocs/v2.php/cloud/capabilities"_s), &data))
        return std::nullopt;
    const QJsonObject pub = data.value(u"capabilities"_s).toObject()
                                .value(u"files_sharing"_s).toObject()
                                .value(u"public"_s).toObject();
    Policy p;
    p.linksAllowed = pub.value(u"enabled"_s).toBool();
    p.uploadAllowed = pub.value(u"upload"_s).toBool();
    p.passwordEnforced = pub.value(u"password"_s).toObject().value(u"enforced"_s).toBool();
    const QJsonObject expiry = pub.value(u"expire_date"_s).toObject();
    p.expiryEnforced = expiry.value(u"enabled"_s).toBool() && expiry.value(u"enforced"_s).toBool();
    p.expiryDays = expiry.value(u"days"_s).toVariant().toInt();
    m_policy = p;
    return p;
}

std::optional<NextcloudShares::Share> NextcloudShares::parseShare(const QJsonObject &data) const
{
    Share s;
    s.id = data.value(u"id"_s).toVariant().toString();
    s.token = data.value(u"token"_s).toString();
    s.permissions = data.value(u"permissions"_s).toVariant().toInt();
    // "2026-12-31 00:00:00" or null
    s.expires = QDate::fromString(data.value(u"expiration"_s).toString().left(10), Qt::ISODate);
    if (s.id.isEmpty() || s.token.isEmpty())
        return std::nullopt;
    return s;
}

std::optional<NextcloudShares::Share> NextcloudShares::createLink(const QString &path, const QString &password)
{
    const auto p = policy();
    if (!p)
        return std::nullopt;
    if (!p->linksAllowed) {
        m_error = u"links-disabled"_s;
        return std::nullopt;
    }
    const auto attempt = [&](int permissions) -> std::optional<Share> {
        QUrlQuery form;
        form.addQueryItem(u"path"_s, path.startsWith(u'/') ? path : u'/' + path);
        form.addQueryItem(u"shareType"_s, u"3"_s);
        form.addQueryItem(u"password"_s, password);
        form.addQueryItem(u"permissions"_s, QString::number(permissions));
        if (p->expiryEnforced && p->expiryDays > 0)
            form.addQueryItem(u"expireDate"_s, QDate::currentDate().addDays(p->expiryDays).toString(Qt::ISODate));
        QJsonObject data;
        if (!ok(send("POST", kShares, form.toString(QUrl::FullyEncoded).toUtf8()), &data))
            return std::nullopt;
        return parseShare(data);
    };
    if (p->uploadAllowed) {
        if (auto share = attempt(kRead | kCreate))
            return share;
    }
    return attempt(kRead);
}

bool NextcloudShares::renew(const Share &share)
{
    const auto p = policy();
    if (!p)
        return false;
    if (!p->expiryEnforced || p->expiryDays <= 0)
        return true;
    QUrlQuery form;
    form.addQueryItem(u"expireDate"_s, QDate::currentDate().addDays(p->expiryDays).toString(Qt::ISODate));
    return ok(send("PUT", kShares + u'/' + share.id, form.toString(QUrl::FullyEncoded).toUtf8()));
}

std::optional<QList<NextcloudShares::Share>> NextcloudShares::linksFor(const QString &path)
{
    const Reply r = send("GET", kShares + u"?path="_s
                                    + QString::fromLatin1(QUrl::toPercentEncoding(path.startsWith(u'/') ? path : u'/' + path))
                                    + u"&reshares=false"_s);
    if (r.status == 404)   // nothing shared there (or the folder is gone)
        return QList<Share>{};
    const QJsonObject ocs = QJsonDocument::fromJson(r.body).object().value(u"ocs"_s).toObject();
    QJsonObject dummy;
    if (!ok(r, &dummy) && !(r.status >= 200 && r.status < 300))
        return std::nullopt;
    QList<Share> out;
    for (const QJsonValue &v : ocs.value(u"data"_s).toArray()) {
        const QJsonObject o = v.toObject();
        if (o.value(u"share_type"_s).toInt() != 3)
            continue;
        if (auto s = parseShare(o))
            out.append(*s);
    }
    return out;
}

bool NextcloudShares::remove(const QString &id)
{
    const Reply r = send("DELETE", kShares + u'/' + id);
    return ok(r) || r.status == 404;
}

} // namespace Qupil
