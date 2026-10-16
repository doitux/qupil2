// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QString>

/*!
    Large texts that are rarely read (archive entries) are stored packed:
    "qz1:" + Base64 of the zlib-compressed UTF-8 text. They stay text, so they
    fit the database, the change log and the sync files like any other value.
    Texts without the prefix are returned as they are.
*/
namespace TextPack {

bool isPacked(const QString &text);
QString pack(const QString &text);
//! The original text (also for texts that were never packed); empty if damaged
QString unpack(const QString &text);

} // namespace TextPack
