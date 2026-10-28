// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Desktop and Android: Qt's file dialog opens files (Android: content:// URIs Qupil can read).

#include "FileImport.h"

namespace FileImport {

bool available()
{
    return false;
}

void pickFile(const QString &into,
              std::function<void(const QString &path, const QString &name, const QString &error)> done)
{
    (void)into;
    if (done)
        done(QString(), QString(), QString());
}

} // namespace FileImport
