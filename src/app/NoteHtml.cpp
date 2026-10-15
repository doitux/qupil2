// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "NoteHtml.h"

#include <QColor>
#include <QList>
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>

using namespace Qt::StringLiterals;

namespace NoteHtml {

QString forList(const QString &html, bool dark)
{
    QTextDocument doc;
    if (html.contains(u'<'))
        doc.setHtml(html);
    else
        doc.setPlainText(html);   // notes from very old versions

    // Explicit colours: black/white become "default text colour", dark colours are
    // lightened in dark mode (and light ones darkened in light mode) to stay readable.
    // First collected, then applied: changing formats merges fragments, and walking
    // the fragments while doing so never ends.
    struct Change
    {
        int position;
        int length;
        QTextCharFormat format;
    };
    QList<Change> changes;
    for (QTextBlock block = doc.begin(); block.isValid(); block = block.next()) {
        for (auto it = block.begin(); !it.atEnd(); ++it) {
            const QTextFragment fragment = it.fragment();
            if (!fragment.isValid())
                continue;
            QTextCharFormat format = fragment.charFormat();
            bool touched = false;
            if (format.hasProperty(QTextFormat::BackgroundBrush)) {
                format.clearBackground();
                touched = true;
            }
            if (format.hasProperty(QTextFormat::ForegroundBrush)) {
                const QColor c = format.foreground().color();
                const float lightness = c.lightnessF();
                if (c.hslSaturationF() < 0.15f && (lightness < 0.25f || lightness > 0.85f)) {
                    format.clearForeground();
                } else if (dark && lightness < 0.6f) {
                    format.setForeground(QColor::fromHslF(c.hslHueF(), c.hslSaturationF(), 0.75f));
                } else if (!dark && lightness > 0.55f) {
                    format.setForeground(QColor::fromHslF(c.hslHueF(), c.hslSaturationF(), 0.4f));
                }
                touched = true;
            }
            if (format.hasProperty(QTextFormat::FontPointSize) || format.hasProperty(QTextFormat::FontFamilies)) {
                format.clearProperty(QTextFormat::FontPointSize);
                format.clearProperty(QTextFormat::FontFamilies);
                touched = true;
            }
            if (touched)
                changes.append({ fragment.position(), fragment.length(), format });
        }
    }
    QTextCursor cursor(&doc);
    const int end = doc.characterCount() - 1;
    for (const Change &change : std::as_const(changes)) {
        // setCharFormat keeps positions and lengths of the text, only formats change
        if (change.position < 0 || change.position + change.length > end)
            continue;
        cursor.setPosition(change.position);
        cursor.setPosition(change.position + change.length, QTextCursor::KeepAnchor);
        cursor.setCharFormat(change.format);
    }
    QString out = doc.toHtml();
    // Only the body: the list sets font and size
    const qsizetype start = out.indexOf(u"<body"_s);
    if (start >= 0) {
        const qsizetype open = out.indexOf(u'>', start) + 1;
        const qsizetype close = out.lastIndexOf(u"</body>"_s);
        out = out.mid(open, close - open).trimmed();
    }
    out.remove(QRegularExpression(u"<!--(StartFragment|EndFragment)-->"_s));
    // Paragraph margins from Qt's own HTML are too large in a list
    out.replace(QRegularExpression(u"margin-top:\\d+px; margin-bottom:\\d+px;"_s), u"margin-top:0px; margin-bottom:4px;"_s);
    return out;
}

} // namespace NoteHtml
