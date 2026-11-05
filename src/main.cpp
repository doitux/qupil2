// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

#include <QCommandLineParser>
#include <QDirIterator>
#include <QFontDatabase>
#include <QGuiApplication>
#ifdef Q_OS_ANDROID
#include <android/log.h>
#endif
#include <QIcon>
#ifdef Q_OS_ANDROID
#include <QJniObject>
#endif
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
#include "KeyboardHeight.h"
#endif
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickStyle>
#include <QScreen>
#include <QSettings>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTimer>

#include <cstdio>
#include <memory>

#include "AppController.h"
#include "BackKey.h"
#include "Clock.h"
#include "DataService.h"
#include "DocumentService.h"
#include "LanguageManager.h"
#include "LessonEndAlert.h"
#include "ReminderAlert.h"
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

//! Desktop: the window opens where and as large as it was when Qupil was quit
void placeWindow(QQuickWindow *window)
{
    QSettings s;
    s.beginGroup(u"window"_s);
    const QRect saved = s.value(u"geometry"_s).toRect();
    const bool maximized = s.value(u"maximized"_s).toBool();
    s.endGroup();
    if (saved.isValid()) {
        // Only if a screen still shows enough of it (a monitor may be gone)
        for (const QScreen *screen : QGuiApplication::screens()) {
            const QRect visible = screen->availableGeometry().intersected(saved);
            if (visible.width() >= 200 && visible.height() >= 150) {
                window->setGeometry(saved);
                break;
            }
        }
    }
    // Saved shortly after every change, not only when Qupil is quit: closed by logging
    // out or shutting down, a desktop does not always let it quit in an orderly way.
    // The size before maximizing is what counts for a maximized window; it is read
    // a moment after a change, as window managers report the new size before the
    // new state.
    auto normal = std::make_shared<QRect>(window->geometry());
    auto *settle = new QTimer(window);
    settle->setSingleShot(true);
    settle->setInterval(700);
    const auto save = [window, normal] {
        const QWindow::Visibility v = window->visibility();
        if (v == QWindow::Windowed)
            *normal = window->geometry();
        if (v != QWindow::Windowed && v != QWindow::Maximized)
            return;   // minimized, full screen, hidden: keep what was saved
        QSettings t;
        t.beginGroup(u"window"_s);
        t.setValue(u"geometry"_s, *normal);
        t.setValue(u"maximized"_s, v == QWindow::Maximized);
    };
    QObject::connect(settle, &QTimer::timeout, window, save);
    const auto changed = [settle] { settle->start(); };
    QObject::connect(window, &QWindow::xChanged, window, changed);
    QObject::connect(window, &QWindow::yChanged, window, changed);
    QObject::connect(window, &QWindow::widthChanged, window, changed);
    QObject::connect(window, &QWindow::heightChanged, window, changed);
    QObject::connect(window, &QWindow::visibilityChanged, window, changed);
    QObject::connect(qApp, &QCoreApplication::aboutToQuit, window, [settle, save] {
        if (settle->isActive()) {
            settle->stop();
            save();
        }
    });
    if (maximized)
        window->showMaximized();
    else
        window->show();
}

} // namespace

#ifdef Q_OS_ANDROID
// Milestones of the start in Android's log (adb logcat -s Qupil): a window stayed black
// after Android had recreated the activity; these show how far Qupil got
#define QUPIL_BOOT(...) __android_log_print(ANDROID_LOG_INFO, "Qupil", __VA_ARGS__)
#else
#define QUPIL_BOOT(...) do {} while (false)
#endif

int main(int argc, char *argv[])
{
    QUPIL_BOOT("main: start");
    QGuiApplication app(argc, argv);
    QUPIL_BOOT("main: application created");
#ifdef Q_OS_ANDROID
    QObject::connect(&app, &QGuiApplication::applicationStateChanged, &app, [](Qt::ApplicationState state) {
        QUPIL_BOOT("main: application state %d", int(state));
    });
#endif
    QGuiApplication::setOrganizationName(u"Qupil"_s);
    QGuiApplication::setOrganizationDomain(u"qupil.de"_s);
    QGuiApplication::setApplicationName(u"Qupil"_s);
    QGuiApplication::setApplicationVersion(QStringLiteral(QUPIL_VERSION));
#if !defined(Q_OS_MACOS) && !defined(Q_OS_IOS) && !defined(Q_OS_ANDROID)
    // Title bar and task bar on Windows and Linux (the Dock on macOS keeps the bundle's
    // icon, which follows Apple's grid; phones and tablets take it from the package)
    QIcon icon;
    for (const int size : {16, 24, 32, 48, 64, 128, 256})
        icon.addFile(u":/qt/qml/Qupil/assets/icons/qupil-%1.png"_s.arg(size), QSize(size, size));
    QGuiApplication::setWindowIcon(icon);
#endif
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    // Wayland shows the icon of the desktop entry (share/applications/de.qupil.app.desktop)
    QGuiApplication::setDesktopFileName(u"de.qupil.app"_s);
#endif

    QCommandLineParser args;
    args.addHelpOption();
    args.addVersionOption();
    const QCommandLineOption dataDirOption(u"data-dir"_s, u"Folder for the database and backups."_s, u"folder"_s);
    const QCommandLineOption langOption(u"lang"_s, u"User interface language (en, de, fr, es, system)."_s, u"code"_s);
    const QCommandLineOption smokeOption(u"smoke-test"_s, u"Load the UI once and fail on QML warnings (CI)."_s);
    const QCommandLineOption screenshotsOption(
        u"screenshots"_s, u"Save screenshots of all screens with sample data into a folder."_s, u"folder"_s);
    const QCommandLineOption dateOption(
        u"date"_s, u"Show the app as if today were this date (sample data, schedule)."_s, u"yyyy-mm-dd"_s);
    args.addOption(dataDirOption);
    args.addOption(langOption);
    args.addOption(smokeOption);
    args.addOption(screenshotsOption);
    args.addOption(dateOption);
    args.process(app);

    const bool smokeTest = args.isSet(smokeOption);
    const bool screenshots = args.isSet(screenshotsOption);
    // The day shown as "today". Screenshots: a Tuesday (lessons, never a weekend),
    // at 14:20 while the first lesson of the day runs
    QDate shownDate;
    if (args.isSet(dateOption)) {
        shownDate = QDate::fromString(args.value(dateOption), Qt::ISODate);
        if (!shownDate.isValid())
            qWarning("--date: not a date (yyyy-mm-dd): %s", qPrintable(args.value(dateOption)));
    }
    if (screenshots)
        Qupil::Clock::pretend(shownDate.isValid() ? shownDate : Qupil::Clock::lastWeekday(QDate::currentDate(), 2),
                              QTime(14, 20));
    else if (shownDate.isValid())
        Qupil::Clock::pretend(shownDate);
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
    QUPIL_BOOT("main: opening data");
    AppController controller(dataDir, nullptr);
    QUPIL_BOOT("main: data open, phase %d", int(controller.phase()));
#ifdef Q_OS_ANDROID
    KeyboardHeight::watch([&controller](qreal height) { controller.setKeyboardHeight(height); });
#endif
#ifdef Q_OS_IOS
    controller.setKeyboardViewHooks(KeyboardHeight::holdView, KeyboardHeight::keepViewInPlace);
#endif
    DataService data(controller.store(), nullptr);
    DocumentService docs(&data, nullptr);
    // Default lists (styles, locations, instruments) in the UI language for new data
    QObject::connect(&controller, &AppController::phaseChanged, &data, [&controller, &data] {
        if (controller.phase() == AppController::Ready)
            data.ensureDefaults();
    });
    QObject::connect(&controller, &AppController::dataReplaced, &data, &DataService::ensureDefaults);
    QObject::connect(&controller, &AppController::testModeChanged, &data, &DataService::ensureDefaults);
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
    // Reminders popping up at start and when a lesson starts
    ReminderAlert reminders(&data, nullptr);
    QObject::connect(&languages, &LanguageManager::languageChanged, &reminders, &ReminderAlert::reschedule);
    QObject::connect(&controller, &AppController::phaseChanged, &reminders, [&controller, &reminders] {
        if (controller.phase() == AppController::Ready)
            reminders.start(false);   // after taking over data: not the "when Qupil starts" ones
    });
    if (screenshots) {
        lessonEnd.setQuiet(true);
        reminders.setQuiet(true);
    }
    if (controller.phase() == AppController::Ready) {
        // As in Qupil 1.x shortly after the window is there
        QTimer::singleShot(1800, &reminders, [&reminders] { reminders.start(true); });
    }
    // Archive entries of Qupil 1.x get the compact layout, in the background once things are calm
    // (also after an import, a restore or a sync that brought such entries)
    if (!smokeTest && !screenshots)
        docs.setArchiveUpkeep(true);
    QUPIL_BOOT("main: services ready");
    QQmlApplicationEngine engine;
    languages.setEngine(&engine);   // retranslate() on a language switch

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app,
                     [] { QCoreApplication::exit(EXIT_FAILURE); }, Qt::QueuedConnection);

    // The window is shown here (before the smoke test and screenshot tour start)
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    const bool desktop = false;
#else
    const bool desktop = true;
#endif
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
                     [desktop, smokeTest, screenshots](QObject *object, const QUrl &) {
                         auto *window = qobject_cast<QQuickWindow *>(object);
                         QUPIL_BOOT("main: interface created, window %s", window ? "yes" : "no");
                         if (!window)
                             return;
                         new BackKey(window);
#ifdef Q_OS_ANDROID
                         // Android's launch screen stays until the first frame with the
                         // interface (QupilActivity.java), then goes straight to it
                         auto ready = std::make_shared<QMetaObject::Connection>();
                         *ready = QObject::connect(window, &QQuickWindow::frameSwapped, qApp, [ready] {
                             QObject::disconnect(*ready);
                             QUPIL_BOOT("main: first frame");
                             QJniObject::callStaticMethod<void>("de/qupil/app/QupilActivity", "contentReady", "()V");
                         }, Qt::QueuedConnection);
                         QObject::connect(window, &QQuickWindow::sceneGraphInitialized, window, [] {
                             QUPIL_BOOT("main: graphics ready");
                         }, Qt::DirectConnection);
                         QObject::connect(window, &QWindow::visibleChanged, window, [window](bool visible) {
                             QUPIL_BOOT("main: window visible %d, exposed %d", int(visible), int(window->isExposed()));
                         });
#endif
                         if (desktop && !smokeTest && !screenshots)
                             placeWindow(window);
                         else
                             window->show();
                     });

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
                                 QTimer::singleShot(300, qApp, [exercise, &engine] {
                                     // Pages and editors are made on first use: make them all,
                                     // so their QML is checked too
                                     if (QObject *window = engine.rootObjects().value(0)) {
                                         for (int page = 4; page >= 0; --page) {
                                             window->setProperty("currentPage", page);
                                             QCoreApplication::processEvents();
                                         }
                                         for (const char *sheet : { "note", "planFinish", "piece", "student", "lesson",
                                                                    "activity", "reminder", "pairing" })
                                             QMetaObject::invokeMethod(window, "sheet", Q_ARG(QVariant, QString::fromLatin1(sheet)));
                                     }
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

    QUPIL_BOOT("main: loading interface");
    engine.loadFromModule("Qupil", "Main");
    QUPIL_BOOT("main: event loop");
    const int code = app.exec();
    QUPIL_BOOT("main: end %d", code);
    return code;
}
