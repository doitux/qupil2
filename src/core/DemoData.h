// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QString>
#include <QStringList>

namespace Qupil::DemoData {

//! The tables of Qupil 1.4.1 (structure revision 3), exactly as 1.4.1 creates them
QStringList legacySchema();

/*!
    Writes a Qupil 1.x data folder (db/qupil.db and config.xml) with invented
    students, lessons, notes, pieces, events and library items. Dates are
    relative to today, so the schedule always has lessons today.
    Used for "Try with sample data" and for screenshots – never real data.
    language: "de" gives German names and texts, anything else English.
*/
bool writeLegacyFolder(const QString &folder, const QString &language, QString *error = nullptr);

} // namespace Qupil::DemoData
