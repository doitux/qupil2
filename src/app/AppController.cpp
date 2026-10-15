// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "AppController.h"

#include <QClipboard>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJSEngine>
#include <QQmlEngine>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QThread>
#include <QTimer>
#include <QUuid>

#include <algorithm>

#include "Database.h"
#include "DemoData.h"
#include "LanguageManager.h"
#include "LegacyImporter.h"
#include "Schema.h"
#include "Store.h"

using namespace Qt::StringLiterals;
using Qupil::Database;
using Qupil::LegacyImporter;
using Qupil::Store;

namespace {

AppController *s_instance = nullptr;

constexpr int kKeepAutoBackups = 14;
constexpr int kKeepSafetyBackups = 10;

//! Path QFile can open: local files, Android content:// URIs, plain paths
QString openablePath(const QUrl &url)
{
    if (url.isLocalFile())
        return url.toLocalFile();
    if (url.scheme().isEmpty())
        return url.path();
    return url.toString();
}

//! Copies a file (also from content:// URIs) in chunks
bool copyFile(const QString &from, const QString &to, QString *error)
{
    QFile in(from);
    if (!in.open(QIODevice::ReadOnly)) {
        *error = in.errorString();
        return false;
    }
    QFile::remove(to);
    QFile out(to);
    if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        *error = out.errorString();
        return false;
    }
    QByteArray chunk;
    while (!(chunk = in.read(1 << 20)).isEmpty()) {
        if (out.write(chunk) != chunk.size()) {
            *error = out.errorString();
            return false;
        }
    }
    return out.flush();
}

//! A staged copy may come with a WAL file: fold it in, so it can be read read-only
void consolidate(const QString &file)
{
    const QString name = u"qupil-stage-"_s + QUuid::createUuid().toString(QUuid::WithoutBraces);
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(u"QSQLITE"_s, name);
        db.setDatabaseName(file);
        if (db.open()) {
            QSqlQuery(db).exec(u"PRAGMA journal_mode = DELETE"_s);
            db.close();
        }
    }
    QSqlDatabase::removeDatabase(name);
}

// A virus scanner may hold a freshly written file for a moment (Windows)
bool renameWithRetry(const QString &from, const QString &to)
{
    for (int attempt = 0; attempt < 20; ++attempt) {
        if (QFile::rename(from, to))
            return true;
        QThread::msleep(100);
    }
    return false;
}

void removeDatabaseFiles(const QString &path)
{
    for (const char *suffix : { "", "-wal", "-shm", "-journal" })
        QFile::remove(path + QLatin1StringView(suffix));
}

QString timestamp()
{
    return QDateTime::currentDateTime().toString(u"yyyy-MM-dd-HHmmss"_s);
}

} // namespace

// ---------------------------------------------------------------------------- lifecycle

AppController::AppController(const QString &dataDir, QObject *parent)
    : QObject(parent)
    , m_dataDir(dataDir)
    , m_db(std::make_unique<Database>())
    , m_store(std::make_unique<Store>(m_db.get()))
{
    Q_ASSERT_X(!s_instance, "AppController", "only one instance allowed");
    s_instance = this;
    QDir().mkpath(m_dataDir);
    QDir().mkpath(QDir(m_dataDir).filePath(u"backups"_s));

    if (QFileInfo(databasePath()).size() > 0) {
        QString error;
        if (openDatabase(&error)) {
            m_phase = Ready;
            QTimer::singleShot(5000, this, &AppController::autoBackupIfDue);
        } else {
            m_startupError = error;
        }
    }
    if (m_phase == Setup)
        refreshLegacyFolders();

    connect(m_store.get(), &Store::dataChanged, this, &AppController::infoChanged);
}

AppController::~AppController()
{
    if (m_db)
        m_db->close();
    if (s_instance == this)
        s_instance = nullptr;
}

AppController *AppController::instance()
{
    return s_instance;
}

AppController *AppController::create(QQmlEngine *, QJSEngine *)
{
    Q_ASSERT_X(s_instance, "AppController", "create the instance in main() before loading QML");
    QJSEngine::setObjectOwnership(s_instance, QJSEngine::CppOwnership);
    return s_instance;
}

QString AppController::databasePath() const
{
    return QDir(m_dataDir).filePath(u"qupil.db"_s);
}

bool AppController::openDatabase(QString *error)
{
    const QString path = databasePath();
    if (m_db->open(path, error))
        return true;
    const int version = Database::schemaVersionOf(path);
    if (version > Qupil::Schema::Version) {
        //: %1 = data version number
        *error = qtTrId("db.newer_version").arg(version);
    } else {
        //: %1 = technical error message
        *error = qtTrId("setup.error.open").arg(*error);
    }
    return false;
}

void AppController::setPhase(Phase phase)
{
    if (m_phase == phase)
        return;
    m_phase = phase;
    emit phaseChanged();
    emit infoChanged();
}

void AppController::setBusy(bool busy)
{
    if (m_busy == busy)
        return;
    m_busy = busy;
    emit busyChanged();
}

void AppController::setProgress(int percent, const QString &step)
{
    m_progress = percent;
    m_progressStep = step;
    emit progressChanged();
}

void AppController::refreshLegacyFolders()
{
    QVariantList found;
    for (const QString &folder : LegacyImporter::defaultDataFolders()) {
        QString count;
        Database::detectFormat(LegacyImporter::findDatabase(folder), &count);
        found << QVariantMap { { u"path"_s, folder },
                               { u"url"_s, QUrl::fromLocalFile(folder) },
                               { u"students"_s, count.toInt() } };
    }
    if (found != m_legacyFolders) {
        m_legacyFolders = found;
        emit legacyFoldersChanged();
    }
}

bool AppController::desktop() const
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    return false;
#else
    return true;
#endif
}

bool AppController::saveDialogAvailable() const
{
#ifdef Q_OS_IOS
    return false;
#else
    return true;
#endif
}

QUrl AppController::backupFolder() const
{
    return QUrl::fromLocalFile(QDir(m_dataDir).filePath(u"backups"_s));
}

QString AppController::build() const
{
#ifdef QUPIL_BUILD
    return QStringLiteral(QUPIL_BUILD);
#else
    return u"dev"_s;
#endif
}

QString AppController::versionText() const
{
    return QCoreApplication::applicationVersion() + u" ("_s + build() + u')';
}

void AppController::copyText(const QString &text) const
{
    if (QClipboard *clipboard = QGuiApplication::clipboard())
        clipboard->setText(text);
}

QVariantMap AppController::info() const
{
    QVariantMap m;
    m.insert(u"databasePath"_s, QDir::toNativeSeparators(databasePath()));
    m.insert(u"backupFolder"_s, QDir::toNativeSeparators(backupFolder().toLocalFile()));
    if (!m_db->isOpen())
        return m;
    m.insert(u"deviceId"_s, m_db->deviceId());
    m.insert(u"importedFrom"_s, m_db->meta(u"imported_from"_s));
    m.insert(u"importedAt"_s, m_db->meta(u"imported_at"_s));
    m.insert(u"restoredFrom"_s, m_db->meta(u"restored_from"_s));
    m.insert(u"restoredAt"_s, m_db->meta(u"restored_at"_s));
    m.insert(u"lastAutoBackup"_s, m_db->meta(u"last_auto_backup"_s));
    m.insert(u"demo"_s, m_db->meta(u"demo"_s) == u"1"_s);
    m.insert(u"awaitingSync"_s, m_db->meta(u"awaiting_sync"_s) == u"1"_s);
    static const std::pair<const char *, const char *> counted[] = {
        { "students", "student" }, { "lessons", "lesson" }, { "events", "event" },
        { "pieces", "piece" }, { "notes", "note" },
    };
    QSqlQuery q(m_db->connection());
    for (const auto &[key, table] : counted) {
        if (q.exec(u"SELECT COUNT(*) FROM %1 WHERE deleted = 0"_s.arg(QLatin1StringView(table))) && q.next())
            m.insert(QLatin1StringView(key), q.value(0).toInt());
    }
    return m;
}

QString AppController::displayPath(const QUrl &url) const
{
    if (url.isLocalFile())
        return QDir::toNativeSeparators(url.toLocalFile());
    // content://…/document/primary%3ADownload%2Fqupil.db → qupil.db
    const QString last = QUrl::fromPercentEncoding(url.path().section(u'/', -1).toUtf8());
    return last.section(u'/', -1).section(u':', -1);
}

// ---------------------------------------------------------------------------- take over

struct AppController::Outcome
{
    bool ok = false;
    QString error;
    QString kind;          // "legacy" | "backup"
    QString source;        // for display
    QString newDatabase;   // ready file in the work folder
    QVariantMap summary;
};

void AppController::takeOver(const QUrl &source)
{
    if (m_busy)
        return;
    setBusy(true);
    setProgress(0, u"lookup"_s);
    const QString workDir = QDir(m_dataDir).filePath(u"work"_s);
    const QString from = openablePath(source);
    const QString shown = displayPath(source);

    QThread *thread = QThread::create([this, from, shown, workDir] {
        Outcome outcome = runTakeOver(this, from, workDir);
        if (outcome.source.isEmpty())
            outcome.source = shown;
        QMetaObject::invokeMethod(this, [this, outcome] { finishTakeOver(outcome); }, Qt::QueuedConnection);
    });
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

// Runs in a worker thread: everything here uses its own connections and files.
AppController::Outcome AppController::runTakeOver(AppController *self, const QString &source, const QString &workDir)
{
    Outcome o;
    QDir(workDir).removeRecursively();
    QDir().mkpath(workDir);
    const QString staged = QDir(workDir).filePath(u"source.db"_s);
    const QString target = QDir(workDir).filePath(u"import.db"_s);
    QString config;

    // 1. Copy the picked data, so the original is never touched or locked
    const QFileInfo info(source);
    QString file = source;
    if (info.isDir()) {
        file = LegacyImporter::findDatabase(source);
        if (file.isEmpty()) {
            //: %1 = folder
            o.error = qtTrId("setup.error.no_data").arg(QDir::toNativeSeparators(source));
            return o;
        }
        o.source = QDir::toNativeSeparators(source);
    } else if (info.isFile()) {
        o.source = QDir::toNativeSeparators(info.absoluteFilePath());
    }
    if (QFileInfo(file).isFile())
        config = LegacyImporter::findConfig(file);

    QString copyError;
    if (!copyFile(file, staged, &copyError)) {
        //: %1 = file, %2 = technical reason
        o.error = qtTrId("setup.error.read").arg(o.source.isEmpty() ? file : o.source, copyError);
        return o;
    }
    if (QFileInfo::exists(file + u"-wal"_s))
        copyFile(file + u"-wal"_s, staged + u"-wal"_s, &copyError);
    consolidate(staged);

    // 2. What is it?
    const int format = Database::detectFormat(staged);
    if (format == 0) {
        //: %1 = file
        o.error = qtTrId("setup.error.unknown").arg(o.source.isEmpty() ? file : o.source);
        return o;
    }

    removeDatabaseFiles(target);
    if (format == 1) {
        // Qupil 1.x: convert into a fresh Qupil 2 database
        o.kind = u"legacy"_s;
        Database db;
        QString error;
        if (!db.open(target, &error)) {
            o.error = qtTrId("setup.error.failed").arg(error);
            return o;
        }
        Store store(&db);
        LegacyImporter importer;
        QObject::connect(&importer, &LegacyImporter::progress, self, [self](int percent, const QString &step) {
            self->setProgress(percent, step);
        });   // queued: the importer lives in this worker thread, self in the GUI thread
        const LegacyImporter::Result r = importer.run(staged, &store, config);
        if (!r.ok) {
            db.close();
            o.error = qtTrId("setup.error.failed").arg(r.error);
            return o;
        }
        db.setMeta(u"imported_from"_s, o.source);
        o.summary = QVariantMap {
            { u"students"_s, r.students }, { u"archivedStudents"_s, r.archivedStudents },
            { u"lessons"_s, r.lessons }, { u"memberships"_s, r.memberships },
            { u"notes"_s, r.notes }, { u"pieces"_s, r.pieces }, { u"activities"_s, r.activities },
            { u"events"_s, r.events }, { u"archivedEvents"_s, r.archivedEvents },
            { u"libraryItems"_s, r.libraryItems }, { u"reminders"_s, r.reminders },
            { u"instruments"_s, r.instruments }, { u"skipped"_s, r.skipped },
            { u"unparsedDates"_s, r.unparsedDates }, { u"settingsFound"_s, !config.isEmpty() },
        };
        db.close();
    } else {
        // Qupil 2 backup: open (migrates older versions), new device id
        o.kind = u"backup"_s;
        const int version = Database::schemaVersionOf(staged);
        if (version > Qupil::Schema::Version) {
            o.error = qtTrId("db.newer_version").arg(version);
            return o;
        }
        if (!QFile::copy(staged, target)) {
            o.error = qtTrId("setup.error.read").arg(o.source, QString());
            return o;
        }
        Database db;
        QString error;
        if (!db.open(target, &error)) {
            o.error = qtTrId("setup.error.failed").arg(error);
            return o;
        }
        db.resetDeviceId();
        db.setMeta(u"restored_from"_s, o.source);
        db.setMeta(u"restored_at"_s, QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
        QSqlQuery q(db.connection());
        for (const char *table : { "student", "lesson", "event", "piece", "note" }) {
            if (q.exec(u"SELECT COUNT(*) FROM %1 WHERE deleted = 0"_s.arg(QLatin1StringView(table))) && q.next())
                o.summary.insert(QLatin1StringView(table) + u"s"_s, q.value(0).toInt());
        }
        db.close();
    }

    removeDatabaseFiles(staged);
    o.newDatabase = target;
    o.ok = true;
    return o;
}

void AppController::finishTakeOver(const Outcome &o)
{
    if (o.ok && m_syncRunning > 0) {
        // Swap the files only when the sync no longer writes to them
        m_afterSync = [this, o] { finishTakeOver(o); };
        return;
    }
    if (!o.ok) {
        m_demoPending = false;
        setBusy(false);
        QDir(QDir(m_dataDir).filePath(u"work"_s)).removeRecursively();
        emit takeOverFailed(o.error);
        return;
    }

    // Keep what was there before, then swap the files
    if (m_db->isOpen() && !m_db->isEmpty())
        safetyBackup(o.kind == u"legacy"_s ? u"before-import"_s : u"before-restore"_s);
    m_db->close();
    const QString live = databasePath();
    if (QFileInfo::exists(live) && m_phase == Setup && !m_startupError.isEmpty()) {
        // Unreadable data from before: keep it, never delete it
        QFile::rename(live, QDir(m_dataDir).filePath(u"backups/unreadable-%1.db"_s.arg(timestamp())));
    }
    removeDatabaseFiles(live);
    QString error;
    if (!renameWithRetry(o.newDatabase, live) || !openDatabase(&error)) {
        setBusy(false);
        emit takeOverFailed(error.isEmpty() ? qtTrId("setup.error.failed").arg(live) : error);
        return;
    }
    QDir(QDir(m_dataDir).filePath(u"work"_s)).removeRecursively();

    QVariantMap summary = o.summary;
    summary.insert(u"kind"_s, o.kind);
    summary.insert(u"source"_s, o.source);
    if (m_demoPending) {
        m_demoPending = false;
        m_db->setMeta(u"demo"_s, u"1"_s);
        m_db->setMeta(u"imported_from"_s, QString());
        m_db->setMeta(u"imported_at"_s, QString());
        QDir(QDir(m_dataDir).filePath(u"demo-source"_s)).removeRecursively();
        summary.insert(u"kind"_s, u"demo"_s);
        summary.insert(u"source"_s, QString());
    }
    m_lastTakeOver = summary;
    m_startupError.clear();
    setBusy(false);
    emit lastTakeOverChanged();
    emit dataReplaced();
    m_store->notifyReset();
    emit infoChanged();
    if (m_phase == Setup)
        emit phaseChanged();   // startupError cleared; phase switches on finishSetup()
}

void AppController::loadDemo()
{
    if (m_busy)
        return;
    const QString folder = QDir(m_dataDir).filePath(u"demo-source"_s);
    QDir(folder).removeRecursively();
    const QString language = LanguageManager::instance() ? LanguageManager::instance()->effectiveLanguage() : u"en"_s;
    QString error;
    if (!Qupil::DemoData::writeLegacyFolder(folder, language, &error)) {
        emit takeOverFailed(qtTrId("setup.error.failed").arg(error));
        return;
    }
    m_demoPending = true;
    takeOver(QUrl::fromLocalFile(folder));
}

bool AppController::startFresh()
{
    return startEmpty(false);
}

bool AppController::startForSync()
{
    return startEmpty(true);
}

bool AppController::startEmpty(bool awaitingSync)
{
    const QString live = databasePath();
    if (!m_db->isOpen() && QFileInfo(live).size() > 0) {
        // Unreadable data: keep it as a backup, never delete it
        QFile::rename(live, QDir(m_dataDir).filePath(u"backups/unreadable-%1.db"_s.arg(timestamp())));
        removeDatabaseFiles(live);
    }
    QString error;
    if (!m_db->isOpen() && !m_db->open(live, &error)) {
        emit takeOverFailed(qtTrId("setup.error.failed").arg(error));
        return false;
    }
    if (awaitingSync && m_db->isEmpty())
        m_db->setMeta(u"awaiting_sync"_s, u"1"_s);
    m_startupError.clear();
    m_lastTakeOver.clear();
    emit lastTakeOverChanged();
    setPhase(Ready);
    m_store->notifyReset();
    return true;
}

void AppController::finishSetup()
{
    if (!m_db->isOpen())
        return;
    m_lastTakeOver.clear();
    emit lastTakeOverChanged();
    setPhase(Ready);
    QTimer::singleShot(5000, this, &AppController::autoBackupIfDue);
}

void AppController::dismissTakeOver()
{
    if (m_lastTakeOver.isEmpty())
        return;
    m_lastTakeOver.clear();
    emit lastTakeOverChanged();
}

// ---------------------------------------------------------------------------- sync

void AppController::beginSync()
{
    ++m_syncRunning;
}

void AppController::endSync()
{
    m_syncRunning = std::max(0, m_syncRunning - 1);
    if (m_syncRunning == 0 && m_afterSync) {
        const auto pending = std::move(m_afterSync);
        m_afterSync = nullptr;
        pending();
    }
}

bool AppController::replaceWithEmpty()
{
    if (m_busy || m_syncRunning > 0)
        return false;
    if (m_db->isOpen() && !m_db->isEmpty())
        safetyBackup(u"before-sync"_s);
    m_db->close();
    const QString live = databasePath();
    removeDatabaseFiles(live);
    QString error;
    if (!openDatabase(&error)) {
        emit takeOverFailed(error);
        return false;
    }
    m_db->setMeta(u"awaiting_sync"_s, u"1"_s);
    m_lastTakeOver.clear();
    emit lastTakeOverChanged();
    setPhase(Ready);
    emit dataReplaced();
    m_store->notifyReset();
    emit infoChanged();
    return true;
}

bool AppController::awaitingSync() const
{
    return m_db->isOpen() && m_db->meta(u"awaiting_sync"_s) == u"1"_s;
}

void AppController::syncArrived()
{
    if (!awaitingSync())
        return;
    m_db->setMeta(u"awaiting_sync"_s, QString());
    emit infoChanged();
}

// ---------------------------------------------------------------------------- backups

QString AppController::suggestedBackupName() const
{
    return u"Qupil-Backup-%1.db"_s.arg(QDate::currentDate().toString(Qt::ISODate));
}

QVariantMap AppController::createBackup(const QUrl &target)
{
    QVariantMap result { { u"ok"_s, false } };
    if (!m_db->isOpen())
        return result;

    QString destination;
    if (target.isEmpty()) {
        const QString docs = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
        QDir().mkpath(docs);
        destination = QDir(docs).filePath(suggestedBackupName());
    } else {
        destination = openablePath(target);
    }

    // VACUUM INTO needs a real file; content:// targets get a copy of it
    const QString temp = QDir(m_dataDir).filePath(u"backup-%1.tmp"_s.arg(timestamp()));
    QString error;
    bool ok = m_db->backupTo(temp, &error);
    if (ok) {
        if (QFileInfo(destination).isAbsolute() && !destination.contains(u"://"_s)) {
            QFile::remove(destination);
            ok = QFile::rename(temp, destination) || copyFile(temp, destination, &error);
        } else {
            ok = copyFile(temp, destination, &error);
        }
    }
    QFile::remove(temp);

    const QString shown = target.isEmpty() ? QDir::toNativeSeparators(destination) : displayPath(target);
    result.insert(u"ok"_s, ok);
    result.insert(u"file"_s, shown);
    if (ok) {
        m_db->setMeta(u"last_backup"_s, QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
        //: %1 = file name
        result.insert(u"message"_s, qtTrId("backup.create.success").arg(shown));
        emit infoChanged();
    } else {
        //: %1 = folder
        result.insert(u"message"_s, qtTrId("backup.create.failed").arg(QFileInfo(shown).absolutePath())
                                        + u"\n\n"_s + error);
    }
    return result;
}

bool AppController::safetyBackup(const QString &reason)
{
    const QString file = QDir(m_dataDir).filePath(u"backups/%1-%2.db"_s.arg(reason, timestamp()));
    const bool ok = m_db->backupTo(file);
    rotate(reason + u"-"_s, kKeepSafetyBackups);
    return ok;
}

bool AppController::autoBackupIfDue()
{
    if (!m_db->isOpen() || m_db->isEmpty())
        return false;
    const QDateTime last = QDateTime::fromString(m_db->meta(u"last_auto_backup"_s), Qt::ISODate);
    if (last.isValid() && last.secsTo(QDateTime::currentDateTimeUtc()) < 20 * 3600)
        return false;
    const QString file = QDir(m_dataDir).filePath(
        u"backups/auto-%1.db"_s.arg(QDate::currentDate().toString(Qt::ISODate)));
    QFile::remove(file);
    if (!m_db->backupTo(file))
        return false;
    m_db->setMeta(u"last_auto_backup"_s, QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    rotate(u"auto-"_s, kKeepAutoBackups);
    emit infoChanged();
    return true;
}

void AppController::rotate(const QString &prefix, int keep)
{
    QDir dir(QDir(m_dataDir).filePath(u"backups"_s));
    QStringList files = dir.entryList({ prefix + u"*.db"_s }, QDir::Files, QDir::Name);
    while (files.size() > keep)
        dir.remove(files.takeFirst());   // names sort by date: oldest first
}
