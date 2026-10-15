// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

#include <QDate>
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
        HybridClock c(u"dev-a"_s);
        c.setWallClock([] { return wall; });
        const QString t1 = c.now();
        const QString t2 = c.now();          // same ms: counter goes up
        QVERIFY(t1 < t2);
        c.observe(HybridClock::format(5000, 3, u"dev-b"_s));   // other device is ahead
        const QString t3 = c.now();
        QVERIFY(t3 > HybridClock::format(5000, 3, u"dev-b"_s));
        wall = 9000;
        QVERIFY(c.now() > t3);
        qint64 ms = 0;
        int counter = 0;
        QString device;
        QVERIFY(HybridClock::parse(t3, &ms, &counter, &device));
        QCOMPARE(device, u"dev-a"_s);

        // A remote change from the very same millisecond as our wall clock: what we
        // write next must still be later, whatever the device ids
        static qint64 still = 20000;
        HybridClock early(u"0-first-in-sort-order"_s);
        early.setWallClock([] { return still; });
        early.observe(HybridClock::format(20000, 2, u"z-device"_s));
        QVERIFY(early.now() > HybridClock::format(20000, 2, u"z-device"_s));
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

        QVERIFY(store.update(u"student"_s, id, { { u"forename"_s, u"Lea"_s }, { u"surname"_s, u"Muster"_s } }));
        QSqlQuery q(db.connection());
        q.exec(u"SELECT COUNT(*) FROM change_log WHERE row_id = '%1' AND field = 'surname'"_s.arg(id));
        q.next();
        QCOMPARE(q.value(0).toInt(), 1);     // unchanged surname not logged again

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
        Peer seenByLaptop;
        Status clientStatus = Status::Failed;
        ServeResult served = run(u"w-ipad.db"_s, ipad, pairing, [&](FrameChannel *ch) {
            clientStatus = pair(ch, laptop, u"000 000"_s, false, &seenByLaptop);
        });
        QCOMPARE(clientStatus, Status::WrongCode);
        QCOMPARE(served.status, Status::WrongCode);

        served = run(u"w-ipad.db"_s, ipad, pairing, [&](FrameChannel *ch) {
            clientStatus = pair(ch, laptop, u"482-913"_s, false, &seenByLaptop);
        });
        QCOMPARE(clientStatus, Status::Ok);
        QCOMPARE(served.status, Status::Ok);
        QCOMPARE(served.kind, ServeResult::Paired);
        QCOMPARE(seenByLaptop.id, ipad.id);
        QCOMPARE(seenByLaptop.publicKey, ipad.publicKey);
        QVERIFY(seenByLaptop.hasData);
        QCOMPARE(served.peer.id, laptop.id);
        QCOMPARE(served.peer.publicKey, laptop.publicKey);
        QVERIFY(!served.peer.hasData);
        const Peer laptopOnIpad = served.peer;

        ServerState notPairing;
        served = run(u"w-ipad.db"_s, ipad, notPairing, [&](FrameChannel *ch) {
            clientStatus = pair(ch, laptop, u"482913"_s, false, &seenByLaptop);
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
