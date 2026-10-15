// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "LanguageManager.h"

#include <QCoreApplication>
#include <QEvent>
#include <QJSEngine>
#include <QLibraryInfo>
#include <QLocale>
#include <QQmlEngine>
#include <QSettings>
#include <QVariantMap>

using namespace Qt::StringLiterals;

namespace {
LanguageManager *s_instance = nullptr;
constexpr auto kSettingsKey = "ui/language";
constexpr auto kSystem = "system";

struct Language { const char *code; const char *nativeName; };
// Order = order in the settings list. Add new languages here and as TS file in CMake.
constexpr Language kLanguages[] = {
    { "en", "English" },
    { "de", "Deutsch" },
    { "fr", "Français" },
    { "es", "Español" },
};
} // namespace

LanguageManager::LanguageManager(QObject *parent)
    : QObject(parent)
{
    Q_ASSERT_X(!s_instance, "LanguageManager", "only one instance allowed");
    s_instance = this;

    const QString stored = QSettings().value(QLatin1StringView(kSettingsKey), QLatin1StringView(kSystem)).toString();
    m_language = (stored == QLatin1StringView(kSystem) || supportedLanguages().contains(stored))
                     ? stored : QString::fromLatin1(kSystem);
    apply();
    QCoreApplication::instance()->installEventFilter(this);
}

LanguageManager::~LanguageManager()
{
    if (s_instance == this)
        s_instance = nullptr;
}

LanguageManager *LanguageManager::instance()
{
    return s_instance;
}

LanguageManager *LanguageManager::create(QQmlEngine *qmlEngine, QJSEngine *)
{
    Q_ASSERT_X(s_instance, "LanguageManager", "create the instance in main() before loading QML");
    QJSEngine::setObjectOwnership(s_instance, QJSEngine::CppOwnership);
    s_instance->setEngine(qmlEngine);
    return s_instance;
}

QStringList LanguageManager::supportedLanguages()
{
    QStringList list;
    for (const auto &l : kLanguages)
        list << QString::fromLatin1(l.code);
    return list;
}

QString LanguageManager::resolveLanguage(const QStringList &preferredLanguages)
{
    const QStringList supported = supportedLanguages();
    for (const QString &entry : preferredLanguages) {
        // "de-DE", "de_AT", "fr", "es-419" → "de", "de", "fr", "es"
        const QString code = entry.section(u'-', 0, 0).section(u'_', 0, 0).toLower();
        if (supported.contains(code))
            return code;
    }
    return u"en"_s;
}

void LanguageManager::setLanguage(const QString &language)
{
    if (language == m_language)
        return;
    if (language != QLatin1StringView(kSystem) && !supportedLanguages().contains(language))
        return;
    m_language = language;
    QSettings().setValue(QLatin1StringView(kSettingsKey), m_language);
    apply();
    emit languageChanged();
}

QVariantList LanguageManager::languages() const
{
    QVariantList list;
    list << QVariantMap{ { u"code"_s, QString::fromLatin1(kSystem) } };
    for (const auto &l : kLanguages)
        list << QVariantMap{ { u"code"_s, QString::fromLatin1(l.code) },
                             { u"nativeName"_s, QString::fromUtf8(l.nativeName) } };
    return list;
}

void LanguageManager::setEngine(QQmlEngine *engine)
{
    m_engine = engine;
}

void LanguageManager::apply()
{
    const QString lang = m_language == QLatin1StringView(kSystem)
                             ? resolveLanguage(QLocale::system().uiLanguages())
                             : m_language;

    QCoreApplication *app = QCoreApplication::instance();
    app->removeTranslator(&m_qtBase);
    app->removeTranslator(&m_app);
    app->removeTranslator(&m_base);

    // English first: Qt searches translators in reverse order of installation,
    // so anything missing in the selected language falls back to English.
    if (m_base.load(u":/i18n/qupil_en.qm"_s))
        app->installTranslator(&m_base);
    else
        qWarning("LanguageManager: :/i18n/qupil_en.qm not found");

    if (lang != u"en" && m_app.load(u":/i18n/qupil_%1.qm"_s.arg(lang)))
        app->installTranslator(&m_app);

    // Qt's own strings. Desktop builds find them in the Qt installation; for
    // iOS/Android copy qtbase_*.qm into the :/i18n resource (see README).
    const QLocale uiLocale(lang);
    if (m_qtBase.load(uiLocale, u"qtbase"_s, u"_"_s, QLibraryInfo::path(QLibraryInfo::TranslationsPath))
        || m_qtBase.load(uiLocale, u"qtbase"_s, u"_"_s, u":/i18n"_s))
        app->installTranslator(&m_qtBase);

    // Language of the UI from the setting, date and number formats from the user's region.
    QLocale::setDefault(QLocale(uiLocale.language(), QLocale::system().territory()));

    m_effective = lang;
    if (m_engine)
        m_engine->retranslate();
}

bool LanguageManager::eventFilter(QObject *watched, QEvent *event)
{
    // The user changed the OS language while Qupil is running (desktop).
    if (event->type() == QEvent::LocaleChange && watched == QCoreApplication::instance()
        && m_language == QLatin1StringView(kSystem)) {
        const QString before = m_effective;
        apply();
        if (m_effective != before)
            emit languageChanged();
    }
    return QObject::eventFilter(watched, event);
}
