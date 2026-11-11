// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "TextFormatting.h"

#include <QFont>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>

#include <algorithm>

namespace TextFormatting {

namespace {

bool wordChar(QChar c)
{
    return c.isLetterOrNumber() || c.isMark() || c == u'-' || c == u'\'' || c == u'’';
}
bool joiner(QChar c)
{
    return c == u'-' || c == u'\'' || c == u'’';
}

bool formatHas(const QTextCharFormat &f, Attribute attribute)
{
    switch (attribute) {
    case Attribute::Bold:
        return f.fontWeight() >= QFont::DemiBold;
    case Attribute::Italic:
        return f.fontItalic();
    case Attribute::Underline:
        return f.fontUnderline();
    case Attribute::Colored:
        return f.hasProperty(QTextFormat::ForegroundBrush);
    }
    return false;
}

int clamp(const QTextDocument &document, int position)
{
    return std::clamp(position, 0, std::max(0, document.characterCount() - 1));
}

//! The range a toggle changes: the selection, or the word under the cursor
void target(const QTextDocument &document, int start, int end, int cursor, int *from, int *to)
{
    start = clamp(document, start);
    end = clamp(document, end);
    if (start != end) {
        *from = std::min(start, end);
        *to = std::max(start, end);
        return;
    }
    wordAt(document.toPlainText(), clamp(document, cursor), from, to);
}

//! Format of the character at position
QTextCharFormat charAt(const QTextDocument &document, int position)
{
    QTextCursor c(const_cast<QTextDocument *>(&document));
    c.setPosition(position);
    c.setPosition(position + 1, QTextCursor::KeepAnchor);
    return c.charFormat();
}

bool isParagraphEnd(const QTextDocument &document, int position)
{
    const QChar c = document.characterAt(position);
    return c == QChar::ParagraphSeparator || c == u'\n' || c == QChar::LineSeparator || c.isNull();
}

//! Every character of [from, to) has it (paragraph ends left out); false without characters
bool allHave(const QTextDocument &document, Attribute attribute, int from, int to)
{
    bool any = false;
    for (int p = from; p < to; ++p) {
        if (isParagraphEnd(document, p))
            continue;
        if (!formatHas(charAt(document, p), attribute))
            return false;
        any = true;
    }
    return any;
}

} // namespace

void wordAt(const QString &text, int position, int *from, int *to)
{
    position = std::clamp(position, 0, int(text.size()));
    int a = position, b = position;
    while (a > 0 && wordChar(text.at(a - 1)))
        --a;
    while (b < text.size() && wordChar(text.at(b)))
        ++b;
    while (a < b && joiner(text.at(a)))
        ++a;
    while (b > a && joiner(text.at(b - 1)))
        --b;
    *from = a;
    *to = b;
}

bool has(const QTextDocument &document, Attribute attribute, int start, int end, int cursor)
{
    start = clamp(document, start);
    end = clamp(document, end);
    if (start != end)
        return allHave(document, attribute, std::min(start, end), std::max(start, end));
    // Where the cursor types: the format of the character before it (at the very start the
    // first one)
    cursor = clamp(document, cursor);
    QTextCursor c(const_cast<QTextDocument *>(&document));
    c.setPosition(cursor);
    if (cursor == 0 && document.characterCount() > 1)
        c.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    return formatHas(c.charFormat(), attribute);
}

bool toggle(QTextDocument &document, Attribute attribute, int start, int end, int cursor, const QColor &color)
{
    int from = 0, to = 0;
    target(document, start, end, cursor, &from, &to);
    if (from == to)
        return has(document, attribute, start, end, cursor);
    const bool on = !allHave(document, attribute, from, to);
    QTextCursor c(&document);
    c.beginEditBlock();
    if (attribute == Attribute::Colored && !on) {
        // Removing a property needs setCharFormat, character by character
        for (int p = from; p < to; ++p) {
            c.setPosition(p);
            c.setPosition(p + 1, QTextCursor::KeepAnchor);
            QTextCharFormat f = c.charFormat();
            f.clearForeground();
            c.setCharFormat(f);
        }
    } else {
        QTextCharFormat f;
        switch (attribute) {
        case Attribute::Bold:
            f.setFontWeight(on ? QFont::Bold : QFont::Normal);
            break;
        case Attribute::Italic:
            f.setFontItalic(on);
            break;
        case Attribute::Underline:
            f.setFontUnderline(on);
            break;
        case Attribute::Colored:
            f.setForeground(color);
            break;
        }
        c.setPosition(from);
        c.setPosition(to, QTextCursor::KeepAnchor);
        c.mergeCharFormat(f);
    }
    c.endEditBlock();
    return on;
}

} // namespace TextFormatting
