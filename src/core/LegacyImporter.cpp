// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "LegacyImporter.h"

#include <QDate>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QStandardPaths>
#include <QTime>
#include <QUuid>
#include <QXmlStreamReader>

#include "Database.h"
#include "Ids.h"
#include "Store.h"

using namespace Qt::StringLiterals;

namespace Qupil {

namespace {

using Row = QVariantMap;

QString str(const Row &r, const char *col)
{
    return r.value(QString::fromLatin1(col)).toString().trimmed();
}

int num(const Row &r, const char *col, int fallback = 0)
{
    const QVariant v = r.value(QString::fromLatin1(col));
    bool ok = false;
    const int i = v.toInt(&ok);
    return ok ? i : fallback;
}

qint64 key(const Row &r, const char *col)
{
    return r.value(QString::fromLatin1(col)).toLongLong();
}

//! ISO date or empty. 1.x uses '9999-99-99' for "open end".
QString date(const QString &raw, int *unparsed = nullptr)
{
    const QString s = raw.trimmed();
    if (s.isEmpty() || s.startsWith(u"9999"_s) || s.startsWith(u"0000"_s))
        return {};
    QDate d = QDate::fromString(s, Qt::ISODate);
    if (!d.isValid())
        d = QDate::fromString(s, u"dd.MM.yyyy"_s);
    if (!d.isValid())
        d = QDate::fromString(s, u"d.M.yyyy"_s);
    if (d.isValid())
        return d.toString(Qt::ISODate);
    if (unparsed)
        ++*unparsed;
    return s;   // keep what the user typed rather than losing it
}

QString time(const QString &raw)
{
    const QString s = raw.trimmed();
    QTime t = QTime::fromString(s, u"hh:mm"_s);
    if (!t.isValid())
        t = QTime::fromString(s, u"h:mm"_s);
    if (!t.isValid())
        t = QTime::fromString(s, u"hh:mm:ss"_s);
    return t.isValid() ? t.toString(u"hh:mm"_s) : QString();
}

class LegacyDb
{
public:
    explicit LegacyDb(const QString &file)
        : m_name(u"qupil-legacy-"_s + QUuid::createUuid().toString(QUuid::WithoutBraces))
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(u"QSQLITE"_s, m_name);
        db.setDatabaseName(file);
        db.setConnectOptions(u"QSQLITE_OPEN_READONLY"_s);
        m_ok = db.open();
        if (!m_ok)
            m_error = db.lastError().text();
        else
            m_tables = db.tables();
    }
    ~LegacyDb()
    {
        {
            QSqlDatabase db = QSqlDatabase::database(m_name, false);
            if (db.isOpen())
                db.close();
        }
        QSqlDatabase::removeDatabase(m_name);
    }

    bool ok() const { return m_ok; }
    QString error() const { return m_error; }
    bool hasTable(const QString &t) const { return m_tables.contains(t); }

    QList<Row> rows(const QString &table, const QString &order = {}) const
    {
        QList<Row> out;
        if (!hasTable(table))
            return out;
        QSqlQuery q(QSqlDatabase::database(m_name, false));
        if (!q.exec(u"SELECT * FROM %1%2"_s.arg(table, order.isEmpty() ? QString() : u" ORDER BY "_s + order)))
            return out;
        while (q.next()) {
            Row r;
            const QSqlRecord rec = q.record();
            for (int i = 0; i < rec.count(); ++i)
                r.insert(rec.fieldName(i).toLower(), q.value(i));
            out << r;
        }
        return out;
    }

    int revision() const
    {
        if (!hasTable(u"dbinfos"_s))
            return 1;
        QSqlQuery q(QSqlDatabase::database(m_name, false));
        if (q.exec(u"SELECT data_structure_rev FROM dbinfos"_s) && q.next())
            return q.value(0).toInt();
        return 2;
    }

private:
    QString m_name;
    bool m_ok = false;
    QString m_error;
    QStringList m_tables;
};

struct LegacyConfig
{
    QHash<QString, QString> values;
    QHash<QString, QStringList> lists;
};

LegacyConfig readConfig(const QString &file)
{
    LegacyConfig cfg;
    QFile f(file);
    if (!f.open(QIODevice::ReadOnly))
        return cfg;
    QXmlStreamReader xml(&f);
    int depth = 0;
    QString current;
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.isStartElement()) {
            ++depth;
            // <Qupil><Configuration><Key value="…" [type="list"]><Item value="…"/>…</Key>
            if (depth == 3) {
                current = xml.name().toString();
                cfg.values.insert(current, xml.attributes().value(u"value"_s).toString());
            } else if (depth == 4 && !current.isEmpty()) {
                cfg.lists[current] << xml.attributes().value(u"value"_s).toString();
            }
        } else if (xml.isEndElement()) {
            if (depth == 3)
                current.clear();
            --depth;
        }
    }
    return cfg;
}

QString jsonList(const QStringList &list)
{
    return QString::fromUtf8(QJsonDocument(QJsonArray::fromStringList(list)).toJson(QJsonDocument::Compact));
}

} // namespace

LegacyImporter::LegacyImporter(QObject *parent)
    : QObject(parent)
{
}

QString LegacyImporter::findDatabase(const QString &fileOrFolder)
{
    const QFileInfo info(fileOrFolder);
    if (info.isFile())
        return Database::detectFormat(info.absoluteFilePath()) == 1 ? info.absoluteFilePath() : QString();
    if (info.isDir()) {
        const QDir dir(info.absoluteFilePath());
        for (const QString &candidate : { u"db/qupil.db"_s, u"qupil.db"_s, u".qupil/db/qupil.db"_s, u"qupil/db/qupil.db"_s }) {
            const QString path = dir.filePath(candidate);
            if (QFileInfo(path).isFile() && Database::detectFormat(path) == 1)
                return QFileInfo(path).absoluteFilePath();
        }
    }
    return {};
}

QString LegacyImporter::findConfig(const QString &fileOrFolder)
{
    QFileInfo info(fileOrFolder);
    QDir dir = info.isDir() ? QDir(info.absoluteFilePath()) : info.absoluteDir();
    for (int up = 0; up < 3; ++up) {
        if (dir.exists(u"config.xml"_s))
            return dir.filePath(u"config.xml"_s);
        if (!dir.cdUp())
            break;
    }
    return {};
}

QStringList LegacyImporter::defaultDataFolders()
{
    QStringList candidates;
#ifdef Q_OS_WIN
    const QString appData = qEnvironmentVariable("APPDATA");
    if (!appData.isEmpty())
        candidates << QDir(appData).filePath(u"qupil"_s);
#else
    candidates << QDir::home().filePath(u".qupil"_s);
#endif
    QStringList existing;
    for (const QString &c : candidates)
        if (!findDatabase(c).isEmpty())
            existing << QDir::toNativeSeparators(c);
    return existing;
}

LegacyImporter::Result LegacyImporter::run(const QString &legacyDatabase, Store *target, const QString &legacyConfig)
{
    Result r;
    LegacyDb old(legacyDatabase);
    if (!old.ok()) {
        r.error = old.error();
        return r;
    }
    if (!old.hasTable(u"pupil"_s) || !old.hasTable(u"lesson"_s)) {
        r.error = u"not a Qupil 1.x database"_s;
        return r;
    }

    int unparsedDates = 0;
    auto d = [&unparsedDates](const QString &s) { return date(s, &unparsedDates); };

    target->beginBatch();
    bool ok = true;
    auto create = [&](const QString &entity, const QVariantMap &fields) {
        if (!ok)
            return QString();
        const QString id = target->create(entity, fields);
        if (id.isEmpty()) {
            ok = false;
            r.error = target->lastError();
        }
        return id;
    };

    // ---------------------------------------------------------------- lookup tables
    emit progress(2, u"lookup"_s);
    QHash<qint64, QString> composers, authors, publishers, lastNames;
    for (const Row &row : old.rows(u"piececomposer"_s))
        composers.insert(key(row, "piececomposerid"), str(row, "composer"));
    for (const Row &row : old.rows(u"smlauthor"_s))
        authors.insert(key(row, "smlauthorid"), str(row, "author"));
    for (const Row &row : old.rows(u"smlpublisher"_s))
        publishers.insert(key(row, "smlpublisherid"), str(row, "publisher"));
    for (const Row &row : old.rows(u"lastlessonname"_s))
        lastNames.insert(key(row, "llnid"), str(row, "lessonname"));

    // ---------------------------------------------------------------- students
    emit progress(8, u"students"_s);
    QHash<qint64, QString> students;
    for (const Row &row : old.rows(u"pupil"_s, u"pupilid"_s)) {
        const QString id = create(u"student"_s, {
            { u"surname"_s, str(row, "surname") },
            { u"forename"_s, str(row, "forename") },
            { u"address"_s, str(row, "address") },
            { u"email"_s, str(row, "email") },
            { u"phone"_s, str(row, "telefon") },
            { u"mobile"_s, str(row, "handy") },
            { u"birthday"_s, d(str(row, "birthday")) },
            { u"notes"_s, row.value(u"notes"_s).toString() },
            { u"father_name"_s, str(row, "fathername") },
            { u"father_job"_s, str(row, "fatherjob") },
            { u"father_phone"_s, str(row, "fathertelefon") },
            { u"mother_name"_s, str(row, "mothername") },
            { u"mother_job"_s, str(row, "motherjob") },
            { u"mother_phone"_s, str(row, "mothertelefon") },
            { u"first_lesson"_s, d(str(row, "firstlessondate")) },
            { u"instrument"_s, str(row, "instrumenttype") },
            { u"instrument_size"_s, str(row, "instrumentsize") },
            { u"next_size"_s, num(row, "ifinstrumentnextsize") },
            { u"rental"_s, num(row, "ifrentinstrument") },
            { u"rental_desc"_s, str(row, "rentinstrumentdesc") },
            { u"rental_since"_s, d(str(row, "rentinstrumentstartdate")) },
            { u"recital_interval"_s, num(row, "recitalinterval", 5) },
            { u"ensemble_expected"_s, num(row, "ensembleactivityrequested", 1) },
        });
        students.insert(key(row, "pupilid"), id);
        ++r.students;
    }

    emit progress(15, u"archive"_s);
    for (const Row &row : old.rows(u"pupilarchive"_s, u"pupilid"_s)) {
        create(u"student_archive"_s, {
            { u"surname"_s, str(row, "surname") },
            { u"forename"_s, str(row, "forename") },
            { u"content"_s, row.value(u"data"_s).toString() },
        });
        ++r.archivedStudents;
    }

    // ---------------------------------------------------------------- lessons
    emit progress(20, u"lessons"_s);
    QHash<qint64, QString> lessons;
    for (const Row &row : old.rows(u"lesson"_s, u"lessonid"_s)) {
        const QString id = create(u"lesson"_s, {
            { u"active"_s, num(row, "state", 1) },
            { u"kind"_s, num(row, "type", 1) },
            { u"auto_name"_s, num(row, "autolessonname", 0) },
            { u"name"_s, str(row, "lessonname") },
            { u"irregular"_s, num(row, "unsteadylesson", 0) },
            { u"weekday"_s, num(row, "lessonday", 0) },
            { u"start_time"_s, time(str(row, "lessonstarttime")) },
            { u"end_time"_s, time(str(row, "lessonstoptime")) },
            { u"location"_s, str(row, "lessonlocation") },
        });
        lessons.insert(key(row, "lessonid"), id);
        ++r.lessons;
    }

    QHash<qint64, QString> memberships;
    int orphans = 0;
    for (const Row &row : old.rows(u"pupilatlesson"_s, u"palid"_s)) {
        const QString lesson = lessons.value(key(row, "lessonid"));
        const QString student = students.value(key(row, "pupilid"));
        if (lesson.isEmpty() || student.isEmpty()) {
            ++orphans;
            continue;
        }
        const QString id = create(u"membership"_s, {
            { u"lesson_id"_s, lesson },
            { u"student_id"_s, student },
            { u"start_date"_s, d(str(row, "startdate")) },
            { u"end_date"_s, d(str(row, "stopdate")) },
            { u"previous_name"_s, lastNames.value(key(row, "llnid")) },
        });
        memberships.insert(key(row, "palid"), id);
        ++r.memberships;
    }

    // ---------------------------------------------------------------- pieces and notes
    emit progress(35, u"pieces"_s);
    QHash<qint64, QString> pieces, pieceGroups;
    QHash<qint64, QString> pieceByCommon;   // cpieceid -> first imported copy (programmes refer to cpieceid)
    for (const Row &row : old.rows(u"piece"_s, u"pieceid"_s)) {
        const QString membership = memberships.value(key(row, "palid"));
        if (membership.isEmpty()) {
            ++orphans;
            continue;
        }
        const qint64 common = key(row, "cpieceid");
        QString group = common > 0 ? pieceGroups.value(common) : QString();
        if (group.isEmpty()) {
            group = newId();
            if (common > 0)
                pieceGroups.insert(common, group);
        }
        const QString id = create(u"piece"_s, {
            { u"membership_id"_s, membership },
            { u"group_key"_s, group },
            { u"composer"_s, composers.value(key(row, "piececomposerid")) },
            { u"title"_s, str(row, "title") },
            { u"genre"_s, str(row, "genre") },
            { u"duration"_s, num(row, "duration", 0) },
            { u"start_date"_s, d(str(row, "startdate")) },
            { u"end_date"_s, d(str(row, "stopdate")) },
            { u"state"_s, num(row, "state", 0) },
        });
        pieces.insert(key(row, "pieceid"), id);
        if (common > 0 && !pieceByCommon.contains(common))
            pieceByCommon.insert(common, id);
        ++r.pieces;
    }

    emit progress(50, u"notes"_s);
    QHash<qint64, QString> noteGroups;
    for (const Row &row : old.rows(u"note"_s, u"noteid"_s)) {
        const QString membership = memberships.value(key(row, "palid"));
        if (membership.isEmpty()) {
            ++orphans;
            continue;
        }
        const qint64 common = key(row, "cnoteid");
        QString group = common > 0 ? noteGroups.value(common) : QString();
        if (group.isEmpty()) {
            group = newId();
            if (common > 0)
                noteGroups.insert(common, group);
        }
        create(u"note"_s, {
            { u"membership_id"_s, membership },
            { u"group_key"_s, group },
            { u"date"_s, d(str(row, "date")) },
            { u"content"_s, row.value(u"content"_s).toString() },
        });
        ++r.notes;
    }

    // ---------------------------------------------------------------- activities
    emit progress(70, u"activities"_s);
    for (const Row &row : old.rows(u"activity"_s, u"activityid"_s)) {
        const QString student = students.value(key(row, "pupilid"));
        if (student.isEmpty()) {
            ++orphans;
            continue;
        }
        create(u"activity"_s, {
            { u"student_id"_s, student },
            { u"continuous"_s, num(row, "ifcontinous", 0) },
            { u"description"_s, str(row, "desc") },
            { u"weekday"_s, num(row, "continousday", 0) },
            { u"time"_s, time(str(row, "continoustime")) },
            { u"date"_s, d(str(row, "date")) },
            { u"end_date"_s, d(str(row, "continousstopdate")) },
            { u"single_kind"_s, num(row, "noncontinoustype", 0) },
            { u"continuous_kind"_s, num(row, "continoustype", 0) },
        });
        ++r.activities;
    }

    // ---------------------------------------------------------------- library
    emit progress(78, u"library"_s);
    for (const Row &row : old.rows(u"sheetmusiclibrary"_s, u"smlid"_s)) {
        const qint64 renter = key(row, "rentpupilid");
        create(u"library_item"_s, {
            { u"author"_s, authors.value(key(row, "author")) },
            { u"title"_s, str(row, "title") },
            { u"publisher"_s, publishers.value(key(row, "publisher")) },
            { u"lent_to"_s, renter > 0 ? students.value(renter) : QString() },
            { u"lent_since"_s, renter > 0 ? d(str(row, "lastrentdate")) : QString() },
        });
        ++r.libraryItems;
    }

    // ---------------------------------------------------------------- events
    emit progress(84, u"events"_s);
    QHash<qint64, QString> events, externals;
    for (const Row &row : old.rows(u"recital"_s, u"recitalid"_s)) {
        const QString id = create(u"event"_s, {
            { u"title"_s, str(row, "desc") },
            { u"date"_s, d(str(row, "date")) },
            { u"time"_s, time(str(row, "time")) },
            { u"location"_s, str(row, "location") },
            { u"organizer"_s, str(row, "organisator") },
            { u"accompanist"_s, str(row, "defaultaccompanist") },
            { u"state"_s, num(row, "state", 0) },
        });
        events.insert(key(row, "recitalid"), id);
        ++r.events;
    }
    for (const Row &row : old.rows(u"externalrecitalpiece"_s, u"erpid"_s)) {
        const QString id = create(u"external_piece"_s, {
            { u"composer"_s, str(row, "composer") },
            { u"title"_s, str(row, "title") },
            { u"genre"_s, str(row, "genre") },
            { u"duration"_s, num(row, "duration", 0) },
            { u"performer"_s, str(row, "musician") },
        });
        externals.insert(key(row, "erpid"), id);
    }
    for (const Row &row : old.rows(u"pieceatrecital"_s, u"recitalid, sorting, parid"_s)) {
        const QString event = events.value(key(row, "recitalid"));
        const bool external = num(row, "ifexternalpiece", 0) != 0;
        // pieceid is the common id of a (group) piece; its first copy may be gone
        const qint64 pieceKey = key(row, "pieceid");
        const QString item = external ? externals.value(pieceKey)
                                      : pieces.value(pieceKey, pieceByCommon.value(pieceKey));
        if (event.isEmpty() || item.isEmpty()) {
            ++orphans;
            continue;
        }
        create(u"event_item"_s, {
            { u"event_id"_s, event },
            { external ? u"external_id"_s : u"piece_id"_s, item },
            { u"position"_s, num(row, "sorting", 0) },
        });
    }
    for (const Row &row : old.rows(u"recitalarchive"_s, u"recitalid"_s)) {
        create(u"event_archive"_s, {
            { u"title"_s, str(row, "desc") },
            { u"content"_s, row.value(u"data"_s).toString() },
        });
        ++r.archivedEvents;
    }

    // ---------------------------------------------------------------- reminders
    emit progress(90, u"reminders"_s);
    for (const Row &row : old.rows(u"reminder"_s, u"reminderid"_s)) {
        const qint64 pupil = key(row, "pupilid");
        create(u"reminder"_s, {
            { u"text"_s, str(row, "desc") },
            { u"mode"_s, num(row, "mode", 0) },
            { u"student_id"_s, pupil > 0 ? students.value(pupil) : QString() },
            { u"sound"_s, num(row, "notificationsound", 0) },
        });
        ++r.reminders;
    }

    // ---------------------------------------------------------------- settings from config.xml
    emit progress(95, u"settings"_s);
    if (!legacyConfig.isEmpty() && QFileInfo::exists(legacyConfig)) {
        const LegacyConfig cfg = readConfig(legacyConfig);
        const QStringList sizes = cfg.lists.value(u"PupilInstrumentSizeList"_s);
        int position = 0;
        for (const QString &name : cfg.lists.value(u"PupilInstrumentList"_s)) {
            if (name.trimmed().isEmpty())
                continue;
            create(u"instrument"_s, { { u"name"_s, name.trimmed() }, { u"sizes"_s, jsonList(sizes) },
                                     { u"position"_s, position++ } });
            ++r.instruments;
        }
        const QList<QPair<QString, QString>> listSettings = {
            { u"PalPiecesGenreList"_s, u"genres"_s },
            { u"LessonLocationList"_s, u"locations"_s },
            { u"PupilInstrumentSizeList"_s, u"instrument_sizes"_s },
        };
        for (const auto &[from, to] : listSettings)
            if (cfg.lists.contains(from))
                create(u"setting"_s, { { u"id"_s, to }, { u"value"_s, jsonList(cfg.lists.value(from)) } });
        const QList<QPair<QString, QString>> valueSettings = {
            { u"BirthdayReminder"_s, u"birthday_reminder"_s },
            { u"LessonEndMsg"_s, u"lesson_end_message"_s },
            { u"MinutesToLessonEndForMsg"_s, u"lesson_end_minutes"_s },
            { u"LessonEndMsgSoundVolume"_s, u"lesson_end_volume"_s },
            { u"RecitalIntervalCheckerOnlySolo"_s, u"recital_check_solo_only"_s },
            { u"SaveNotesPiecesForAllPupil"_s, u"save_for_all_students"_s },
            { u"LimitLoadLessonNotes"_s, u"limit_notes"_s },
            { u"LoadLessonNotesNumber"_s, u"limit_notes_count"_s },
            { u"LimitLoadMusicPieces"_s, u"limit_pieces"_s },
            { u"LoadMusicPiecesNumber"_s, u"limit_pieces_count"_s },
            { u"RecitalModerationDuration"_s, u"event_moderation_minutes"_s },
            { u"RecitalBetweenPiecesDuration"_s, u"event_between_minutes"_s },
        };
        for (const auto &[from, to] : valueSettings)
            if (cfg.values.contains(from) && !cfg.values.value(from).isEmpty())
                create(u"setting"_s, { { u"id"_s, to }, { u"value"_s, cfg.values.value(from) } });
    }

    r.skipped = orphans;
    r.unparsedDates = unparsedDates;
    if (orphans > 0)
        r.warnings << u"%1 records without their student or lesson were skipped"_s.arg(orphans);
    if (unparsedDates > 0)
        r.warnings << u"%1 dates had an unknown format and were kept as text"_s.arg(unparsedDates);

    if (ok) {
        Database *db = target->database();
        db->setMeta(u"imported_from"_s, QFileInfo(legacyDatabase).absoluteFilePath());
        db->setMeta(u"imported_revision"_s, QString::number(old.revision()));
        db->setMeta(u"imported_at"_s, QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    }
    r.ok = target->endBatch(ok) && ok;
    emit progress(100, u"done"_s);
    return r;
}

} // namespace Qupil
