// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

#include <QSettings>
#include <QSignalSpy>
#include <QTest>

#include "LanguageManager.h"

using namespace Qt::StringLiterals;

class TestLanguageManager : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        QCoreApplication::setOrganizationName(u"QupilTests"_s);
        QCoreApplication::setApplicationName(u"tst_languagemanager"_s);
        QSettings().clear();
    }

    void cleanupTestCase() { QSettings().clear(); }

    void resolveLanguage_data()
    {
        QTest::addColumn<QStringList>("preferred");
        QTest::addColumn<QString>("expected");
        QTest::newRow("german region") << QStringList{ u"de-AT"_s, u"en-US"_s } << u"de"_s;
        QTest::newRow("underscore") << QStringList{ u"fr_CA"_s } << u"fr"_s;
        QTest::newRow("latin american spanish") << QStringList{ u"es-419"_s } << u"es"_s;
        QTest::newRow("first supported wins") << QStringList{ u"it-IT"_s, u"es-ES"_s, u"de-DE"_s } << u"es"_s;
        QTest::newRow("unsupported only") << QStringList{ u"ja-JP"_s, u"it"_s } << u"en"_s;
        QTest::newRow("empty") << QStringList{} << u"en"_s;
    }

    void resolveLanguage()
    {
        QFETCH(QStringList, preferred);
        QFETCH(QString, expected);
        QCOMPARE(LanguageManager::resolveLanguage(preferred), expected);
    }

    void languageList()
    {
        QCOMPARE(LanguageManager::supportedLanguages(), (QStringList{ u"en"_s, u"de"_s, u"fr"_s, u"es"_s }));
        LanguageManager manager;
        const QVariantList list = manager.languages();
        QCOMPARE(list.size(), 5);
        QCOMPARE(list.first().toMap().value(u"code"_s).toString(), u"system"_s);
        QCOMPARE(list.at(2).toMap().value(u"nativeName"_s).toString(), u"Deutsch"_s);
    }

    void translations_data()
    {
        QTest::addColumn<QString>("language");
        QTest::addColumn<QString>("cancel");
        QTest::addColumn<QString>("bFlat");
        QTest::addColumn<QString>("monthsAgo");
        QTest::newRow("en") << u"en"_s << u"Cancel"_s << u"B♭"_s << u"3 months ago"_s;
        QTest::newRow("de") << u"de"_s << u"Abbrechen"_s << u"B"_s << u"3 Monaten"_s;
        QTest::newRow("fr") << u"fr"_s << u"Annuler"_s << u"Si♭"_s << u"il y a 3 mois"_s;
        QTest::newRow("es") << u"es"_s << u"Cancelar"_s << u"Si♭"_s << u"hace 3 meses"_s;
    }

    void translations()
    {
        QFETCH(QString, language);
        QFETCH(QString, cancel);
        QFETCH(QString, bFlat);
        QFETCH(QString, monthsAgo);

        LanguageManager manager;
        manager.setLanguage(language);
        QCOMPARE(manager.effectiveLanguage(), language);
        QCOMPARE(qtTrId("common.cancel"), cancel);
        QCOMPARE(qtTrId("metronome.note.b_flat"), bFlat);
        QCOMPARE(qtTrId("candidates.ago.months", 3), monthsAgo);
    }

    void singularPlural()
    {
        LanguageManager manager;
        manager.setLanguage(u"es"_s);
        QVERIFY(qtTrId("import.csv.done", 1).endsWith(u"Se ha añadido 1 entrada."_s));
        QVERIFY(qtTrId("import.csv.done", 12).endsWith(u"Se han añadido 12 entradas."_s));
    }

    void persistsAndSignals()
    {
        {
            LanguageManager manager;
            QSignalSpy spy(&manager, &LanguageManager::languageChanged);
            manager.setLanguage(u"fr"_s);
            QCOMPARE(spy.count(), 1);
            manager.setLanguage(u"fr"_s);            // unchanged: no signal
            QCOMPARE(spy.count(), 1);
            manager.setLanguage(u"xx"_s);            // unsupported: ignored
            QCOMPARE(manager.language(), u"fr"_s);
            QCOMPARE(spy.count(), 1);
        }
        QCOMPARE(QSettings().value(u"ui/language"_s).toString(), u"fr"_s);

        LanguageManager restored;                     // next start
        QCOMPARE(restored.language(), u"fr"_s);
        QCOMPARE(restored.effectiveLanguage(), u"fr"_s);
        QCOMPARE(qtTrId("common.cancel"), u"Annuler"_s);

        restored.setLanguage(u"system"_s);
        QCOMPARE(restored.language(), u"system"_s);
        QCOMPARE(restored.effectiveLanguage(),
                 LanguageManager::resolveLanguage(QLocale::system().uiLanguages()));
    }
};

QTEST_GUILESS_MAIN(TestLanguageManager)
#include "tst_languagemanager.moc"
