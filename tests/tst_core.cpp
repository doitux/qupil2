// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

#include <QDate>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSignalSpy>
#include <QSqlQuery>
#include <QMap>
#include <QMutex>
#include <QQueue>
#include <QThread>
#include <QWaitCondition>
#include <QSet>
#include <QTemporaryDir>
#include <QTest>

#include <algorithm>
#include <memory>

#include "Crypto.h"
#include "Csv.h"
#include "Database.h"
#include "FolderSync.h"
#include "DemoData.h"
#include "Ids.h"
#include "LegacyFixture.h"
#include "LegacyImporter.h"
#include "Schema.h"
#include "LessonAlerts.h"
#include "Mdns.h"
#include "Store.h"
#include "TakeOver.h"
#include "Wav.h"
#include "WifiSession.h"

using namespace Qupil;
using namespace Qt::StringLiterals;

//! A sync folder in memory (instead of Nextcloud)
class MemoryFolder : public RemoteFolder
{
public:
    QMap<QString, QByteArray> files;
    QSet<QString> folders;
    bool offline = false;
    bool notFound = false;

    bool makeFolder(const QString &path) override
    {
        if (offline)
            return false;
        folders.insert(path);
        return true;
    }
    std::optional<QStringList> list(const QString &path) override
    {
        if (offline)
            return std::nullopt;
        QStringList names;
        const QString prefix = path + u'/';
        for (const QString &f : folders)
            if (f.startsWith(prefix) && !f.mid(prefix.size()).contains(u'/'))
                names << f.mid(prefix.size()) + u'/';
        for (auto it = files.cbegin(); it != files.cend(); ++it)
            if (it.key().startsWith(prefix) && !it.key().mid(prefix.size()).contains(u'/'))
                names << it.key().mid(prefix.size());
        return names;
    }
    std::optional<QByteArray> read(const QString &path) override
    {
        notFound = !offline && !files.contains(path);
        if (offline || notFound)
            return std::nullopt;
        return files.value(path);
    }
    bool write(const QString &path, const QByteArray &data) override
    {
        if (offline)
            return false;
        files.insert(path, data);
        return true;
    }
    bool move(const QString &from, const QString &to) override
    {
        if (offline)
            return false;
        QMap<QString, QByteArray> movedFiles;
        for (auto it = files.cbegin(); it != files.cend(); ++it)
            movedFiles.insert(it.key() == from || it.key().startsWith(from + u'/') ? to + it.key().mid(from.size())
                                                                                    : it.key(),
                              it.value());
        QSet<QString> movedFolders;
        for (const QString &f : std::as_const(folders))
            movedFolders.insert(f == from || f.startsWith(from + u'/') ? to + f.mid(from.size()) : f);
        files = movedFiles;
        folders = movedFolders;
        return true;
    }
    QString lastError() const override { return QStringLiteral("offline"); }
    bool lastErrorWasNotFound() const override { return notFound; }
};

// Two connected ends of an in-memory connection (Wi-Fi tests)
struct PipeQueue
{
    QMutex mutex;
    QWaitCondition ready;
    QQueue<QByteArray> frames;
    bool closed = false;
};

class PipeEnd : public FrameChannel
{
public:
    PipeEnd(std::shared_ptr<PipeQueue> in, std::shared_ptr<PipeQueue> out)
        : m_in(std::move(in))
        , m_out(std::move(out))
    {
    }
    ~PipeEnd() override
    {
        QMutexLocker lock(&m_out->mutex);
        m_out->closed = true;
        m_out->ready.wakeAll();
    }
    bool send(const QByteArray &frame) override
    {
        QMutexLocker lock(&m_out->mutex);
        if (m_out->closed)
            return false;
        m_out->frames.enqueue(frame);
        m_out->ready.wakeAll();
        return true;
    }
    std::optional<QByteArray> receive() override
    {
        QMutexLocker lock(&m_in->mutex);
        while (m_in->frames.isEmpty() && !m_in->closed) {
            if (!m_in->ready.wait(&m_in->mutex, 10000))
                return std::nullopt;
        }
        if (m_in->frames.isEmpty())
            return std::nullopt;
        return m_in->frames.dequeue();
    }

private:
    std::shared_ptr<PipeQueue> m_in;
    std::shared_ptr<PipeQueue> m_out;
};

class TestCore : public QObject
{
    Q_OBJECT

    QTemporaryDir m_dir;
    QString file(const QString &name) const { return m_dir.filePath(name); }

    //! Imports a 1.x database made of the given statements into \a db
    LegacyImporter::Result importLegacy(const QString &name, const QStringList &statements, Database &db)
    {
        LegacyImporter::Result failed;
        const QString legacy = file(name + u".qupil1.db"_s);
        failed.error = LegacyFixture::create(legacy, statements);
        if (!failed.error.isEmpty() || !db.open(file(name + u".db"_s), &failed.error))
            return failed;
        Store store(&db);
        LegacyImporter importer;
        return importer.run(legacy, &store);
    }

    static QByteArray contents(const QString &path)
    {
        QFile f(path);
        return f.open(QIODevice::ReadOnly) ? f.readAll() : QByteArray();
    }

    static qint64 fakeWall()
    {
        static qint64 now = 1'800'000'000'000;
        return now += 7;
    }

private slots:
    void ids()
    {
        const QString a = newId();
        const QString b = newId();
        QCOMPARE(a.size(), 36);
        QCOMPARE(a.at(14), u'7');            // version 7
        QVERIFY(a != b);
        QVERIFY(a.left(13) <= b.left(13));   // time ordered
    }

    void hybridClock()
    {
        static qint64 wall = 1000;
        HybridClock c;
        c.setWallClock([] { return wall; });
        const qint64 t1 = c.now();
        const qint64 t2 = c.now();          // same ms: counter goes up
        QVERIFY(t1 < t2);
        QCOMPARE(t1 >> 16, 1000);
        c.observe(HybridClock::make(5000, 3));   // other device is ahead
        const qint64 t3 = c.now();
        QVERIFY(t3 > HybridClock::make(5000, 3));
        wall = 9000;
        QVERIFY(c.now() > t3);
        QCOMPARE(c.lastMs(), 9000);

        // Text form of sync format 1
        const QString text = HybridClock::format(HybridClock::make(1'800'000'000'123, 0x2a), u"dev-a"_s);
        QCOMPARE(text, u"1800000000123.002a.dev-a"_s);
        qint64 time = 0;
        QString device;
        QVERIFY(HybridClock::parse(text, &time, &device));
        QCOMPARE(time, HybridClock::make(1'800'000'000'123, 0x2a));
        QCOMPARE(device, u"dev-a"_s);
        QVERIFY(!HybridClock::parse(u"no clock"_s, &time));

        // A remote change from the very same millisecond as our wall clock: what we
        // write next must still be later
        static qint64 still = 20000;
        HybridClock early;
        early.setWallClock([] { return still; });
        early.observe(HybridClock::make(20000, 2));
        QVERIFY(early.now() > HybridClock::make(20000, 2));
    }

    void storeWritesAndLogs()
    {
        Database db;
        QString error;
        QVERIFY2(db.open(file(u"store.db"_s), &error), qPrintable(error));
        QVERIFY(!db.deviceId().isEmpty());
        QVERIFY(db.isEmpty());

        Store store(&db);
        QSignalSpy spy(&store, &Store::dataChanged);
        const QString id = store.create(u"student"_s, { { u"surname"_s, u"Muster"_s }, { u"forename"_s, u"Lena"_s } });
        QVERIFY(!id.isEmpty());
        QCOMPARE(spy.count(), 1);
        QCOMPARE(store.get(u"student"_s, id).value(u"forename"_s).toString(), u"Lena"_s);
        QCOMPARE(store.get(u"student"_s, id).value(u"recital_interval"_s).toInt(), 5);   // default
        QVERIFY(!db.isEmpty());

        // A new record is one change with all fields, and one clock for the record
        QSqlQuery q(db.connection());
        const auto count = [&q, &id](const QString &table) {
            q.exec(u"SELECT COUNT(*) FROM %1 WHERE row_id = '%2'"_s.arg(table, id));
            return q.next() ? q.value(0).toInt() : -1;
        };
        QCOMPARE(count(u"change_log"_s), 1);
        QCOMPARE(count(u"field_clock"_s), 1);
        const QList<Change> created = store.changesOf(db.deviceId(), 0);
        QCOMPARE(created.size(), 1);
        QCOMPARE(created.first().fields.value(u"recital_interval"_s).toInt(), 5);
        QCOMPARE(created.first().fields.value(u"deleted"_s).toInt(), 0);
        QCOMPARE(created.first().fields.size(), Schema::entity(u"student"_s)->fields.size() + 1);

        QVERIFY(store.update(u"student"_s, id, { { u"forename"_s, u"Lea"_s }, { u"surname"_s, u"Muster"_s } }));
        QCOMPARE(count(u"change_log"_s), 2);
        QCOMPARE(count(u"field_clock"_s), 2);   // the record and the forename
        const QList<Change> updated = store.changesOf(db.deviceId(), created.first().seq);
        QCOMPARE(updated.size(), 1);
        QCOMPARE(updated.first().fields, (QVariantMap { { u"forename"_s, u"Lea"_s } }));   // unchanged surname not logged
        QVERIFY(store.update(u"student"_s, id, { { u"forename"_s, u"Lea"_s } }));   // nothing changed: nothing logged
        QCOMPARE(count(u"change_log"_s), 2);

        QTest::ignoreMessage(QtWarningMsg, "Store: unknown field student.no_such_field");
        QVERIFY(!store.update(u"student"_s, id, { { u"no_such_field"_s, 1 } }));
        QVERIFY(store.remove(u"student"_s, id));
        QVERIFY(store.select(u"student"_s).isEmpty());
        QCOMPARE(store.get(u"student"_s, id).value(u"deleted"_s).toInt(), 1);
    }

    void syncMergesFieldByField()
    {
        Database a, b;
        QVERIFY(a.open(file(u"a.db"_s)));
        QVERIFY(b.open(file(u"b.db"_s)));
        QVERIFY(a.deviceId() != b.deviceId());
        // One shared, steadily advancing clock: "later" must not depend on the timer
        // resolution of the system (about 16 ms on Windows)
        a.clock().setWallClock(&TestCore::fakeWall);
        b.clock().setWallClock(&TestCore::fakeWall);
        Store sa(&a), sb(&b);

        const QString id = sa.create(u"student"_s, { { u"surname"_s, u"Muster"_s }, { u"forename"_s, u"Lena"_s } });
        QVERIFY(sb.applyRemote(sa.changesSince(sb.versionVector())) > 0);
        QCOMPARE(sb.get(u"student"_s, id).value(u"surname"_s).toString(), u"Muster"_s);

        // Both devices change different fields offline …
        QVERIFY(sa.update(u"student"_s, id, { { u"email"_s, u"a@example.org"_s } }));
        QVERIFY(sb.update(u"student"_s, id, { { u"phone"_s, u"0123"_s } }));
        // … and the same field: the later change wins on both sides
        QVERIFY(sa.update(u"student"_s, id, { { u"forename"_s, u"Lea"_s } }));
        QVERIFY(sb.update(u"student"_s, id, { { u"forename"_s, u"Lene"_s } }));

        sb.applyRemote(sa.changesSince(sb.versionVector()));
        sa.applyRemote(sb.changesSince(sa.versionVector()));

        for (Store *s : { &sa, &sb }) {
            const QVariantMap r = s->get(u"student"_s, id);
            QCOMPARE(r.value(u"email"_s).toString(), u"a@example.org"_s);
            QCOMPARE(r.value(u"phone"_s).toString(), u"0123"_s);
            QCOMPARE(r.value(u"forename"_s).toString(), u"Lene"_s);
        }
        QCOMPARE(sa.versionVector(), sb.versionVector());
        // Nothing new after a full exchange; applying twice changes nothing
        QVERIFY(sa.changesSince(sb.versionVector()).isEmpty());
        QCOMPARE(sb.applyRemote(sa.changesSince({})), 0);
    }

    void rewriteInPlace()
    {
        // A derived form of the same content (e.g. an archive entry in the compact
        // layout) replaces the old one in the table and in the log, without a new change
        Database a, b;
        QVERIFY(a.open(file(u"rewrite-a.db"_s)));
        QVERIFY(b.open(file(u"rewrite-b.db"_s)));
        a.clock().setWallClock(&TestCore::fakeWall);
        b.clock().setWallClock(&TestCore::fakeWall);
        Store sa(&a), sb(&b);
        const QString id = sa.create(u"student_archive"_s, { { u"surname"_s, u"Muster"_s }, { u"content"_s, u"old"_s } });
        QVERIFY(sa.update(u"student_archive"_s, id, { { u"forename"_s, u"Lena"_s } }));
        QVERIFY(sb.applyRemote(sa.changesSince(sb.versionVector())) > 0);
        const auto vector = sa.versionVector();

        QSignalSpy spy(&sa, &Store::dataChanged);
        QVERIFY(sa.rewrite(u"student_archive"_s, id, u"content"_s, u"old"_s, u"new"_s));
        QCOMPARE(spy.count(), 1);
        QCOMPARE(sa.get(u"student_archive"_s, id).value(u"content"_s).toString(), u"new"_s);
        QCOMPARE(sa.versionVector(), vector);   // nothing to send
        const QList<Change> log = sa.changesOf(a.deviceId(), 0);
        QCOMPARE(log.size(), 2);
        QCOMPARE(log.first().fields.value(u"content"_s).toString(), u"new"_s);
        QCOMPARE(log.first().fields.value(u"surname"_s).toString(), u"Muster"_s);
        QVERIFY(!log.last().fields.contains(u"content"_s));
        // Only where the content is still the old form
        QVERIFY(sa.rewrite(u"student_archive"_s, id, u"content"_s, u"old"_s, u"other"_s));
        QCOMPARE(sa.get(u"student_archive"_s, id).value(u"content"_s).toString(), u"new"_s);
        QCOMPARE(spy.count(), 1);

        // The other device rewrites its own copy of the same entries the same way
        QVERIFY(sb.rewrite(u"student_archive"_s, id, u"content"_s, u"old"_s, u"new"_s));
        QCOMPARE(sb.changesOf(a.deviceId(), 0).first().fields.value(u"content"_s).toString(), u"new"_s);
        // A later real change still wins on both sides
        QVERIFY(sb.update(u"student_archive"_s, id, { { u"content"_s, u"edited"_s } }));
        QVERIFY(sa.applyRemote(sb.changesSince(sa.versionVector())) > 0);
        QCOMPARE(sa.get(u"student_archive"_s, id).value(u"content"_s).toString(), u"edited"_s);
        QCOMPARE(sa.versionVector(), sb.versionVector());
    }

    void relayThroughThirdDevice()
    {
        Database a, b, c;
        QVERIFY(a.open(file(u"r-a.db"_s)) && b.open(file(u"r-b.db"_s)) && c.open(file(u"r-c.db"_s)));
        Store sa(&a), sb(&b), sc(&c);
        const QString id = sa.create(u"lesson"_s, { { u"name"_s, u"Gruppe"_s } });
        sb.applyRemote(sa.changesSince(sb.versionVector()));   // A → B (e.g. Nextcloud)
        sc.applyRemote(sb.changesSince(sc.versionVector()));   // B → C (e.g. Wi-Fi)
        QCOMPARE(sc.get(u"lesson"_s, id).value(u"name"_s).toString(), u"Gruppe"_s);
        sc.remove(u"lesson"_s, id);
        sa.applyRemote(sc.changesSince(sa.versionVector()));
        QCOMPARE(sa.get(u"lesson"_s, id).value(u"deleted"_s).toInt(), 1);
    }

    void updateArrivesBeforeCreate()
    {
        // C hears of B's change before it gets A's new record (e.g. B by Wi-Fi, A only later)
        Database a, b, c;
        QVERIFY(a.open(file(u"o-a.db"_s)) && b.open(file(u"o-b.db"_s)) && c.open(file(u"o-c.db"_s)));
        for (Database *d : { &a, &b, &c })
            d->clock().setWallClock(&TestCore::fakeWall);
        Store sa(&a), sb(&b), sc(&c);
        const QString id = sa.create(u"student"_s, { { u"surname"_s, u"Muster"_s }, { u"forename"_s, u"Lena"_s } });
        sb.applyRemote(sa.changesSince(sb.versionVector()));
        QVERIFY(sb.update(u"student"_s, id, { { u"phone"_s, u"0123"_s } }));

        QCOMPARE(sc.applyRemote(sb.changesOf(b.deviceId(), 0)), 1);
        QCOMPARE(sc.applyRemote(sa.changesOf(a.deviceId(), 0)), 1);
        QVariantMap r = sc.get(u"student"_s, id);
        QCOMPARE(r.value(u"surname"_s).toString(), u"Muster"_s);
        QCOMPARE(r.value(u"forename"_s).toString(), u"Lena"_s);
        QCOMPARE(r.value(u"phone"_s).toString(), u"0123"_s);
        QSqlQuery q(c.connection());
        QVERIFY(q.exec(u"SELECT field FROM field_clock WHERE row_id = '%1' ORDER BY field"_s.arg(id)));
        QStringList fields;
        while (q.next())
            fields << q.value(0).toString();
        QCOMPARE(fields, (QStringList { QString(), u"phone"_s }));   // the record and the phone

        // Later changes still decide by time
        QVERIFY(sa.update(u"student"_s, id, { { u"phone"_s, u"0456"_s } }));
        QCOMPARE(sc.applyRemote(sa.changesOf(a.deviceId(), 0)), 1);
        QCOMPARE(sc.get(u"student"_s, id).value(u"phone"_s).toString(), u"0456"_s);
        QCOMPARE(sb.applyRemote(sc.changesSince(sb.versionVector())), 1);
        QCOMPARE(sb.get(u"student"_s, id), sc.get(u"student"_s, id));
    }

    void sameTimeDeviceDecides()
    {
        // Two devices write the same field at exactly the same time: the larger device id wins everywhere
        Database a, b;
        QVERIFY(a.open(file(u"t-a.db"_s)) && b.open(file(u"t-b.db"_s)));
        Store sa(&a), sb(&b);
        const QString id = sa.create(u"event"_s, { { u"title"_s, u"Vorspiel"_s } });
        sb.applyRemote(sa.changesSince({}));
        const qint64 later = a.clock().now() + 1000;
        const Change low { u"00000000-0000-7000-8000-000000000001"_s, 1, later, u"event"_s, id, { { u"title"_s, u"Klein"_s } } };
        const Change high { u"ffffffff-0000-7000-8000-000000000001"_s, 1, later, u"event"_s, id, { { u"title"_s, u"Groß"_s } } };
        QCOMPARE(sa.applyRemote({ low }), 1);
        QCOMPARE(sa.applyRemote({ high }), 1);
        QCOMPARE(sb.applyRemote({ high }), 1);
        QCOMPARE(sb.applyRemote({ low }), 0);
        QCOMPARE(sa.get(u"event"_s, id).value(u"title"_s).toString(), u"Groß"_s);
        QCOMPARE(sb.get(u"event"_s, id).value(u"title"_s).toString(), u"Groß"_s);
    }

    void reopenKeepsSequenceAndClock()
    {
        QString path = file(u"reopen.db"_s);
        QString device;
        {
            Database db;
            QVERIFY(db.open(path));
            Store s(&db);
            s.create(u"event"_s, { { u"title"_s, u"Vorspiel"_s } });
            device = db.deviceId();
        }
        Database db;
        QVERIFY(db.open(path));
        QCOMPARE(db.deviceId(), device);
        Store s(&db);
        QVERIFY(!s.create(u"event"_s, { { u"title"_s, u"Zweites"_s } }).isEmpty());   // no seq collision
        QCOMPARE(s.select(u"event"_s).size(), 2);
    }

    void legacyImport()
    {
        const QString legacy = file(u"legacy/db/qupil.db"_s);
        QDir().mkpath(file(u"legacy/db"_s));
        QCOMPARE(LegacyFixture::create(legacy), QString());
        LegacyFixture::writeConfig(file(u"legacy/config.xml"_s));

        QCOMPARE(Database::detectFormat(legacy), 1);
        QCOMPARE(LegacyImporter::findDatabase(file(u"legacy"_s)), QFileInfo(legacy).absoluteFilePath());
        QCOMPARE(LegacyImporter::findConfig(file(u"legacy"_s)), file(u"legacy/config.xml"_s));

        Database db;
        QVERIFY(db.open(file(u"imported.db"_s)));
        Store store(&db);
        LegacyImporter importer;
        QSignalSpy progress(&importer, &LegacyImporter::progress);
        const LegacyImporter::Result r = importer.run(legacy, &store, LegacyImporter::findConfig(file(u"legacy"_s)));
        QVERIFY2(r.ok, qPrintable(r.error));
        QVERIFY(progress.count() > 5);

        QCOMPARE(r.students, 4);
        QCOMPARE(r.archivedStudents, 1);
        QCOMPARE(r.lessons, 4);
        QCOMPARE(r.memberships, 5);
        QCOMPARE(r.pieces, 4);
        QCOMPARE(r.notes, 5);
        QCOMPARE(r.activities, 3);
        QCOMPARE(r.events, 1);
        QCOMPARE(r.archivedEvents, 1);
        QCOMPARE(r.libraryItems, 2);
        QCOMPARE(r.reminders, 2);
        QCOMPARE(r.instruments, 3);
        QVERIFY(r.warnings.isEmpty());

        // Students and dates
        const auto lena = store.select(u"student"_s, u"forename = ?"_s, { u"Lena"_s });
        QCOMPARE(lena.size(), 1);
        QCOMPARE(lena.first().value(u"birthday"_s).toString(), u"2014-03-02"_s);
        QCOMPARE(lena.first().value(u"rental"_s).toInt(), 1);
        const auto tom = store.select(u"student"_s, u"forename = ?"_s, { u"Tom"_s });
        QCOMPARE(tom.first().value(u"birthday"_s).toString(), u"2013-06-15"_s);   // dd.MM.yyyy converted
        const QString lenaId = lena.first().value(u"id"_s).toString();

        // Memberships: open end becomes empty, old lesson name is kept
        const auto tomMembership = store.select(u"membership"_s, u"student_id = ?"_s, { tom.first().value(u"id"_s) });
        QCOMPARE(tomMembership.first().value(u"end_date"_s).toString(), u"2023-07-15"_s);
        QCOMPARE(tomMembership.first().value(u"previous_name"_s).toString(), u"EU - 30 Min. - TomPro"_s);
        const auto lenaMemberships = store.select(u"membership"_s, u"student_id = ?"_s, { lenaId });
        QCOMPARE(lenaMemberships.size(), 2);
        QCOMPARE(lenaMemberships.first().value(u"end_date"_s).toString(), QString());

        // Group note and group piece share their key
        const auto groupNotes = store.select(u"note"_s, u"content LIKE ?"_s, { u"%Bogenwechsel%"_s });
        QCOMPARE(groupNotes.size(), 2);
        QCOMPARE(groupNotes.at(0).value(u"group_key"_s), groupNotes.at(1).value(u"group_key"_s));
        const auto groupPieces = store.select(u"piece"_s, u"title = ?"_s, { u"Cripple Creek"_s });
        QCOMPARE(groupPieces.size(), 2);
        QCOMPARE(groupPieces.at(0).value(u"group_key"_s), groupPieces.at(1).value(u"group_key"_s));
        const auto bourree = store.select(u"piece"_s, u"title = ?"_s, { u"Bourrée"_s });
        QCOMPARE(bourree.first().value(u"composer"_s).toString(), u"J. S. Bach"_s);
        QCOMPARE(bourree.first().value(u"end_date"_s).toString(), QString());
        QVERIFY(store.select(u"note"_s, u"content LIKE ?"_s, { u"%color:#ff0000%"_s }).size() == 1);   // rich text kept

        // Recital programme: order and external contribution
        const auto items = store.select(u"event_item"_s, {}, {}, u"position"_s);
        QCOMPARE(items.size(), 3);
        QVERIFY(!items.at(0).value(u"external_id"_s).toString().isEmpty());
        QCOMPARE(items.at(1).value(u"piece_id"_s), bourree.first().value(u"id"_s));

        // Library loan points to the new student id
        const auto lent = store.select(u"library_item"_s, u"lent_to <> ''"_s);
        QCOMPARE(lent.size(), 1);
        QCOMPARE(lent.first().value(u"lent_to"_s).toString(), lenaId);
        QCOMPARE(lent.first().value(u"number"_s).toInt(), 1);   // index on the music (1.x smlid)

        // Settings from config.xml
        QCOMPARE(store.get(u"setting"_s, u"locations"_s).value(u"value"_s).toString(), u"[\"Raum 20\",\"Aula\"]"_s);
        QCOMPARE(store.get(u"setting"_s, u"event_moderation_minutes"_s).value(u"value"_s).toString(), u"4"_s);
        QCOMPARE(db.meta(u"imported_revision"_s), u"3"_s);

        // The old database is untouched
        QCOMPARE(Database::detectFormat(legacy), 1);
    }

    void legacyRevision1()
    {
        // The oldest 1.x databases: no dbinfos table, no composers, recitals or reminders
        Database db;
        const LegacyImporter::Result r = importLegacy(u"rev1"_s, LegacyFixture::schemaUpTo(1) + LegacyFixture::sampleDataRev1(), db);
        QVERIFY2(r.ok, qPrintable(r.error));
        QVERIFY(r.warnings.isEmpty());
        QCOMPARE(r.students, 2);
        QCOMPARE(r.archivedStudents, 1);
        QCOMPARE(r.lessons, 2);
        QCOMPARE(r.memberships, 3);
        QCOMPARE(r.pieces, 3);
        QCOMPARE(r.notes, 3);
        QCOMPARE(r.activities, 2);
        QCOMPARE(r.libraryItems, 1);
        QCOMPARE(r.events, 0);
        QCOMPARE(r.reminders, 0);
        QCOMPARE(db.meta(u"imported_revision"_s), u"1"_s);

        Store store(&db);
        const auto clara = store.select(u"student"_s, u"forename = ?"_s, { u"Clara"_s });
        QCOMPARE(clara.size(), 1);
        QCOMPARE(clara.first().value(u"father_name"_s).toString(), u"Peter Alt"_s);
        QCOMPARE(clara.first().value(u"father_note"_s).toString(), u"Bäcker"_s);   // the old "occupation"
        QCOMPARE(clara.first().value(u"mother_phone"_s).toString(), u"01234 2222"_s);
        QCOMPARE(clara.first().value(u"recital_interval"_s).toInt(), 5);   // came with revision 2: default
        QCOMPARE(clara.first().value(u"ensemble_expected"_s).toInt(), 1);  // came with revision 3: default
        const auto ole = store.select(u"student"_s, u"forename = ?"_s, { u"Ole"_s });
        QCOMPARE(ole.first().value(u"birthday"_s).toString(), u"2011-07-04"_s);

        const auto sonatine = store.select(u"piece"_s, u"title = ?"_s, { u"Sonatine"_s });
        QCOMPARE(sonatine.first().value(u"composer"_s).toString(), QString());
        QCOMPARE(sonatine.first().value(u"end_date"_s).toString(), u"2018-06-30"_s);
        const auto kanon = store.select(u"piece"_s, u"title = ?"_s, { u"Kanon"_s });
        QCOMPARE(kanon.size(), 2);
        QCOMPARE(kanon.at(0).value(u"group_key"_s), kanon.at(1).value(u"group_key"_s));

        const auto single = store.select(u"activity"_s, u"description = ?"_s, { u"Vorspiel"_s });
        QCOMPARE(single.first().value(u"single_kind"_s).toInt(), 0);
        QCOMPARE(single.first().value(u"end_date"_s).toString(), QString());   // NULL before revision 3
        const auto lent = store.select(u"library_item"_s);
        QCOMPARE(lent.first().value(u"lent_to"_s).toString(), ole.first().value(u"id"_s).toString());
    }

    void legacyRevision2()
    {
        // Recitals and composers, but no reminders and no kinds of regular activities yet
        Database db;
        const LegacyImporter::Result r = importLegacy(u"rev2"_s, LegacyFixture::schemaUpTo(2) + LegacyFixture::sampleDataRev2(), db);
        QVERIFY2(r.ok, qPrintable(r.error));
        QVERIFY(r.warnings.isEmpty());
        QCOMPARE(r.students, 2);
        QCOMPARE(r.pieces, 2);
        QCOMPARE(r.notes, 1);
        QCOMPARE(r.activities, 2);
        QCOMPARE(r.events, 1);
        QCOMPARE(r.archivedEvents, 1);
        QCOMPARE(r.reminders, 0);
        QCOMPARE(db.meta(u"imported_revision"_s), u"2"_s);

        Store store(&db);
        const auto ida = store.select(u"student"_s, u"forename = ?"_s, { u"Ida"_s });
        QCOMPARE(ida.first().value(u"recital_interval"_s).toInt(), 3);
        QCOMPARE(ida.first().value(u"ensemble_expected"_s).toInt(), 1);
        const auto karl = store.select(u"student"_s, u"forename = ?"_s, { u"Karl"_s });
        QCOMPARE(karl.first().value(u"recital_interval"_s).toInt(), 5);
        QCOMPARE(karl.first().value(u"rental"_s).toInt(), 1);

        const auto sarabande = store.select(u"piece"_s, u"title = ?"_s, { u"Sarabande"_s });
        QCOMPARE(sarabande.first().value(u"composer"_s).toString(), u"G. F. Händel"_s);
        const auto menuett = store.select(u"piece"_s, u"title = ?"_s, { u"Menuett"_s });
        QCOMPARE(menuett.first().value(u"composer"_s).toString(), QString());

        const auto competition = store.select(u"activity"_s, u"description = ?"_s, { u"Wettbewerb"_s });
        QCOMPARE(competition.first().value(u"single_kind"_s).toInt(), 2);
        const auto regular = store.select(u"activity"_s, u"description = ?"_s, { u"Kammermusik"_s });
        QCOMPARE(regular.first().value(u"continuous_kind"_s).toInt(), 0);
        QCOMPARE(regular.first().value(u"end_date"_s).toString(), QString());   // NULL: still running

        // Programme in the order of 1.x
        const auto items = store.select(u"event_item"_s, {}, {}, u"position"_s);
        QCOMPARE(items.size(), 2);
        QCOMPARE(items.at(0).value(u"piece_id"_s), sarabande.first().value(u"id"_s));
        QCOMPARE(items.at(1).value(u"piece_id"_s), menuett.first().value(u"id"_s));
    }

    void legacyMessyData()
    {
        // What years of use leave behind: the import goes through and keeps what it can
        Database db;
        const LegacyImporter::Result r = importLegacy(u"messy"_s, LegacyFixture::schemaRev3() + LegacyFixture::messyData(), db);
        QVERIFY2(r.ok, qPrintable(r.error));
        QCOMPARE(r.students, 3);
        QCOMPARE(r.lessons, 2);
        QCOMPARE(r.memberships, 2);
        QCOMPARE(r.pieces, 2);
        QCOMPARE(r.notes, 2);
        QCOMPARE(r.activities, 1);
        QCOMPARE(r.libraryItems, 1);
        QCOMPARE(r.events, 1);
        QCOMPARE(r.reminders, 1);
        QCOMPARE(r.skipped, 8);
        QCOMPARE(r.unparsedDates, 5);
        QCOMPARE(r.warnings.size(), 2);

        Store store(&db);
        QCOMPARE(store.select(u"student"_s).size(), 3);
        const auto nameless = store.select(u"student"_s, u"surname = '' AND forename = ''"_s);
        QCOMPARE(nameless.size(), 1);
        QCOMPARE(nameless.first().value(u"birthday"_s).toString(), u"31.02.2010"_s);   // kept as typed
        QCOMPARE(nameless.first().value(u"first_lesson"_s).toString(), u"Sept. 2019"_s);
        QCOMPARE(nameless.first().value(u"rental"_s).toInt(), 0);              // 'ja'
        QCOMPARE(nameless.first().value(u"recital_interval"_s).toInt(), 5);    // 'oft'

        const auto zoe = store.select(u"student"_s, u"forename = ?"_s, { u"Zoë 🎻"_s });
        QCOMPARE(zoe.size(), 1);
        QCOMPARE(zoe.first().value(u"surname"_s).toString(), u"O'Neill; DROP TABLE pupil;--"_s);
        QCOMPARE(zoe.first().value(u"notes"_s).toString(), u"<p><b>nicht geschlossen <i>kursiv</p"_s);
        const auto blanks = store.select(u"student"_s, u"forename = ?"_s, { u"Zeichen"_s });
        QCOMPARE(blanks.first().value(u"surname"_s).toString(), u"Leer"_s);
        QCOMPARE(blanks.first().value(u"birthday"_s).toString(), u"2012-02-01"_s);

        const auto odd = store.select(u"lesson"_s, u"name = ''"_s);
        QCOMPARE(odd.size(), 1);
        QCOMPARE(odd.first().value(u"start_time"_s).toString(), QString());
        QCOMPARE(odd.first().value(u"end_time"_s).toString(), QString());
        QCOMPARE(odd.first().value(u"active"_s).toInt(), 1);

        const auto noComposer = store.select(u"piece"_s, u"title = ?"_s, { u"Ohne Komponist"_s });
        QCOMPARE(noComposer.first().value(u"composer"_s).toString(), QString());
        QCOMPARE(noComposer.first().value(u"duration"_s).toInt(), 0);
        QCOMPARE(store.select(u"note"_s, u"content = ?"_s, { u"<div><p>Kaputt <b>fett</div>"_s }).size(), 1);
        QCOMPARE(store.select(u"note"_s, u"date = ?"_s, { u"2026-13-45"_s }).size(), 1);

        // References to students that are gone point nowhere, not to someone else
        QCOMPARE(store.select(u"library_item"_s).first().value(u"lent_to"_s).toString(), QString());
        QCOMPARE(store.select(u"reminder"_s).first().value(u"student_id"_s).toString(), QString());
        QCOMPARE(store.select(u"event_item"_s).size(), 1);
        QCOMPARE(store.select(u"event"_s).first().value(u"date"_s).toString(), u"Herbst"_s);
    }

    void takeOverLeavesOriginal()
    {
        // A 1.x data folder whose newest changes are still in the WAL file, as when it
        // was copied while Qupil 1.x ran or Qupil 1.x did not end normally
        const QString folder = file(u"takeover/qupil"_s);
        const QString legacy = folder + u"/db/qupil.db"_s;
        QDir().mkpath(folder + u"/db"_s);
        {
            const QString running = file(u"takeover/running.db"_s);
            QCOMPARE(LegacyFixture::create(running), QString());
            const QString name = u"qupil1-running"_s;
            {
                QSqlDatabase qupil1 = QSqlDatabase::addDatabase(u"QSQLITE"_s, name);
                qupil1.setDatabaseName(running);
                QVERIFY(qupil1.open());
                QSqlQuery q(qupil1);
                QVERIFY(q.exec(u"PRAGMA journal_mode = WAL"_s) && q.next());
                QCOMPARE(q.value(0).toString(), u"wal"_s);
                QVERIFY(q.exec(u"PRAGMA wal_autocheckpoint = 0"_s));
                QVERIFY(q.exec(u"INSERT INTO pupil (pupilid, surname, forename, ifinstrumentnextsize, ifrentinstrument) "
                               u"VALUES (5, 'Spät', 'Nora', 0, 0)"_s));
                QVERIFY(QFileInfo(running + u"-wal"_s).size() > 0);
                QVERIFY(QFile::copy(running, legacy));
                QVERIFY(QFile::copy(running + u"-wal"_s, legacy + u"-wal"_s));
                qupil1.close();
            }
            QSqlDatabase::removeDatabase(name);
        }
        LegacyFixture::writeConfig(folder + u"/config.xml"_s);
        const QByteArray before = contents(legacy);
        const QByteArray walBefore = contents(legacy + u"-wal"_s);
        QVERIFY(!walBefore.isEmpty());
        const QDateTime modified = QFileInfo(legacy).lastModified();

        const QString work = file(u"takeover/work"_s);
        const QString backups = file(u"takeover/backups"_s);
        int progressCalls = 0;
        const TakeOver::Outcome o = TakeOver::run(folder, work, backups, [&](int, const QString &) { ++progressCalls; });
        QVERIFY2(o.ok, qPrintable(o.error));
        QCOMPARE(o.kind, u"legacy"_s);
        QCOMPARE(o.source, QDir::toNativeSeparators(folder));
        QCOMPARE(o.summary.value(u"students"_s).toInt(), 5);   // with the one only in the WAL file
        QVERIFY(o.summary.value(u"settingsFound"_s).toBool());
        QVERIFY(progressCalls > 5);

        // The original is exactly as it was, WAL file included
        QCOMPARE(contents(legacy), before);
        QCOMPARE(contents(legacy + u"-wal"_s), walBefore);
        QCOMPARE(QFileInfo(legacy).lastModified(), modified);

        // Only the result is left in the work folder
        QCOMPARE(o.newDatabase, QDir(work).filePath(u"import.db"_s));
        QCOMPARE(QDir(work).entryList(QDir::Files), QStringList { u"import.db"_s });

        // A copy of the 1.x data as taken over, in one file, next to the backups
        QVERIFY(!o.legacyCopy.isEmpty());
        QCOMPARE(QFileInfo(o.legacyCopy).absolutePath(), QFileInfo(backups).absoluteFilePath());
        QVERIFY(QFileInfo(o.legacyCopy).fileName().startsWith(u"qupil1-"_s));
        QVERIFY(!QFileInfo::exists(o.legacyCopy + u"-wal"_s));
        QString pupils;
        QCOMPARE(Database::detectFormat(o.legacyCopy, &pupils), 1);
        QCOMPARE(pupils, u"5"_s);

        Database db;
        QVERIFY(db.open(o.newDatabase));
        QCOMPARE(db.meta(u"imported_copy"_s), QFileInfo(o.legacyCopy).fileName());
        QCOMPARE(db.meta(u"imported_revision"_s), u"3"_s);
        Store store(&db);
        QCOMPARE(store.select(u"student"_s, u"forename = ?"_s, { u"Nora"_s }).size(), 1);
        db.close();

        // Without a backup folder (demo data): no copy
        const TakeOver::Outcome demo = TakeOver::run(legacy, work, QString());
        QVERIFY2(demo.ok, qPrintable(demo.error));
        QVERIFY(demo.legacyCopy.isEmpty());
        QCOMPARE(QDir(backups).entryList(QDir::Files).size(), 1);
        QCOMPARE(contents(legacy), before);
    }

    void takeOverBackupOrForeignFile()
    {
        // A Qupil 2 backup: taken over with a new device id, the backup itself unchanged
        const QString backup = file(u"takeover2/Qupil-Backup.db"_s);
        QDir().mkpath(file(u"takeover2"_s));
        QString device;
        {
            Database db;
            QVERIFY(db.open(file(u"takeover2/live.db"_s)));
            Store(&db).create(u"student"_s, { { u"surname"_s, u"Gesichert"_s } });
            QVERIFY(db.backupTo(backup));
            device = db.deviceId();
        }
        const QByteArray before = contents(backup);
        const QString work = file(u"takeover2/work"_s);
        const QString backups = file(u"takeover2/backups"_s);
        const TakeOver::Outcome o = TakeOver::run(backup, work, backups);
        QVERIFY2(o.ok, qPrintable(o.error));
        QCOMPARE(o.kind, u"backup"_s);
        QCOMPARE(o.summary.value(u"students"_s).toInt(), 1);
        QVERIFY(o.legacyCopy.isEmpty());
        QVERIFY(!QFileInfo::exists(backups));
        QCOMPARE(contents(backup), before);
        Database db;
        QVERIFY(db.open(o.newDatabase));
        QVERIFY(db.deviceId() != device);
        QCOMPARE(db.meta(u"ask_before_joining"_s), u"1"_s);
        db.close();

        // Anything else is refused, and nothing is left behind
        const QString foreign = file(u"takeover2/notes.txt"_s);
        QFile f(foreign);
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write("not a database");
        f.close();
        const TakeOver::Outcome refused = TakeOver::run(foreign, work, backups);
        QVERIFY(!refused.ok);
        QVERIFY(!refused.error.isEmpty());
        QVERIFY(!QFileInfo::exists(backups));
        const TakeOver::Outcome empty = TakeOver::run(file(u"takeover2/nothing-here"_s), work, backups);
        QVERIFY(!empty.ok);
    }

    void backupAndRestore()
    {
        const QString live = file(u"live.db"_s);
        const QString backup = file(u"Qupil-Backup.db"_s);
        QString device;
        {
            Database db;
            QVERIFY(db.open(live));
            Store s(&db);
            s.create(u"student"_s, { { u"surname"_s, u"Sicher"_s } });
            QString error;
            QVERIFY2(db.backupTo(backup, &error), qPrintable(error));
            device = db.deviceId();
        }
        QString count;
        QCOMPARE(Database::detectFormat(backup, &count), 2);
        QCOMPARE(count, u"1"_s);

        // Restore on another device: same data, new device id
        const QString restored = file(u"restored.db"_s);
        QVERIFY(QFile::copy(backup, restored));
        Database db;
        QVERIFY(db.open(restored));
        QCOMPARE(db.deviceId(), device);
        QVERIFY(db.resetDeviceId());
        QVERIFY(db.deviceId() != device);
        Store s(&db);
        QCOMPARE(s.select(u"student"_s).size(), 1);
        QVERIFY(!s.create(u"student"_s, { { u"surname"_s, u"Neu"_s } }).isEmpty());
    }

    void demoDataImports()
    {
        const QString folder = file(u"demo"_s);
        QString error;
        QVERIFY2(DemoData::writeLegacyFolder(folder, u"de"_s, &error), qPrintable(error));
        const QString legacy = LegacyImporter::findDatabase(folder);
        QVERIFY(!legacy.isEmpty());
        QVERIFY(!LegacyImporter::findConfig(legacy).isEmpty());

        Database db;
        QVERIFY(db.open(file(u"demo-target.db"_s)));
        Store store(&db);
        LegacyImporter importer;
        const LegacyImporter::Result r = importer.run(legacy, &store, LegacyImporter::findConfig(legacy));
        QVERIFY2(r.ok, qPrintable(r.error));
        QCOMPARE(r.students, 9);
        QCOMPARE(r.lessons, 10);
        QVERIFY(r.notes > 20);
        QVERIFY(r.pieces > 15);
        QCOMPARE(r.events, 2);
        QCOMPARE(r.libraryItems, 6);
        QCOMPARE(r.skipped, 0);
        QCOMPARE(r.unparsedDates, 0);
        QVERIFY2(r.warnings.isEmpty(), qPrintable(r.warnings.join(u'\n')));

        // Today always has lessons, and the next event has a programme incl. the external piece
        const int today = QDate::currentDate().dayOfWeek() - 1;
        QVERIFY(store.select(u"lesson"_s, u"active = 1 AND irregular = 0 AND weekday = ?"_s, { today }).size() >= 4);
        const auto upcoming = store.select(u"event"_s, u"state = 0"_s);
        QCOMPARE(upcoming.size(), 1);
        const auto programme = store.select(u"event_item"_s, u"event_id = ?"_s, { upcoming.first().value(u"id"_s) });
        QVERIFY(programme.size() >= 3);
        QCOMPARE(std::count_if(programme.cbegin(), programme.cend(),
                               [](const QVariantMap &m) { return !m.value(u"external_id"_s).toString().isEmpty(); }), 1);

        // English variant has English names
        const QString en = file(u"demo-en"_s);
        QVERIFY(DemoData::writeLegacyFolder(en, u"en"_s));
        QString count;
        QCOMPARE(Database::detectFormat(LegacyImporter::findDatabase(en), &count), 1);
        QCOMPARE(count, u"9"_s);
    }

    void crypto()
    {
        const QByteArray key = Crypto::randomBytes(Crypto::KeySize);
        const QByteArray sealed = Crypto::seal(key, "Notiz", "ad");
        QCOMPARE(Crypto::open(key, sealed, "ad").value_or(QByteArray()), QByteArray("Notiz"));
        QVERIFY(!Crypto::open(key, sealed, "other ad"));                     // wrong context
        QVERIFY(!Crypto::open(Crypto::randomBytes(32), sealed, "ad"));        // wrong key
        QByteArray tampered = sealed;
        tampered[tampered.size() - 1] = char(tampered.at(tampered.size() - 1) ^ 1);
        QVERIFY(!Crypto::open(key, tampered, "ad"));
        QVERIFY(Crypto::seal(key, "x") != Crypto::seal(key, "x"));           // random nonces

        const Crypto::KdfParams fast { 64, 1 };
        const QByteArray salt = Crypto::randomBytes(Crypto::SaltSize);
        QCOMPARE(Crypto::deriveKey(u"Geige"_s, salt, fast), Crypto::deriveKey(u"Geige"_s, salt, fast));
        QVERIFY(Crypto::deriveKey(u"Geige"_s, salt, fast) != Crypto::deriveKey(u"geige"_s, salt, fast));

        const auto a = Crypto::x25519KeyPair();
        const auto b = Crypto::x25519KeyPair();
        QCOMPARE(Crypto::x25519Shared(a.secretKey, b.publicKey), Crypto::x25519Shared(b.secretKey, a.publicKey));
    }

    void folderSync()
    {
        MemoryFolder cloud;
        Database a, b, c;
        QVERIFY(a.open(file(u"fs-a.db"_s)) && b.open(file(u"fs-b.db"_s)) && c.open(file(u"fs-c.db"_s)));
        a.clock().setWallClock(&TestCore::fakeWall);
        b.clock().setWallClock(&TestCore::fakeWall);
        c.clock().setWallClock(&TestCore::fakeWall);
        Store sa(&a), sb(&b), sc(&c);
        const QString lena = sa.create(u"student"_s, { { u"surname"_s, u"Muster"_s }, { u"forename"_s, u"Lena"_s } });

        // First device sets up the folder
        FolderSync fa(&sa, &cloud);
        QCOMPARE(fa.isSetUp().value_or(true), false);
        FolderSync::Status status;
        const QByteArray keyA = fa.unlock(u"Bratsche 4/4"_s, &status);
        QCOMPARE(status, FolderSync::Status::Ok);
        QCOMPARE(fa.run(keyA, u"Laptop"_s, nullptr), FolderSync::Status::Ok);
        QVERIFY(cloud.files.contains(u"Qupil/key.json"_s));
        // Nothing readable on the server
        for (auto it = cloud.files.cbegin(); it != cloud.files.cend(); ++it)
            QVERIFY(!it.value().contains("Muster"));

        // Second device: wrong password, then the right one
        FolderSync fb(&sb, &cloud);
        QCOMPARE(fb.isSetUp().value_or(false), true);
        QVERIFY(fb.unlock(u"falsch"_s, &status).isEmpty());
        QCOMPARE(status, FolderSync::Status::WrongPassword);
        const QByteArray keyB = fb.unlock(u"Bratsche 4/4"_s, &status);
        QCOMPARE(keyB, keyA);
        FolderSync::Stats stats;
        QCOMPARE(fb.run(keyB, u"iPad"_s, &stats), FolderSync::Status::Ok);
        QVERIFY(stats.applied > 0);
        QCOMPARE(stats.devices, QStringList { u"Laptop"_s });
        // Which devices have changes in the folder (to recognise the same data)
        QCOMPARE(fb.origins().value_or(QStringList()), QStringList { a.deviceId() });
        QCOMPARE(sb.get(u"student"_s, lena).value(u"forename"_s).toString(), u"Lena"_s);

        // Changes travel both ways
        QVERIFY(sb.update(u"student"_s, lena, { { u"phone"_s, u"0123"_s } }));
        QCOMPARE(fb.run(keyB, u"iPad"_s, nullptr), FolderSync::Status::Ok);
        QCOMPARE(fa.run(keyA, u"Laptop"_s, nullptr), FolderSync::Status::Ok);
        QCOMPARE(sa.get(u"student"_s, lena).value(u"phone"_s).toString(), u"0123"_s);

        // C only talks to B over Wi-Fi; B relays C's change into the folder for A
        sc.applyRemote(sb.changesSince(sc.versionVector()));
        QVERIFY(sc.update(u"student"_s, lena, { { u"email"_s, u"lena@example.org"_s } }));
        sb.applyRemote(sc.changesSince(sb.versionVector()));
        QCOMPARE(fb.run(keyB, u"iPad"_s, nullptr), FolderSync::Status::Ok);
        QCOMPARE(fa.run(keyA, u"Laptop"_s, nullptr), FolderSync::Status::Ok);
        QCOMPARE(sa.get(u"student"_s, lena).value(u"email"_s).toString(), u"lena@example.org"_s);
        QCOMPARE(sa.versionVector(), sb.versionVector());

        // Offline: a clear failure, nothing lost
        cloud.offline = true;
        QVERIFY(sa.update(u"student"_s, lena, { { u"notes"_s, u"offline"_s } }));
        QCOMPARE(fa.run(keyA, u"Laptop"_s, nullptr), FolderSync::Status::Failed);
        cloud.offline = false;
        QCOMPARE(fa.run(keyA, u"Laptop"_s, nullptr), FolderSync::Status::Ok);
        QCOMPARE(fb.run(keyB, u"iPad"_s, nullptr), FolderSync::Status::Ok);
        QCOMPARE(sb.get(u"student"_s, lena).value(u"notes"_s).toString(), u"offline"_s);

        // A starts over with a new password: the old folder is kept, B has to unlock again
        const QByteArray keyA2 = fa.startOver(u"Cello 7/8"_s, &status);
        QCOMPARE(status, FolderSync::Status::Ok);
        QVERIFY(keyA2 != keyA);
        QVERIFY(std::any_of(cloud.files.keyBegin(), cloud.files.keyEnd(),
                            [](const QString &f) { return f.startsWith(u"Qupil-"_s) && f.endsWith(u"/key.json"_s); }));
        QCOMPARE(fa.run(keyA2, u"Laptop"_s, nullptr), FolderSync::Status::Ok);
        QCOMPARE(fb.run(keyB, u"iPad"_s, nullptr), FolderSync::Status::WrongPassword);
        const QByteArray keyB2 = fb.unlock(u"Cello 7/8"_s, &status);
        QCOMPARE(keyB2, keyA2);
        QCOMPARE(fb.run(keyB2, u"iPad"_s, nullptr), FolderSync::Status::Ok);

        // The folder was deleted on the server: B needs the password again
        for (const QString &f : cloud.files.keys())
            if (f.startsWith(u"Qupil/"_s))
                cloud.files.remove(f);
        QCOMPARE(fb.run(keyB2, u"iPad"_s, nullptr), FolderSync::Status::NoPassword);
    }

    void mdnsMessages()
    {
        Mdns::Message m;
        m.response = true;
        Mdns::Record ptr;
        ptr.name = u"_qupil-sync._tcp.local"_s;
        ptr.type = Mdns::PTR;
        ptr.target = u"Qupil-1a2b3c4d._qupil-sync._tcp.local"_s;
        ptr.ttl = 4500;
        Mdns::Record srv;
        srv.name = ptr.target;
        srv.type = Mdns::SRV;
        srv.cacheFlush = true;
        srv.port = 47123;
        srv.target = u"qupil-1a2b3c4d.local"_s;
        Mdns::Record txt;
        txt.name = ptr.target;
        txt.type = Mdns::TXT;
        txt.txt = { "id=1a2b3c4d", "n=Geigen-iPad", "pair=1" };
        Mdns::Record a;
        a.name = srv.target;
        a.type = Mdns::A;
        a.address = QByteArray("\xC0\xA8\x01\x17", 4);
        m.answers << ptr;
        m.additional << srv << txt << a;

        const auto back = Mdns::decode(Mdns::encode(m));
        QVERIFY(back.has_value());
        QVERIFY(back->response);
        QCOMPARE(back->answers.size(), 1);
        QCOMPARE(back->additional.size(), 3);
        QCOMPARE(back->answers.at(0).target, ptr.target);
        QCOMPARE(back->additional.at(0).port, quint16(47123));
        QVERIFY(back->additional.at(0).cacheFlush);
        QCOMPARE(Mdns::txtValue(back->additional.at(1).txt, "n"), u"Geigen-iPad"_s);
        QCOMPARE(back->additional.at(2).address, a.address);
        QVERIFY(Mdns::sameName(u"_QUPIL-sync._tcp.local."_s, ptr.name));

        // A query with a compressed name, as other responders send them
        QByteArray q("\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00", 12);
        q += QByteArray("\x0b_qupil-sync\x04_tcp\x05local\x00\x00\x0c\x80\x01", 28);
        q += QByteArray("\x05other\xc0\x0c\x00\x21\x00\x01", 12);   // other._qupil-sync._tcp.local SRV
        const auto query = Mdns::decode(q);
        QVERIFY(query.has_value());
        QCOMPARE(query->questions.size(), 2);
        QVERIFY(query->questions.at(0).unicastResponse);
        QCOMPARE(query->questions.at(1).name, u"other._qupil-sync._tcp.local"_s);
        QCOMPARE(query->questions.at(1).type, quint16(Mdns::SRV));

        // Garbage and loops are rejected, not crashed on
        QVERIFY(!Mdns::decode("abc").has_value());
        QByteArray loop("\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\xc0\x0c\x00\x0c\x00\x01", 18);
        QVERIFY(!Mdns::decode(loop).has_value());
    }

    void wifiPairAndSync()
    {
        using namespace WifiSync;
        auto identity = [](const QString &id, const QString &name) {
            const Crypto::KeyPair keys = Crypto::x25519KeyPair();
            return Identity { id, name, keys.secretKey, keys.publicKey };
        };
        const Identity laptop = identity(u"laptop-id"_s, u"Laptop"_s);
        const Identity ipad = identity(u"ipad-id"_s, u"iPad"_s);
        QCOMPARE(normalizeCode(u"482 913"_s), u"482913"_s);
        QCOMPARE(newCode().size(), 6);

        // Runs the server end in its own thread with its own connection to the file
        auto run = [this](const QString &serverDb, const Identity &server, const ServerState &state,
                          const std::function<void(FrameChannel *)> &client) {
            auto toServer = std::make_shared<PipeQueue>();
            auto toClient = std::make_shared<PipeQueue>();
            ServeResult result;
            QThread *thread = QThread::create([&, toServer, toClient] {
                PipeEnd end(toServer, toClient);
                Database db;
                db.open(file(serverDb));
                Store store(&db);
                result = serve(&end, &store, server, state);
            });
            thread->start();
            {
                PipeEnd end(toClient, toServer);
                client(&end);
            }
            thread->wait();
            delete thread;
            return result;
        };

        // The iPad shows a code, the laptop enters it
        ServerState pairing;
        pairing.pairingCode = u"482913"_s;
        pairing.hasData = true;
        pairing.history = { u"ipad-db"_s, u"cloud-device"_s };
        Peer seenByLaptop;
        Status clientStatus = Status::Failed;
        ServeResult served = run(u"w-ipad.db"_s, ipad, pairing, [&](FrameChannel *ch) {
            clientStatus = pair(ch, laptop, u"000 000"_s, false, {}, &seenByLaptop);
        });
        QCOMPARE(clientStatus, Status::WrongCode);
        QCOMPARE(served.status, Status::WrongCode);

        served = run(u"w-ipad.db"_s, ipad, pairing, [&](FrameChannel *ch) {
            clientStatus = pair(ch, laptop, u"482-913"_s, false, { u"laptop-db"_s, u"cloud-device"_s },
                                &seenByLaptop);
        });
        QCOMPARE(clientStatus, Status::Ok);
        QCOMPARE(served.status, Status::Ok);
        QCOMPARE(served.kind, ServeResult::Paired);
        QCOMPARE(seenByLaptop.id, ipad.id);
        QCOMPARE(seenByLaptop.publicKey, ipad.publicKey);
        QVERIFY(seenByLaptop.hasData);
        // Which devices are in each change log: the two have one in common (the same data)
        QCOMPARE(seenByLaptop.history, (QStringList { u"ipad-db"_s, u"cloud-device"_s }));
        QCOMPARE(served.peer.history, (QStringList { u"laptop-db"_s, u"cloud-device"_s }));
        QCOMPARE(served.peer.id, laptop.id);
        QCOMPARE(served.peer.publicKey, laptop.publicKey);
        QVERIFY(!served.peer.hasData);
        const Peer laptopOnIpad = served.peer;

        ServerState notPairing;
        served = run(u"w-ipad.db"_s, ipad, notPairing, [&](FrameChannel *ch) {
            clientStatus = pair(ch, laptop, u"482913"_s, false, {}, &seenByLaptop);
        });
        QCOMPARE(clientStatus, Status::NotPairing);

        // Data on both devices, then a sync in both directions
        QString laptopLena;
        {
            Database db;
            QVERIFY(db.open(file(u"w-laptop.db"_s)));
            Store s(&db);
            laptopLena = s.create(u"student"_s, { { u"surname"_s, u"Muster"_s }, { u"forename"_s, u"Lena"_s } });
        }
        {
            Database db;
            QVERIFY(db.open(file(u"w-ipad.db"_s)));
            Store s(&db);
            s.create(u"lesson"_s, { { u"name"_s, u"Streicherensemble"_s } });
        }
        ServerState paired;
        paired.peers << laptopOnIpad;
        Database laptopDb;
        QVERIFY(laptopDb.open(file(u"w-laptop.db"_s)));
        Store laptopStore(&laptopDb);
        Stats stats;
        served = run(u"w-ipad.db"_s, ipad, paired, [&](FrameChannel *ch) {
            clientStatus = sync(ch, &laptopStore, laptop, seenByLaptop, &stats);
        });
        QCOMPARE(clientStatus, Status::Ok);
        QCOMPARE(served.status, Status::Ok);
        QCOMPARE(served.kind, ServeResult::Synced);
        QVERIFY(stats.sent > 0 && stats.received > 0 && stats.applied > 0);
        QVERIFY(served.stats.applied > 0);
        QCOMPARE(laptopStore.select(u"lesson"_s).size(), 1);
        {
            Database db;
            QVERIFY(db.open(file(u"w-ipad.db"_s)));
            Store s(&db);
            QCOMPARE(s.get(u"student"_s, laptopLena).value(u"forename"_s).toString(), u"Lena"_s);
            QCOMPARE(s.versionVector(), laptopStore.versionVector());
        }

        // Nothing new: only the version vectors travel
        served = run(u"w-ipad.db"_s, ipad, paired, [&](FrameChannel *ch) {
            stats = {};
            clientStatus = sync(ch, &laptopStore, laptop, seenByLaptop, &stats);
        });
        QCOMPARE(clientStatus, Status::Ok);
        QCOMPARE(stats.sent + stats.received, 0);

        // Paused, unknown, or a key that does not fit
        ServerState paused = paired;
        paused.paused << laptop.id;
        run(u"w-ipad.db"_s, ipad, paused, [&](FrameChannel *ch) {
            clientStatus = sync(ch, &laptopStore, laptop, seenByLaptop, nullptr);
        });
        QCOMPARE(clientStatus, Status::Refused);
        run(u"w-ipad.db"_s, ipad, ServerState(), [&](FrameChannel *ch) {
            clientStatus = sync(ch, &laptopStore, laptop, seenByLaptop, nullptr);
        });
        QCOMPARE(clientStatus, Status::UnknownDevice);
        Peer impostor = seenByLaptop;
        impostor.publicKey = Crypto::x25519KeyPair().publicKey;
        served = run(u"w-ipad.db"_s, ipad, paired, [&](FrameChannel *ch) {
            clientStatus = sync(ch, &laptopStore, laptop, impostor, nullptr);
        });
        QCOMPARE(clientStatus, Status::AuthFailed);
        QCOMPARE(served.status, Status::AuthFailed);
    }

    void csvAddressBook()
    {
        // Outlook-like: commas, quotes with commas and line breaks inside, byte order mark
        const QByteArray outlook = "\xEF\xBB\xBF" "First Name,Last Name,Home Street,Birthday,Mobile Phone\r\n"
                                   "Lena,Muster,\"Musterweg 7, Hinterhaus\",12/31/2012,0170 1\r\n"
                                   "\"Tom \"\"T.\"\"\",Wagner,\"Zeile 1\nZeile 2\",31.01.2013,\r\n"
                                   ",,,,\r\n";
        Csv::Error error;
        const auto table = Csv::parse(outlook, true, &error);
        QVERIFY(table.has_value());
        QCOMPARE(table->separator, QChar(u','));
        QCOMPARE(table->headers.size(), 5);
        QCOMPARE(table->headers.first(), u"First Name"_s);
        QCOMPARE(table->rows.size(), 2);   // the empty line is left out
        QCOMPARE(table->rows.at(0).at(2), u"Musterweg 7, Hinterhaus"_s);
        QCOMPARE(table->rows.at(1).at(0), u"Tom \"T.\""_s);
        QCOMPARE(table->rows.at(1).at(2), u"Zeile 1\nZeile 2"_s);

        // German Excel: semicolons, Latin-1
        const auto german = Csv::parse("Vorname;Nachname;Stra\xDF" "e\nJ\xFC" "rgen;M\xFC" "ller;Hauptstr. 1\n", false);
        QVERIFY(german.has_value());
        QCOMPARE(german->separator, QChar(u';'));
        QCOMPARE(german->headers.at(2), u"Straße"_s);
        QCOMPARE(german->rows.at(0).at(1), u"Müller"_s);

        QVERIFY(!Csv::parse("just one column\nvalue\n", true, &error).has_value());
        QCOMPARE(error, Csv::Error::NoSeparator);

        QCOMPARE(Csv::isoDate(u"31.12.2012"_s), u"2012-12-31"_s);
        QCOMPARE(Csv::isoDate(u"2012-12-31"_s), u"2012-12-31"_s);
        QCOMPARE(Csv::isoDate(u"12/31/2012"_s), u"2012-12-31"_s);
        QCOMPARE(Csv::isoDate(u"31/12/2012"_s), u"2012-12-31"_s);
        QCOMPARE(Csv::isoDate(u"31.02.2012"_s), QString());
        QCOMPARE(Csv::isoDate(u"irgendwann"_s), QString());
    }

    void wavFiles()
    {
        Wav::Sound sound;
        sound.sampleRate = 8000;
        sound.channels = 2;
        sound.samples = { 0, 1000, -1000, 32767, -32768, 20000, 5, -5 };
        const QByteArray file = Wav::write(sound);
        QCOMPARE(file.size(), 44 + 16);
        const auto back = Wav::read(file);
        QVERIFY(back.has_value());
        QCOMPARE(back->sampleRate, 8000);
        QCOMPARE(back->channels, 2);
        QCOMPARE(back->samples, sound.samples);
        QCOMPARE(back->peak(), 32768);
        QCOMPARE(back->seconds(), 4.0 / 8000);

        // Volume: half and clipped
        const Wav::Sound half = Wav::scaled(sound, 0.5);
        QCOMPARE(half.samples.at(1), qint16(500));
        QCOMPARE(half.samples.at(4), qint16(-16384));
        QCOMPARE(Wav::scaled(sound, 2.0).samples.at(5), qint16(32767));

        // Cut after a second, fading out to silence
        Wav::Sound tone;
        tone.sampleRate = 1000;
        tone.channels = 1;
        tone.samples = QList<qint16>(3000, 10000);
        const Wav::Sound cut = Wav::trimmed(tone, 1.0);
        QCOMPARE(cut.samples.size(), 1000);
        QCOMPARE(cut.samples.first(), qint16(10000));
        QCOMPARE(cut.samples.last(), qint16(0));
        QCOMPARE(Wav::trimmed(tone, 5.0).samples.size(), 3000);

        // Mono to stereo, both channels the same; silence after it
        Wav::Sound single;
        single.sampleRate = 1000;
        single.channels = 1;
        single.samples = { 1, 2, 3 };
        const Wav::Sound both = Wav::stereo(single);
        QCOMPARE(both.channels, 2);
        QCOMPARE(both.samples, (QList<qint16> { 1, 1, 2, 2, 3, 3 }));
        QCOMPARE(Wav::stereo(both).samples, both.samples);
        const Wav::Sound tail = Wav::padded(both, 0.5);
        QCOMPARE(tail.samples.size(), 6 + 1000);
        QCOMPARE(tail.samples.last(), qint16(0));
        QCOMPARE(tail.seconds(), 0.003 + 0.5);

        // 24-bit mono with an extra chunk before the data, as audio editors write it
        QByteArray wav24 = "RIFF\0\0\0\0WAVEfmt "_ba;
        const auto le = [](QByteArray &b, quint32 v, int bytes) {
            for (int i = 0; i < bytes; ++i)
                b.append(char((v >> (8 * i)) & 0xff));
        };
        le(wav24, 16, 4);
        le(wav24, 1, 2);      // PCM
        le(wav24, 1, 2);      // mono
        le(wav24, 48000, 4);
        le(wav24, 48000 * 3, 4);
        le(wav24, 3, 2);
        le(wav24, 24, 2);
        wav24 += "LIST"_ba;
        le(wav24, 3, 4);
        wav24 += "abc\0"_ba;  // odd size + pad byte
        wav24 += "data"_ba;
        le(wav24, 6, 4);
        le(wav24, 0x123456, 3);
        le(wav24, 0xFFFF00, 3);   // -256 → top 16 bits -1
        const auto mono = Wav::read(wav24);
        QVERIFY(mono.has_value());
        QCOMPARE(mono->sampleRate, 48000);
        QCOMPARE(mono->samples, QList<qint16>({ 0x1234, -1 }));

        QVERIFY(!Wav::read("RIFF....WAVEjunk"_ba).has_value());
        QVERIFY(!Wav::read("OggS"_ba).has_value());
    }

    void lessonAlerts()
    {
        using namespace LessonAlerts;
        const QList<Lesson> lessons = {
            { 2, u"10:00"_s, u"Anna"_s },     // Wednesday
            { 2, u"10:00"_s, u"Ben"_s },      // same end: one alert
            { 0, u"9:45"_s, u"Clara"_s },     // Monday, without leading zero
            { 0, u"00:01"_s, u"Night"_s },    // alert on Sunday evening
            { 4, u"later"_s, u"Broken"_s },   // no time: ignored
        };
        const QList<Alert> alerts = weekly(lessons, 3);
        QCOMPARE(alerts.size(), 3);
        QCOMPARE(alerts.at(0).at, 9 * 60 + 42);                      // Monday 09:42
        QCOMPARE(alerts.at(1).at, 2 * kMinutesPerDay + 9 * 60 + 57); // Wednesday 09:57
        QCOMPARE(alerts.at(1).names, QStringList({ u"Anna"_s, u"Ben"_s }));
        QCOMPARE(alerts.at(2).at, kMinutesPerWeek - 2);              // Sunday 23:58
        QCOMPARE(alerts.at(2).weekday(), 6);
        QCOMPARE(alerts.at(2).minuteOfDay(), 23 * 60 + 58);

        // 2026-10-14 is a Wednesday
        const Alert wednesday = alerts.at(1);
        QCOMPARE(nextTime(wednesday, QDateTime(QDate(2026, 10, 14), QTime(8, 0))),
                 QDateTime(QDate(2026, 10, 14), QTime(9, 57)));
        QCOMPARE(nextTime(wednesday, QDateTime(QDate(2026, 10, 14), QTime(9, 57))),
                 QDateTime(QDate(2026, 10, 21), QTime(9, 57)));
        QCOMPARE(nextTime(alerts.at(2), QDateTime(QDate(2026, 10, 14), QTime(12, 0))),
                 QDateTime(QDate(2026, 10, 18), QTime(23, 58)));

        // Checked every 10 seconds: exactly one tick sees it
        const QDateTime tick(QDate(2026, 10, 14), QTime(9, 56, 55));
        QCOMPARE(due(alerts, tick, tick.addSecs(10)).size(), 1);
        QCOMPARE(due(alerts, tick.addSecs(10), tick.addSecs(20)).size(), 0);
        QCOMPARE(due(alerts, tick.addSecs(-10), tick).size(), 0);
        // Late (computer was asleep): still while the lesson runs, not after it ended
        QCOMPARE(due(alerts, tick, tick.addSecs(120)).size(), 1);
        QCOMPARE(due(alerts, tick, tick.addSecs(200)).size(), 0);

        // Order from Wednesday noon: Sunday night, Monday, next Wednesday
        const QList<Alert> next = upcoming(alerts, QDateTime(QDate(2026, 10, 14), QTime(12, 0)));
        QCOMPARE(next.at(0).at, kMinutesPerWeek - 2);
        QCOMPARE(next.at(1).at, 9 * 60 + 42);
        QCOMPARE(next.at(2).names.first(), u"Anna"_s);
    }

    void migratesSchema()
    {
        // A database of schema version 1 (before the library number) gets the new column
        const QString path = file(u"v1.db"_s);
        {
            Database db;
            QString error;
            QVERIFY2(db.open(path, &error), qPrintable(error));
            QSqlQuery q(db.connection());
            QVERIFY(q.exec(u"CREATE TABLE library_v1 AS SELECT id, author, title, publisher, lent_to, lent_since, deleted "
                           u"FROM library_item"_s));
            QVERIFY(q.exec(u"DROP TABLE library_item"_s));
            QVERIFY(q.exec(u"ALTER TABLE library_v1 RENAME TO library_item"_s));
            QVERIFY(q.exec(u"INSERT INTO library_item (id, author, title, publisher, lent_to, lent_since, deleted) "
                           u"VALUES ('x', 'Bach', 'Suiten', '', '', '', 0)"_s));
            QVERIFY(db.setMeta(u"schema_version"_s, u"1"_s));
            db.close();
        }
        Database db;
        QString error;
        QVERIFY2(db.open(path, &error), qPrintable(error));
        QCOMPARE(db.meta(u"schema_version"_s), QString::number(Schema::Version));
        QSqlQuery q(db.connection());
        QVERIFY(q.exec(u"SELECT number, title FROM library_item WHERE id = 'x'"_s) && q.next());
        QCOMPARE(q.value(0).toInt(), 0);
        QCOMPARE(q.value(1).toString(), u"Suiten"_s);
    }

    void migratesSyncLog()
    {
        // A database as schema 2 wrote it: one log row and one clock per field, the clock as text
        const QString path = file(u"v2-sync.db"_s);
        const QString me = u"0190a0b0-0000-7000-8000-00000000000a"_s;
        const QString other = u"0190a0b0-0000-7000-8000-00000000000b"_s;
        const QString id = u"0190a0b0-1111-7000-8000-000000000001"_s;
        const qint64 ms = 1'800'000'000'000;
        const EntityDef *def = Schema::entity(u"student"_s);
        const qint64 createdSeq = def->fields.size() + 1;
        {
            Database db;
            QVERIFY(db.open(path));
            QSqlQuery q(db.connection());
            for (const char *sql : {
                     "DROP TABLE change_log", "DROP TABLE field_clock", "DROP TABLE origin",
                     "CREATE TABLE field_clock (entity TEXT NOT NULL, row_id TEXT NOT NULL, field TEXT NOT NULL, "
                     "hlc TEXT NOT NULL, PRIMARY KEY (entity, row_id, field)) WITHOUT ROWID",
                     "CREATE TABLE change_log (origin TEXT NOT NULL, seq INTEGER NOT NULL, hlc TEXT NOT NULL, "
                     "entity TEXT NOT NULL, row_id TEXT NOT NULL, field TEXT NOT NULL, value TEXT, "
                     "PRIMARY KEY (origin, seq)) WITHOUT ROWID",
                     "CREATE INDEX idx_change_log_hlc ON change_log (hlc)" })
                QVERIFY2(q.exec(QString::fromLatin1(sql)), sql);
            QVERIFY(db.setMeta(u"device_id"_s, me));
            QVERIFY(db.setMeta(u"schema_version"_s, u"2"_s));
            QVERIFY(q.exec(u"INSERT INTO student (id) VALUES ('%1')"_s.arg(id)));
            int counter = 0;
            const auto write = [&](const QString &origin, qint64 seq, qint64 at, const QString &field, const QVariant &value) {
                const QString hlc = HybridClock::format(HybridClock::make(at, counter++), origin);
                q.prepare(u"INSERT INTO change_log VALUES (?, ?, ?, 'student', ?, ?, ?)"_s);
                for (const QVariant &v : { QVariant(origin), QVariant(seq), QVariant(hlc), QVariant(id), QVariant(field),
                                           QVariant(Store::encodeValue(value)) })
                    q.addBindValue(v);
                QVERIFY(q.exec());
                q.prepare(u"INSERT INTO field_clock VALUES ('student', ?, ?, ?) "
                          u"ON CONFLICT(entity, row_id, field) DO UPDATE SET hlc = excluded.hlc"_s);
                for (const QVariant &v : { QVariant(id), QVariant(field), QVariant(hlc) })
                    q.addBindValue(v);
                QVERIFY(q.exec());
                q.prepare(u"UPDATE student SET %1 = ? WHERE id = ?"_s.arg(field));
                q.addBindValue(value);
                q.addBindValue(id);
                QVERIFY(q.exec());
            };
            // The old Store: a new record wrote every field with its own time, "deleted" last …
            qint64 seq = 0;
            for (const FieldDef &f : def->fields)
                write(me, ++seq, ms, f.name, f.type == u"INTEGER"_s ? QVariant(3) : QVariant(u"alt-"_s + f.name));
            write(me, ++seq, ms + 1, u"deleted"_s, 0);
            // … and changes later on: one here, one on another device
            write(me, ++seq, ms + 60'000, u"forename"_s, u"Lena"_s);
            write(other, 1, ms + 120'000, u"phone"_s, u"0123"_s);
        }

        Database db;
        db.setMigrationBackup(file(u"v2-sync-backup.db"_s));
        QString error;
        QVERIFY2(db.open(path, &error), qPrintable(error));
        QCOMPARE(db.meta(u"schema_version"_s).toInt(), Schema::Version);
        QCOMPARE(Database::schemaVersionOf(file(u"v2-sync-backup.db"_s)), 2);   // the copy from before
        QCOMPARE(db.deviceId(), me);
        QCOMPARE(db.localOrigin(), 1);
        Store store(&db);

        // Same data, same change identities (device and last sequence number)
        const QVariantMap r = store.get(u"student"_s, id);
        QCOMPARE(r.value(u"surname"_s).toString(), u"alt-surname"_s);
        QCOMPARE(r.value(u"forename"_s).toString(), u"Lena"_s);
        QCOMPARE(r.value(u"phone"_s).toString(), u"0123"_s);
        QCOMPARE(r.value(u"recital_interval"_s).toInt(), 3);
        const VersionVector vv = store.versionVector();
        QCOMPARE(vv.value(me), createdSeq + 1);
        QCOMPARE(vv.value(other), 1);

        // The new record is one log row now, the clock one for the record plus the fields changed later
        const QList<Change> mine = store.changesOf(me, 0);
        QCOMPARE(mine.size(), 2);
        QCOMPARE(mine.at(0).seq, createdSeq);
        QCOMPARE(mine.at(0).fields.size(), def->fields.size() + 1);
        QCOMPARE(mine.at(0).fields.value(u"forename"_s).toString(), u"alt-forename"_s);
        QCOMPARE(mine.at(1).fields, (QVariantMap { { u"forename"_s, u"Lena"_s } }));
        QSqlQuery q(db.connection());
        QVERIFY(q.exec(u"SELECT field FROM field_clock WHERE row_id = '%1' ORDER BY field"_s.arg(id)));
        QStringList clocks;
        while (q.next())
            clocks << q.value(0).toString();
        QCOMPARE(clocks, (QStringList { QString(), u"forename"_s, u"phone"_s }));
        QVERIFY(q.exec(u"SELECT COUNT(*) FROM sqlite_master WHERE name = 'idx_change_log_hlc'"_s) && q.next());
        QCOMPARE(q.value(0).toInt(), 0);

        // Decisions stay the same: older than the phone change loses, newer than the record wins
        const QString third = u"0190a0b0-0000-7000-8000-00000000000c"_s;
        QCOMPARE(store.applyRemote({ { third, 1, HybridClock::make(ms + 90'000, 0), u"student"_s, id,
                                       { { u"phone"_s, u"alt"_s }, { u"email"_s, u"neu@example.org"_s } } } }),
                 1);
        QCOMPARE(store.get(u"student"_s, id).value(u"phone"_s).toString(), u"0123"_s);
        QCOMPARE(store.get(u"student"_s, id).value(u"email"_s).toString(), u"neu@example.org"_s);

        // A new device gets everything from the updated log
        Database fresh;
        QVERIFY(fresh.open(file(u"v2-fresh.db"_s)));
        Store sf(&fresh);
        QVERIFY(sf.applyRemote(store.changesSince({})) > 0);
        QCOMPARE(sf.get(u"student"_s, id), store.get(u"student"_s, id));
        QCOMPARE(sf.versionVector(), store.versionVector());

        // New changes continue after the last number
        QVERIFY(!store.create(u"event"_s, { { u"title"_s, u"Neu"_s } }).isEmpty());
        QCOMPARE(store.versionVector().value(me), createdSeq + 2);
    }

    void migratesParentNotes()
    {
        // Schema 3: the parents' occupation (mother_job, father_job) with clocks and log rows
        const QString path = file(u"v3-parents.db"_s);
        const QString me = u"0190a0b0-0000-7000-8000-00000000000a"_s;
        const QString newer = u"0190a0b0-0000-7000-8000-00000000000b"_s;   // already on schema 4
        const QString older = u"0190a0b0-0000-7000-8000-00000000000c"_s;   // still on schema 3
        const QString id = u"0190a0b0-2222-7000-8000-000000000001"_s;
        const qint64 t0 = HybridClock::make(1'800'000'000'000, 0);
        const qint64 t1 = HybridClock::make(1'800'000'060'000, 0);
        const qint64 t2 = HybridClock::make(1'800'000'120'000, 0);
        int nMe = 0;
        int nNewer = 0;
        {
            Database db;
            QVERIFY(db.open(path));
            QSqlQuery q(db.connection());
            QVERIFY(q.exec(u"ALTER TABLE student RENAME COLUMN mother_note TO mother_job"_s));
            QVERIFY(q.exec(u"ALTER TABLE student RENAME COLUMN father_note TO father_job"_s));
            QVERIFY(db.setMeta(u"schema_version"_s, u"3"_s));
            QVERIFY(db.setMeta(u"device_id"_s, me));
            nMe = db.originNumber(me);
            nNewer = db.originNumber(newer);
            QVERIFY(q.exec(u"INSERT INTO student (id, surname, mother_job, father_job) "
                           u"VALUES ('%1', 'Muster', 'Ärztin', 'Bäcker, Frühschicht')"_s.arg(id)));
            const auto log = [&](int origin, qint64 seq, qint64 time, const QVariantMap &data) {
                q.prepare(u"INSERT INTO change_log VALUES (?, ?, ?, 'student', ?, ?)"_s);
                for (const QVariant &v : { QVariant(origin), QVariant(seq), QVariant(time), QVariant(id),
                                           QVariant(Store::encodeFields(data)) })
                    q.addBindValue(v);
                QVERIFY(q.exec());
            };
            const auto clock = [&](const QString &field, qint64 time, int origin) {
                q.prepare(u"INSERT INTO field_clock VALUES ('student', ?, ?, ?, ?)"_s);
                for (const QVariant &v : { QVariant(id), QVariant(field), QVariant(time), QVariant(origin) })
                    q.addBindValue(v);
                QVERIFY(q.exec());
            };
            log(nMe, 1, t0, { { u"surname"_s, u"Muster"_s }, { u"mother_job"_s, u"Ärztin"_s },
                              { u"father_job"_s, u"Bäcker"_s }, { u"deleted"_s, 0 } });
            clock(u""_s, t0, nMe);   // the record (not a null string: NOT NULL)
            log(nMe, 2, t1, { { u"father_job"_s, u"Bäcker, Frühschicht"_s } });
            clock(u"father_job"_s, t1, nMe);
            // Received from a device with the new build while this one was older: in the
            // log, not applied (the field was unknown here); one newer, one older than ours
            log(nNewer, 1, t2, { { u"mother_note"_s, u"Bitte per Mail"_s } });
            log(nNewer, 2, t0 - 1, { { u"father_note"_s, u"zu alt"_s } });
        }

        Database db;
        db.setMigrationBackup(file(u"v3-parents-backup.db"_s));
        QString error;
        QVERIFY2(db.open(path, &error), qPrintable(error));
        QCOMPARE(db.meta(u"schema_version"_s).toInt(), Schema::Version);
        QCOMPARE(Database::schemaVersionOf(file(u"v3-parents-backup.db"_s)), 3);

        // The occupation columns are gone, their values are the notes
        QSqlQuery q(db.connection());
        QStringList columns;
        QVERIFY(q.exec(u"PRAGMA table_info(student)"_s));
        while (q.next())
            columns << q.value(1).toString();
        QVERIFY(!columns.contains(u"mother_job"_s) && !columns.contains(u"father_job"_s));
        Store store(&db);
        QVariantMap s = store.get(u"student"_s, id);
        QCOMPARE(s.value(u"mother_note"_s).toString(), u"Bitte per Mail"_s);        // the newer device's note
        QCOMPARE(s.value(u"father_note"_s).toString(), u"Bäcker, Frühschicht"_s);   // copied, the older note lost

        // Clocks and log under the new names, the same changes (device and number)
        QVERIFY(q.exec(u"SELECT field, time, origin FROM field_clock WHERE row_id = '%1' ORDER BY field"_s.arg(id)));
        QList<QStringList> clocks;
        while (q.next())
            clocks << QStringList { q.value(0).toString(), q.value(1).toString(), q.value(2).toString() };
        QCOMPARE(clocks, (QList<QStringList> { { QString(), QString::number(t0), QString::number(nMe) },
                                               { u"father_note"_s, QString::number(t1), QString::number(nMe) },
                                               { u"mother_note"_s, QString::number(t2), QString::number(nNewer) } }));
        const QList<Change> mine = store.changesOf(me, 0);
        QCOMPARE(mine.size(), 2);
        QCOMPARE(mine.at(0).fields.value(u"mother_note"_s).toString(), u"Ärztin"_s);
        QVERIFY(!mine.at(0).fields.contains(u"mother_job"_s));
        QCOMPARE(mine.at(1).fields, (QVariantMap { { u"father_note"_s, u"Bäcker, Frühschicht"_s } }));
        QCOMPARE(store.versionVector().value(me), 2);
        QCOMPARE(store.versionVector().value(newer), 2);

        // A device still on the old build sends the occupation: it arrives as the note
        QCOMPARE(store.applyRemote({ { older, 1, HybridClock::make(1'800'000'180'000, 0), u"student"_s, id,
                                       { { u"mother_job"_s, u"Nur abends"_s } } } }),
                 1);
        QCOMPARE(store.get(u"student"_s, id).value(u"mother_note"_s).toString(), u"Nur abends"_s);
        QCOMPARE(store.changesOf(older, 0).first().fields, (QVariantMap { { u"mother_note"_s, u"Nur abends"_s } }));
        QVERIFY(!store.update(u"student"_s, id, { { u"mother_job"_s, u"x"_s } }));
        QVERIFY(store.update(u"student"_s, id, { { u"father_note"_s, u"Spricht Englisch"_s } }));

        // A new device gets the same from the log
        Database fresh;
        QVERIFY(fresh.open(file(u"v3-parents-fresh.db"_s)));
        Store sf(&fresh);
        QVERIFY(sf.applyRemote(store.changesSince({})) > 0);
        QCOMPARE(sf.get(u"student"_s, id), store.get(u"student"_s, id));
    }

    void syncFormats()
    {
        const Change c { u"dev-a"_s, 42, HybridClock::make(1'800'000'000'123, 7), u"note"_s, u"n1"_s,
                         { { u"content"_s, u"**Tonleiter** G-Dur"_s }, { u"deleted"_s, 0 }, { u"pieces"_s, QVariant() } } };
        int version = 0;
        const auto back = SyncFormat::decode(SyncFormat::encode({ c }), &version);
        QVERIFY(back);
        QCOMPARE(version, 2);
        QCOMPARE(back->size(), 1);
        const Change &d = back->first();
        QCOMPARE(d.origin, c.origin);
        QCOMPARE(d.seq, c.seq);
        QCOMPARE(d.time, c.time);
        QCOMPARE(d.entity, c.entity);
        QCOMPARE(d.rowId, c.rowId);
        QCOMPARE(d.fields.value(u"content"_s).toString(), u"**Tonleiter** G-Dur"_s);
        QCOMPARE(d.fields.value(u"deleted"_s).toInt(), 0);
        QVERIFY(d.fields.contains(u"pieces"_s) && d.fields.value(u"pieces"_s).isNull());

        // Files of older builds: one field per change, the clock as text
        const QJsonArray old { u"dev-a"_s, 7.0, u"1800000000123.0007.dev-a"_s, u"note"_s, u"n1"_s, u"content"_s,
                               u"[\"alt\"]"_s };
        QJsonArray list;   // not QJsonArray { old }: some compilers make that a copy of old
        list.append(old);
        const auto fromV1 = SyncFormat::decode(
            qCompress(QJsonDocument(QJsonObject { { u"v"_s, 1 }, { u"changes"_s, list } }).toJson(), 6), &version);
        QVERIFY(fromV1);
        QCOMPARE(version, 1);
        QCOMPARE(fromV1->first().time, c.time);
        QCOMPARE(fromV1->first().seq, 7);
        QCOMPARE(fromV1->first().fields, (QVariantMap { { u"content"_s, u"alt"_s } }));

        // A newer format is refused, and recognised as newer
        QVERIFY(!SyncFormat::decode(qCompress(QByteArray("{\"v\":3,\"changes\":[]}"), 6), &version));
        QCOMPARE(version, 3);
    }

    void readsOldSyncFiles()
    {
        // The sync folder still holds files of a build before sync format 2
        MemoryFolder cloud;
        Database a;
        QVERIFY(a.open(file(u"old-files.db"_s)));
        Store sa(&a);
        FolderSync fa(&sa, &cloud);
        FolderSync::Status status;
        const QByteArray key = fa.unlock(u"Geige 3/4"_s, &status);
        QCOMPARE(status, FolderSync::Status::Ok);

        const QString origin = u"0190a0b0-0000-7000-8000-0000000000dd"_s;
        const QString id = u"0190a0b0-2222-7000-8000-000000000001"_s;
        QJsonArray changes;
        qint64 seq = 0;
        const auto add = [&](const QString &field, const QVariant &value) {
            ++seq;
            changes.append(QJsonArray { origin, double(seq),
                                        HybridClock::format(HybridClock::make(1'800'000'000'000, int(seq)), origin),
                                        u"event"_s, id, field, Store::encodeValue(value) });
        };
        for (const FieldDef &f : Schema::entity(u"event"_s)->fields)
            add(f.name, f.name == u"title"_s ? QVariant(u"Vorspiel"_s)
                                             : (f.type == u"INTEGER"_s ? QVariant(0) : QVariant(QString())));
        add(u"deleted"_s, 0);
        const QByteArray plain =
            qCompress(QJsonDocument(QJsonObject { { u"v"_s, 1 }, { u"changes"_s, changes } }).toJson(QJsonDocument::Compact), 6);
        const QString name = u"%1-%2.bin"_s.arg(1, 12, 10, QLatin1Char('0')).arg(seq, 12, 10, QLatin1Char('0'));
        cloud.folders.insert(u"Qupil/changes"_s);
        cloud.folders.insert(u"Qupil/changes/"_s + origin);
        cloud.files.insert(u"Qupil/changes/"_s + origin + u'/' + name,
                           Crypto::seal(key, plain, (origin + u'/' + name).toUtf8()));

        FolderSync::Stats stats;
        QCOMPARE(fa.run(key, u"Laptop"_s, &stats), FolderSync::Status::Ok);
        QCOMPARE(sa.get(u"event"_s, id).value(u"title"_s).toString(), u"Vorspiel"_s);
        QCOMPARE(stats.maxTime, HybridClock::make(1'800'000'000'000, int(seq)));
        // Compacted like an updated database: one log row, one clock
        const QList<Change> logged = sa.changesOf(origin, 0);
        QCOMPARE(logged.size(), 1);
        QCOMPARE(logged.first().seq, seq);
        QCOMPARE(sa.versionVector().value(origin), seq);
        QSqlQuery q(a.connection());
        QVERIFY(q.exec(u"SELECT COUNT(*) FROM field_clock WHERE row_id = '%1'"_s.arg(id)) && q.next());
        QCOMPARE(q.value(0).toInt(), 1);
        // Nothing of it is uploaded again
        const int files = int(cloud.files.size());
        QCOMPARE(fa.run(key, u"Laptop"_s, &stats), FolderSync::Status::Ok);
        QCOMPARE(int(cloud.files.size()), files);
    }

    void rejectsForeignFiles()
    {
        const QString foreign = file(u"foreign.db"_s);
        {
            QSqlDatabase d = QSqlDatabase::addDatabase(u"QSQLITE"_s, u"foreign"_s);
            d.setDatabaseName(foreign);
            QVERIFY(d.open());
            QSqlQuery(d).exec(u"CREATE TABLE something (x)"_s);
            d.close();
        }
        QSqlDatabase::removeDatabase(u"foreign"_s);
        QCOMPARE(Database::detectFormat(foreign), 0);
        Database db;
        QString error;
        QVERIFY(!db.open(foreign, &error));
        QVERIFY(!error.isEmpty());
    }
};

QTEST_GUILESS_MAIN(TestCore)
#include "tst_core.moc"
