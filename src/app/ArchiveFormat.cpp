// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "ArchiveFormat.h"

#include <QColor>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextList>
#include <QTextTable>

using namespace Qt::StringLiterals;

namespace ArchiveFormat {

namespace {

//! A colour someone chose on purpose: not black, white or grey (as for notes)
bool colourful(const QColor &c)
{
    return c.isValid() && c.alpha() > 0 && c.hslSaturationF() >= 0.2f;
}

struct Writer
{
    QString title;

    QString inlineText(const QTextBlock &block, bool heading, bool *allUnderlined = nullptr) const
    {
        QString out;
        bool underlined = true;
        bool any = false;
        for (auto it = block.begin(); !it.atEnd(); ++it) {
            const QTextFragment f = it.fragment();
            if (!f.isValid())
                continue;
            QString text = f.text();
            text.replace(QChar::Nbsp, u' ');
            text.remove(QChar::ObjectReplacementCharacter);
            if (text.isEmpty())
                continue;
            const QTextCharFormat cf = f.charFormat();
            const bool underline = cf.fontUnderline() && !cf.isAnchor();
            if (!text.trimmed().isEmpty()) {
                any = true;
                underlined = underlined && underline;
            }
            text = text.toHtmlEscaped();
            text.replace(QChar::LineSeparator, u"<br>"_s);
            QString open;
            QString close;
            if (!heading && cf.fontWeight() >= QFont::DemiBold) {
                open += u"<b>"_s;
                close.prepend(u"</b>"_s);
            }
            if (cf.fontItalic()) {
                open += u"<i>"_s;
                close.prepend(u"</i>"_s);
            }
            if (!heading && underline && !allUnderlined) {
                open += u"<u>"_s;
                close.prepend(u"</u>"_s);
            }
            if ((cf.hasProperty(QTextFormat::ForegroundBrush) && colourful(cf.foreground().color()))
                || (cf.hasProperty(QTextFormat::BackgroundBrush) && colourful(cf.background().color()))) {
                open += u"<span class=\"m\">"_s;
                close.prepend(u"</span>"_s);
            }
            out += open + text + close;
        }
        if (allUnderlined)
            *allUnderlined = any && underlined;
        return out.trimmed();
    }

    void block(const QTextBlock &b, QString &out)
    {
        if (b.blockFormat().hasProperty(QTextFormat::BlockTrailingHorizontalRulerWidth)) {
            out += u"<hr>"_s;
            return;
        }
        if (b.text().trimmed().isEmpty())
            return;
        const int level = b.blockFormat().headingLevel();
        const QString text = inlineText(b, level > 0);
        if (text.isEmpty())
            return;
        if (level > 0 && level <= 6) {
            if (title.isEmpty())
                title = b.text().trimmed();
            out += u"<h%1>%2</h%1>"_s.arg(level).arg(text);
        } else if (QTextList *list = b.textList()) {
            const QTextListFormat::Style style = list->format().style();
            const bool numbered = style == QTextListFormat::ListDecimal || style == QTextListFormat::ListLowerAlpha
                                  || style == QTextListFormat::ListUpperAlpha;
            out += u"<p>"_s + (numbered ? QString::number(list->itemNumber(b) + 1) + u". "_s : u"• "_s) + text + u"</p>"_s;
        } else {
            out += u"<p>"_s + text + u"</p>"_s;
        }
    }

    //! A cell as one header text, or empty if it is not one (all of it underlined)
    QString headerText(const QTextTableCell &cell) const
    {
        QString text;
        for (auto it = cell.begin(); !it.atEnd(); ++it) {
            if (it.currentFrame())
                return {};
            const QTextBlock b = it.currentBlock();
            if (b.text().trimmed().isEmpty())
                continue;
            if (b.blockFormat().headingLevel() > 0)
                return {};
            bool underlined = false;
            const QString t = inlineText(b, false, &underlined);
            if (!underlined || !text.isEmpty())
                return {};
            text = t;
        }
        return text;
    }

    void table(QTextTable *t, QString &out)
    {
        // A layout table of one column: only its content
        if (t->columns() == 1) {
            for (int r = 0; r < t->rows(); ++r)
                frame(t->cellAt(r, 0).begin(), out);
            return;
        }
        QString rows;
        for (int r = 0; r < t->rows(); ++r) {
            // The first row with underlined titles in every cell: a header row
            bool header = rows.isEmpty();
            QStringList headers;
            for (int c = 0; header && c < t->columns(); ++c) {
                const QTextTableCell cell = t->cellAt(r, c);
                if (cell.row() != r || cell.column() != c || cell.columnSpan() > 1) {
                    header = false;   // titles are one per column
                    break;
                }
                const QString h = headerText(cell);
                header = !h.isEmpty();
                headers << h;
            }
            if (header && headers.size() == t->columns()) {
                rows += u"<tr>"_s;
                for (const QString &h : std::as_const(headers))
                    rows += u"<th>"_s + h + u"</th>"_s;
                rows += u"</tr>"_s;
                continue;
            }
            QString cells;
            bool any = false;
            for (int c = 0; c < t->columns(); ++c) {
                const QTextTableCell cell = t->cellAt(r, c);
                if (cell.row() != r || cell.column() != c)
                    continue;
                QString inner;
                frame(cell.begin(), inner);
                // One paragraph needs no <p>
                if (inner.startsWith(u"<p>"_s) && inner.indexOf(u"<p>"_s, 3) < 0 && inner.endsWith(u"</p>"_s))
                    inner = inner.mid(3, inner.size() - 7);
                any = any || !inner.isEmpty();
                cells += (cell.columnSpan() > 1 ? u"<td colspan=\"%1\">"_s.arg(cell.columnSpan()) : u"<td>"_s) + inner
                         + u"</td>"_s;
            }
            if (any)
                rows += u"<tr>"_s + cells + u"</tr>"_s;
        }
        if (rows.isEmpty())
            return;
        out += t->format().border() > 0 ? u"<table border=\"1\" cellspacing=\"0\" cellpadding=\"4\" width=\"100%\">"_s
                                        : u"<table cellspacing=\"0\" cellpadding=\"3\">"_s;
        out += rows + u"</table>"_s;
    }

    void frame(QTextFrame::iterator it, QString &out)
    {
        for (; !it.atEnd(); ++it) {
            if (QTextFrame *child = it.currentFrame()) {
                if (auto *t = qobject_cast<QTextTable *>(child))
                    table(t, out);
                else
                    frame(child->begin(), out);
            } else {
                block(it.currentBlock(), out);
            }
        }
    }
};

} // namespace

bool isLegacy(const QString &html)
{
    qsizetype start = 0;
    while (start < html.size() && html.at(start).isSpace())
        ++start;
    return QStringView(html).mid(start, 9).compare(u"<!DOCTYPE", Qt::CaseInsensitive) == 0;
}

QString compactBody(const QString &legacyHtml, QString *title)
{
    QTextDocument document;
    document.setHtml(legacyHtml);
    Writer writer;
    QString out;
    writer.frame(document.rootFrame()->begin(), out);
    if (title)
        *title = writer.title;
    return out;
}

} // namespace ArchiveFormat
