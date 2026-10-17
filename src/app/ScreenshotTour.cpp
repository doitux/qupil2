// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "ScreenshotTour.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QImage>
#include <QQuickWindow>
#include <QTimer>

#include <cstdio>
#include <iterator>

#include "AppController.h"
#include "ReminderAlert.h"
#include "DataService.h"
#include "DocumentService.h"

using namespace Qt::StringLiterals;

namespace {

constexpr int kSettleMs = 450;      // animations and layouts settle
constexpr int kPollMs = 100;
constexpr int kTimeoutMs = 30000;

const char *const kPages[] = { "today", "students", "events", "metronome", "more" };

} // namespace

ScreenshotTour::ScreenshotTour(QQuickWindow *window, QObject *theme, QObject *nav, AppController *app,
                               const QString &folder, QObject *parent)
    : QObject(parent)
    , m_window(window)
    , m_theme(theme)
    , m_nav(nav)
    , m_app(app)
    , m_folder(folder)
{
    QDir().mkpath(m_folder);

    auto mode = [this](const char *m) {
        return Step { [this, m] {
                          if (m_theme)
                              m_theme->setProperty("mode", QString::fromLatin1(m));
                      },
                      {} };
    };
    auto size = [this](int w, int h) {
        return Step { [this, w, h] {
                          m_window->setProperty("currentPage", 0);
                          m_window->resize(w, h);
                      },
                      {} };
    };
    auto snap = [this](const QString &name) { return Step { [this, name] { shot(name); }, {} }; };
    // Scrolls the first Flickable of the window (setup screen on a phone)
    auto scroll = [this](int y) {
        return Step { [this, y] {
                          for (QObject *o : m_window->findChildren<QObject *>()) {
                              if (o->inherits("QQuickFlickable")) {
                                  o->setProperty("contentY", y);
                                  break;
                              }
                          }
                      },
                      {} };
    };

    // Setup screen
    m_steps << size(1180, 800) << mode("light") << snap(u"01-setup-light"_s)
            << mode("dark") << snap(u"02-setup-dark"_s) << mode("light")
            << size(390, 844) << snap(u"01-setup-phone"_s) << scroll(600) << snap(u"01-setup-phone-2"_s)
            << size(1180, 800);
    // Sample data through the real 1.x import
    m_steps << Step { [this] { m_app->loadDemo(); },
                      [this] { return !m_app->busy() && !m_app->lastTakeOver().isEmpty(); } }
            << snap(u"03-imported-light"_s)
            << Step { [this] { m_app->finishSetup(); }, [this] { return m_app->phase() == AppController::Ready; } }
            << Step { [this] { writeDocuments(); }, {} };
    addPages(u"1-desktop-light"_s);
    addDetails(u"1-desktop-light"_s);
    m_steps << mode("dark");
    addPages(u"2-desktop-dark"_s);
    addDetails(u"2-desktop-dark"_s);
    m_steps << mode("light") << size(390, 844);
    addPages(u"3-phone-light"_s);
    addDetails(u"3-phone-light"_s);
    m_steps << mode("dark");
    addPages(u"4-phone-dark"_s);
    addDetails(u"4-phone-dark"_s);
}

void ScreenshotTour::addPages(const QString &prefix)
{
    for (int i = 0; i < int(std::size(kPages)); ++i) {
        const QString name = u"%1-%2-%3"_s.arg(prefix).arg(i).arg(QLatin1StringView(kPages[i]));
        m_steps << Step { [this, i] {
                              m_window->setProperty("currentPage", i);
                              // A lesson is running in the pictures: 14:20 today
                              const QDateTime lessonTime(QDate::currentDate(), QTime(14, 20));
                              for (QObject *o : m_window->findChildren<QObject *>(u"scheduleOverview"_s))
                                  o->setProperty("now", lessonTime);
                          },
                          {} }
                << Step { [this, name] { shot(name); }, {} };
    }
}

// A student with lessons, an event with a programme, the note and piece editors
void ScreenshotTour::addDetails(const QString &prefix)
{
    auto nav = [this](const char *signal, const QString &a, const QString &b = {}) {
        return Step { [this, signal, a, b] {
                          QObject *nav = m_nav;
                          if (!nav)
                              return;
                          if (b.isNull())
                              QMetaObject::invokeMethod(nav, signal, Q_ARG(QString, a));
                          else
                              QMetaObject::invokeMethod(nav, signal, Q_ARG(QString, a), Q_ARG(QString, b));
                      },
                      {} };
    };
    auto snap = [this](const QString &name) { return Step { [this, name] { shot(name); }, {} }; };
    auto close = Step { [this] { QMetaObject::invokeMethod(m_window, "closeEditors"); }, {} };
    auto more = [this](const char *subPage) {
        return Step { [this, subPage] {
                          m_window->setProperty("currentPage", 4);
                          for (QObject *o : m_window->findChildren<QObject *>(u"morePage"_s))
                              QMetaObject::invokeMethod(o, "showSubPage", Q_ARG(QVariant, QString::fromLatin1(subPage)));
                      },
                      {} };
    };

    m_steps << Step { [this] { pickExamples(); }, {} };
    m_steps << Step { [this, nav] { nav("showStudent", m_student, m_membership).action(); }, {} }
            << snap(prefix + u"-5-student"_s)
            << Step { [this] { revealNote(m_note); }, {} }
            << snap(prefix + u"-5b-note-actions"_s)
            << Step { [this] { revealNote(QString()); }, {} }
            << Step { [this] { callAll("openMoreMenu()"); }, {} }
            << snap(prefix + u"-5d-student-menu"_s)
            << Step { [this] { callAll("closeMoreMenu()"); }, {} }
            << Step { [this, nav] { nav("newNoteFrom", m_membership, m_note).action(); }, {} }
            << snap(prefix + u"-5c-note-copy"_s) << close
            << Step { [this, nav] { nav("editNote", m_membership, u""_s).action(); }, {} }
            << snap(prefix + u"-6-note-editor"_s) << close
            << Step { [this, nav] { nav("editPiece", m_membership, m_piece).action(); }, {} }
            << snap(prefix + u"-7-piece-editor"_s) << close
            << Step { [this, nav] { nav("showEvent", m_event).action(); }, {} }
            << snap(prefix + u"-8-event"_s)
            << Step { [this] {
                          if (m_nav)
                              QMetaObject::invokeMethod(m_nav, "editLesson", Q_ARG(QString, m_lesson), Q_ARG(int, -1));
                      },
                      {} }
            << snap(prefix + u"-9-lesson-editor"_s)
            << Step { [this] { callAll("openList()"); }, {} }
            << snap(prefix + u"-9b-lesson-students"_s)
            << Step { [this] { callAll("closeList()"); }, {} } << close
            << Step { [this] {
                          if (m_nav)
                              QMetaObject::invokeMethod(m_nav, "showSync");
                      },
                      {} }
            << snap(prefix + u"-10-sync"_s)
            << Step { [this] {
                          if (m_nav)
                              QMetaObject::invokeMethod(m_nav, "pairDevice");
                      },
                      {} }
            << snap(prefix + u"-11-pairing"_s) << close
            << more("library") << snap(prefix + u"-12-library"_s)
            << more("instruments") << snap(prefix + u"-13-instruments"_s)
            << more("ensemble") << snap(prefix + u"-14-ensemble"_s)
            << more("ready") << snap(prefix + u"-14b-ready"_s)
            << more("archive") << snap(prefix + u"-15-archive"_s)
            << more("archiveEntry") << snap(prefix + u"-16-archive-entry"_s)
            << more("settings") << snap(prefix + u"-17-settings"_s)
            << more("appearance") << snap(prefix + u"-17a-appearance"_s)
            << more("alerts") << snap(prefix + u"-17b-alerts"_s)
            << more("lessonSettings") << snap(prefix + u"-17c-lesson-settings"_s)
            << more("lists") << snap(prefix + u"-17d-lists"_s)
            << more("data") << snap(prefix + u"-17e-data"_s)
            << more("about") << snap(prefix + u"-18-about"_s)
            << more("license") << snap(prefix + u"-19-license"_s)
            << Step { [] {
                          if (ReminderAlert *reminders = ReminderAlert::instance())
                              reminders->showAll();
                      },
                      {} }
            << snap(prefix + u"-20-reminder"_s) << close
            << Step { [this] {
                          for (QObject *o : m_window->findChildren<QObject *>(u"morePage"_s))
                              QMetaObject::invokeMethod(o, "home");
                          m_window->setProperty("currentPage", 0);
                      },
                      {} };
}

// The PDF documents with the sample data, next to the screenshots
void ScreenshotTour::writeDocuments()
{
    DocumentService *docs = DocumentService::instance();
    DataService *data = DataService::instance();
    if (!docs || !data)
        return;
    pickExamples();
    const QString folder = m_folder + u"/documents"_s;
    const QString today = QDate::currentDate().toString(Qt::ISODate);
    docs->writePdf(docs->dayOverviewHtml(today, 3, 2), u"day-overview"_s, false, folder);
    docs->writePdf(docs->timetableHtml(), u"timetable"_s, false, folder);
    docs->writePdf(docs->rentalListHtml(), u"rental-instruments"_s, false, folder);
    if (!m_event.isEmpty())
        docs->writePdf(docs->programmeHtml(m_event), u"programme"_s, true, folder);
    if (!m_student.isEmpty())
        docs->writePdf(docs->studentArchiveHtml(m_student), u"student-archive"_s, false, folder);
}

void ScreenshotTour::callAll(const char *method)
{
    const QByteArray signature = QMetaObject::normalizedSignature(method);
    const QByteArray name = signature.left(signature.indexOf('('));
    for (QObject *o : m_window->findChildren<QObject *>()) {
        if (o->metaObject()->indexOfMethod(signature.constData()) >= 0)
            QMetaObject::invokeMethod(o, name.constData());
    }
}

void ScreenshotTour::revealNote(const QString &noteId)
{
    for (QObject *o : m_window->findChildren<QObject *>()) {
        if (o->property("revealedNote").isValid())
            o->setProperty("revealedNote", noteId);
    }
}

void ScreenshotTour::pickExamples()
{
    DataService *data = DataService::instance();
    if (!data)
        return;
    // The student of the first group lesson today: notes shared with the group
    for (const QVariant &s : data->students()) {
        const QVariantMap student = data->student(s.toMap().value(u"id"_s).toString());
        const QVariantList memberships = student.value(u"memberships"_s).toList();
        if (memberships.isEmpty())
            continue;
        m_student = student.value(u"id"_s).toString();
        m_membership = memberships.first().toMap().value(u"id"_s).toString();
        m_lesson = memberships.first().toMap().value(u"lesson_id"_s).toString();
        const QVariantList pieces = data->pieces(m_membership);
        m_piece = pieces.isEmpty() ? QString() : pieces.first().toMap().value(u"id"_s).toString();
        const QVariantList notes = data->notes(m_membership);
        m_note = notes.isEmpty() ? QString() : notes.first().toMap().value(u"id"_s).toString();
        break;
    }
    for (const QVariant &e : data->events()) {
        if (e.toMap().value(u"state"_s).toInt() == 0) {
            m_event = e.toMap().value(u"id"_s).toString();
            break;
        }
    }
}

void ScreenshotTour::start()
{
    QTimer::singleShot(kSettleMs, this, &ScreenshotTour::run);
}

void ScreenshotTour::run()
{
    if (!m_window || m_index >= m_steps.size()) {
        std::fprintf(stderr, "screenshots: %d saved in %s\n", m_saved, qPrintable(QDir::toNativeSeparators(m_folder)));
        QCoreApplication::exit(m_saved > 0 ? EXIT_SUCCESS : EXIT_FAILURE);
        return;
    }
    Step &step = m_steps[m_index];
    if (m_waited == 0 && step.action)
        step.action();
    if (step.ready && !step.ready()) {
        m_waited += kPollMs;
        if (m_waited > kTimeoutMs) {
            std::fprintf(stderr, "screenshots: step %d timed out\n", m_index);
            QCoreApplication::exit(EXIT_FAILURE);
            return;
        }
        QTimer::singleShot(kPollMs, this, &ScreenshotTour::run);
        return;
    }
    m_waited = 0;
    ++m_index;
    QTimer::singleShot(kSettleMs, this, &ScreenshotTour::run);
}

void ScreenshotTour::shot(const QString &name)
{
    const QImage image = m_window->grabWindow();
    const QString file = QDir(m_folder).filePath(name + u".png"_s);
    if (!image.isNull() && image.save(file))
        ++m_saved;
    else
        std::fprintf(stderr, "screenshots: could not save %s\n", qPrintable(file));
}
