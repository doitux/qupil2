// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

#include <QDate>
#include <QSignalSpy>
#include <QSqlQuery>
#include <QMap>
#include <QSet>
#include <QTemporaryDir>
#include <QTest>

#include "Crypto.h"
#include "Database.h"
#include "FolderSync.h"
#include "DemoData.h"
#include "Ids.h"
#include "LegacyFixture.h"
#include "LegacyImporter.h"
#include "Store.h"

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
    QString lastError() const override { return QStringLiteral("offline"); }
    bool lastErrorWasNotFound() const override { return notFound; }
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
