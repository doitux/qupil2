// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <QString>
#include <QUrl>

#include <optional>

/*!
    The student's booklet ("Übeheft") and the way back, shared by the teacher's
    Qupil and the student app (docs/student-app.md).

    The teacher's Qupil writes one booklet per coupled student into a folder of
    the teacher's Nextcloud and shares that folder with a public link. The link,
    the folder's key and an optional link password reach the student only through
    the coupling link (QR code). The part after "#" never goes to any server.

    - Booklet: file "heft.qupil" = sealed(compressed JSON), see seal()
    - Feedback: files "fb-<device>-<time>.qupil" the student app uploads into the
      same folder (the link allows reading and creating files, not changing them).
      Each holds the full current feedback of one device; the newest entry wins.

    Every file is sealed with XChaCha20-Poly1305 (Crypto::seal) and a purpose
    string as additional data, so a booklet can never be taken for feedback.
*/
namespace Qupil::Booklet {

constexpr int Format = 1;                    // booklet "format"; the student app refuses a higher one
constexpr int LinkVersion = 1;

//! Where the coupling link points (shows the store links when no app is installed)
inline constexpr char LinkPrefix[] = "https://qupil.de/u#";
inline constexpr char BookletFile[] = "heft.qupil";
inline constexpr char FeedbackPrefix[] = "fb-";
inline constexpr char FileSuffix[] = ".qupil";

//! What the student needs to read the booklet and send feedback
struct Link
{
    QString server;        // https://cloud.example.org (no trailing slash)
    QString token;         // public share token
    QString password;      // link password, if the Nextcloud requires one
    QByteArray key;        // 32 bytes

    bool isValid() const;
    //! A stable id for this coupling on the student's device (hash of server + token)
    QString id() const;
};

//! https://qupil.de/u#<base64url of compact JSON>
QString encodeLink(const Link &link);
//! Also accepts the bare fragment and a "qupil-student:" link; nullopt if it is none
std::optional<Link> decodeLink(const QString &text);

//! Public WebDAV address of a file in the shared folder (Nextcloud 29 and later)
QUrl fileUrl(const Link &link, const QString &fileName);
//! Basic authentication for the public WebDAV ("anonymous" and the link password)
QByteArray authorization(const Link &link);
//! Older Nextclouds (before 29): the old public WebDAV, signed in with token and password
QUrl legacyFileUrl(const Link &link, const QString &fileName);
QByteArray legacyAuthorization(const Link &link);

enum class Purpose { Booklet, Feedback };

QByteArray seal(const QByteArray &key, Purpose purpose, const QJsonObject &content);
//! nullopt if the key is wrong, the data was changed or it is not this kind of file
std::optional<QJsonObject> open(const QByteArray &key, Purpose purpose, const QByteArray &sealed);

//! A file name for one upload of feedback ("fb-<device>-<ms since epoch>.qupil")
QString feedbackFileName(const QString &deviceId, qint64 msecsSinceEpoch);
bool isFeedbackFileName(const QString &name);

} // namespace Qupil::Booklet
