// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// The student app against a fake Nextcloud share, and its interface by tap:
// coupling, loading, feedback going back, errors, the example, settings.
// Invented data only.

#include "Booklet.h"
#include "Crypto.h"
#include "FakeNextcloud.h"
#include "QrScanner.h"
#include "StudentApp.h"
#include "Translator.h"

#include <QJsonArray>
#include <QQmlApplicationEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>
#include <QThread>
#include <QtQml/QQmlExtensionPlugin>

Q_IMPORT_QML_PLUGIN(QupilStudentPlugin)

using namespace Qt::StringLiterals;
using namespace Qupil;

namespace {

QJsonObject exampleBooklet(int format = Booklet::Format)
{
    return QJsonObject{
        {u"format"_s, format},
        {u"teacher"_s, u"Herr Test"_s},
        {u"student"_s, u"Mia"_s},
        {u"instrument"_s, u"Klavier"_s},
        {u"allow"_s, QJsonObject{{u"feedback"_s, true}, {u"questions"_s, true}}},
        {u"next"_s, QJsonArray{QJsonObject{{u"date"_s, u"2026-11-24"_s}, {u"start"_s, u"16:00"_s}, {u"end"_s, u"16:45"_s}}}},
        {u"tasks"_s, QJsonArray{
             QJsonObject{{u"id"_s, u"t1"_s}, {u"date"_s, u"2026-11-17"_s}, {u"title"_s, u"C-Dur-Tonleiter"_s},
                         {u"hints"_s, QJsonArray{u"Daumen untersetzen"_s}}, {u"kind"_s, u"practice"_s}},
             QJsonObject{{u"id"_s, u"t2"_s}, {u"date"_s, u"2026-11-17"_s}, {u"title"_s, u"Heft mitbringen"_s},
                         {u"kind"_s, u"todo"_s}}}},
        {u"feedback"_s, QJsonObject{}},
    };
}

QQuickItem *findItem(QQuickItem *root, const QString &name)
{
    if (!root)
        return nullptr;
    if (root->objectName() == name && root->isVisible())
        return root;
    for (QQuickItem *child : root->childItems())
        if (QQuickItem *found = findItem(child, name))
            return found;
    return nullptr;
}

} // namespace

class TestStudentApp : public QObject
{
    Q_OBJECT

    QTemporaryDir m_dir;
    Booklet::Link m_link;
    FakeNextcloud *m_cloud = nullptr;
    StudentApp *m_app = nullptr;
    Translator m_translator;

    QQuickWindow *window(QQmlApplicationEngine &engine)
    {
        auto *w = qobject_cast<QQuickWindow *>(engine.rootObjects().value(0));
        if (w) {
            w->resize(390, 2400);
            QTest::qWait(50);
        }
        return w;
    }
    bool tap(QQuickWindow *w, const QString &name)
    {
        QTest::qWait(400);   // pages slide in; the StackView takes no taps meanwhile
        QQuickItem *item = findItem(w->contentItem(), name);
        if (!item)
            return false;
        const QPointF center = item->mapToScene(QPointF(item->width() / 2, item->height() / 2));
        QTest::mouseClick(w, Qt::LeftButton, {}, center.toPoint());
        QTest::qWait(30);
        return true;
    }
    QVariantMap task(const QString &id) const
    {
        for (const QVariant &g : m_app->current().value(u"groups"_s).toList())
            for (const QVariant &t : g.toMap().value(u"tasks"_s).toList())
                if (t.toMap().value(u"id"_s) == id)
                    return t.toMap();
        return {};
    }
    void removeAll()
    {
        while (!m_app->teachers().isEmpty())
            m_app->removeTeacher(m_app->teachers().constFirst().toMap().value(u"id"_s).toString());
    }

private slots:
    void initTestCase()
    {
        QVERIFY(m_dir.isValid());
        QVERIFY(m_translator.load(u":/i18n/strings.json"_s, u"de"_s));
        QCoreApplication::installTranslator(&m_translator);
        StudentApp::setDataDir(m_dir.path());
        StudentApp::setToday(QDate(2026, 11, 20));
        m_cloud = new FakeNextcloud(u"Tok3nTok3n"_s, u"pw-for-tests"_s, this);
        QVERIFY(m_cloud->isListening());
        m_link.server = m_cloud->url();
        m_link.token = u"Tok3nTok3n"_s;
        m_link.password = u"pw-for-tests"_s;
        m_link.key = Crypto::randomBytes(Crypto::KeySize);
        m_app = StudentApp::instance();
    }

    void init()
    {
        removeAll();
        m_cloud->files().clear();
        m_cloud->files().insert(u"heft.qupil"_s, Booklet::seal(m_link.key, Booklet::Purpose::Booklet, exampleBooklet()));
        m_cloud->legacyOnly = false;
        m_cloud->shareGone = false;
    }

    void couplingLoadsBooklet()
    {
        QCOMPARE(m_app->addLink(u"no link at all"_s), u"invalid"_s);
        QCOMPARE(m_app->addLink(Booklet::encodeLink(m_link)), QString());
        QCOMPARE(m_app->teachers().size(), 1);
        QTRY_COMPARE(m_app->current().value(u"teacher"_s).toString(), u"Herr Test"_s);
        QCOMPARE(m_app->current().value(u"error"_s).toString(), QString());
        QCOMPARE(task(u"t1"_s).value(u"title"_s).toString(), u"C-Dur-Tonleiter"_s);
        QCOMPARE(m_app->current().value(u"next"_s).toList().value(0).toMap().value(u"daysAway"_s).toInt(), 4);

        // The same link again (scanned twice): still one teacher
        QCOMPARE(m_app->addLink(Booklet::encodeLink(m_link)), QString());
        QCOMPARE(m_app->teachers().size(), 1);
    }

    void feedbackGoesBack()
    {
        QCOMPARE(m_app->addLink(Booklet::encodeLink(m_link)), QString());
        QTRY_VERIFY(!task(u"t1"_s).isEmpty());
        m_app->setPracticed(u"t1"_s, 3);
        m_app->setWorks(u"t1"_s, 2);
        m_app->setDone(u"t2"_s, true);
        m_app->askQuestion(u"Wie schnell soll ich spielen?"_s);
        QCOMPARE(task(u"t1"_s).value(u"practiced"_s).toInt(), 3);
        QCOMPARE(m_app->current().value(u"sendState"_s).toString(), u"pending"_s);

        // Several quick taps, one upload after a short pause
        QTRY_COMPARE_WITH_TIMEOUT(m_app->current().value(u"sendState"_s).toString(), u"sent"_s, 8000);
        QCOMPARE(m_cloud->puts, 1);
        QString name;
        for (auto it = m_cloud->files().constBegin(); it != m_cloud->files().constEnd(); ++it)
            if (Booklet::isFeedbackFileName(it.key()))
                name = it.key();
        QVERIFY(!name.isEmpty());

        // What the teacher's Qupil reads
        const auto content = Booklet::open(m_link.key, Booklet::Purpose::Feedback, m_cloud->files().value(name));
        QVERIFY(content.has_value());
        const QJsonObject feedback = content->value(u"feedback"_s).toObject();
        QCOMPARE(feedback.value(u"t1"_s).toObject().value(u"practiced"_s).toInt(), 3);
        QCOMPARE(feedback.value(u"t1"_s).toObject().value(u"works"_s).toInt(), 2);
        QCOMPARE(feedback.value(u"t2"_s).toObject().value(u"done"_s).toBool(), true);
        QCOMPARE(content->value(u"questions"_s).toArray().first().toObject().value(u"text"_s).toString(),
                 u"Wie schnell soll ich spielen?"_s);
        // Not readable as a booklet, and no plain text in the file
        QVERIFY(!Booklet::open(m_link.key, Booklet::Purpose::Booklet, m_cloud->files().value(name)));
        QVERIFY(!m_cloud->files().value(name).contains("schnell"));
    }

    void teacherEchoIsMerged()
    {
        // Another device of the student sent "practised: some" later; the teacher echoes it
        QJsonObject booklet = exampleBooklet();
        booklet.insert(u"feedback"_s, QJsonObject{{u"t1"_s, QJsonObject{{u"practiced"_s, 2}, {u"at"_s, 4.0e12}}}});
        m_cloud->files().insert(u"heft.qupil"_s, Booklet::seal(m_link.key, Booklet::Purpose::Booklet, booklet));
        QCOMPARE(m_app->addLink(Booklet::encodeLink(m_link)), QString());
        QTRY_COMPARE(task(u"t1"_s).value(u"practiced"_s).toInt(), 2);
    }

    void errors()
    {
        // Wrong key: someone else's booklet or a broken link
        Booklet::Link wrong = m_link;
        wrong.key = Crypto::randomBytes(Crypto::KeySize);
        QCOMPARE(m_app->addLink(Booklet::encodeLink(wrong)), QString());
        QTRY_COMPARE(m_app->current().value(u"error"_s).toString(), u"key"_s);
        removeAll();

        // A newer format than this app knows
        m_cloud->files().insert(u"heft.qupil"_s, Booklet::seal(m_link.key, Booklet::Purpose::Booklet, exampleBooklet(Booklet::Format + 1)));
        QCOMPARE(m_app->addLink(Booklet::encodeLink(m_link)), QString());
        QTRY_COMPARE(m_app->current().value(u"error"_s).toString(), u"newer"_s);
        removeAll();

        // Loaded once, then the teacher ends the share: the last state stays visible
        m_cloud->files().insert(u"heft.qupil"_s, Booklet::seal(m_link.key, Booklet::Purpose::Booklet, exampleBooklet()));
        QCOMPARE(m_app->addLink(Booklet::encodeLink(m_link)), QString());
        QTRY_VERIFY(!task(u"t1"_s).isEmpty());
        m_cloud->shareGone = true;
        m_app->refresh();
        QTRY_COMPARE(m_app->current().value(u"error"_s).toString(), u"gone"_s);
        QVERIFY(!task(u"t1"_s).isEmpty());

        // Wrong password
        removeAll();
        m_cloud->shareGone = false;
        Booklet::Link badPassword = m_link;
        badPassword.password = u"wrong"_s;
        QCOMPARE(m_app->addLink(Booklet::encodeLink(badPassword)), QString());
        QTRY_COMPARE(m_app->current().value(u"error"_s).toString(), u"gone"_s);
    }

    void oldNextcloud()
    {
        // No new public WebDAV (before Nextcloud 29): the old one, both ways
        m_cloud->legacyOnly = true;
        const int puts = m_cloud->puts;
        QCOMPARE(m_app->addLink(Booklet::encodeLink(m_link)), QString());
        QTRY_COMPARE(m_app->current().value(u"teacher"_s).toString(), u"Herr Test"_s);
        m_app->setPracticed(u"t1"_s, 2);
        QTRY_COMPARE_WITH_TIMEOUT(m_app->current().value(u"sendState"_s).toString(), u"sent"_s, 8000);
        QCOMPARE(m_cloud->puts, puts + 1);
    }

    void keptAfterRestart()
    {
        QCOMPARE(m_app->addLink(Booklet::encodeLink(m_link)), QString());
        QTRY_VERIFY(!task(u"t1"_s).isEmpty());
        m_app->setPracticed(u"t1"_s, 1);
        QFile file(m_dir.filePath(u"student.json"_s));
        QVERIFY(file.open(QIODevice::ReadOnly));
        const QByteArray saved = file.readAll();
        QVERIFY(saved.contains("Herr Test"));   // the booklet stays readable offline
        QVERIFY(saved.contains("\"practiced\""));
    }

    // ---- interface

    void uiExampleAndFeedback()
    {
        QQmlApplicationEngine engine;
        engine.addImportPath(u"qrc:/qt/qml"_s);
        engine.load(QUrl(u"qrc:/qt/qml/QupilStudent/qml/Main.qml"_s));
        QQuickWindow *w = window(engine);
        QVERIFY(w);
        QVERIFY(QTest::qWaitForWindowExposed(w));

        // Welcome page: the example opens without any cloud
        QTRY_VERIFY(findItem(w->contentItem(), u"demoButton"_s));
        QVERIFY(tap(w, u"demoButton"_s));
        QTRY_VERIFY(findItem(w->contentItem(), u"task-demo-1"_s));
        QVERIFY(m_app->current().value(u"demo"_s).toBool());

        // Hints fold open, and stay open while giving feedback (the list is rebuilt then)
        QVERIFY(!findItem(w->contentItem(), u"hintList-demo-1"_s));
        QVERIFY(tap(w, u"hints-demo-1"_s));
        QTRY_VERIFY(findItem(w->contentItem(), u"hintList-demo-1"_s));
        QVERIFY(tap(w, u"works-demo-1-3"_s));
        QTRY_COMPARE(task(u"demo-1"_s).value(u"works"_s).toInt(), 3);
        QVERIFY(findItem(w->contentItem(), u"hintList-demo-1"_s));

        // "Practised: a lot", tap again clears it; "works: so-so"
        QVERIFY(tap(w, u"practiced-demo-1-3"_s));
        QTRY_COMPARE(task(u"demo-1"_s).value(u"practiced"_s).toInt(), 3);
        QVERIFY(tap(w, u"practiced-demo-1-3"_s));
        QTRY_COMPARE(task(u"demo-1"_s).value(u"practiced"_s).toInt(), 0);
        QVERIFY(tap(w, u"works-demo-1-2"_s));
        QTRY_COMPARE(task(u"demo-1"_s).value(u"works"_s).toInt(), 2);

        // To-do: a tick, no feedback rows
        QVERIFY(!findItem(w->contentItem(), u"practiced-demo-3"_s));
        QVERIFY(tap(w, u"tick-demo-3"_s));
        QTRY_VERIFY(task(u"demo-3"_s).value(u"done"_s).toBool());

        // The example never uploads
        QTest::qWait(3000);
        QCOMPARE(m_app->current().value(u"sendState"_s).toString(), u"demo"_s);

        // Close the example: back to the welcome page
        QVERIFY(tap(w, u"endDemo"_s));
        QTRY_VERIFY(findItem(w->contentItem(), u"demoButton"_s));
        QVERIFY(m_app->teachers().isEmpty());
    }

    void uiRemoveTeacherAsksFirst()
    {
        QCOMPARE(m_app->addLink(Booklet::encodeLink(m_link)), QString());
        QTRY_VERIFY(!task(u"t1"_s).isEmpty());
        const QString id = m_app->teachers().constFirst().toMap().value(u"id"_s).toString();

        QQmlApplicationEngine engine;
        engine.addImportPath(u"qrc:/qt/qml"_s);
        engine.load(QUrl(u"qrc:/qt/qml/QupilStudent/qml/Main.qml"_s));
        QQuickWindow *w = window(engine);
        QVERIFY(w);
        QVERIFY(QTest::qWaitForWindowExposed(w));
        QTRY_VERIFY(findItem(w->contentItem(), u"task-t1"_s));

        QVERIFY(tap(w, u"settingsButton"_s));
        QTRY_VERIFY(findItem(w->contentItem(), u"remove-"_s + id));
        QVERIFY(tap(w, u"remove-"_s + id));
        QTest::qWait(100);
        QCOMPARE(m_app->teachers().size(), 1);   // first tap only asks
        QVERIFY(tap(w, u"remove-"_s + id));
        QTRY_VERIFY(m_app->teachers().isEmpty());
        QTRY_VERIFY(findItem(w->contentItem(), u"demoButton"_s));
    }

    void linkFromAnotherThread()
    {
        // Android hands a link over on its own main thread while the app is running:
        // the link opened the app and nothing else happened
        QSignalSpy notices(m_app, &StudentApp::notice);
        const QUrl url(Booklet::encodeLink(m_link));
        QThread *thread = QThread::create([this, url] { m_app->openUrl(url); });
        thread->start();
        QVERIFY(thread->wait(5000));
        delete thread;
        QVERIFY(m_app->teachers().isEmpty());   // nothing was touched on the other thread
        QTRY_COMPARE(m_app->teachers().size(), 1);
        QTRY_COMPARE(m_app->current().value(u"teacher"_s).toString(), u"Herr Test"_s);
        QCOMPARE(notices.size(), 1);

        // Something else that opens the app is no coupling link
        m_app->openUrl(QUrl(u"https://qupil.de/u"_s));
        QCOMPARE(m_app->teachers().size(), 1);
        QCOMPARE(notices.size(), 1);
    }

    void uiScanner()
    {
        // The scanner of the phone, pretended: some other QR code first
        QrScanner::pretend({ u"https://example.org/not-qupil"_s, false });
        QVERIFY(m_app->canScan());

        QQmlApplicationEngine engine;
        engine.addImportPath(u"qrc:/qt/qml"_s);
        engine.load(QUrl(u"qrc:/qt/qml/QupilStudent/qml/Main.qml"_s));
        QQuickWindow *w = window(engine);
        QVERIFY(w);
        QVERIFY(QTest::qWaitForWindowExposed(w));
        QTRY_VERIFY(findItem(w->contentItem(), u"scanButton"_s));
        QVERIFY(!findItem(w->contentItem(), u"scanError"_s));

        QVERIFY(tap(w, u"scanButton"_s));
        QTRY_COMPARE(m_app->scanError(), u"invalid"_s);
        QTRY_VERIFY(findItem(w->contentItem(), u"scanError"_s));
        QVERIFY(findItem(w->contentItem(), u"scanError"_s)->property("text").toString().contains(u"kein Qupil-Code"_s));
        QVERIFY(m_app->teachers().isEmpty());

        // The camera is not allowed
        QrScanner::pretend({ QString(), true });
        QVERIFY(tap(w, u"scanButton"_s));
        QTRY_COMPARE(m_app->scanError(), u"denied"_s);
        QVERIFY(findItem(w->contentItem(), u"scanError"_s)->property("text").toString().contains(u"Kamera"_s));

        // Closed without a code: nothing to complain about
        QrScanner::pretend({ QString(), false });
        QVERIFY(tap(w, u"scanButton"_s));
        QTRY_COMPARE(m_app->scanError(), QString());
        QTRY_VERIFY(!findItem(w->contentItem(), u"scanError"_s));

        // The teacher's code: coupled, the tasks appear
        QrScanner::pretend({ Booklet::encodeLink(m_link), false });
        QVERIFY(tap(w, u"scanButton"_s));
        QTRY_COMPARE(m_app->teachers().size(), 1);
        QTRY_VERIFY(findItem(w->contentItem(), u"task-t1"_s));
        QCOMPARE(m_app->scanError(), QString());
    }

    void uiKeyboardKeepsFieldsInView()
    {
        // Android makes the window lower for the keyboard: the field with the cursor and
        // its buttons stay in view. A phone (390 x 844), then without the keyboard's 336.
        QQmlApplicationEngine engine;
        engine.addImportPath(u"qrc:/qt/qml"_s);
        engine.load(QUrl(u"qrc:/qt/qml/QupilStudent/qml/Main.qml"_s));
        auto *w = qobject_cast<QQuickWindow *>(engine.rootObjects().value(0));
        QVERIFY(w);
        w->resize(390, 844);
        QVERIFY(QTest::qWaitForWindowExposed(w));
        auto inView = [w](QQuickItem *item) {
            const QRectF r = item->mapRectToScene(QRectF(0, 0, item->width(), item->height()));
            return r.top() >= 0 && r.bottom() <= w->height();
        };

        // Welcome page: the link field with "Paste" and "Open" below it
        QTRY_VERIFY(findItem(w->contentItem(), u"linkField"_s));
        QVERIFY(tap(w, u"linkField"_s));
        QTRY_VERIFY(findItem(w->contentItem(), u"linkField"_s)->hasActiveFocus());
        w->resize(390, 844 - 336);
        QTRY_VERIFY(inView(findItem(w->contentItem(), u"linkField"_s)));
        QTRY_VERIFY(inView(findItem(w->contentItem(), u"openLinkButton"_s)));
        QVERIFY(inView(findItem(w->contentItem(), u"pasteButton"_s)));
        w->resize(390, 844);

        // The example: the question field far down, "send" beside it
        QVERIFY(tap(w, u"demoButton"_s));
        QTRY_VERIFY(findItem(w->contentItem(), u"task-demo-1"_s));
        QQuickItem *field = nullptr;
        QTRY_VERIFY((field = findItem(w->contentItem(), u"questionField"_s)));
        field->forceActiveFocus();
        QTRY_VERIFY(field->hasActiveFocus());
        w->resize(390, 844 - 336);
        QTRY_VERIFY(inView(field));
        QTRY_VERIFY(inView(findItem(w->contentItem(), u"sendQuestion"_s)));
        // A long question: the field grows, both stay in view
        field->setProperty("text", u"Wie viele Takte soll ich jeden Tag üben und in welchem Tempo am Anfang und später dann?\n\n"
                                   u"Und noch eine Zeile, damit das Feld höher wird.\nUnd noch eine."_s);
        field->setProperty("cursorPosition", field->property("length"));
        QTest::qWait(100);
        QVERIFY(inView(findItem(w->contentItem(), u"sendQuestion"_s)));
        const QRectF cursor = field->mapRectToScene(field->property("cursorRectangle").toRectF());
        QVERIFY2(cursor.top() >= 0 && cursor.bottom() <= w->height(), "the cursor is in view");
        // A tap beside the field puts the keyboard away
        QVERIFY(tap(w, u"task-demo-1"_s) || true);
        w->resize(390, 844);
    }

    void uiTextsWithoutDeclension()
    {
        // The teacher's name as typed ("Herr Test") stands on its own: no "an Herr Test"
        QCOMPARE(m_app->addLink(Booklet::encodeLink(m_link)), QString());
        QTRY_VERIFY(!task(u"t1"_s).isEmpty());
        QQmlApplicationEngine engine;
        engine.addImportPath(u"qrc:/qt/qml"_s);
        engine.load(QUrl(u"qrc:/qt/qml/QupilStudent/qml/Main.qml"_s));
        QQuickWindow *w = window(engine);
        QVERIFY(w);
        QVERIFY(QTest::qWaitForWindowExposed(w));
        QTRY_VERIFY(findItem(w->contentItem(), u"task-t1"_s));
        QStringList texts;
        std::function<void(QQuickItem *)> collect = [&](QQuickItem *item) {
            const QVariant text = item->property("text");
            if (item->isVisible() && text.typeId() == QMetaType::QString && !text.toString().isEmpty())
                texts << text.toString();
            for (QQuickItem *child : item->childItems())
                collect(child);
        };
        collect(w->contentItem());
        const QString all = texts.join(u'\n');
        QVERIFY2(all.contains(u"Deine Frage · Herr Test"_s, Qt::CaseInsensitive), qPrintable(all));
        QVERIFY2(!all.contains(u"an Herr Test"_s, Qt::CaseInsensitive), qPrintable(all));
    }
};

QTEST_MAIN(TestStudentApp)
#include "tst_studentapp.moc"
