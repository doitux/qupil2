// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Lesson notes in the compact format (NoteFormat) and old HTML notes. The texts are invented.

#include <QTest>
#include <QTextDocument>

#include "NoteFormat.h"

using namespace Qt::StringLiterals;

namespace {

// As Qupil 1.4.1 stored notes (QTextEdit::toHtml)
const QString kLegacy =
    u"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
    u"<html><head><meta name=\"qrichtext\" content=\"0\" /><style type=\"text/css\">\n"
    u"p, li { white-space: pre-wrap; }\n"
    u"</style></head><body style=\" font-family:'Sans Serif'; font-size:9pt; font-weight:400; font-style:normal;\">\n"
    u"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600;\">G-Dur-Tonleiter</span> über zwei Oktaven, <span style=\" color:#ff0000;\">langsam mit Metronom (60)</span>.</p>\n"
    u"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
    u"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p>\n"
    u"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-family:'Arial'; font-size:14pt; color:#000000;\">Bourrée: Takt 9–16 </span><span style=\" font-size:14pt; font-style:italic; text-decoration: underline; color:#000000;\">auswendig</span>.</p></body></html>"_s;

//! Shown in the editor and read back: the editor's way
QString throughEditor(const QString &note)
{
    QTextDocument document;
    document.setHtml(NoteFormat::toHtml(note, u"#B3261E"_s));
    return NoteFormat::fromDocument(document);
}

} // namespace

class TestNotes : public QObject
{
    Q_OBJECT

private slots:
    void legacyHtml()
    {
        QVERIFY(NoteFormat::isHtml(kLegacy));
        const QString note = NoteFormat::fromHtml(kLegacy);
        // Fonts and sizes gone, black is normal text, red is marked, empty lines become one
        QCOMPARE(note, u"**G-Dur-Tonleiter** über zwei Oktaven, ==langsam mit Metronom (60)==.\n\n"
                       u"Bourrée: Takt 9–16 *++auswendig++*."_s);
        QVERIFY(!NoteFormat::isHtml(note));
        QCOMPARE(NoteFormat::normalized(kLegacy), note);
        QCOMPARE(NoteFormat::normalized(note), note);
        // A fraction of the size
        QVERIFY2(note.toUtf8().size() * 8 < kLegacy.toUtf8().size(),
                 qPrintable(u"%1 vs %2 bytes"_s.arg(note.toUtf8().size()).arg(kLegacy.toUtf8().size())));
    }

    void roundTrip_data()
    {
        QTest::addColumn<QString>("note");
        QTest::newRow("formats") << u"**fett** normal *kursiv* ++unterstrichen++ ==markiert=="_s;
        QTest::newRow("nested") << u"**fett *fett und kursiv* fett** und ==**markiert fett**=="_s;
        QTest::newRow("lines") << u"Erste Zeile\n\n**Dritte** Zeile\nVierte"_s;
        QTest::newRow("escapes") << u"2 \\* 3 = 6, C\\+\\+ und a\\=\\=b, Pfad C:\\\\Noten, x+y, a=b"_s;
        QTest::newRow("starts with <") << u"\\<3 Grüße an die Eltern"_s;
        QTest::newRow("plus before format") << u"a\\+++unter++ und ==b\\==="_s;
        QTest::newRow("umlauts") << u"Übung für *Größe ¾* – „Präludium“ ♯ ♭"_s;
    }

    void roundTrip()
    {
        QFETCH(QString, note);
        QCOMPARE(throughEditor(note), note);
        QVERIFY(!NoteFormat::isHtml(note));
    }

    void plainText()
    {
        const QString text = u"2 * 3 = 6\nC++ und a==b\n<3"_s;
        const QString note = NoteFormat::fromPlainText(text);
        QCOMPARE(note, u"2 \\* 3 = 6\nC\\+\\+ und a\\=\\=b\n<3"_s);
        QCOMPARE(NoteFormat::toPlainText(note), text);
        QVERIFY(!NoteFormat::isHtml(NoteFormat::fromPlainText(u"<3 so gut"_s)));
        QCOMPARE(NoteFormat::toPlainText(u"**fett** und ==rot=="_s), u"fett und rot"_s);
        QCOMPARE(NoteFormat::toPlainText(kLegacy).simplified(),
                 u"G-Dur-Tonleiter über zwei Oktaven, langsam mit Metronom (60). Bourrée: Takt 9–16 auswendig."_s);
    }

    void display()
    {
        const QString html = NoteFormat::toHtml(u"**a** ==b== <c>\n\nd"_s, u"#123456"_s);
        QVERIFY(html.contains(u"<b>a</b>"_s));
        QVERIFY(html.contains(u"<span style=\"color:#123456;\">b</span>"_s));
        QVERIFY(html.contains(u"&lt;c&gt;"_s));
        QCOMPARE(html.count(u"<p "_s), 3);
        // Old HTML is shown in the same simple way
        const QString legacy = NoteFormat::toHtml(kLegacy, u"#123456"_s);
        QVERIFY(!legacy.contains(u"font-size"_s));
        QVERIFY(legacy.contains(u"color:#123456"_s));
    }

    void colours()
    {
        // Black, white and grey are normal text; colours and coloured backgrounds are marked
        const QString html = u"<p><span style=\"color:#000000;\">a</span><span style=\"color:#ffffff;\">b</span>"
                             u"<span style=\"color:#808080;\">c</span><span style=\"color:#2060d0;\">d</span>"
                             u"<span style=\"background-color:#ffff00;\">e</span>"
                             u"<span style=\"background-color:#ffffff;\">f</span></p>"_s;
        QCOMPARE(NoteFormat::fromHtml(html), u"abc==de==f"_s);
        // Links keep only their text
        QCOMPARE(NoteFormat::fromHtml(u"<p>siehe <a href=\"https://qupil.de\">qupil.de</a></p>"_s), u"siehe qupil.de"_s);
    }

    void listsAndBreaks()
    {
        const QString html = u"<ul><li>Tonleitern</li><li><b>Etüde</b></li></ul><ol><li>eins</li><li>zwei</li></ol>"
                             u"<p>Zeile<br />neue Zeile</p>"_s;
        QCOMPARE(NoteFormat::fromHtml(html), u"• Tonleitern\n• **Etüde**\n1. eins\n2. zwei\nZeile\nneue Zeile"_s);
    }

    void noHang()
    {
        // Used to hang when formats were changed while walking the text
        const QString note = u"<p><span style=\" color:#000000;\">Tonleiter</span>"
                             u"<span style=\" color:#ffffff;\"> G-Dur</span> langsam</p>"_s;
        QCOMPARE(NoteFormat::fromHtml(note), u"Tonleiter G-Dur langsam"_s);

        QString many = u"<p>"_s;
        for (int i = 0; i < 300; ++i) {
            many += u"<span style=\" font-size:%1pt; color:#%2; background-color:#ffff00;\">w%3 </span>"_s
                        .arg(10 + i % 3)
                        .arg(i % 2 ? u"000000"_s : u"1a1a1a"_s)
                        .arg(i);
        }
        many += u"</p>"_s;
        const QString converted = NoteFormat::fromHtml(many);
        QVERIFY(converted.startsWith(u"==w0 w1 w2"_s));
        QVERIFY(converted.endsWith(u"w299=="_s));
    }
};

QTEST_MAIN(TestNotes)
#include "tst_notes.moc"
