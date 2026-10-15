// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "DemoData.h"

#include <QDate>
#include <QHash>
#include <QTime>
#include <QDir>
#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTextStream>
#include <QUuid>
#include <QVariantList>

#include <algorithm>
#include <array>
#include <iterator>

using namespace Qt::StringLiterals;

namespace Qupil::DemoData {

QStringList legacySchema()
{
    return {
        // revision 1 (mydbhandler.cpp)
        u"CREATE TABLE pupil (pupilid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, surname TEXT, forename TEXT, address TEXT, email TEXT, telefon TEXT, handy TEXT, birthday TEXT, notes TEXT, fathername TEXT, fatherjob TEXT, fathertelefon TEXT, mothername TEXT, motherjob TEXT, mothertelefon TEXT, firstlessondate TEXT, instrumenttype TEXT, instrumentsize TEXT, ifinstrumentnextsize INTEGER, ifrentinstrument INTEGER, rentinstrumentdesc TEXT, rentinstrumentstartdate TEXT)"_s,
        u"CREATE TABLE pupilarchive (pupilid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, surname TEXT, forename TEXT, data TEXT)"_s,
        u"CREATE TABLE lesson (lessonid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, state INTEGER, type INTEGER, autolessonname INTEGER, lessonname TEXT, unsteadylesson INTEGER, lessonday INTEGER, lessonstarttime TEXT, lessonstoptime TEXT, lessonlocation TEXT)"_s,
        u"CREATE TABLE lastlessonname ( llnid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, lessonname TEXT)"_s,
        u"CREATE TABLE pupilatlesson (palid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, lessonid INTEGER NOT NULL, pupilid INTEGER NOT NULL, llnid INTEGER, startdate TEXT, stopdate TEXT)"_s,
        u"CREATE TABLE piece (pieceid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, cpieceid INTEGER, palid INTEGER, title TEXT, genre Text, duration INTEGER, startdate TEXT, stopdate TEXT, state INTEGER)"_s,
        u"CREATE TABLE note (noteid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, cnoteid INTEGER, palid INTEGER, date TEXT, content TEXT)"_s,
        u"CREATE TABLE activity (activityid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, pupilid INTEGER, ifcontinous INTEGER, desc TEXT, continousday INTEGER, continoustime TEXT, date TEXT, continousstopdate TEXT)"_s,
        u"CREATE TABLE sheetmusiclibrary (smlid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, author INTEGER, title TEXT, publisher INTEGER, rentpupilid INTEGER, lastrentdate TEXT)"_s,
        u"CREATE TABLE smlauthor (smlauthorid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, author TEXT)"_s,
        u"CREATE TABLE smlpublisher (smlpublisherid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, publisher TEXT)"_s,
        // revision 2 (dbupdater.cpp)
        u"CREATE TABLE dbinfos (id INTEGER PRIMARY KEY NOT NULL, data_structure_rev INTEGER NOT NULL)"_s,
        u"REPLACE INTO dbinfos (id, data_structure_rev) VALUES ( 0,2 )"_s,
        u"ALTER TABLE activity ADD COLUMN noncontinoustype INTEGER NOT NULL DEFAULT 0"_s,
        u"ALTER TABLE pupil ADD COLUMN recitalinterval INTEGER NOT NULL DEFAULT 5"_s,
        u"CREATE TABLE piececomposer (piececomposerid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, composer TEXT)"_s,
        u"INSERT INTO piececomposer (composer) VALUES ('')"_s,
        u"ALTER TABLE piece ADD COLUMN piececomposerid INTEGER DEFAULT 1"_s,
        u"CREATE TABLE recital (recitalid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, desc TEXT, date TEXT, time TEXT, location TEXT, organisator TEXT, defaultaccompanist TEXT, state INTEGER DEFAULT 0)"_s,
        u"CREATE TABLE recitalarchive (recitalid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, desc TEXT, data TEXT)"_s,
        u"CREATE TABLE pieceatrecital (parid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, pieceid INTEGER NOT NULL, recitalid INTEGER NOT NULL, sorting INTEGER DEFAULT 0, ifexternalpiece INTEGER DEFAULT 0)"_s,
        u"CREATE TABLE externalrecitalpiece (erpid INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, composer TEXT, title TEXT, genre TEXT, duration INTEGER, musician TEXT)"_s,
        // revision 3
        u"CREATE TABLE reminder (reminderid INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT, desc TEXT, mode INTEGER DEFAULT 0, pupilid INTEGER, notificationsound INTEGER DEFAULT 0)"_s,
        u"ALTER TABLE activity ADD COLUMN continoustype INTEGER NOT NULL DEFAULT 0"_s,
        u"UPDATE activity SET continousstopdate = '9999-99-99' WHERE continousstopdate IS NULL"_s,
        u"ALTER TABLE pupil ADD COLUMN ensembleactivityrequested INTEGER NOT NULL DEFAULT 1"_s,
        u"REPLACE INTO dbinfos (id, data_structure_rev) VALUES ( 0,3 )"_s,
    };
}

namespace {

constexpr auto kOpen = "9999-99-99";

struct Text
{
    bool de;
    QString operator()(const char *german, const char *english) const
    {
        return QString::fromUtf8(de ? german : english);
    }
};

struct Student
{
    const char *forename;
    const char *surname;
    const char *instrumentDe;
    const char *instrumentEn;
    const char *size;
    int ageYears;
    int birthdayInDays;     // next birthday relative to today
    int learningYears;
};

// Invented people – any similarity is coincidental
const Student kStudentsDe[] = {
    { "Lena", "Muster", "Violine", "Violin", "3/4", 11, 3, 4 },
    { "Jonas", "Becker", "Viola", "Viola", "4/4", 14, 40, 6 },
    { "Mia", "Schneider", "Violine", "Violin", "1/2", 8, 120, 2 },
    { "Tom", "Wagner", "Cello", "Cello", "1/2", 9, 200, 3 },
    { "Emma", "Hoffmann", "Violine", "Violin", "4/4", 16, 65, 9 },
    { "Paul", "Richter", "Cello", "Cello", "4/4", 15, 15, 7 },
    { "Sophie", "Klein", "Violine", "Violin", "3/4", 10, 300, 3 },
    { "Ben", "Wolf", "Viola", "Viola", "4/4", 17, 250, 8 },
    { "Hannah", "Koch", "Kontrabass", "Double Bass", "3/4", 13, 90, 2 },
};
const Student kStudentsEn[] = {
    { "Lily", "Parker", "Violine", "Violin", "3/4", 11, 3, 4 },
    { "Noah", "Brooks", "Viola", "Viola", "4/4", 14, 40, 6 },
    { "Ava", "Turner", "Violine", "Violin", "1/2", 8, 120, 2 },
    { "Leo", "Hughes", "Cello", "Cello", "1/2", 9, 200, 3 },
    { "Grace", "Morgan", "Violine", "Violin", "4/4", 16, 65, 9 },
    { "Oliver", "Bennett", "Cello", "Cello", "4/4", 15, 15, 7 },
    { "Chloe", "Foster", "Violine", "Violin", "3/4", 10, 300, 3 },
    { "Jack", "Ellis", "Viola", "Viola", "4/4", 17, 250, 8 },
    { "Ella", "Price", "Kontrabass", "Double Bass", "3/4", 13, 90, 2 },
};

struct Lesson
{
    int type;               // 1 single, 2 group, 3 ensemble
    int dayOffset;          // weekday relative to today; -1 = irregular
    const char *start;
    const char *end;
    int location;           // index into the location list
    QList<int> students;    // indexes into kStudents*
    const char *manualName = nullptr;
    bool manualGerman = true;
};

class Writer
{
public:
    explicit Writer(QSqlDatabase db) : m_db(db) {}

    qint64 insert(const QString &table, const QStringList &columns, const QVariantList &values)
    {
        QSqlQuery q(m_db);
        QStringList marks;
        for (int i = 0; i < columns.size(); ++i)
            marks << u"?"_s;
        q.prepare(u"INSERT INTO %1 (%2) VALUES (%3)"_s.arg(table, columns.join(u", "_s), marks.join(u", "_s)));
        for (const QVariant &v : values)
            q.addBindValue(v);
        if (!q.exec()) {
            if (m_error.isEmpty())
                m_error = q.lastError().text() + u" ("_s + table + u")"_s;
            return -1;
        }
        return q.lastInsertId().toLongLong();
    }
    void exec(const QString &sql)
    {
        QSqlQuery q(m_db);
        if (!q.exec(sql) && m_error.isEmpty())
            m_error = q.lastError().text() + u" ("_s + sql.left(60) + u")"_s;
    }
    QString error() const { return m_error; }

private:
    QSqlDatabase m_db;
    QString m_error;
};

QString iso(const QDate &d)
{
    return d.toString(Qt::ISODate);
}

//! Most recent date (today or before) that falls on the given weekday (0 = Monday)
QDate lastWeekday(const QDate &today, int weekday)
{
    const int diff = (today.dayOfWeek() - 1 - weekday + 7) % 7;
    return today.addDays(-diff);
}

QString autoName(const Lesson &l, const Student *students, const QStringList &locations, bool de)
{
    QString name = l.type == 1 ? (de ? u"EU-"_s : u"IL-"_s)
                 : l.type == 2 ? (de ? u"GU-"_s : u"GL-"_s)
                               : (de ? u"EnsU-"_s : u"EnsL-"_s);
    if (l.dayOffset >= 0)
        name += QString::number(QTime::fromString(QLatin1StringView(l.start), u"hh:mm"_s)
                                    .secsTo(QTime::fromString(QLatin1StringView(l.end), u"hh:mm"_s)) / 60)
                + u"-"_s;
    name += locations.value(l.location).left(3) + u"-"_s;
    if (l.type == 1) {
        const Student &s = students[l.students.first()];
        name += QString::fromUtf8(s.forename).left(3) + QString::fromUtf8(s.surname).left(3);
    } else {
        QList<int> sorted = l.students;
        std::sort(sorted.begin(), sorted.end(), [students](int a, int b) {
            return QString::fromUtf8(students[a].surname) < QString::fromUtf8(students[b].surname);
        });
        for (int i : sorted)
            name += QString::fromUtf8(students[i].forename).left(1);
    }
    return name;
}

void writeConfig(const QString &path, const QStringList &locations, bool de)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    const Text t { de };
    QTextStream s(&f);
    s << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n<Qupil>\n    <Configuration>\n"
      << "        <ConfigRevision value=\"32\" />\n";
    auto list = [&s](const char *key, const char *tag, const QStringList &items) {
        s << "        <" << key << " value=\"" << tag << "\" type=\"list\">\n";
        for (const QString &item : items)
            s << "            <" << tag << " value=\"" << item.toHtmlEscaped() << "\" />\n";
        s << "        </" << key << ">\n";
    };
    list("PalPiecesGenreList", "PiecesGenre",
         { t("Lied", "Tune"), t("Barock", "Baroque"), t("Klassik", "Classical"), t("Romantik", "Romantic"),
           t("Moderne", "Modernism"), u"Irish Folk"_s, u"Pop"_s });
    list("LessonLocationList", "LessonLocation", locations);
    list("PupilInstrumentList", "Instrument",
         { t("Violine", "Violin"), u"Viola"_s, u"Cello"_s, t("Kontrabass", "Double Bass") });
    list("PupilInstrumentSizeList", "InstrumentSize",
         { u"4/4"_s, u"3/4"_s, u"1/2"_s, u"1/4"_s, u"1/8"_s, u"1/16"_s });
    s << "        <BirthdayReminder value=\"1\" />\n"
      << "        <SaveNotesPiecesForAllPupil value=\"1\" />\n"
      << "        <RecitalModerationDuration value=\"4\" />\n"
      << "        <RecitalBetweenPiecesDuration value=\"2\" />\n"
      << "    </Configuration>\n</Qupil>\n";
}

} // namespace

bool writeLegacyFolder(const QString &folder, const QString &language, QString *error)
{
    const bool de = language.startsWith(u"de"_s);
    const Text t { de };
    const Student *students = de ? kStudentsDe : kStudentsEn;
    constexpr int studentCount = int(std::size(kStudentsDe));
    const QDate today = QDate::currentDate();
    const QStringList locations { t("Raum 20", "Room 20"), t("Raum 16", "Room 16"), t("Aula", "Auditorium") };

    QDir().mkpath(QDir(folder).filePath(u"db"_s));
    const QString path = QDir(folder).filePath(u"db/qupil.db"_s);
    QFile::remove(path);
    writeConfig(QDir(folder).filePath(u"config.xml"_s), locations, de);

    const QString connection = u"qupil-demo-"_s + QUuid::createUuid().toString(QUuid::WithoutBraces);
    QString failure;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(u"QSQLITE"_s, connection);
        db.setDatabaseName(path);
        if (!db.open()) {
            failure = db.lastError().text();
        } else {
            Writer w(db);
            w.exec(u"BEGIN"_s);
            for (const QString &sql : legacySchema())
                w.exec(sql);

            // ------------------------------------------------------------ students
            QList<qint64> pupil;
            for (int i = 0; i < studentCount; ++i) {
                const Student &s = students[i];
                const QDate birthday = today.addDays(s.birthdayInDays).addYears(-s.ageYears);
                const bool rental = i == 0 || i == 3;
                pupil << w.insert(u"pupil"_s,
                    { u"surname"_s, u"forename"_s, u"address"_s, u"email"_s, u"telefon"_s, u"handy"_s, u"birthday"_s,
                      u"notes"_s, u"mothername"_s, u"mothertelefon"_s, u"firstlessondate"_s, u"instrumenttype"_s,
                      u"instrumentsize"_s, u"ifinstrumentnextsize"_s, u"ifrentinstrument"_s, u"rentinstrumentdesc"_s,
                      u"rentinstrumentstartdate"_s, u"recitalinterval"_s, u"ensembleactivityrequested"_s },
                    { QString::fromUtf8(s.surname), QString::fromUtf8(s.forename),
                      t("Musterweg %1, 12345 Notenstadt", "%1 Example Lane, Melody Town").arg(3 + i * 4),
                      QString::fromUtf8(s.forename).toLower() + u"@example.org"_s,
                      u"0123 45 67%1"_s.arg(10 + i), u"0170 000 00%1"_s.arg(10 + i), iso(birthday),
                      i == 0 ? t("Übt am liebsten morgens. Spielt gern Barock.", "Likes to practise in the morning. Loves baroque music.")
                             : QString(),
                      t("Mutter von %1", "Mother of %1").arg(QString::fromUtf8(s.forename)), u"0123 98 76%1"_s.arg(i),
                      iso(today.addYears(-s.learningYears).addMonths(-1)), t(s.instrumentDe, s.instrumentEn),
                      QString::fromLatin1(s.size), i == 0 || i == 3 ? 1 : 0, rental ? 1 : 0,
                      rental ? t("Leihinstrument Nr. %1", "Rental instrument no. %1").arg(12 + i) : QString(),
                      rental ? iso(today.addMonths(-14)) : QString(), i == 2 ? 3 : 5, i == 2 ? 0 : 1 });
            }
            w.insert(u"pupilarchive"_s, { u"surname"_s, u"forename"_s, u"data"_s },
                     { t("Früher", "Former"), t("Anna", "Amy"),
                       t("<h1>Archiveintrag: Anna Früher</h1><p>Unterricht von 2016 bis 2022.</p>",
                         "<h1>Archive entry: Amy Former</h1><p>Lessons from 2016 to 2022.</p>") });

            // ------------------------------------------------------------ lessons
            const QList<Lesson> lessons {
                { 1, 0, "14:00", "14:45", 0, { 0 } },
                { 1, 0, "14:45", "15:15", 0, { 6 } },
                { 2, 0, "15:30", "16:30", 0, { 1, 2 } },
                { 3, 0, "17:00", "18:30", 2, { 0, 4, 5, 7, 8 }, "Streicherensemble", true },
                { 1, 1, "15:00", "15:30", 1, { 3 } },
                { 1, 1, "16:00", "16:45", 1, { 4 } },
                { 1, 2, "14:30", "15:15", 0, { 5 } },
                { 1, 3, "16:15", "17:00", 0, { 7 } },
                { 1, -1, "", "", 1, { 8 } },
            };
            QList<QList<qint64>> memberships;   // per lesson: palid per student
            QList<QDate> lastLesson;            // per lesson: date of the most recent lesson
            for (const Lesson &l : lessons) {
                const QString name = l.manualName
                                         ? (de || !l.manualGerman ? QString::fromUtf8(l.manualName) : u"String ensemble"_s)
                                         : autoName(l, students, locations, de);
                const int weekday = l.dayOffset < 0 ? 0 : (today.dayOfWeek() - 1 + l.dayOffset) % 7;
                const qint64 lesson = w.insert(u"lesson"_s,
                    { u"state"_s, u"type"_s, u"autolessonname"_s, u"lessonname"_s, u"unsteadylesson"_s,
                      u"lessonday"_s, u"lessonstarttime"_s, u"lessonstoptime"_s, u"lessonlocation"_s },
                    { 1, l.type, l.manualName ? 0 : 1, name, l.dayOffset < 0 ? 1 : 0, weekday,
                      QString::fromLatin1(l.start), QString::fromLatin1(l.end), locations.value(l.location) });
                QList<qint64> pals;
                for (int s : l.students) {
                    pals << w.insert(u"pupilatlesson"_s, { u"lessonid"_s, u"pupilid"_s, u"startdate"_s, u"stopdate"_s },
                                     { lesson, pupil.value(s), iso(today.addMonths(-13 - s)), QString::fromLatin1(kOpen) });
                }
                memberships << pals;
                lastLesson << (l.dayOffset < 0 ? today.addDays(-9) : lastWeekday(today.addDays(-1), weekday));
            }
            // A former lesson of Tom / Leo, ended last summer
            const qint64 llnid = w.insert(u"lastlessonname"_s, { u"lessonname"_s }, { t("EU-30-Rau-TomWag", "IL-30-Roo-LeoHug") });
            const qint64 oldLesson = w.insert(u"lesson"_s,
                { u"state"_s, u"type"_s, u"autolessonname"_s, u"lessonname"_s, u"unsteadylesson"_s, u"lessonday"_s,
                  u"lessonstarttime"_s, u"lessonstoptime"_s, u"lessonlocation"_s },
                { 0, 1, 1, t("EU-30-Rau-TomWag", "IL-30-Roo-LeoHug"), 0, 3, u"15:00"_s, u"15:30"_s, locations[0] });
            const qint64 oldPal = w.insert(u"pupilatlesson"_s,
                { u"lessonid"_s, u"pupilid"_s, u"llnid"_s, u"startdate"_s, u"stopdate"_s },
                { oldLesson, pupil.value(3), llnid, iso(today.addYears(-3)), iso(today.addMonths(-4)) });

            // ------------------------------------------------------------ notes
            const QStringList noteTexts {
                t("<p><b>G-Dur-Tonleiter</b> über zwei Oktaven, <span style=\"color:#d02020;\">langsam mit Metronom (60)</span>.</p><p>Bourrée: Takt 9–16 auswendig.</p>",
                  "<p><b>G major scale</b> over two octaves, <span style=\"color:#d02020;\">slowly with metronome (60)</span>.</p><p>Bourrée: bars 9–16 by heart.</p>"),
                t("<p>Bogeneinteilung im Mittelteil geklärt. <i>Sehr schöner Ton!</i></p>",
                  "<p>Sorted out the bow distribution in the middle section. <i>Lovely tone!</i></p>"),
                t("<p>Lagenwechsel 1.–3. Lage üben, jeden Tag 5 Minuten.</p>",
                  "<p>Shifting 1st–3rd position, five minutes every day.</p>"),
                t("<p>Neues Stück ausgesucht. Noten bis nächste Woche besorgen.</p>",
                  "<p>Picked a new piece. Get the sheet music by next week.</p>"),
                t("<p><b>Vibrato</b>: Handgelenk locker lassen. Etüde Nr. 4 vorbereiten.</p>",
                  "<p><b>Vibrato</b>: keep the wrist relaxed. Prepare study no. 4.</p>"),
            };
            for (int li = 0; li < lessons.size(); ++li) {
                const int weeks = lessons[li].type == 3 ? 2 : 4;
                for (int k = 0; k < weeks; ++k) {
                    const QDate date = lastLesson[li].addDays(-7 * k);
                    const QString content = noteTexts[(li + k) % noteTexts.size()];
                    qint64 common = -1;
                    for (qint64 pal : memberships[li]) {
                        const qint64 id = w.insert(u"note"_s, { u"palid"_s, u"date"_s, u"content"_s },
                                                   { pal, iso(date), content });
                        if (common < 0)
                            common = id;
                        w.exec(u"UPDATE note SET cnoteid = %1 WHERE noteid = %2"_s.arg(common).arg(id));
                    }
                }
            }
            {
                const qint64 id = w.insert(u"note"_s, { u"palid"_s, u"date"_s, u"content"_s },
                                           { oldPal, iso(today.addMonths(-4).addDays(-2)),
                                             t("<p>Letzte Stunde vor dem Wechsel zu Raum 16.</p>",
                                               "<p>Last lesson before moving to room 16.</p>") });
                w.exec(u"UPDATE note SET cnoteid = %1 WHERE noteid = %1"_s.arg(id));
            }

            // ------------------------------------------------------------ pieces
            struct Piece { const char *composer; const char *title; const char *genreDe; const char *genreEn; int duration; int state; };
            const QList<Piece> repertoire {
                { "J. S. Bach", "Bourrée (Suite Nr. 3)", "Barock", "Baroque", 3, 3 },
                { "A. Vivaldi", "Konzert a-Moll, 1. Satz", "Barock", "Baroque", 4, 1 },
                { "O. Rieding", "Concertino op. 35", "Romantik", "Romantic", 6, 1 },
                { "F. Seitz", "Schülerkonzert Nr. 2", "Romantik", "Romantic", 7, 2 },
                { "W. A. Mozart", "Menuett KV 2", "Klassik", "Classical", 2, 4 },
                { "Traditional", "Cripple Creek", "Irish Folk", "Irish Folk", 2, 3 },
                { "G. F. Händel", "Sonate F-Dur, 2. Satz", "Barock", "Baroque", 3, 0 },
                { "C. Saint-Saëns", "Der Schwan", "Romantik", "Romantic", 3, 3 },
            };
            QHash<QString, qint64> composers;
            auto composerId = [&](const char *name) {
                const QString key = QString::fromUtf8(name);
                if (!composers.contains(key))
                    composers.insert(key, w.insert(u"piececomposer"_s, { u"composer"_s }, { key }));
                return composers.value(key);
            };
            QList<qint64> readyPieces;   // common ids of pieces ready for the next recital
            for (int li = 0; li < lessons.size(); ++li) {
                const int count = lessons[li].type == 3 ? 2 : 3;
                for (int k = 0; k < count; ++k) {
                    const Piece &p = repertoire[(li * 3 + k) % repertoire.size()];
                    const int state = lessons[li].type == 3 ? 1 : p.state;
                    qint64 common = -1;
                    for (qint64 pal : memberships[li]) {
                        const qint64 id = w.insert(u"piece"_s,
                            { u"palid"_s, u"title"_s, u"genre"_s, u"duration"_s, u"startdate"_s, u"stopdate"_s,
                              u"state"_s, u"piececomposerid"_s },
                            { pal, QString::fromUtf8(p.title), t(p.genreDe, p.genreEn), p.duration,
                              iso(today.addDays(-20 * (k + 1) - li)),
                              state == 4 ? QVariant(iso(today.addDays(-10))) : QVariant(), state,
                              composerId(p.composer) });
                        if (common < 0)
                            common = id;
                        w.exec(u"UPDATE piece SET cpieceid = %1 WHERE pieceid = %2"_s.arg(common).arg(id));
                    }
                    if (state == 3 && readyPieces.size() < 5)
                        readyPieces << common;
                }
            }

            // ------------------------------------------------------------ activities
            const QDate pastRecital = today.addMonths(-4).addDays(-3);
            for (int i : { 0, 4, 5 }) {
                w.insert(u"activity"_s,
                    { u"pupilid"_s, u"ifcontinous"_s, u"desc"_s, u"continousday"_s, u"continoustime"_s, u"date"_s,
                      u"continousstopdate"_s, u"continoustype"_s, u"noncontinoustype"_s },
                    { pupil.value(i), 1, t("Jugendorchester", "Youth orchestra"), (today.dayOfWeek() + 2) % 7,
                      u"17:30"_s, iso(today.addYears(-1)), QString::fromLatin1(kOpen), 0, 0 });
            }
            w.insert(u"activity"_s,
                { u"pupilid"_s, u"ifcontinous"_s, u"desc"_s, u"continousday"_s, u"continoustime"_s, u"date"_s,
                  u"continousstopdate"_s, u"continoustype"_s, u"noncontinoustype"_s },
                { pupil.value(1), 1, t("Musiktheorie", "Music theory"), 2, u"18:00"_s, iso(today.addMonths(-8)),
                  QString::fromLatin1(kOpen), 1, 0 });
            for (int i : { 0, 1, 4 }) {
                w.insert(u"activity"_s,
                    { u"pupilid"_s, u"ifcontinous"_s, u"desc"_s, u"date"_s, u"noncontinoustype"_s },
                    { pupil.value(i), 0, t("Sommervorspiel, Aula: J. S. Bach - Bourrée", "Summer recital, Auditorium: J. S. Bach - Bourrée"),
                      iso(pastRecital), 0 });
            }
            w.insert(u"activity"_s, { u"pupilid"_s, u"ifcontinous"_s, u"desc"_s, u"date"_s, u"noncontinoustype"_s },
                     { pupil.value(4), 0, t("Wettbewerb „Jugend musiziert“", "Regional youth music competition"),
                       iso(today.addMonths(-7)), 2 });

            // ------------------------------------------------------------ library
            const QList<std::array<const char *, 3>> library {
                std::array<const char *, 3> { "F. Wohlfahrt", "60 Etüden op. 45", "Edition Beispiel" },
                { "J. Trott", "Melodious Double-Stops", "Example Music" },
                { "S. Suzuki", "Violin School, Vol. 2", "Example Music" },
                { "O. Rieding", "Concertino op. 35", "Edition Beispiel" },
                { "J. F. Mazas", "Etüden op. 36", "Edition Beispiel" },
                { "Diverse", "", "Example Music" },   // title depends on the language, see below
            };
            QHash<QString, qint64> authors, publishers;
            for (int k = 0; k < library.size(); ++k) {
                const QString author = QString::fromUtf8(library[k][0]);
                const QString publisher = QString::fromUtf8(library[k][2]);
                if (!authors.contains(author))
                    authors.insert(author, w.insert(u"smlauthor"_s, { u"author"_s }, { author }));
                if (!publishers.contains(publisher))
                    publishers.insert(publisher, w.insert(u"smlpublisher"_s, { u"publisher"_s }, { publisher }));
                const bool lent = k == 0 || k == 3;
                w.insert(u"sheetmusiclibrary"_s, { u"author"_s, u"title"_s, u"publisher"_s, u"rentpupilid"_s, u"lastrentdate"_s },
                         { authors.value(author), k == 5 ? t("Weihnachtslieder für Streicher", "Christmas carols for strings")
                                                         : QString::fromUtf8(library[k][1]),
                           publishers.value(publisher), lent ? pupil.value(k == 0 ? 0 : 6) : -1,
                           lent ? QVariant(iso(today.addDays(-21 - k))) : QVariant() });
            }

            // ------------------------------------------------------------ events
            const qint64 recital = w.insert(u"recital"_s,
                { u"desc"_s, u"date"_s, u"time"_s, u"location"_s, u"organisator"_s, u"defaultaccompanist"_s, u"state"_s },
                { t("Herbstvorspiel", "Autumn recital"), iso(today.addDays(18)), u"16:00"_s, locations[2],
                  t("Musikschule Notenstadt", "Melody Town music school"), t("Frau Flügel", "Ms Keys"), 0 });
            int position = 0;
            for (qint64 piece : readyPieces) {
                w.insert(u"pieceatrecital"_s, { u"pieceid"_s, u"recitalid"_s, u"sorting"_s, u"ifexternalpiece"_s },
                         { piece, recital, position++, 0 });
            }
            const qint64 external = w.insert(u"externalrecitalpiece"_s,
                { u"composer"_s, u"title"_s, u"genre"_s, u"duration"_s, u"musician"_s },
                { u"F. Schubert"_s, t("Ständchen", "Serenade"), t("Romantik", "Romantic"), 4,
                  t("Gast, Clara (15) - Gesang", "Guest, Clara (15) - Voice") });
            w.insert(u"pieceatrecital"_s, { u"pieceid"_s, u"recitalid"_s, u"sorting"_s, u"ifexternalpiece"_s },
                     { external, recital, position++, 1 });
            w.insert(u"recital"_s,
                { u"desc"_s, u"date"_s, u"time"_s, u"location"_s, u"organisator"_s, u"defaultaccompanist"_s, u"state"_s },
                { t("Sommervorspiel", "Summer recital"), iso(pastRecital), u"17:00"_s, locations[2],
                  t("Musikschule Notenstadt", "Melody Town music school"), t("Frau Flügel", "Ms Keys"), 1 });
            w.insert(u"recitalarchive"_s, { u"desc"_s, u"data"_s },
                     { iso(today.addYears(-1)) + u"_17:00 "_s + t("Frühlingsvorspiel", "Spring recital"),
                       t("<h1>Frühlingsvorspiel</h1><p>Programm mit 8 Beiträgen</p>",
                         "<h1>Spring recital</h1><p>Programme with 8 contributions</p>") });

            // ------------------------------------------------------------ reminders
            w.insert(u"reminder"_s, { u"desc"_s, u"mode"_s, u"pupilid"_s, u"notificationsound"_s },
                     { t("Leihinstrument zur Kontrolle mitbringen lassen", "Ask to bring the rental instrument for a check"),
                       2, pupil.value(3), 1 });
            w.insert(u"reminder"_s, { u"desc"_s, u"mode"_s, u"pupilid"_s, u"notificationsound"_s },
                     { t("Noten für das Ensemble bestellen", "Order sheet music for the ensemble"), 0, pupil.value(0), 0 });

            w.exec(u"COMMIT"_s);
            failure = w.error();
            db.close();
        }
    }
    QSqlDatabase::removeDatabase(connection);
    if (error)
        *error = failure;
    return failure.isEmpty();
}

} // namespace Qupil::DemoData
