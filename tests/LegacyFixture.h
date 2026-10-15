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

inline QString create(const QString &path)
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
        for (const QString &sql : schemaRev3() + sampleData()) {
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
