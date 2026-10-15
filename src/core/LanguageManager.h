// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTranslator>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

class QQmlEngine;
class QJSEngine;

/*!
    Chooses and installs the UI language.

    English is the source and fallback language. The setting "system" (default)
    picks the first of the user's preferred OS languages that Qupil supports and
    falls back to English otherwise. Translations are switched at runtime; QML
    bindings that use qsTrId() update via QQmlEngine::retranslate().

    Create exactly one instance in main() before the QML engine loads, then call
    setEngine(). In QML it is available as the singleton "LanguageManager".
*/
class LanguageManager : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged FINAL)
    Q_PROPERTY(QString effectiveLanguage READ effectiveLanguage NOTIFY languageChanged FINAL)
    Q_PROPERTY(QVariantList languages READ languages CONSTANT FINAL)

public:
    explicit LanguageManager(QObject *parent = nullptr);
    ~LanguageManager() override;

    static LanguageManager *instance();
    static LanguageManager *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    //! "system" or one of supportedLanguages()
    QString language() const { return m_language; }
    void setLanguage(const QString &language);

    //! The language actually in use, e.g. "de"
    QString effectiveLanguage() const { return m_effective; }

    //! [{ code: "system" }, { code: "en", nativeName: "English" }, …]
    QVariantList languages() const;

    void setEngine(QQmlEngine *engine);

    static QStringList supportedLanguages();
    //! First supported language in a list such as QLocale::uiLanguages(), else "en"
    static QString resolveLanguage(const QStringList &preferredLanguages);

signals:
    void languageChanged();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void apply();

    QString m_language;
    QString m_effective;
    QTranslator m_base;    // English, always installed: fallback for missing strings
    QTranslator m_app;     // selected language
    QTranslator m_qtBase;  // Qt's own strings (dialog buttons etc.)
    QQmlEngine *m_engine = nullptr;
};
