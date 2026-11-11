// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QColor>
#include <QString>

class QTextDocument;

/*!
    Bold, italic, underline and colour of a selection in a rich-text document – the rules
    behind the format buttons (TextFormatter), without Qt Quick so they can be tested.

    A selection counts as bold when every character in it is bold; the ends of paragraphs
    inside it do not count (a whole paragraph selected by a double or triple click ends at
    the start of the next one – reading the format there showed "not bold"). Toggling makes
    all of it bold, or all of it normal when it already was. Without a selection the word
    under the cursor is changed, and the buttons show the format where the cursor types.
*/
namespace TextFormatting {

enum class Attribute { Bold, Italic, Underline, Colored };

//! The word around position (letters, digits, hyphens and apostrophes; "G-Dur" is one
//! word); from == to when there is none. The same rule as selectWordAt() in TextTouch.qml.
void wordAt(const QString &text, int position, int *from, int *to);

//! Is the attribute set for the selection (start != end) or at the cursor?
bool has(const QTextDocument &document, Attribute attribute, int start, int end, int cursor);

//! Sets the attribute for the selection or the word under the cursor, or removes it when
//! all of it has it already. Returns the new state.
bool toggle(QTextDocument &document, Attribute attribute, int start, int end, int cursor, const QColor &color = {});

} // namespace TextFormatting
