// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "Booklet.h"

#include "Crypto.h"

#include <QJsonDocument>
#include <QRegularExpression>

using namespace Qt::StringLiterals;

namespace Qupil::Booklet {

namespace {

constexpr auto Base64Url = QByteArray::Base64UrlEncoding | QByteArray::OmitTrailingEquals;

QByteArray adFor(Purpose purpose)
{
    return purpose == Purpose::Booklet ? "qupil-booklet-1"_ba : "qupil-feedback-1"_ba;
}

QString normalizedServer(QString server)
{
    server = server.trimmed();
    while (server.endsWith(u'/'))
        server.chop(1);
    return server;
}

} // namespace

bool Link::isValid() const
{
    const QUrl url(server);
    return url.isValid() && (url.scheme() == u"https"_s || url.scheme() == u"http"_s) && !url.host().isEmpty()
           && !token.isEmpty() && !token.contains(u'/') && key.size() == Crypto::KeySize;
}

QString Link::id() const
{
    return QString::fromLatin1(Crypto::hash((normalizedServer(server) + u'\n' + token).toUtf8(), 12)
                                   .toBase64(Base64Url));
}

QString encodeLink(const Link &link)
{
    QJsonObject o{{u"v"_s, LinkVersion}, {u"s"_s, normalizedServer(link.server)}, {u"t"_s, link.token},
                  {u"k"_s, QString::fromLatin1(link.key.toBase64(Base64Url))}};
    if (!link.password.isEmpty())
        o.insert(u"p"_s, link.password);
    return QString::fromLatin1(LinkPrefix)
           + QString::fromLatin1(QJsonDocument(o).toJson(QJsonDocument::Compact).toBase64(Base64Url));
}

std::optional<Link> decodeLink(const QString &text)
{
    QString s = text.trimmed();
    // From a QR code, a message or the clipboard: the part after "#" (or after the scheme)
    if (const qsizetype hash = s.indexOf(u'#'); hash >= 0)
        s = s.mid(hash + 1);
    else if (s.startsWith(u"qupil-student:"_s))
        s = s.mid(14);
    s.remove(QRegularExpression(u"\\s"_s));
    if (s.isEmpty())
        return std::nullopt;
    const auto decoded = QByteArray::fromBase64Encoding(s.toLatin1(), Base64Url | QByteArray::AbortOnBase64DecodingErrors);
    if (!decoded)
        return std::nullopt;
    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(*decoded, &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject())
        return std::nullopt;
    const QJsonObject o = doc.object();
    if (o.value(u"v"_s).toInt() != LinkVersion)
        return std::nullopt;
    Link link;
    link.server = normalizedServer(o.value(u"s"_s).toString());
    link.token = o.value(u"t"_s).toString();
    link.password = o.value(u"p"_s).toString();
    const auto key = QByteArray::fromBase64Encoding(o.value(u"k"_s).toString().toLatin1(),
                                                    Base64Url | QByteArray::AbortOnBase64DecodingErrors);
    if (key)
        link.key = *key;
    if (!link.isValid())
        return std::nullopt;
    return link;
}

QUrl fileUrl(const Link &link, const QString &fileName)
{
    return QUrl(normalizedServer(link.server) + u"/public.php/dav/files/"_s + link.token + u'/'
                + QString::fromLatin1(QUrl::toPercentEncoding(fileName)));
}

QUrl legacyFileUrl(const Link &link, const QString &fileName)
{
    return QUrl(normalizedServer(link.server) + u"/public.php/webdav/"_s
                + QString::fromLatin1(QUrl::toPercentEncoding(fileName)));
}

QByteArray legacyAuthorization(const Link &link)
{
    return "Basic " + (link.token + u':' + link.password).toUtf8().toBase64();
}

QByteArray authorization(const Link &link)
{
    return "Basic " + (u"anonymous:"_s + link.password).toUtf8().toBase64();
}

QByteArray seal(const QByteArray &key, Purpose purpose, const QJsonObject &content)
{
    const QByteArray json = QJsonDocument(content).toJson(QJsonDocument::Compact);
    return Crypto::seal(key, qCompress(json, 9), adFor(purpose));
}

std::optional<QJsonObject> open(const QByteArray &key, Purpose purpose, const QByteArray &sealed)
{
    const auto packed = Crypto::open(key, sealed, adFor(purpose));
    if (!packed)
        return std::nullopt;
    const QByteArray json = qUncompress(*packed);
    if (json.isEmpty())
        return std::nullopt;
    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject())
        return std::nullopt;
    return doc.object();
}

QString feedbackFileName(const QString &deviceId, qint64 msecsSinceEpoch)
{
    QString device = deviceId;
    device.remove(QRegularExpression(u"[^A-Za-z0-9_]"_s));
    return QString::fromLatin1(FeedbackPrefix) + device.left(24) + u'-' + QString::number(msecsSinceEpoch)
           + QString::fromLatin1(FileSuffix);
}

bool isFeedbackFileName(const QString &name)
{
    static const QRegularExpression re(u"^fb-[A-Za-z0-9_]{1,24}-[0-9]{1,16}\\.qupil$"_s);
    return re.match(name).hasMatch();
}

} // namespace Qupil::Booklet
