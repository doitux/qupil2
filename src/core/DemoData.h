// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QString>
#include <QStringList>

namespace Qupil {
class Store;
}

namespace Qupil::DemoData {

//! The tables of Qupil 1.4.1 (structure revision 3), exactly as 1.4.1 creates them
QStringList legacySchema();

/*!
    Writes a Qupil 1.x data folder (db/qupil.db and config.xml) with invented
    students, lessons, notes, pieces, events, library items and reminders: the full
    week of a teacher of strings and piano (string classes at a primary school,
    single lessons from 13:30 until the evening, groups, ensembles). Lessons are on
    fixed weekdays from Monday to Friday; other dates are relative to Clock::today().
    The notes of the newer lessons are in the format of Qupil 2 (one line, two lines,
    blocks, a whole practice programme), kept as they are by the import. Plans for
    coming lessons, which Qupil 1.x did not have, go into plans.json next to it
    (see addPlans).
    Used for "Try with sample data" and for screenshots – never real data.
    language: "de" gives German names and texts, anything else English.
*/
bool writeLegacyFolder(const QString &folder, const QString &language, QString *error = nullptr);

//! After the import of such a folder: its plans (plans.json); returns the plans added
int addPlans(const QString &folder, Store *store);

} // namespace Qupil::DemoData
