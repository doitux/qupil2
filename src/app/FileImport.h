// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QString>

#include <functional>

/*!
    Lets the user choose a file to open (a backup, Qupil 1.x data, a CSV
    address book, a sound) with the system's own picker, which brings a local
    copy. On iOS Qt's file dialog hands over files of other apps (Nextcloud,
    iCloud Drive, …) only as paths that Qupil is not allowed to read
    ("Operation not permitted"). Other systems use Qt's file dialog
    (available() is false).
*/
namespace FileImport {

bool available();

/*!
    Shows the picker. done(path, name, error) comes later on the main thread:
    path is a local copy of the chosen file in the folder \a into (emptied
    first), empty if the user cancelled (error empty) or the copy failed
    (error set). name is the chosen file's name.
*/
void pickFile(const QString &into,
              std::function<void(const QString &path, const QString &name, const QString &error)> done);

} // namespace FileImport
