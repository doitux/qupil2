// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "AppController.h"

#include <QClipboard>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QDateTime>
#include <QDate>
#include <QDir>
#include <QHash>
#include <QSqlError>
#include <QFile>
#include <QFileInfo>
#include <QJSEngine>
#include <QQmlEngine>
#include <QRegularExpression>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QThread>
#include <QTimer>
#include <QUuid>

#include <algorithm>
#include <tuple>

#include "Database.h"
#include "DemoData.h"
#include "FileExport.h"
#include "FileImport.h"
#include "LanguageManager.h"
#include "LegacyImporter.h"
#include "NoteFormat.h"
#include "Schema.h"
#include "Store.h"
#include "TakeOver.h"
#include "TextPack.h"

using namespace Qt::StringLiterals;
using Qupil::Database;
using Qupil::LegacyImporter;
using Qupil::Store;
using Qupil::TakeOver::copyFile;
using Qupil::TakeOver::removeDatabaseFiles;
using Qupil::TakeOver::timestamp;

namespace {

AppController *s_instance = nullptr;

constexpr int kKeepAutoBackups = 14;
constexpr int kKeepSafetyBackups = 10;
constexpr int kKeepLegacyCopies = 5;

//! Path QFile can open: local files, Android content:// URIs, plain paths
QString openablePath(const QUrl &url)
{
    if (url.isLocalFile())
        return url.toLocalFile();
    if (url.scheme().isEmpty())
        return url.path();
    return url.toString();
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
    // An older layout is updated when opened: keep a copy of it first
    const QString backup = QDir(m_dataDir).filePath(u"backups/before-update-%1.db"_s.arg(timestamp()));
    QDir().mkpath(QFileInfo(backup).absolutePath());
    m_db->setMigrationBackup(backup);
    const bool opened = m_db->open(path, error);
    m_db->setMigrationBackup({});
    if (QFileInfo::exists(backup))
        rotate(u"before-update-"_s, kKeepSafetyBackups);
    if (opened) {
        // Compact notes and archives of older versions, then give the space back
        if (upgradeNotes() + upgradeArchives() > 0)
            QSqlQuery(m_db->connection()).exec(u"VACUUM"_s);
        return true;
    }
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

/*!
    Lesson notes of Qupil 1.x and of earlier test builds are whole HTML documents.
    They are converted once to the compact format (NoteFormat), in the notes and
    in the change log, and the file is shrunk. Notes that come later from a
    device with an older version are converted the next time.
*/
int AppController::upgradeNotes()
{
    QSqlDatabase db = m_db->connection();
    QHash<QString, QString> converted;   // copies of group notes and log entries are the same
    const auto convert = [&converted](const QString &html) {
        auto it = converted.constFind(html);
        if (it == converted.constEnd())
            it = converted.insert(html, NoteFormat::fromHtml(html));
        return *it;
    };
    QList<QPair<QString, QString>> notes;   // id, content
    QList<std::tuple<int, qint64, QString>> log;   // origin, seq, data
    {
        QSqlQuery q(db);
        q.exec(u"SELECT id, content FROM note WHERE content LIKE '%<%'"_s);
        while (q.next()) {
            const QString content = q.value(1).toString();
            if (NoteFormat::isHtml(content))
                notes.append({ q.value(0).toString(), convert(content) });
        }
        q.exec(u"SELECT origin, seq, data FROM change_log WHERE entity = 'note' AND data LIKE '%<%'"_s);
        while (q.next()) {
            QVariantMap fields = Qupil::Store::decodeFields(q.value(2).toString());
            const QVariant content = fields.value(u"content"_s);
            if (content.typeId() == QMetaType::QString && NoteFormat::isHtml(content.toString())) {
                fields.insert(u"content"_s, convert(content.toString()));
                log.append({ q.value(0).toInt(), q.value(1).toLongLong(), Qupil::Store::encodeFields(fields) });
            }
        }
    }
    if (notes.isEmpty() && log.isEmpty())
        return 0;

    safetyBackup(u"before-notes"_s);
    m_db->transaction();
    bool ok = true;
    QSqlQuery note(db);
    note.prepare(u"UPDATE note SET content = ? WHERE id = ?"_s);
    for (const auto &[id, content] : std::as_const(notes)) {
        note.addBindValue(content);
        note.addBindValue(id);
        ok = ok && note.exec();
    }
    QSqlQuery entry(db);
    entry.prepare(u"UPDATE change_log SET data = ? WHERE origin = ? AND seq = ?"_s);
    for (const auto &[origin, seq, value] : std::as_const(log)) {
        entry.addBindValue(value);
        entry.addBindValue(origin);
        entry.addBindValue(seq);
        ok = ok && entry.exec();
    }
    if (!ok) {
        qWarning("Could not convert the lesson notes: %s", qPrintable(db.lastError().text()));
        m_db->rollback();
        return 0;
    }
    m_db->commit();
    return int(notes.size());
}

/*!
    Archive entries are stored packed (TextPack): in the tables and in the change
    log. Entries of Qupil 1.x and of earlier test builds are packed once here; the
    compact layout follows in the background (DocumentService::convertLegacyArchives).
*/
int AppController::upgradeArchives()
{
    QSqlDatabase db = m_db->connection();
    QList<QPair<QString, QString>> rows;   // entity, id
    QList<QPair<int, qint64>> log;         // origin, seq
    {
        QSqlQuery q(db);
        for (const QString &entity : { u"student_archive"_s, u"event_archive"_s }) {
            q.exec(u"SELECT id FROM %1 WHERE content <> '' AND content NOT LIKE 'qz1:%'"_s.arg(entity));
            while (q.next())
                rows.append({ entity, q.value(0).toString() });
        }
        // Candidates without anything packed; then exactly (whatever the JSON looks like)
        QList<QPair<int, qint64>> candidates;
        q.exec(u"SELECT origin, seq FROM change_log WHERE entity IN ('student_archive', 'event_archive') "
               u"AND data LIKE '%\"content\"%' AND data NOT LIKE '%qz1:%'"_s);
        while (q.next())
            candidates.append({ q.value(0).toInt(), q.value(1).toLongLong() });
        q.prepare(u"SELECT data FROM change_log WHERE origin = ? AND seq = ?"_s);
        for (const auto &entry : std::as_const(candidates)) {
            q.addBindValue(entry.first);
            q.addBindValue(entry.second);
            if (!q.exec() || !q.next())
                continue;
            const QString content = Qupil::Store::decodeFields(q.value(0).toString()).value(u"content"_s).toString();
            if (!content.isEmpty() && !TextPack::isPacked(content))
                log.append(entry);
        }
    }
    if (rows.isEmpty() && log.isEmpty())
        return 0;

    safetyBackup(u"before-archives"_s);
    m_db->transaction();
    bool ok = true;
    // One at a time: the entries can be large
    QSqlQuery read(db);
    QSqlQuery write(db);
    for (const auto &[entity, id] : std::as_const(rows)) {
        read.prepare(u"SELECT content FROM %1 WHERE id = ?"_s.arg(entity));
        read.addBindValue(id);
        if (!read.exec() || !read.next())
            continue;
        const QString packed = TextPack::pack(read.value(0).toString());
        read.finish();
        write.prepare(u"UPDATE %1 SET content = ? WHERE id = ?"_s.arg(entity));
        write.addBindValue(packed);
        write.addBindValue(id);
        ok = ok && write.exec();
    }
    read.prepare(u"SELECT data FROM change_log WHERE origin = ? AND seq = ?"_s);
    write.prepare(u"UPDATE change_log SET data = ? WHERE origin = ? AND seq = ?"_s);
    for (const auto &[origin, seq] : std::as_const(log)) {
        read.addBindValue(origin);
        read.addBindValue(seq);
        if (!read.exec() || !read.next())
            continue;
        QVariantMap fields = Qupil::Store::decodeFields(read.value(0).toString());
        read.finish();
        fields.insert(u"content"_s, TextPack::pack(fields.value(u"content"_s).toString()));
        write.addBindValue(Qupil::Store::encodeFields(fields));
        write.addBindValue(origin);
        write.addBindValue(seq);
        ok = ok && write.exec();
    }
    if (!ok) {
        qWarning("Could not pack the archive entries: %s", qPrintable(db.lastError().text()));
        m_db->rollback();
        return 0;
    }
    m_db->commit();
    return int(rows.size() + log.size());
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

bool AppController::exportPickerAvailable() const
{
    return FileExport::available();
}

bool AppController::importPickerAvailable() const
{
    return FileImport::available();
}

namespace {
//! Where picked files are copied to (iOS); emptied with every pick and after a take-over
QString importFolder()
{
    return QDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation)).filePath(u"import"_s);
}
} // namespace

void AppController::pickFile(const QString &purpose)
{
    FileImport::pickFile(importFolder(), [this, purpose](const QString &path, const QString &name, const QString &error) {
        if (!error.isEmpty()) {
            //: %1 = file, %2 = technical reason
            emit filePicked(purpose, {}, qtTrId("setup.error.read").arg(name, error));
            return;
        }
        // Cancelled: an empty file, so the caller stops waiting
        emit filePicked(purpose, path.isEmpty() ? QUrl() : QUrl::fromLocalFile(path), QString());
    });
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

QString AppController::licenseText() const
{
    // The texts come with line breaks for 80 columns: paragraphs flow on the screen
    const auto read = [](const QString &file) {
        QFile f(u":/qt/qml/Qupil/"_s + file);
        if (!f.open(QIODevice::ReadOnly))
            return QString();
        static const QRegularExpression blankLine(u"\\n\\s*\\n"_s);
        static const QRegularExpression underline(u"^[-=]{3,}$"_s);
        QStringList paragraphs;
        for (const QString &block : QString::fromUtf8(f.readAll()).split(blankLine)) {
            QStringList lines;
            for (const QString &line : block.split(u'\n')) {
                const QString trimmed = line.trimmed();
                if (!trimmed.isEmpty() && !underline.match(trimmed).hasMatch())
                    lines << trimmed;
            }
            if (!lines.isEmpty())
                paragraphs << lines.join(u' ');
        }
        return paragraphs.join(u"\n\n"_s);
    };
    const QString rule(20, u'─');
    return u"Qupil %1\n© 2006–%2 Felix Hammer · https://qupil.de\n"
           u"GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception\n\n"_s
               .arg(versionText())
               .arg(QDate::currentDate().year())
        + read(u"LICENSES/LicenseRef-Qupil-AppStore-exception.txt"_s) + u"\n\n"_s + rule + u"\n\n"_s
        + read(u"LICENSE"_s) + u"\n\n"_s + rule
        + u"\n\nFigtree font: © The Figtree Project Authors\n\n"_s + read(u"LICENSES/OFL-1.1.txt"_s)
        + u"\n\n"_s + rule + u"\n\nMonocypher: © 2017–2020 Loup Vaillant\n\n"_s
        + read(u"3rdparty/monocypher/LICENCE.md"_s) + u"\n\n"_s + rule
        + u"\n\nQt: © The Qt Company Ltd. and other contributors, used under the GNU Lesser General "
          u"Public License version 3 (https://www.gnu.org/licenses/lgpl-3.0.html). "
          u"Source code: https://code.qt.io\n"_s
#ifdef Q_OS_ANDROID
        + u"\n"_s + rule + u"\n\nOpenSSL: © The OpenSSL Project Authors, https://www.openssl.org "
                             u"(built by KDAB, https://github.com/KDAB/android_openssl)\n\n"_s
        + read(u"LICENSES/Apache-2.0.txt"_s) + u"\n"_s
#endif
        ;
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
#ifdef Q_OS_ANDROID
    // The name the document provider shows (also for content://…/document/msf%3A123)
    if (const QString name = QFileInfo(url.toString()).fileName(); !name.isEmpty() && !name.contains(u'/'))
        return name;
#endif
    // content://…/document/primary%3ADownload%2Fqupil.db → qupil.db
    const QString last = QUrl::fromPercentEncoding(url.path().section(u'/', -1).toUtf8());
    return last.section(u'/', -1).section(u':', -1);
}

QString AppController::displayFolder(const QUrl &url) const
{
    if (url.isLocalFile())
        return QDir::toNativeSeparators(QFileInfo(url.toLocalFile()).absolutePath());
    // content://…/document/primary%3ADownload%2Fqupil.db → Download
    const QString document = QUrl::fromPercentEncoding(url.path().section(u'/', -1).toUtf8()).section(u':', -1);
    return document.contains(u'/') ? document.section(u'/', 0, -2) : displayPath(url);
}

// ---------------------------------------------------------------------------- take over

void AppController::takeOver(const QUrl &source)
{
    if (m_busy)
        return;
    setBusy(true);
    setProgress(0, u"lookup"_s);
    const QString workDir = QDir(m_dataDir).filePath(u"work"_s);
    // The demo data needs no copy of its 1.x database
    const QString backupDir = m_demoPending ? QString() : QDir(m_dataDir).filePath(u"backups"_s);
    const QString from = openablePath(source);
    const QString shown = displayPath(source);
    // A copy the iOS picker brought: name the file, not the temporary folder
    const QString shownAs = QFileInfo(from).absolutePath() == QFileInfo(importFolder()).absoluteFilePath()
                                ? QFileInfo(from).fileName() : QString();

    QThread *thread = QThread::create([this, from, shown, shownAs, workDir, backupDir] {
        const auto progress = [this](int percent, const QString &step) {
            QMetaObject::invokeMethod(this, [this, percent, step] { setProgress(percent, step); }, Qt::QueuedConnection);
        };
        Qupil::TakeOver::Outcome outcome = Qupil::TakeOver::run(from, workDir, backupDir, progress, shownAs);
        if (outcome.source.isEmpty())
            outcome.source = shown;
        QMetaObject::invokeMethod(this, [this, outcome] { finishTakeOver(outcome); }, Qt::QueuedConnection);
    });
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

void AppController::finishTakeOver(const Qupil::TakeOver::Outcome &o)
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
        QDir(importFolder()).removeRecursively();
        emit takeOverFailed(o.error);
        return;
    }

    // Keep what was there before, then swap the files
    if (m_db->isOpen() && !m_db->isEmpty())
        safetyBackup(o.kind == u"legacy"_s ? u"before-import"_s : u"before-restore"_s);
    if (o.kind == u"legacy"_s)
        rotate(u"qupil1-"_s, kKeepLegacyCopies);
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
    QDir(importFolder()).removeRecursively();   // a picked copy (iOS)

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

QStringList AppController::syncHistory() const
{
    if (!m_db->isOpen() || !m_db->meta(u"ask_before_joining"_s).isEmpty())
        return {};
    return m_store->versionVector().keys();
}

void AppController::joinDecided()
{
    if (m_db->isOpen() && !m_db->meta(u"ask_before_joining"_s).isEmpty())
        m_db->setMeta(u"ask_before_joining"_s, QString());
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
        const QString folder = target.isEmpty() ? QFileInfo(destination).absolutePath() : displayFolder(target);
        result.insert(u"message"_s, qtTrId("backup.create.failed").arg(QDir::toNativeSeparators(folder))
                                        + u"\n\n"_s + error);
    }
    return result;
}

void AppController::exportBackup()
{
    if (!m_db->isOpen())
        return;
    // A copy under its final name: the picker shows and keeps that name
    const QString dir = QDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation)).filePath(u"export"_s);
    QDir(dir).removeRecursively();
    QDir().mkpath(dir);
    const QString file = QDir(dir).filePath(suggestedBackupName());
    QString error;
    if (!m_db->backupTo(file, &error)) {
        QFile::remove(file);
        emit backupExported({ { u"ok"_s, false },
                              { u"message"_s, qtTrId("backup.create.failed").arg(QDir::toNativeSeparators(dir))
                                                  + u"\n\n"_s + error } });
        return;
    }
    FileExport::exportFile(file, [this, file](bool saved, const QString &name) {
        QFile::remove(file);
        if (!saved) {
            emit backupExported({ { u"ok"_s, false }, { u"cancelled"_s, true } });
            return;
        }
        m_db->setMeta(u"last_backup"_s, QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
        emit infoChanged();
        //: %1 = file name
        emit backupExported({ { u"ok"_s, true },
                              { u"message"_s, qtTrId("backup.create.success").arg(name.isEmpty() ? QFileInfo(file).fileName()
                                                                                                  : name) } });
    });
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
