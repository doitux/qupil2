// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "TextFormatter.h"

#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>

#include <algorithm>

TextFormatter::TextFormatter(QObject *parent)
    : QObject(parent)
{
    connect(this, &TextFormatter::selectionChanged, this, &TextFormatter::formatChanged);
}

void TextFormatter::setDocument(QQuickTextDocument *document)
{
    if (document == m_document)
        return;
    m_document = document;
    emit documentChanged();
    emit formatChanged();
}

QTextCursor TextFormatter::cursor() const
{
    QTextDocument *doc = m_document ? m_document->textDocument() : nullptr;
    if (!doc)
        return {};
    QTextCursor c(doc);
    // The editor may report positions of the text before it was replaced
    const auto valid = [doc](int position) { return std::clamp(position, 0, std::max(0, doc->characterCount() - 1)); };
    if (m_selectionStart != m_selectionEnd) {
        c.setPosition(valid(m_selectionStart));
        c.setPosition(valid(m_selectionEnd), QTextCursor::KeepAnchor);
    } else {
        c.setPosition(valid(m_cursorPosition));
    }
    return c;
}

QTextCharFormat TextFormatter::currentFormat() const
{
    QTextCursor c = cursor();
    if (c.isNull())
        return {};
    // The format at a position is the one of the character before it
    if (!c.hasSelection() && c.position() == 0 && c.document()->characterCount() > 1)
        c.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor);
    return c.charFormat();
}

bool TextFormatter::bold() const
{
    return currentFormat().fontWeight() >= QFont::DemiBold;
}

bool TextFormatter::italic() const
{
    return currentFormat().fontItalic();
}

bool TextFormatter::underline() const
{
    return currentFormat().fontUnderline();
}

bool TextFormatter::colored() const
{
    return currentFormat().hasProperty(QTextFormat::ForegroundBrush);
}

void TextFormatter::merge(const QTextCharFormat &format)
{
    QTextCursor c = cursor();
    if (c.isNull())
        return;
    if (!c.hasSelection())
        c.select(QTextCursor::WordUnderCursor);
    c.mergeCharFormat(format);
    emit formatChanged();
}

void TextFormatter::toggleBold()
{
    QTextCharFormat f;
    f.setFontWeight(bold() ? QFont::Normal : QFont::Bold);
    merge(f);
}

void TextFormatter::toggleItalic()
{
    QTextCharFormat f;
    f.setFontItalic(!italic());
    merge(f);
}

void TextFormatter::toggleUnderline()
{
    QTextCharFormat f;
    f.setFontUnderline(!underline());
    merge(f);
}

void TextFormatter::toggleColor(const QColor &color)
{
    QTextCursor c = cursor();
    if (c.isNull())
        return;
    if (!c.hasSelection())
        c.select(QTextCursor::WordUnderCursor);
    if (colored()) {
        QTextCharFormat f = c.charFormat();
        f.clearForeground();
        c.setCharFormat(f);
    } else {
        QTextCharFormat f;
        f.setForeground(color);
        c.mergeCharFormat(f);
    }
    emit formatChanged();
}

bool TextFormatter::isEmpty() const
{
    QTextDocument *doc = m_document ? m_document->textDocument() : nullptr;
    return !doc || doc->toPlainText().trimmed().isEmpty();
}

QString TextFormatter::html() const
{
    QTextDocument *doc = m_document ? m_document->textDocument() : nullptr;
    if (!doc || isEmpty())
        return {};
    return doc->toHtml();
}
