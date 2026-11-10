// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QList>
#include <QString>

/*!
    The performers of a piece in an event programme, as one line.

    Everyone plays the same instrument: it is named once at the end
    ("Lena Muster (10) · Max Beispiel (11) – Violine"). Different instruments, as in a
    string quartet: each one after the name ("Lena Muster (10), Violine · Max Beispiel
    (11), Viola"). Someone without an instrument is shown without one; the others keep
    theirs.
*/
namespace Qupil::Performers {

struct Person
{
    QString name;        //!< as shown, with the age if known ("Lena Muster (10)")
    QString instrument;  //!< empty if not known
};

QString line(const QList<Person> &people);

} // namespace Qupil::Performers
