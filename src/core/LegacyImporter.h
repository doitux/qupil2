// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

namespace Qupil {

class Store;

/*!
    Takes over the data of Qupil 1.x (database structure revision 1 to 3).

    The old database is opened read-only and never changed. Records get new
    ids; relations (lessons, group notes and pieces, recital programmes, library
    loans, reminders) are kept. The target should be an empty Qupil 2 database.
*/
class LegacyImporter : public QObject
{
    Q_OBJECT

public:
    struct Result
    {
        bool ok = false;
        QString error;
        int students = 0;
        int archivedStudents = 0;
        int lessons = 0;
        int memberships = 0;
        int notes = 0;
        int pieces = 0;
        int activities = 0;
        int events = 0;
        int archivedEvents = 0;
        int libraryItems = 0;
        int reminders = 0;
        int instruments = 0;
        int skipped = 0;          //!< records whose student or lesson no longer exists
        int unparsedDates = 0;    //!< dates in an unknown format, kept as text
        QStringList warnings;
    };

    explicit LegacyImporter(QObject *parent = nullptr);

    /*!
        Finds the 1.x database for a path the user picked: the file itself,
        or a Qupil 1.x data folder (containing db/qupil.db).
    */
    static QString findDatabase(const QString &fileOrFolder);
    //! The 1.x settings file next to a data folder, if any
    static QString findConfig(const QString &fileOrFolder);
    //! Usual 1.x data folders on this computer that exist
    static QStringList defaultDataFolders();

    Result run(const QString &legacyDatabase, Store *target, const QString &legacyConfig = {});

signals:
    void progress(int percent, const QString &step);
};

} // namespace Qupil
