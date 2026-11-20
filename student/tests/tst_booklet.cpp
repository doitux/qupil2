// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// The booklet format between the teacher's Qupil and the student app: coupling link,
// encryption, file names. Invented values only.

#include "Booklet.h"
#include "Crypto.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QTest>

using namespace Qt::StringLiterals;
using namespace Qupil;

class TestBooklet : public QObject
{
    Q_OBJECT

    static Booklet::Link example()
    {
        Booklet::Link link;
        link.server = u"https://cloud.example.org"_s;
        link.token = u"AbCdEf123456"_s;
        link.password = u"not-a-real-password"_s;
        link.key = QByteArray(32, '\x07');
        return link;
    }

private slots:
    void linkRoundTrip()
    {
        const Booklet::Link link = example();
        const QString text = Booklet::encodeLink(link);
        QVERIFY(text.startsWith(u"https://qupil.de/u#"_s));
        // No characters a messenger would cut or change
        const QString fragment = text.mid(text.indexOf(u'#') + 1);
        QVERIFY(!fragment.contains(u'+') && !fragment.contains(u'/') && !fragment.contains(u'='));

        const auto back = Booklet::decodeLink(text);
        QVERIFY(back.has_value());
        QCOMPARE(back->server, link.server);
        QCOMPARE(back->token, link.token);
        QCOMPARE(back->password, link.password);
        QCOMPARE(back->key, link.key);
        QCOMPARE(back->id(), link.id());
    }

    void linkForms()
    {
        const QString text = Booklet::encodeLink(example());
        const QString fragment = text.mid(text.indexOf(u'#') + 1);
        // Pasted with spaces around it, only the part after #, the app's own scheme
        QVERIFY(Booklet::decodeLink(u"  "_s + text + u"\n"_s).has_value());
        QVERIFY(Booklet::decodeLink(u"#"_s + fragment).has_value());
        QVERIFY(Booklet::decodeLink(u"qupil-student:"_s + fragment).has_value());
    }

    void linkInvalid_data()
    {
        QTest::addColumn<QString>("text");
        QTest::newRow("empty") << QString();
        QTest::newRow("other site") << u"https://example.org/u#abc"_s;
        QTest::newRow("garbage") << u"https://qupil.de/u#%%%"_s;
        QTest::newRow("not json") << u"https://qupil.de/u#aGVsbG8"_s;
    }
    void linkInvalid()
    {
        QFETCH(QString, text);
        QVERIFY(!Booklet::decodeLink(text).has_value());
    }

    void linkIncomplete()
    {
        Booklet::Link link = example();
        link.server = u"ftp://cloud.example.org"_s;
        QVERIFY(!link.isValid());
        link = example();
        link.token.clear();
        QVERIFY(!link.isValid());
        link = example();
        link.key = QByteArray(16, 'x');
        QVERIFY(!link.isValid());
    }

    void sealOpen()
    {
        const QByteArray key = Crypto::randomBytes(Crypto::KeySize);
        const QJsonObject content{{u"format"_s, Booklet::Format},
                                  {u"student"_s, u"Jonas"_s},
                                  {u"tasks"_s, QJsonArray{QJsonObject{{u"id"_s, u"t1"_s}, {u"title"_s, u"Tonleiter"_s}}}}};
        const QByteArray sealed = Booklet::seal(key, Booklet::Purpose::Booklet, content);
        QVERIFY(!sealed.isEmpty());
        QVERIFY(!sealed.contains("Jonas"));

        const auto opened = Booklet::open(key, Booklet::Purpose::Booklet, sealed);
        QVERIFY(opened.has_value());
        QCOMPARE(*opened, content);

        // Wrong key, wrong purpose (feedback file passed off as the booklet), tampered
        QVERIFY(!Booklet::open(Crypto::randomBytes(Crypto::KeySize), Booklet::Purpose::Booklet, sealed));
        QVERIFY(!Booklet::open(key, Booklet::Purpose::Feedback, sealed));
        QByteArray tampered = sealed;
        tampered[tampered.size() - 1] = char(tampered.at(tampered.size() - 1) ^ 1);
        QVERIFY(!Booklet::open(key, Booklet::Purpose::Booklet, tampered));
        QVERIFY(!Booklet::open(key, Booklet::Purpose::Booklet, QByteArray("short")));
    }

    void feedbackNames()
    {
        const QString name = Booklet::feedbackFileName(u"a1B2c3"_s, 1763600000000);
        QCOMPARE(name, u"fb-a1B2c3-1763600000000.qupil"_s);
        QVERIFY(Booklet::isFeedbackFileName(name));
        QVERIFY(!Booklet::isFeedbackFileName(QString::fromLatin1(Booklet::BookletFile)));
        QVERIFY(!Booklet::isFeedbackFileName(u"fb-../x-1.qupil"_s));
        QVERIFY(!Booklet::isFeedbackFileName(u"fb-a-1.qupil.php"_s));
    }

    void urls()
    {
        const Booklet::Link link = example();
        QCOMPARE(Booklet::fileUrl(link, QString::fromLatin1(Booklet::BookletFile)).toString(),
                 u"https://cloud.example.org/public.php/dav/files/AbCdEf123456/heft.qupil"_s);
        QVERIFY(Booklet::authorization(link).startsWith("Basic "));
    }
};

QTEST_GUILESS_MAIN(TestBooklet)
#include "tst_booklet.moc"
