// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

#include <QDirIterator>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QTimer>

#include <cstdio>

#include "LanguageManager.h"

using namespace Qt::StringLiterals;

namespace {

// --smoke-test: load the UI once, quit, and fail on any QML warning.
// Used by CI (ctest "qml_smoke") on every desktop platform.
int s_qmlProblems = 0;
QtMessageHandler s_previousHandler = nullptr;

void smokeTestHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    const bool fromQml = (context.file && QByteArray(context.file).endsWith(".qml"))
                         || message.contains(u".qml:"_s);
    if (type >= QtWarningMsg && fromQml)
        ++s_qmlProblems;
    if (s_previousHandler)
        s_previousHandler(type, context, message);
}

} // namespace

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setOrganizationName(u"Qupil"_s);
    QGuiApplication::setOrganizationDomain(u"qupil.de"_s);
    QGuiApplication::setApplicationName(u"Qupil"_s);
    QGuiApplication::setApplicationVersion(QStringLiteral(QUPIL_VERSION));
    QGuiApplication::setWindowIcon(QIcon(u":/qt/qml/Qupil/assets/icons/qupil.png"_s));

    const bool smokeTest = QCoreApplication::arguments().contains(u"--smoke-test"_s);
    if (smokeTest) {
        // Keep the user's real settings untouched
        QGuiApplication::setOrganizationName(u"QupilSmokeTest"_s);
        s_previousHandler = qInstallMessageHandler(smokeTestHandler);
    }

    // Figtree in four weights, bundled so the app looks the same everywhere
    QDirIterator fonts(u":/qt/qml/Qupil/assets/fonts"_s, { u"*.ttf"_s });
    while (fonts.hasNext()) {
        if (QFontDatabase::addApplicationFont(fonts.next()) < 0)
            qWarning("Could not load font %s", qPrintable(fonts.filePath()));
    }

    // Our own look on every platform; the colours come from Theme.qml
    QQuickStyle::setStyle(u"Basic"_s);

    LanguageManager languages(nullptr);  // translators are installed before any UI exists
    QQmlApplicationEngine engine;
    languages.setEngine(&engine);   // retranslate() on a language switch

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app,
                     [] { QCoreApplication::exit(EXIT_FAILURE); }, Qt::QueuedConnection);

    if (smokeTest) {
        QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
                         [&engine, &languages](QObject *object, const QUrl &) {
                             if (!object)
                                 return;
                             // Exercise live language and colour scheme switching, then quit
                             QTimer::singleShot(200, qApp, [&engine, &languages] {
                                 for (const QString &lang : LanguageManager::supportedLanguages()) {
                                     languages.setLanguage(lang);
                                     QCoreApplication::processEvents();
                                 }
                                 languages.setLanguage(u"system"_s);
                                 if (QObject *theme = engine.singletonInstance<QObject *>("Qupil", "Theme")) {
                                     for (const char *mode : { "dark", "light", "system" }) {
                                         theme->setProperty("mode", QString::fromLatin1(mode));
                                         QCoreApplication::processEvents();
                                     }
                                 } else {
                                     std::fprintf(stderr, "smoke test: Theme singleton not found\n");
                                     ++s_qmlProblems;
                                 }
                                 QTimer::singleShot(200, qApp, [] {
                                     std::fprintf(stderr, "smoke test: %d QML problem(s)\n", s_qmlProblems);
                                     QCoreApplication::exit(s_qmlProblems == 0 ? EXIT_SUCCESS : 2);
                                 });
                             });
                         });
    }

    engine.loadFromModule("Qupil", "Main");
    return app.exec();
}
