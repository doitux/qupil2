// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QString>

class QTextDocument;

/*!
    Lesson notes are stored as short Markdown-like text instead of HTML:

        **bold**  *italic*  ++underlined++  ==marked==

    One line of text per paragraph ("\n" between lines). A backslash makes the
    next character plain (\*, \\, \+ and \= where they would be read as a
    mark-up, \< at the very start). Formats end at the end of a line.
    ++…++ and ==…== are the "ins" and "mark" extensions known from
    markdown-it; marked text is shown in the marking colour chosen under
    Appearance. Font sizes, fonts and other colours are not kept.

    Notes of Qupil 1.x and of earlier 2.0 test builds are HTML documents. They
    always start with "<" (a note in the new format never does), are converted
    once when the database is opened, and are still shown correctly wherever
    they come from (e.g. from a device with an older version).
*/
namespace NoteFormat {

//! HTML of Qupil 1.x or an earlier test build (not the new format)
bool isHtml(const QString &note);

//! Any note (HTML or the new format) in the new format
QString normalized(const QString &note);
QString fromHtml(const QString &html);
//! Plain text: characters that are mark-up get a backslash
QString fromPlainText(const QString &text);
//! Bold, italic, underline and coloured text of a rich text document (the note editor)
QString fromDocument(const QTextDocument &document);

/*!
    Simple HTML for showing and editing (b, i, u, a coloured span per mark,
    one paragraph per line). markColor: colour of marked text.
*/
QString toHtml(const QString &note, const QString &markColor);
//! The text without any mark-up
QString toPlainText(const QString &note);

} // namespace NoteFormat
