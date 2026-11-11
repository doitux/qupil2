// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "TextFormatter.h"

#include "NoteFormat.h"
#include "TextFormatting.h"

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

QTextDocument *TextFormatter::doc() const
{
    return m_document ? m_document->textDocument() : nullptr;
}

bool TextFormatter::has(int attribute) const
{
    QTextDocument *d = doc();
    return d && TextFormatting::has(*d, TextFormatting::Attribute(attribute), m_selectionStart, m_selectionEnd,
                                    m_cursorPosition);
}

void TextFormatter::toggle(int attribute, const QColor &color)
{
    QTextDocument *d = doc();
    if (!d)
        return;
    TextFormatting::toggle(*d, TextFormatting::Attribute(attribute), m_selectionStart, m_selectionEnd,
                           m_cursorPosition, color);
    emit formatChanged();
}

bool TextFormatter::bold() const
{
    return has(int(TextFormatting::Attribute::Bold));
}

bool TextFormatter::italic() const
{
    return has(int(TextFormatting::Attribute::Italic));
}

bool TextFormatter::underline() const
{
    return has(int(TextFormatting::Attribute::Underline));
}

bool TextFormatter::colored() const
{
    return has(int(TextFormatting::Attribute::Colored));
}

void TextFormatter::toggleBold()
{
    toggle(int(TextFormatting::Attribute::Bold));
}

void TextFormatter::toggleItalic()
{
    toggle(int(TextFormatting::Attribute::Italic));
}

void TextFormatter::toggleUnderline()
{
    toggle(int(TextFormatting::Attribute::Underline));
}

void TextFormatter::toggleColor(const QColor &color)
{
    toggle(int(TextFormatting::Attribute::Colored), color);
}

bool TextFormatter::isEmpty() const
{
    QTextDocument *doc = m_document ? m_document->textDocument() : nullptr;
    return !doc || doc->toPlainText().trimmed().isEmpty();
}

QString TextFormatter::markup() const
{
    QTextDocument *doc = m_document ? m_document->textDocument() : nullptr;
    if (!doc || isEmpty())
        return {};
    return NoteFormat::fromDocument(*doc);
}
