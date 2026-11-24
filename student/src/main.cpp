// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// Qupil Practice ("Qupil – Musik üben"): the students' app for the tasks the teacher's Qupil writes.
//
//   --data-dir <dir>     keep the app's state there (tests, screenshots)
//   --today <yyyy-MM-dd> pretend it is that day (example data and screenshots)
//   --lang <de|en|fr|es> language instead of the phone's
//   --demo               start with the example
//   --screenshot <file>  save the window after loading and quit
//   --size <WxH>         window size (screenshots)
//   --smoke-test         load the interface and quit (CI)
//   --scan               open the QR code scanner at the start (tests on a device)
//   --pretend-scan <text> the scanner counts as there and reads this text (screenshots, tests)

#include "IncomingLinks.h"
#include "KeyboardHeight.h"
#include "QrScanner.h"
#include "StudentApp.h"
#include "Translator.h"

#include <QCommandLineParser>
#include <QDir>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QLocale>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QTimer>
#include <QtQml/QQmlExtensionPlugin>

Q_IMPORT_QML_PLUGIN(QupilStudentPlugin)

using namespace Qt::StringLiterals;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setOrganizationName(u"Qupil"_s);
    QGuiApplication::setOrganizationDomain(u"qupil.de"_s);
    QGuiApplication::setApplicationName(u"Qupil Student"_s);
    QGuiApplication::setApplicationVersion(QStringLiteral(QUPIL_STUDENT_VERSION));

    QCommandLineParser parser;
    parser.addHelpOption();
    const QCommandLineOption dataDir(u"data-dir"_s, u"State directory"_s, u"dir"_s);
    const QCommandLineOption today(u"today"_s, u"Pretend it is this day"_s, u"yyyy-MM-dd"_s);
    const QCommandLineOption lang(u"lang"_s, u"Language"_s, u"code"_s);
    const QCommandLineOption demo(u"demo"_s, u"Start with the example"_s);
    const QCommandLineOption screenshot(u"screenshot"_s, u"Save the window and quit"_s, u"file"_s);
    const QCommandLineOption smoke(u"smoke-test"_s, u"Load and quit"_s);
    const QCommandLineOption size(u"size"_s, u"Window size (screenshots)"_s, u"WxH"_s);
    const QCommandLineOption scan(u"scan"_s, u"Open the QR code scanner at the start"_s);
    const QCommandLineOption pretendScan(u"pretend-scan"_s, u"The scanner reads this text"_s, u"text"_s);
    parser.addOptions({dataDir, today, lang, demo, screenshot, smoke, size, scan, pretendScan});
    parser.process(app);

    if (parser.isSet(lang))
        QLocale::setDefault(QLocale(parser.value(lang)));
    if (parser.isSet(dataDir))
        StudentApp::setDataDir(parser.value(dataDir));
    if (parser.isSet(today))
        StudentApp::setToday(QDate::fromString(parser.value(today), Qt::ISODate));
    if (parser.isSet(pretendScan))
        QrScanner::pretend({ parser.value(pretendScan), false });

    Translator translator;
    const QString language = QLocale().name().section(u'_', 0, 0);
    if (translator.load(u":/i18n/strings.json"_s, language))
        QCoreApplication::installTranslator(&translator);

    for (const char *weight : {"Regular", "SemiBold", "Bold", "ExtraBold"})
        QFontDatabase::addApplicationFont(u":/fonts/Figtree-%1.ttf"_s.arg(QLatin1StringView(weight)));
    QQuickStyle::setStyle(u"Basic"_s);

    QQmlApplicationEngine engine;
    engine.addImportPath(u"qrc:/qt/qml"_s);   // default from Qt 6.5 on
    StudentApp *studentApp = StudentApp::instance();

    // Links from the camera (QR code) and from messages: https://qupil.de/u#… and qupil-student:…
    IncomingLinks::watch(studentApp, [studentApp](const QUrl &url) { studentApp->openUrl(url); });
    // Android: the on-screen keyboard's height (the window keeps its size, the pages make room)
    KeyboardHeight::watch([studentApp](qreal height) { studentApp->setKeyboardHeight(height); });
    // Opened with a link on the desktop (handy for trying it out)
    for (const QString &argument : parser.positionalArguments())
        studentApp->openUrl(QUrl(argument));
    if (parser.isSet(demo))
        studentApp->openDemo();

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed, &app,
                     [] { QCoreApplication::exit(1); }, Qt::QueuedConnection);
    engine.load(QUrl(u"qrc:/qt/qml/QupilStudent/qml/Main.qml"_s));
    if (engine.rootObjects().isEmpty()) {
        IncomingLinks::unwatch();
        return 1;
    }

    // For the tests on devices and simulators: the interface is loaded
    qInfo("Qupil student: started (%d teacher(s))", int(studentApp->teachers().size()));

    if (parser.isSet(size)) {
        auto *window = qobject_cast<QQuickWindow *>(engine.rootObjects().constFirst());
        const QStringList wh = parser.value(size).split(u'x');
        if (window && wh.size() == 2)
            window->resize(wh.at(0).toInt(), wh.at(1).toInt());
    }
    if (parser.isSet(scan))
        QTimer::singleShot(1500, studentApp, &StudentApp::scan);
    if (parser.isSet(smoke))
        QTimer::singleShot(500, &app, &QCoreApplication::quit);
    if (parser.isSet(screenshot)) {
        auto *window = qobject_cast<QQuickWindow *>(engine.rootObjects().constFirst());
        const QString file = parser.value(screenshot);
        QTimer::singleShot(1200, &app, [window, file] {
            const bool ok = window->grabWindow().save(file);
            QCoreApplication::exit(ok ? 0 : 2);
        });
    }
    const int result = app.exec();
    IncomingLinks::unwatch();
    return result;
}
