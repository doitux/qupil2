// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

#include <QCommandLineParser>
#include <QDirIterator>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickStyle>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTimer>

#include <cstdio>
#include <memory>

#include "AppController.h"
#include "DataService.h"
#include "DocumentService.h"
#include "LanguageManager.h"
#include "LessonEndAlert.h"
#include "ScreenshotTour.h"
#include "SyncController.h"
#include "WifiController.h"

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

    QCommandLineParser args;
    args.addHelpOption();
    args.addVersionOption();
    const QCommandLineOption dataDirOption(u"data-dir"_s, u"Folder for the database and backups."_s, u"folder"_s);
    const QCommandLineOption langOption(u"lang"_s, u"User interface language (en, de, fr, es, system)."_s, u"code"_s);
    const QCommandLineOption smokeOption(u"smoke-test"_s, u"Load the UI once and fail on QML warnings (CI)."_s);
    const QCommandLineOption screenshotsOption(
        u"screenshots"_s, u"Save screenshots of all screens with sample data into a folder."_s, u"folder"_s);
    args.addOption(dataDirOption);
    args.addOption(langOption);
    args.addOption(smokeOption);
    args.addOption(screenshotsOption);
    args.process(app);

    const bool smokeTest = args.isSet(smokeOption);
    const bool screenshots = args.isSet(screenshotsOption);
    QTemporaryDir scratchDir;
    if (smokeTest || screenshots) {
        // Keep the user's real settings and data untouched
        QGuiApplication::setOrganizationName(smokeTest ? u"QupilSmokeTest"_s : u"QupilScreenshots"_s);
    }
    if (smokeTest)
        s_previousHandler = qInstallMessageHandler(smokeTestHandler);
    const QString dataDir = args.isSet(dataDirOption) ? args.value(dataDirOption)
                            : (smokeTest || screenshots) ? scratchDir.path()
                                                         : QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    // Figtree in four weights, bundled so the app looks the same everywhere
    QDirIterator fonts(u":/qt/qml/Qupil/assets/fonts"_s, { u"*.ttf"_s });
    while (fonts.hasNext()) {
        if (QFontDatabase::addApplicationFont(fonts.next()) < 0)
            qWarning("Could not load font %s", qPrintable(fonts.filePath()));
    }

    // Our own look on every platform; the colours come from Theme.qml
    QQuickStyle::setStyle(u"Basic"_s);

    LanguageManager languages(nullptr);  // translators are installed before any UI exists
    if (args.isSet(langOption))
        languages.setLanguage(args.value(langOption));
    AppController controller(dataDir, nullptr);
    DataService data(controller.store(), nullptr);
    DocumentService docs(&data, nullptr);
    // Default lists (styles, locations, instruments) in the UI language for new data
    QObject::connect(&controller, &AppController::phaseChanged, &data, [&controller, &data] {
        if (controller.phase() == AppController::Ready)
            data.ensureDefaults();
    });
    QObject::connect(&controller, &AppController::dataReplaced, &data, &DataService::ensureDefaults);
    if (controller.phase() == AppController::Ready)
        data.ensureDefaults();
    SyncController sync(&controller, nullptr);
    QObject::connect(&sync, &SyncController::firstSyncDone, &data, &DataService::ensureDefaults);
    WifiController wifi(&controller, &sync, nullptr);
    QObject::connect(&wifi, &WifiController::firstSyncDone, &data, &DataService::ensureDefaults);
    // Sound and message shortly before a lesson ends (texts in the UI language)
    LessonEndAlert lessonEnd(&data, nullptr);
    QObject::connect(&languages, &LanguageManager::languageChanged, &lessonEnd, &LessonEndAlert::reschedule);
    QObject::connect(&controller, &AppController::phaseChanged, &lessonEnd, &LessonEndAlert::reschedule);
    QObject::connect(&controller, &AppController::dataReplaced, &lessonEnd, &LessonEndAlert::reschedule);
    QQmlApplicationEngine engine;
    languages.setEngine(&engine);   // retranslate() on a language switch

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app,
                     [] { QCoreApplication::exit(EXIT_FAILURE); }, Qt::QueuedConnection);

    if (smokeTest) {
        QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
                         [&engine, &languages, &controller](QObject *object, const QUrl &) {
                             if (!object)
                                 return;
                             // Exercise live language and colour scheme switching, then quit
                             // Setup screen first (empty data folder), then the main UI
                             QTimer::singleShot(200, qApp, [&engine, &languages, &controller] {
                                 const auto exercise = [&engine, &languages] {
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
                                         // Own colours, also a light main and a dark highlight colour
                                         const char *colours[][2] = { { "#1F4E8C", "#FFE066" }, { "#FFE066", "#3F4A5A" },
                                                                      { "", "" } };
                                         for (const auto &pair : colours) {
                                             theme->setProperty("accentChoice", QString::fromLatin1(pair[0]));
                                             theme->setProperty("highlightChoice", QString::fromLatin1(pair[1]));
                                             QCoreApplication::processEvents();
                                         }
                                     } else {
                                         std::fprintf(stderr, "smoke test: Theme singleton not found\n");
                                         ++s_qmlProblems;
                                     }
                                 };
                                 exercise();
                                 if (!controller.startFresh()) {
                                     std::fprintf(stderr, "smoke test: could not create a database\n");
                                     ++s_qmlProblems;
                                 }
                                 QTimer::singleShot(300, qApp, [exercise] {
                                     exercise();
                                     QTimer::singleShot(200, qApp, [] {
                                         std::fprintf(stderr, "smoke test: %d QML problem(s)\n", s_qmlProblems);
                                         QCoreApplication::exit(s_qmlProblems == 0 ? EXIT_SUCCESS : 2);
                                     });
                                 });
                             });
                         });
    }

    std::unique_ptr<ScreenshotTour> tour;
    if (screenshots) {
        QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
                         [&engine, &controller, &tour, &args, &screenshotsOption](QObject *object, const QUrl &) {
                             auto *window = qobject_cast<QQuickWindow *>(object);
                             if (!window)
                                 return;
                             QObject *theme = engine.singletonInstance<QObject *>("Qupil", "Theme");
                             QObject *nav = engine.singletonInstance<QObject *>("Qupil", "Nav");
                             tour = std::make_unique<ScreenshotTour>(window, theme, nav, &controller,
                                                                     args.value(screenshotsOption));
                             tour->start();
                         });
    }

    engine.loadFromModule("Qupil", "Main");
    return app.exec();
}
