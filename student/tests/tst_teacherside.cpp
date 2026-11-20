// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// The teacher's side of the practice book, against a fake Nextcloud: links for a
// student's folder (with and without uploads, enforced expiry), and collecting what
// the student app(s) sent back. Invented data only.

#include "Booklet.h"
#include "BookletInbox.h"
#include "Crypto.h"
#include "FakeNextcloud.h"
#include "NextcloudShares.h"

#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSignalSpy>
#include <QTest>

using namespace Qt::StringLiterals;
using namespace Qupil;

namespace {

QByteArray feedbackFile(const QByteArray &key, const QJsonObject &feedback, const QJsonArray &questions = {})
{
    return Booklet::seal(key, Booklet::Purpose::Feedback,
                         QJsonObject{{u"device"_s, u"x"_s}, {u"feedback"_s, feedback}, {u"questions"_s, questions}});
}

QJsonObject entry(int practiced, int works, double at, bool done = false)
{
    return QJsonObject{{u"practiced"_s, practiced}, {u"works"_s, works}, {u"done"_s, done}, {u"at"_s, at}};
}

} // namespace

class TestTeacherSide : public QObject
{
    Q_OBJECT

    FakeNextcloud *m_cloud = nullptr;

    NextcloudShares shares() const
    {
        return NextcloudShares(QUrl(m_cloud->url()), m_cloud->teacherLogin, m_cloud->teacherPassword);
    }

    // What the student app does with the link: read the booklet, put a file
    int studentPut(const Booklet::Link &link, const QString &name, const QByteArray &data)
    {
        QNetworkAccessManager net;
        QNetworkRequest request(Booklet::fileUrl(link, name));
        request.setRawHeader("Authorization", Booklet::authorization(link));
        request.setRawHeader("X-Requested-With", "XMLHttpRequest");
        QNetworkReply *reply = net.put(request, data);
        QSignalSpy done(reply, &QNetworkReply::finished);
        done.wait(5000);
        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        delete reply;
        return status;
    }

private slots:
    void init()
    {
        delete m_cloud;
        m_cloud = new FakeNextcloud;
        QVERIFY(m_cloud->isListening());
    }
    void cleanupTestCase() { delete m_cloud; }

    void linkWithUpload()
    {
        NextcloudShares api = shares();
        const QString path = u"/Qupil/Übeheft/a1b2"_s;
        const auto share = api.createLink(path, u"Lnk-pw-0123456789"_s);
        QVERIFY2(share.has_value(), qPrintable(api.lastError()));
        QCOMPARE(share->permissions, 5);   // read + create, nothing else
        QVERIFY(share->canReceive());
        QVERIFY(!share->expires.isValid());

        // The student app can read and add, but not overwrite or delete
        Booklet::Link link{m_cloud->url(), share->token, u"Lnk-pw-0123456789"_s, Crypto::randomBytes(Crypto::KeySize)};
        m_cloud->folders[path].insert(u"heft.qupil"_s, "sealed booklet");
        QCOMPARE(studentPut(link, u"fb-dev1-1.qupil"_s, "x"), 201);
        QCOMPARE(studentPut(link, u"heft.qupil"_s, "replaced"), 403);
        QCOMPARE(m_cloud->folders[path].value(u"heft.qupil"_s), QByteArray("sealed booklet"));

        // Found again, and ended
        const auto found = api.linksFor(path);
        QVERIFY(found.has_value());
        QCOMPARE(found->size(), 1);
        QCOMPARE(found->constFirst().token, share->token);
        QVERIFY(api.remove(share->id));
        QCOMPARE(api.linksFor(path)->size(), 0);
        QCOMPARE(studentPut(link, u"fb-dev1-2.qupil"_s, "x"), 404);
        QVERIFY(api.remove(share->id));   // already gone: fine
    }

    void linkWithoutUpload()
    {
        // The admin does not allow uploads by link: read only, no feedback
        m_cloud->uploadAllowed = false;
        NextcloudShares api = shares();
        const auto share = api.createLink(u"/Qupil/Übeheft/c3d4"_s, u"Lnk-pw-0123456789"_s);
        QVERIFY2(share.has_value(), qPrintable(api.lastError()));
        QCOMPARE(share->permissions, 1);
        QVERIFY(!share->canReceive());
    }

    void enforcedExpiry()
    {
        m_cloud->expiryEnforced = true;
        m_cloud->expiryDays = 30;
        NextcloudShares api = shares();
        const auto share = api.createLink(u"/Qupil/Übeheft/e5f6"_s, u"Lnk-pw-0123456789"_s);
        QVERIFY2(share.has_value(), qPrintable(api.lastError()));
        QCOMPARE(share->expires, QDate::currentDate().addDays(30));

        // Some days later: renewed to 30 days from then
        m_cloud->links[share->token].expires = QDate::currentDate().addDays(3);
        QVERIFY(api.renew(*share));
        QCOMPARE(m_cloud->links.value(share->token).expires, QDate::currentDate().addDays(30));
    }

    void wrongPassword()
    {
        NextcloudShares api(QUrl(m_cloud->url()), m_cloud->teacherLogin, u"wrong"_s);
        QVERIFY(!api.createLink(u"/Qupil/x"_s, u"Lnk-pw-0123456789"_s));
        QVERIFY(api.lastError().startsWith(u"HTTP 401"_s));
    }

    void inboxNewestPerDevice()
    {
        const QByteArray key = Crypto::randomBytes(Crypto::KeySize);
        QHash<QString, QByteArray> files;
        // Phone: an old and a new upload; the new one holds everything
        files.insert(u"fb-phone-1000.qupil"_s, feedbackFile(key, {{u"t1"_s, entry(1, 1, 1000)}}));
        files.insert(u"fb-phone-2000.qupil"_s, feedbackFile(key, {{u"t1"_s, entry(3, 2, 2000)}, {u"t2"_s, entry(0, 0, 1900, true)}},
                                                            QJsonArray{QJsonObject{{u"id"_s, u"q1"_s}, {u"text"_s, u"Wie schnell?"_s}, {u"at"_s, 1950.0}}}));
        // Tablet: a later answer for t1
        files.insert(u"fb-tablet-1500.qupil"_s, feedbackFile(key, {{u"t1"_s, entry(2, 3, 2500)}}));
        // Not ours
        files.insert(u"fb-other-3000.qupil"_s, feedbackFile(Crypto::randomBytes(Crypto::KeySize), {{u"t1"_s, entry(1, 1, 9999)}}));
        files.insert(u"heft.qupil"_s, "booklet");
        files.insert(u"notes.txt"_s, "?");

        const Booklet::Inbox inbox = Booklet::collect(key, files);
        QVERIFY(inbox.changed);
        QCOMPARE(inbox.superseded, QStringList{u"fb-phone-1000.qupil"_s});
        QCOMPARE(inbox.unreadable, QStringList{u"fb-other-3000.qupil"_s});
        QCOMPARE(inbox.feedback.value(u"t1"_s).toObject().value(u"practiced"_s).toInt(), 2);   // tablet is newer
        QCOMPARE(inbox.feedback.value(u"t1"_s).toObject().value(u"works"_s).toInt(), 3);
        QVERIFY(inbox.feedback.value(u"t2"_s).toObject().value(u"done"_s).toBool());
        QCOMPARE(inbox.questions.size(), 1);
        QCOMPARE(inbox.questions.at(0).toObject().value(u"text"_s).toString(), u"Wie schnell?"_s);

        // Again with what the teacher has now: nothing new
        const Booklet::Inbox again = Booklet::collect(key, files,
            QJsonObject{{u"feedback"_s, inbox.feedback}, {u"questions"_s, inbox.questions}});
        QVERIFY(!again.changed);
        QCOMPARE(again.questions.size(), 1);
    }

    void inboxCleansValues()
    {
        // A changed app (or a broken file) cannot put odd values into the teacher's Qupil
        const QByteArray key = Crypto::randomBytes(Crypto::KeySize);
        QJsonObject odd{{u"practiced"_s, 99}, {u"works"_s, -4}, {u"at"_s, 10.0}, {u"script"_s, u"<b>x</b>"_s}};
        QHash<QString, QByteArray> files{{u"fb-a-10.qupil"_s, feedbackFile(key, {{u"t1"_s, odd}},
            QJsonArray{QJsonObject{{u"id"_s, u""_s}, {u"text"_s, u"no id"_s}},
                       QJsonObject{{u"id"_s, u"q2"_s}, {u"text"_s, u"   "_s}},
                       QJsonObject{{u"id"_s, u"q3"_s}, {u"text"_s, QString(5000, u'a')}, {u"at"_s, 5.0}}})}};
        const Booklet::Inbox inbox = Booklet::collect(key, files);
        const QJsonObject t1 = inbox.feedback.value(u"t1"_s).toObject();
        QCOMPARE(t1.value(u"practiced"_s).toInt(), 3);
        QCOMPARE(t1.value(u"works"_s).toInt(), 0);
        QVERIFY(!t1.contains(u"script"_s));
        QCOMPARE(inbox.questions.size(), 1);
        QCOMPARE(inbox.questions.at(0).toObject().value(u"text"_s).toString().size(), 1000);
    }
};

QTEST_GUILESS_MAIN(TestTeacherSide)
#include "tst_teacherside.moc"
