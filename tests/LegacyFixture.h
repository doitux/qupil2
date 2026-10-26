// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Builds a Qupil 1.4.1 database (structure revision 3) with invented data,
// using the exact statements of 1.4.1 (mydbhandler.cpp, dbupdater.cpp).
// No real data is ever used for tests.
#pragma once

#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QUuid>

#include "DemoData.h"

namespace LegacyFixture {

//! Exact 1.4.1 statements for revisions 1 → 2 → 3 (shared with the demo data)
inline QStringList schemaRev3()
{
    return Qupil::DemoData::legacySchema();
}

//! The structure of an older 1.x database: revision 1 (no dbinfos table yet)
//! or revision 2 (recitals, composers; no reminders yet)
inline QStringList schemaUpTo(int revision)
{
    const QString next = revision == 1 ? QStringLiteral("CREATE TABLE dbinfos")
                                       : QStringLiteral("CREATE TABLE reminder");
    QStringList out;
    for (const QString &sql : schemaRev3()) {
        if (sql.startsWith(next))
            break;
        out << sql;
    }
    return out;
}

//! Invented sample data: 4 students (2 in a group lesson, 1 in an ensemble), notes,
//! group notes and pieces, activities, a recital with an external contribution, library, reminder.
inline QStringList sampleData()
{
    return {
        "INSERT INTO pupil (pupilid, surname, forename, address, email, telefon, handy, birthday, notes, firstlessondate, instrumenttype, instrumentsize, ifinstrumentnextsize, ifrentinstrument, rentinstrumentdesc, rentinstrumentstartdate, recitalinterval, ensembleactivityrequested) VALUES "
        "(1, 'Muster', 'Lena', 'Lindenweg 1, 12345 Musterstadt', 'lena@example.org', '01234 5678', '', '2014-03-02', 'Spielt gern Barock', '2019-09-01', 'Violine', '3/4', 1, 1, 'Leihgeige Nr. 7', '2023-09-01', 5, 1)",
        "INSERT INTO pupil (pupilid, surname, forename, birthday, instrumenttype, instrumentsize, ifinstrumentnextsize, ifrentinstrument, recitalinterval, ensembleactivityrequested) VALUES (2, 'Beispiel', 'Jonas', '2012-11-30', 'Viola', '4/4', 0, 0, 3, 0)",
        "INSERT INTO pupil (pupilid, surname, forename, birthday, instrumenttype, ifinstrumentnextsize, ifrentinstrument) VALUES (3, 'Test', 'Mia', '', 'Violine', 0, 0)",
        "INSERT INTO pupil (pupilid, surname, forename, birthday, instrumenttype, ifinstrumentnextsize, ifrentinstrument) VALUES (4, 'Probe', 'Tom', '15.06.2013', 'Cello', 0, 0)",
        "INSERT INTO pupilarchive (surname, forename, data) VALUES ('Alt', 'Anna', '<p>Unterricht 2015-2018</p>')",

        "INSERT INTO lesson VALUES (1, 1, 1, 1, 'EU - 45 Min. - LenMus', 0, 2, '14:00', '14:45', 'Raum 20')",
        "INSERT INTO lesson VALUES (2, 1, 2, 0, 'Gruppe Mittwoch', 0, 2, '14:45', '15:45', 'Raum 20')",
        "INSERT INTO lesson VALUES (3, 1, 3, 0, 'Streicherensemble', 0, 2, '17:00', '18:00', 'Aula')",
        "INSERT INTO lesson VALUES (4, 0, 1, 1, 'EU - 30 Min. - TomPro', 1, 0, '', '', '')",

        "INSERT INTO lastlessonname (llnid, lessonname) VALUES (1, 'EU - 30 Min. - TomPro')",
        "INSERT INTO pupilatlesson (palid, lessonid, pupilid, startdate, stopdate) VALUES (1, 1, 1, '2019-09-01', '9999-99-99')",
        "INSERT INTO pupilatlesson (palid, lessonid, pupilid, startdate, stopdate) VALUES (2, 2, 2, '2024-09-01', '9999-99-99')",
        "INSERT INTO pupilatlesson (palid, lessonid, pupilid, startdate, stopdate) VALUES (3, 2, 3, '2024-09-01', '9999-99-99')",
        "INSERT INTO pupilatlesson (palid, lessonid, pupilid, startdate, stopdate) VALUES (4, 3, 1, '2022-09-01', '9999-99-99')",
        "INSERT INTO pupilatlesson (palid, lessonid, pupilid, llnid, startdate, stopdate) VALUES (5, 4, 4, 1, '2020-09-01', '2023-07-15')",

        "INSERT INTO piececomposer (piececomposerid, composer) VALUES (2, 'J. S. Bach')",
        "INSERT INTO piece VALUES (1, 1, 1, 'Bourrée', 'Barock', 3, '2026-07-15', '9999-99-99', 3, 2)",
        "INSERT INTO piece VALUES (2, 2, 1, 'Concertino', 'Romantik', 6, '2026-09-02', '9999-99-99', 1, 1)",
        "INSERT INTO piece VALUES (3, 3, 2, 'Cripple Creek', 'Irish Folk', 2, '2026-09-10', '9999-99-99', 1, 1)",
        "INSERT INTO piece VALUES (4, 3, 3, 'Cripple Creek', 'Irish Folk', 2, '2026-09-10', '9999-99-99', 1, 1)",

        "INSERT INTO note VALUES (1, 1, 1, '2026-10-07', '<p><b>G-Dur-Tonleiter</b> über 2 Oktaven. <span style=\"color:#ff0000;\">Langsam üben!</span></p>')",
        "INSERT INTO note VALUES (2, 2, 1, '2026-09-30', '<p>Neue Etüde</p>')",
        "INSERT INTO note VALUES (3, 3, 2, '2026-10-07', '<p>Bogenwechsel an der Spitze</p>')",
        "INSERT INTO note VALUES (4, 3, 3, '2026-10-07', '<p>Bogenwechsel an der Spitze</p>')",
        "INSERT INTO note VALUES (5, 5, 4, '2023-06-20', '<p>Letzte Stunde vor dem Umzug</p>')",

        "INSERT INTO activity (activityid, pupilid, ifcontinous, desc, continousday, continoustime, date, continousstopdate, noncontinoustype, continoustype) VALUES (1, 1, 1, 'Jugendorchester', 4, '17:30', '2025-09-01', '9999-99-99', 0, 1)",
        "INSERT INTO activity (activityid, pupilid, ifcontinous, desc, continousday, continoustime, date, continousstopdate, noncontinoustype, continoustype) VALUES (2, 1, 0, 'Sommervorspiel', 0, '', '2026-06-20', '', 0, 0)",
        "INSERT INTO activity (activityid, pupilid, ifcontinous, desc, continousday, continoustime, date, continousstopdate, noncontinoustype, continoustype) VALUES (3, 2, 0, 'Wettbewerb', 0, '', '2026-03-14', '', 2, 0)",

        "INSERT INTO smlauthor (smlauthorid, author) VALUES (1, 'Wohlfahrt')",
        "INSERT INTO smlpublisher (smlpublisherid, publisher) VALUES (1, 'Musikverlag Beispiel')",
        "INSERT INTO sheetmusiclibrary VALUES (1, 1, 'Etüden op. 45', 1, 1, '2026-09-30')",
        "INSERT INTO sheetmusiclibrary VALUES (2, 1, 'Etüden op. 74', 1, 0, '')",

        "INSERT INTO recital VALUES (1, 'Herbstvorspiel', '2026-11-07', '16:00', 'Aula', 'Musikschule', 'Frau Klavier', 0)",
        "INSERT INTO externalrecitalpiece VALUES (1, 'F. Schubert', 'Ständchen', 'Romantik', 4, 'Gastsängerin')",
        "INSERT INTO pieceatrecital VALUES (1, 1, 1, 1, 0)",
        "INSERT INTO pieceatrecital VALUES (2, 1, 1, 0, 1)",
        "INSERT INTO pieceatrecital VALUES (3, 3, 1, 2, 0)",
        "INSERT INTO recitalarchive (desc, data) VALUES ('Frühlingsvorspiel 2026', '<p>Programm</p>')",

        "INSERT INTO reminder (desc, mode, pupilid, notificationsound) VALUES ('Leihgeige zurück', 1, 2, 1)",
        "INSERT INTO reminder (desc, mode, pupilid, notificationsound) VALUES ('Noten bestellen', 0, 0, 0)",
    };
}

//! Invented data in the columns of revision 1: no composers, recitals, reminders,
//! recital interval or kinds of activities yet
inline QStringList sampleDataRev1()
{
    return {
        "INSERT INTO pupil VALUES (1, 'Alt', 'Clara', 'Am Markt 2, 12345 Musterstadt', '', '', '', '2009-05-04', '', "
        "'Peter Alt', 'Bäcker', '01234 1111', 'Eva Alt', '', '01234 2222', '2016-09-01', 'Violine', '4/4', 0, 0, '', '')",
        "INSERT INTO pupil VALUES (2, 'Frueh', 'Ole', '', '', '', '', '04.07.2011', '', '', '', '', '', '', '', '', 'Viola', '', 0, 0, '', '')",
        "INSERT INTO lesson VALUES (1, 1, 1, 1, 'EU - 45 Min. - CAlt', 0, 1, '15:00', '15:45', 'Raum 3')",
        "INSERT INTO lesson VALUES (2, 1, 2, 0, 'Duo', 0, 3, '16:00', '17:00', 'Raum 3')",
        "INSERT INTO pupilatlesson (palid, lessonid, pupilid, startdate, stopdate) VALUES (1, 1, 1, '2016-09-01', '9999-99-99')",
        "INSERT INTO pupilatlesson (palid, lessonid, pupilid, startdate, stopdate) VALUES (2, 2, 1, '2018-09-01', '9999-99-99')",
        "INSERT INTO pupilatlesson (palid, lessonid, pupilid, startdate, stopdate) VALUES (3, 2, 2, '2018-09-01', '9999-99-99')",
        "INSERT INTO piece VALUES (1, 1, 1, 'Sonatine', 'Klassik', 4, '2018-01-10', '2018-06-30', 2)",
        "INSERT INTO piece VALUES (2, 2, 2, 'Kanon', 'Barock', 3, '2018-10-01', '9999-99-99', 1)",
        "INSERT INTO piece VALUES (3, 2, 3, 'Kanon', 'Barock', 3, '2018-10-01', '9999-99-99', 1)",
        "INSERT INTO note VALUES (1, 1, 1, '2018-01-17', '<p>Lagenwechsel</p>')",
        "INSERT INTO note VALUES (2, 2, 2, '2018-10-04', '<p>Zusammen einsetzen</p>')",
        "INSERT INTO note VALUES (3, 2, 3, '2018-10-04', '<p>Zusammen einsetzen</p>')",
        "INSERT INTO activity VALUES (1, 1, 1, 'Schulorchester', 2, '13:30', '2017-09-01', '9999-99-99')",
        "INSERT INTO activity VALUES (2, 2, 0, 'Vorspiel', 0, '', '2018-12-12', NULL)",
        "INSERT INTO smlauthor (smlauthorid, author) VALUES (1, 'Sevcik')",
        "INSERT INTO smlpublisher (smlpublisherid, publisher) VALUES (1, 'Notenverlag Beispiel')",
        "INSERT INTO sheetmusiclibrary VALUES (1, 1, 'Op. 1 Heft 1', 1, 2, '2018-11-05')",
        "INSERT INTO pupilarchive (surname, forename, data) VALUES ('Vorher', 'Ben', '<p>Unterricht 2010-2014</p>')",
    };
}

//! Invented data in the columns of revision 2: recitals and composers, no reminders yet
inline QStringList sampleDataRev2()
{
    return {
        "INSERT INTO pupil (pupilid, surname, forename, birthday, instrumenttype, ifinstrumentnextsize, ifrentinstrument, recitalinterval) "
        "VALUES (1, 'Mittel', 'Ida', '2010-02-03', 'Violoncello', 1, 0, 3)",
        "INSERT INTO pupil (pupilid, surname, forename, birthday, instrumenttype, ifinstrumentnextsize, ifrentinstrument) "
        "VALUES (2, 'Weiter', 'Karl', '2012-08-09', 'Violine', 0, 1)",
        "INSERT INTO lesson VALUES (1, 1, 1, 1, 'EU - 30 Min. - IdaMit', 0, 0, '14:00', '14:30', 'Raum 5')",
        "INSERT INTO lesson VALUES (2, 1, 1, 1, 'EU - 30 Min. - KarWei', 0, 0, '14:30', '15:00', 'Raum 5')",
        "INSERT INTO pupilatlesson (palid, lessonid, pupilid, startdate, stopdate) VALUES (1, 1, 1, '2020-09-01', '9999-99-99')",
        "INSERT INTO pupilatlesson (palid, lessonid, pupilid, startdate, stopdate) VALUES (2, 2, 2, '2021-09-01', '9999-99-99')",
        "INSERT INTO piececomposer (piececomposerid, composer) VALUES (2, 'G. F. Händel')",
        "INSERT INTO piece VALUES (1, 1, 1, 'Sarabande', 'Barock', 3, '2021-02-01', '9999-99-99', 3, 2)",
        "INSERT INTO piece VALUES (2, 2, 2, 'Menuett', 'Barock', 2, '2021-02-01', '9999-99-99', 3, 1)",
        "INSERT INTO note VALUES (1, 1, 1, '2021-02-08', '<p>Bogeneinteilung</p>')",
        "INSERT INTO activity (activityid, pupilid, ifcontinous, desc, continousday, continoustime, date, continousstopdate, noncontinoustype) "
        "VALUES (1, 1, 0, 'Wettbewerb', 0, '', '2021-03-20', NULL, 2)",
        "INSERT INTO activity (activityid, pupilid, ifcontinous, desc, continousday, continoustime, date, continousstopdate, noncontinoustype) "
        "VALUES (2, 2, 1, 'Kammermusik', 3, '18:00', '2021-09-01', NULL, 0)",
        "INSERT INTO recital VALUES (1, 'Frühlingskonzert', '2021-04-24', '15:00', 'Saal', 'Musikschule', '', 0)",
        "INSERT INTO pieceatrecital VALUES (1, 2, 1, 1, 0)",
        "INSERT INTO pieceatrecital VALUES (2, 1, 1, 0, 0)",
        "INSERT INTO recitalarchive (desc, data) VALUES ('Sommerkonzert 2020', '<p>Programm</p>')",
    };
}

/*!
    Invented data the way years of use and hand edits leave it: names missing,
    dates in odd formats, text in number columns, records whose student, lesson
    or recital is gone, a table and a column no Qupil version ever made.

    Records left over: 8 (2 lesson memberships, a piece, a note, an activity,
    3 recital entries). Dates in a format that cannot be read: 5.
*/
inline QStringList messyData()
{
    return {
        "CREATE TABLE notes_backup_by_hand (id INTEGER, text TEXT)",
        "ALTER TABLE pupil ADD COLUMN favouritecolour TEXT",
        // no name at all, an impossible birthday, a date in words, text in number columns
        "INSERT INTO pupil (pupilid, surname, forename, birthday, firstlessondate, ifinstrumentnextsize, ifrentinstrument, recitalinterval, favouritecolour) "
        "VALUES (1, NULL, NULL, '31.02.2010', 'Sept. 2019', NULL, 'ja', 'oft', 'blau')",
        // quotes, SQL-like text, emoji and half-closed tags
        "INSERT INTO pupil (pupilid, surname, forename, notes, ifinstrumentnextsize, ifrentinstrument) "
        "VALUES (2, 'O''Neill; DROP TABLE pupil;--', 'Zoë 🎻', '<p><b>nicht geschlossen <i>kursiv</p', 0, 0)",
        // a date with blanks and no leading zeros
        "INSERT INTO pupil (pupilid, surname, forename, birthday, ifinstrumentnextsize, ifrentinstrument) "
        "VALUES (3, '  Leer  ', 'Zeichen', ' 1.2.2012 ', 0, 0)",

        "INSERT INTO lesson VALUES (1, NULL, 1, 1, '', 0, NULL, '9:5', '25:00', NULL)",
        "INSERT INTO lesson VALUES (2, 1, 2, 0, 'Gruppe', 0, 4, '16:00', '17:00', 'Raum 1')",

        "INSERT INTO pupilatlesson (palid, lessonid, pupilid, startdate, stopdate) VALUES (1, 1, 1, '2019-09-01', '9999-99-99')",
        "INSERT INTO pupilatlesson (palid, lessonid, pupilid, startdate, stopdate) VALUES (2, 99, 2, '2020-09-01', '9999-99-99')",
        "INSERT INTO pupilatlesson (palid, lessonid, pupilid, startdate, stopdate) VALUES (3, 2, 77, '2020-09-01', '9999-99-99')",
        "INSERT INTO pupilatlesson (palid, lessonid, pupilid, startdate, stopdate) VALUES (4, 2, 2, 'gestern', NULL)",

        "INSERT INTO piece VALUES (1, NULL, 2, 'Verwaist', '', 1, '', '', 0, 1)",
        "INSERT INTO piece VALUES (2, 0, 1, 'Ohne Komponist', NULL, 'lang', NULL, NULL, NULL, 42)",
        "INSERT INTO piece VALUES (3, 5, 4, 'Gruppenstück', 'Folk', 2, '2020-10-01', '9999-99-99', 1, NULL)",

        "INSERT INTO note VALUES (1, 1, 3, '2020-10-01', '<p>Verwaist</p>')",
        "INSERT INTO note VALUES (2, 2, 1, '2026-13-45', '<div><p>Kaputt <b>fett</div>')",
        "INSERT INTO note VALUES (3, 3, 4, NULL, NULL)",

        "INSERT INTO activity (activityid, pupilid, ifcontinous, desc, continousday, continoustime, date, continousstopdate) VALUES (1, 55, 0, 'Verwaist', 0, '', '', '')",
        "INSERT INTO activity (activityid, pupilid, ifcontinous, desc, continousday, continoustime, date, continousstopdate) VALUES (2, 1, 1, NULL, NULL, 'abends', NULL, NULL)",

        "INSERT INTO sheetmusiclibrary VALUES (1, 9, 'Ausgeliehen an Ehemalige', 8, 66, '2026-01-01')",

        "INSERT INTO recital VALUES (1, 'Herbstfest', 'Herbst', '16 Uhr', '', '', '', NULL)",
        "INSERT INTO pieceatrecital VALUES (1, 999, 1, 0, 0)",
        "INSERT INTO pieceatrecital VALUES (2, 2, 5, 0, 0)",
        "INSERT INTO pieceatrecital VALUES (3, 7, 1, 1, 1)",
        "INSERT INTO pieceatrecital VALUES (4, 2, 1, 2, 0)",

        "INSERT INTO reminder (desc, mode, pupilid, notificationsound) VALUES ('Für jemanden, den es nicht mehr gibt', 1, 88, 1)",
    };
}

inline QString create(const QString &path, const QStringList &statements = schemaRev3() + sampleData())
{
    QFile::remove(path);
    const QString name = QStringLiteral("fixture-") + QUuid::createUuid().toString(QUuid::WithoutBraces);
    QString error;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), name);
        db.setDatabaseName(path);
        if (!db.open())
            return db.lastError().text();
        QSqlQuery q(db);
        for (const QString &sql : statements) {
            if (!q.exec(sql)) {
                error = q.lastError().text() + QStringLiteral(" in: ") + sql;
                break;
            }
        }
        db.close();
    }
    QSqlDatabase::removeDatabase(name);
    return error;
}

inline void writeConfig(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;
    QTextStream s(&f);
    s << R"(<?xml version="1.0" encoding="UTF-8" ?>
<Qupil>
    <Configuration>
        <ConfigRevision value="32" />
        <UserDir value="/home/test/.qupil/" />
        <PalPiecesGenreList value="PiecesGenre" type="list">
            <PiecesGenre value="Lied" />
            <PiecesGenre value="Barock" />
        </PalPiecesGenreList>
        <LessonLocationList value="LessonLocation" type="list">
            <LessonLocation value="Raum 20" />
            <LessonLocation value="Aula" />
        </LessonLocationList>
        <PupilInstrumentList value="Instrument" type="list">
            <Instrument value="Violine" />
            <Instrument value="Viola" />
            <Instrument value="Violoncello" />
        </PupilInstrumentList>
        <PupilInstrumentSizeList value="InstrumentSize" type="list">
            <InstrumentSize value="4/4" />
            <InstrumentSize value="3/4" />
        </PupilInstrumentSizeList>
        <SaveNotesPiecesForAllPupil value="1" />
        <RecitalModerationDuration value="4" />
    </Configuration>
</Qupil>
)";
}

} // namespace LegacyFixture
