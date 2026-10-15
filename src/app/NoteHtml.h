// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QString>

/*!
    Lesson notes are rich text (HTML, from Qupil 1.x or the note editor).
*/
namespace NoteHtml {

/*!
    The note for showing in a list: black/white text and backgrounds become the
    theme's colours, other colours stay readable in light and dark mode, font
    sizes and families go (the list sets them). Returns the body only.
*/
QString forList(const QString &html, bool dark);

} // namespace NoteHtml
