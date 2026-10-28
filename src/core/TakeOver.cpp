// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "TakeOver.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QUuid>

#include "Database.h"
#include "LegacyImporter.h"
#include "Schema.h"
#include "Store.h"

using namespace Qt::StringLiterals;

namespace Qupil::TakeOver {

namespace {

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

} // namespace

bool copyFile(const QString &from, const QString &to, QString *error)
{
    QFile in(from);
    if (!in.open(QIODevice::ReadOnly)) {
        *error = in.errorString();
        return false;
    }
    // Not for content:// URIs (Android): removing deletes the document the save
    // dialog has just created, and it cannot be opened for writing any more
    if (!to.contains(u"://"_s))
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

void removeDatabaseFiles(const QString &path)
{
    for (const char *suffix : { "", "-wal", "-shm", "-journal" })
        QFile::remove(path + QLatin1StringView(suffix));
}

QString timestamp()
{
    return QDateTime::currentDateTime().toString(u"yyyy-MM-dd-HHmmss"_s);
}

Outcome run(const QString &source, const QString &workDir, const QString &backupDir,
            const std::function<void(int, const QString &)> &progress, const QString &shownAs)
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
    if (!shownAs.isEmpty())
        o.source = shownAs;
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
        if (progress)
            QObject::connect(&importer, &LegacyImporter::progress, &importer, progress);
        const LegacyImporter::Result r = importer.run(staged, &store, config);
        if (!r.ok) {
            db.close();
            o.error = qtTrId("setup.error.failed").arg(r.error);
            return o;
        }
        db.setMeta(u"imported_from"_s, o.source);
        // The 1.x database as it was taken over, next to Qupil 2's own backups:
        // the original stays untouched, but may be cleared away one day
        if (!backupDir.isEmpty()) {
            QDir().mkpath(backupDir);
            const QString copy = QDir(backupDir).filePath(u"qupil1-%1.db"_s.arg(timestamp()));
            QString copyFailed;
            if (copyFile(staged, copy, &copyFailed)) {
                o.legacyCopy = copy;
                db.setMeta(u"imported_copy"_s, QFileInfo(copy).fileName());
            }
        }
        o.summary = QVariantMap {
            { u"students"_s, r.students }, { u"archivedStudents"_s, r.archivedStudents },
            { u"lessons"_s, r.lessons }, { u"memberships"_s, r.memberships },
            { u"notes"_s, r.notes }, { u"pieces"_s, r.pieces }, { u"activities"_s, r.activities },
            { u"events"_s, r.events }, { u"archivedEvents"_s, r.archivedEvents },
            { u"libraryItems"_s, r.libraryItems }, { u"reminders"_s, r.reminders },
            { u"instruments"_s, r.instruments }, { u"skipped"_s, r.skipped },
            // What the skipped records were (Qupil 1.x no longer showed them either)
            { u"skippedDetail"_s, QVariantMap { { u"activities"_s, r.skippedActivities },
                                                { u"programme"_s, r.skippedProgramme },
                                                { u"notes"_s, r.skippedNotes },
                                                { u"pieces"_s, r.skippedPieces },
                                                { u"memberships"_s, r.skippedMemberships } } },
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
        // An older state of data that other devices may have changed since: ask before joining them
        db.setMeta(u"ask_before_joining"_s, u"1"_s);
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

} // namespace Qupil::TakeOver
