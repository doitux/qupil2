// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Lesson notes as rich text in lists (NoteHtml). The texts are invented.

#include <QTest>

#include "NoteHtml.h"

using namespace Qt::StringLiterals;

class TestNotes : public QObject
{
    Q_OBJECT

private slots:
    void blackAndWhiteBecomeThemeColours()
    {
        // Used to hang: changing a fragment's format merged it with the next one
        // while the fragments were walked
        const QString note = u"<p><span style=\" color:#000000;\">Tonleiter</span>"
                             u"<span style=\" color:#ffffff;\"> G-Dur</span> langsam</p>"_s;
        for (bool dark : { false, true }) {
            const QString html = NoteHtml::forList(note, dark);
            QVERIFY(html.contains(u"Tonleiter G-Dur langsam"_s));
            QVERIFY(!html.contains(u"color:"_s));
        }
    }

    void manyFragments()
    {
        QString note = u"<p>"_s;
        for (int i = 0; i < 200; ++i) {
            note += u"<span style=\" font-size:%1pt; color:#%2; background-color:#ffff00;\">w%3 </span>"_s
                        .arg(10 + i % 3)
                        .arg(i % 2 ? u"000000"_s : u"1a1a1a"_s)
                        .arg(i);
        }
        note += u"</p>"_s;
        const QString html = NoteHtml::forList(note, true);
        QVERIFY(html.contains(u"w0 w1 w2"_s));
        QVERIFY(html.contains(u"w199"_s));
        QVERIFY(!html.contains(u"font-size"_s));
        QVERIFY(!html.contains(u"background-color"_s));
    }

    void coloursStayReadable()
    {
        const QString red = u"<p><span style=\" color:#ff0000;\">laut</span> leise</p>"_s;
        QVERIFY(NoteHtml::forList(red, false).contains(u"color:#ff0000"_s));   // dark enough on white
        QVERIFY(!NoteHtml::forList(red, true).contains(u"color:#ff0000"_s));   // lightened on dark
        const QString plain = NoteHtml::forList(u"ohne <Format>"_s.toHtmlEscaped(), false);
        QVERIFY(plain.contains(u"ohne"_s));
    }
};

QTEST_MAIN(TestNotes)
#include "tst_notes.moc"
