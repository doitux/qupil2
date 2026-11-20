// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QByteArray>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>

namespace Qupil::Booklet {

/*!
    What came back from the student app(s) in a student's folder (the teacher's side).

    Every upload of a device holds that device's whole current feedback, so only the
    newest file per device counts; the older ones can go. Between devices (phone and
    tablet) and against what the teacher already has, the newer answer per task wins.
*/
struct Inbox
{
    QJsonObject feedback;       // task id -> {practiced, works, done, at}
    QJsonArray questions;       // {id, text, task, at}, each once, oldest first
    QStringList superseded;     // older files of a device: can be deleted
    QStringList unreadable;     // not ours, or another key: left alone
    bool changed = false;       // anything new compared with \a known
};

//! \a files: name -> content of the folder's fb-*.qupil files; \a known: the feedback
//! and questions the teacher's Qupil has so far ({feedback, questions})
Inbox collect(const QByteArray &key, const QHash<QString, QByteArray> &files, const QJsonObject &known = {});

} // namespace Qupil::Booklet
