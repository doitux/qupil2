// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Desktop and Android: Qt's file dialog chooses where a file goes.

#include "FileExport.h"

namespace FileExport {

bool available()
{
    return false;
}

void exportFile(const QString &path, std::function<void(bool saved, const QString &name)> done)
{
    (void)path;
    if (done)
        done(false, QString());
}

} // namespace FileExport
