// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QString>

/*!
    Archive entries of Qupil 1.x are whole HTML documents built from nested
    layout tables, and every lesson note inside is a complete HTML document of
    its own, with fonts and sizes on every paragraph. Opening a large one takes
    seconds. They are turned once into the simple layout of Qupil 2: headings,
    paragraphs, tables (header rows as such), bold, italic, underlined and
    marked text (class "m"). Layout tables, fonts, sizes and other colours go.
*/
namespace ArchiveFormat {

//! An archive entry as Qupil 1.x stored it (a complete HTML 4 document)
bool isLegacy(const QString &html);

/*!
    The body of the compact version (without <html>/<body>). title: the text
    of the first heading.
*/
QString compactBody(const QString &legacyHtml, QString *title = nullptr);

/*!
    The credit line at the end of an entry without the author's name, as the
    documents print it now: "Qupil 1.4.1 - ©2006-2026 - qupil.de" (Qupil 1.x),
    "Qupil 2.0 – © 2006–2026 qupil.de" (entries archived with Qupil 2.0).
    The stored entry stays as it is.
*/
QString withoutAuthor(const QString &html);

} // namespace ArchiveFormat
