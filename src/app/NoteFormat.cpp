// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "NoteFormat.h"

#include <QColor>
#include <QList>
#include <QStringList>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTextList>

using namespace Qt::StringLiterals;

namespace NoteFormat {

namespace {

enum Style { Bold = 1, Italic = 2, Underline = 4, Mark = 8 };

// The order in which formats are opened (and closed the other way round)
constexpr Style kOrder[] = { Mark, Bold, Italic, Underline };

QString marker(int style)
{
    switch (style) {
    case Bold:
        return u"**"_s;
    case Italic:
        return u"*"_s;
    case Underline:
        return u"++"_s;
    default:
        return u"=="_s;
    }
}

struct Run
{
    QString text;
    int styles = 0;
};
using Line = QList<Run>;

//! A colour someone chose on purpose: not black, white or grey
bool colourful(const QColor &c)
{
    return c.isValid() && c.alpha() > 0 && c.hslSaturationF() >= 0.2f;
}

int stylesOf(const QTextCharFormat &format)
{
    int styles = 0;
    if (format.fontWeight() >= QFont::DemiBold)
        styles |= Bold;
    if (format.fontItalic())
        styles |= Italic;
    if (format.isAnchor())
        return styles;   // links: their underline and colour come with the link
    if (format.fontUnderline())
        styles |= Underline;
    if ((format.hasProperty(QTextFormat::ForegroundBrush) && colourful(format.foreground().color()))
        || (format.hasProperty(QTextFormat::BackgroundBrush) && colourful(format.background().color())))
        styles |= Mark;
    return styles;
}

/*!
    One line: formats are opened and closed like brackets, so that Markdown
    readers understand it too; a literal mark-up character gets a backslash
    where it would otherwise be read as one.
*/
QString writeLine(const Line &line, bool first)
{
    // Atoms: mark-up or one literal character
    struct Atom
    {
        QString markup;
        QChar c;
    };
    QList<Atom> atoms;
    QList<int> open;
    for (const Run &run : line) {
        if (run.text.isEmpty())
            continue;
        qsizetype keep = 0;
        while (keep < open.size() && (run.styles & open.at(keep)))
            ++keep;
        for (qsizetype i = open.size() - 1; i >= keep; --i)
            atoms.append({ marker(open.at(i)), {} });
        open.resize(keep);
        for (Style s : kOrder) {
            if ((run.styles & s) && !open.contains(s)) {
                atoms.append({ marker(s), {} });
                open.append(s);
            }
        }
        for (const QChar c : run.text)
            atoms.append({ {}, c });
    }
    for (qsizetype i = open.size() - 1; i >= 0; --i)
        atoms.append({ marker(open.at(i)), {} });

    QString out;
    for (qsizetype i = 0; i < atoms.size(); ++i) {
        const Atom &a = atoms.at(i);
        if (!a.markup.isEmpty()) {
            out += a.markup;
            continue;
        }
        const QChar next = i + 1 < atoms.size()
                               ? (atoms.at(i + 1).markup.isEmpty() ? atoms.at(i + 1).c : atoms.at(i + 1).markup.front())
                               : QChar();
        const QChar previous = i > 0 ? (atoms.at(i - 1).markup.isEmpty() ? atoms.at(i - 1).c : atoms.at(i - 1).markup.back())
                                     : QChar();
        bool escape = a.c == u'\\' || a.c == u'*';
        // "+" and "=" only next to another one ("x+y" stays as it is, "C++" becomes "C\+\+")
        if ((a.c == u'+' || a.c == u'=') && (next == a.c || previous == a.c))
            escape = true;
        if (first && out.isEmpty() && a.c == u'<')
            escape = true;   // a note never starts with "<": that is HTML
        if (escape)
            out += u'\\';
        out += a.c;
    }
    return out;
}

//! Lines of runs from the stored text
QList<Line> parse(const QString &note)
{
    QList<Line> lines;
    for (const QString &text : note.split(u'\n')) {
        Line line;
        int styles = 0;
        QString buffer;
        const auto flush = [&] {
            if (!buffer.isEmpty())
                line.append({ buffer, styles });
            buffer.clear();
        };
        for (qsizetype i = 0; i < text.size(); ++i) {
            const QChar c = text.at(i);
            const QChar next = i + 1 < text.size() ? text.at(i + 1) : QChar();
            if (c == u'\\' && !next.isNull()) {
                buffer += next;
                ++i;
            } else if ((c == u'*' || c == u'+' || c == u'=') && next == c) {
                flush();
                styles ^= c == u'*' ? Bold : c == u'+' ? Underline : Mark;
                ++i;
            } else if (c == u'*') {
                flush();
                styles ^= Italic;
            } else {
                buffer += c;
            }
        }
        flush();
        lines.append(line);
    }
    return lines;
}

QString write(QList<Line> lines)
{
    // Empty lines at the start and end go, several in a row become one
    const auto empty = [](const Line &l) {
        for (const Run &r : l)
            if (!r.text.trimmed().isEmpty())
                return false;
        return true;
    };
    while (!lines.isEmpty() && empty(lines.first()))
        lines.removeFirst();
    while (!lines.isEmpty() && empty(lines.last()))
        lines.removeLast();
    QStringList out;
    bool previousEmpty = false;
    for (Line &line : lines) {
        const bool isEmpty = empty(line);
        if (isEmpty && previousEmpty)
            continue;
        previousEmpty = isEmpty;
        if (isEmpty) {
            out << QString();
            continue;
        }
        // No spaces at the end of a line
        while (!line.isEmpty()) {
            QString &last = line.last().text;
            while (!last.isEmpty() && last.back().isSpace())
                last.chop(1);
            if (!last.isEmpty())
                break;
            line.removeLast();
        }
        out << writeLine(line, out.isEmpty());
    }
    return out.join(u'\n');
}

} // namespace

bool isHtml(const QString &note)
{
    for (const QChar c : note) {
        if (!c.isSpace())
            return c == u'<';
    }
    return false;
}

QString fromDocument(const QTextDocument &document)
{
    QList<Line> lines;
    for (QTextBlock block = document.begin(); block.isValid(); block = block.next()) {
        Line line;
        if (QTextList *list = block.textList()) {
            const QTextListFormat::Style style = list->format().style();
            const bool numbered = style == QTextListFormat::ListDecimal || style == QTextListFormat::ListLowerAlpha
                                  || style == QTextListFormat::ListUpperAlpha || style == QTextListFormat::ListLowerRoman
                                  || style == QTextListFormat::ListUpperRoman;
            line.append({ numbered ? QString::number(list->itemNumber(block) + 1) + u". "_s : u"• "_s, 0 });
        }
        for (auto it = block.begin(); !it.atEnd(); ++it) {
            const QTextFragment fragment = it.fragment();
            if (!fragment.isValid())
                continue;
            const int styles = stylesOf(fragment.charFormat());
            QString text = fragment.text();
            text.replace(QChar::Nbsp, u' ');
            text.remove(QChar::ObjectReplacementCharacter);   // pictures
            // A line break inside the paragraph (<br>) starts a new line
            const QStringList parts = text.split(QChar::LineSeparator);
            for (qsizetype i = 0; i < parts.size(); ++i) {
                if (i > 0) {
                    lines.append(line);
                    line.clear();
                }
                if (!parts.at(i).isEmpty())
                    line.append({ parts.at(i), styles });
            }
        }
        lines.append(line);
    }
    return write(lines);
}

QString fromHtml(const QString &html)
{
    QTextDocument document;
    document.setHtml(html);
    return fromDocument(document);
}

QString fromPlainText(const QString &text)
{
    QList<Line> lines;
    for (const QString &line : text.split(u'\n'))
        lines.append(Line { Run { line, 0 } });
    return write(lines);
}

QString normalized(const QString &note)
{
    return isHtml(note) ? fromHtml(note) : note;
}

QString toHtml(const QString &note, const QString &markColor)
{
    const QList<Line> lines = parse(normalized(note));
    QString out;
    for (const Line &line : lines) {
        out += u"<p style=\"margin-top:0px; margin-bottom:4px; white-space:pre-wrap;\">"_s;
        if (line.isEmpty())
            out += u"&nbsp;"_s;
        for (const Run &run : line) {
            QString open;
            QString close;
            if (run.styles & Mark) {
                open += u"<span style=\"color:%1;\">"_s.arg(markColor.isEmpty() ? u"#B3261E"_s : markColor);
                close.prepend(u"</span>"_s);
            }
            if (run.styles & Bold) {
                open += u"<b>"_s;
                close.prepend(u"</b>"_s);
            }
            if (run.styles & Italic) {
                open += u"<i>"_s;
                close.prepend(u"</i>"_s);
            }
            if (run.styles & Underline) {
                open += u"<u>"_s;
                close.prepend(u"</u>"_s);
            }
            out += open + run.text.toHtmlEscaped() + close;
        }
        out += u"</p>"_s;
    }
    return out;
}

QString toPlainText(const QString &note)
{
    if (isHtml(note))
        return QTextDocumentFragment::fromHtml(note).toPlainText();
    QStringList out;
    for (const Line &line : parse(note)) {
        QString text;
        for (const Run &run : line)
            text += run.text;
        out << text;
    }
    return out.join(u'\n');
}

} // namespace NoteFormat
