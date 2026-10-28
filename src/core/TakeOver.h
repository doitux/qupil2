// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QString>
#include <QVariantMap>

#include <functional>

namespace Qupil::TakeOver {

struct Outcome
{
    bool ok = false;
    QString error;
    QString kind;          //!< "legacy" | "backup"
    QString source;        //!< for display
    QString newDatabase;   //!< ready file in the work folder
    QString legacyCopy;    //!< legacy: the copy of the 1.x database in the backups folder
    QVariantMap summary;
};

/*!
    Takes over the data the user picked: a Qupil 1.x database or data folder, or a
    Qupil 2 backup. Runs in a worker thread, with its own connections and files.

    The picked file is only ever read: it is copied into \a workDir first (with its
    WAL file, folded in), and everything else happens on that copy. The result is
    \c workDir/import.db, ready to replace the live database.

    A Qupil 1.x database is also kept as it was taken over, as
    \c qupil1-<time>.db in \a backupDir (none if \a backupDir is empty).

    \a shownAs replaces the path in messages and in the database's notes on
    where the data came from (e.g. the file name for a temporary copy).
*/
Outcome run(const QString &source, const QString &workDir, const QString &backupDir,
            const std::function<void(int percent, const QString &step)> &progress = {},
            const QString &shownAs = {});

//! Copies a file (also from content:// URIs) in chunks
bool copyFile(const QString &from, const QString &to, QString *error);

//! Removes a database file with its WAL, shared memory and journal files
void removeDatabaseFiles(const QString &path);

//! Local time for file names: yyyy-MM-dd-HHmmss
QString timestamp();

} // namespace Qupil::TakeOver
