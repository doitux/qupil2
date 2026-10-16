// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QString>

#include <functional>

/*!
    Lets the user choose where a copy of a file goes, with the system's own
    picker. On iOS that is "Save to Files" (iCloud Drive, On My iPad, Nextcloud,
    …), because Qt's file dialog cannot save there. Other systems use Qt's
    file dialog instead (available() is false).
*/
namespace FileExport {

bool available();

/*!
    Shows the picker for the local file path. done(saved, name) comes later on
    the main thread: saved false means the user cancelled (or it failed), name
    is the file name chosen. The file itself is not removed.
*/
void exportFile(const QString &path, std::function<void(bool saved, const QString &name)> done);

} // namespace FileExport
