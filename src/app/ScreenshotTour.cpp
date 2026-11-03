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
#include "Clock.h"
#include "ReminderAlert.h"
#include "DataService.h"
#include "DocumentService.h"
#include "LanguageManager.h"

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
    // An older copy started on data of a newer Qupil
    m_steps << scroll(0) << Step { [this] { m_app->simulateNewerData(true); }, {} } << snap(u"01-older-copy-light"_s)
            << mode("dark") << snap(u"02-older-copy-dark"_s) << mode("light")
            << size(390, 844) << snap(u"01-older-copy-phone"_s) << size(1180, 800)
            << Step { [this] { m_app->simulateNewerData(false); }, {} };
    // Sample data through the real 1.x import
    m_steps << Step { [this] { m_app->loadDemo(); },
                      [this] { return !m_app->busy() && !m_app->lastTakeOver().isEmpty(); } }
            << snap(u"03-imported-light"_s)
            << Step { [this] { m_app->finishSetup(); }, [this] { return m_app->phase() == AppController::Ready; } }
            << Step { [this] { addSamplePlans(); }, {} }
            << Step { [this] { addLongNote(); }, {} }
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
                              // A lesson is running in the pictures (main.cpp stops the clock at 14:20)
                              for (QObject *o : m_window->findChildren<QObject *>(u"scheduleOverview"_s))
                                  o->setProperty("now", Qupil::Clock::now());
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
            << Step { [this, nav] { nav("editPlan", m_membership, u""_s).action(); }, {} }
            << snap(prefix + u"-6b-plan-editor"_s) << close
            << Step { [this, nav] { nav("finishPlan", m_membership, m_plan).action(); }, {} }
            << snap(prefix + u"-6c-plan-finish"_s) << close
            << Step { [this, nav] { nav("editPiece", m_membership, m_piece).action(); }, {} }
            << snap(prefix + u"-7-piece-editor"_s) << close
            // A long note: shortened with "Show more", then unfolded
            << Step { [this, nav] { nav("showStudent", m_longStudent, m_longMembership).action(); }, {} }
            << snap(prefix + u"-7b-long-note"_s)
            << Step { [this] { setAll("unfolded", QVariantMap { { m_longNote + u"/1"_s, true } }); }, {} }
            << snap(prefix + u"-7c-long-note-open"_s)
            << Step { [this] { setAll("unfolded", QVariantMap()); }, {} }
            // Blocks: what was covered (one looked into), then blocks of the note for the next lesson
            << Step { [this, nav] { nav("finishPlan", m_longMembership, m_longPlan).action(); }, {} }
            << Step { [this] {
                          setAll("checks", QVariantList { true, false, true });
                          setAll("peeked", QVariantList { false, true, false });
                      },
                      {} }
            << snap(prefix + u"-7d-plan-finish-blocks"_s) << close
            << Step { [this, nav] { nav("planFromNote", m_longMembership, m_longNote).action(); }, {} }
            << Step { [this] { setAll("checks", QVariantList { false, false, true, false, false }); }, {} }
            << snap(prefix + u"-7e-plan-from-note"_s) << close
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

// Plans for today: the running lesson (time to make the note) and the group later on
void ScreenshotTour::addSamplePlans()
{
    DataService *data = DataService::instance();
    if (!data)
        return;
    const bool de = LanguageManager::instance() && LanguageManager::instance()->effectiveLanguage().startsWith(u"de"_s);
    const QString today = Qupil::Clock::today().toString(Qt::ISODate);
    for (const QVariant &v : data->lessonsOn(Qupil::Clock::today().dayOfWeek() - 1)) {
        const QVariantMap lesson = v.toMap();
        const QVariantList students = lesson.value(u"students"_s).toList();
        if (students.isEmpty())
            continue;
        const QString membership = students.first().toMap().value(u"membership_id"_s).toString();
        if (lesson.value(u"start_time"_s).toString() == u"14:00"_s) {
            data->savePlan(membership, {}, today,
                           de ? u"**G-Dur-Tonleiter** über zwei Oktaven, langsam mit Metronom (60)\nBourrée: Takt 9–16 auswendig\n"
                                u"Neues Stück aussuchen: Gavotte oder Menuett"_s
                              : u"**G major scale** over two octaves, slowly with metronome (60)\nBourrée: bars 9–16 by heart\n"
                                u"Choose a new piece: gavotte or minuet"_s,
                           false);
        } else if (lesson.value(u"kind"_s).toInt() == 2) {
            m_plan = data->savePlan(membership, {}, today,
                                    de ? u"Aufwärmen: Rhythmus-Kanon im 6/8-Takt\nLagenwechsel 1.–3. Lage: abhören\n"
                                         u"Duett „Frühling“: Einsätze und Dynamik"_s
                                       : u"Warm-up: rhythm canon in 6/8\nShifting 1st–3rd position: check\n"
                                         u"Duet “Spring”: entries and dynamics"_s,
                                    true);
        }
    }
}

// One of the long notes some teachers write (a whole practice programme), for a violin or
// viola student with a lesson of their own a few days ago: it shows shortened ("Show more")
void ScreenshotTour::addLongNote()
{
    DataService *data = DataService::instance();
    if (!data)
        return;
    const bool de = LanguageManager::instance() && LanguageManager::instance()->effectiveLanguage().startsWith(u"de"_s);
    const QDate today = Qupil::Clock::today();
    for (int back = 1; back < 7 && m_longMembership.isEmpty(); ++back) {
        const QDate day = today.addDays(-back);
        for (const QVariant &v : data->lessonsOn(day.dayOfWeek() - 1)) {
            const QVariantList students = v.toMap().value(u"students"_s).toList();
            if (students.size() != 1)
                continue;
            const QString membership = students.first().toMap().value(u"membership_id"_s).toString();
            const QString student = data->membershipInfo(membership).value(u"student_id"_s).toString();
            if (!data->student(student).value(u"instrument"_s).toString().startsWith(u"Viol"_s))
                continue;   // the note is about violin or viola
            m_longMembership = membership;
            m_longStudent = student;
            m_longNote = data->addNote(membership, day.toString(Qt::ISODate),
                          de ? u"**Haltung:**\n"
                               u"Instrument locker ablegen, nicht mit der Schulter festhalten\n"
                               u"Schultern unten lassen – kein Hochziehen beim Lagenwechsel\n"
                               u"Noten auf Augenhöhe, dafür ruhig einen Schritt zurück\n"
                               u"\n"
                               u"**Jeden Tag (etwa 20 Minuten):**\n"
                               u"- Tonleiter D-Dur über zwei Oktaven: erst ganze Bögen, dann Achtel, mit Metronom (60)\n"
                               u"- Wohlfahrt op. 45 Nr. 8: Zeile 1–4, saubere Saitenwechsel\n"
                               u"- Lange Töne auf leeren Saiten, vom Frosch bis zur Spitze gleich laut\n"
                               u"- Einmal pro Woche: Tonleiter in Terzen\n"
                               u"\n"
                               u"**Vivaldi a-Moll, 1. Satz:**\n"
                               u"- Takt 1–24 auswendig\n"
                               u"- Sechzehntel locker aus dem Handgelenk, nicht aus dem Arm\n"
                               u"- ==Dynamik deutlicher: forte wirklich laut, piano trotzdem tragend==\n"
                               u"- Kopie für die Klavierbegleitung mitbringen\n"
                               u"\n"
                               u"**Vorspiel im November:**\n"
                               u"- Das Stück zweimal am Stück vor der Familie spielen\n"
                               u"- Auftritt und Verbeugung üben\n"
                               u"\n"
                               u"*Viel selbst zuhören – nicht nur Noten spielen, sondern Musik machen!*"_s
                             : u"**Posture:**\n"
                               u"Let the instrument rest, don't hold it with the shoulder\n"
                               u"Keep the shoulders down – no lifting when shifting\n"
                               u"Music at eye level, step back a little for it\n"
                               u"\n"
                               u"**Every day (about 20 minutes):**\n"
                               u"- D major scale over two octaves: whole bows first, then quavers, with metronome (60)\n"
                               u"- Wohlfahrt op. 45 no. 8: lines 1–4, clean string crossings\n"
                               u"- Long notes on open strings, equally loud from frog to tip\n"
                               u"- Once a week: scale in thirds\n"
                               u"\n"
                               u"**Vivaldi A minor, 1st movement:**\n"
                               u"- Bars 1–24 by heart\n"
                               u"- Semiquavers loosely from the wrist, not from the arm\n"
                               u"- ==Clearer dynamics: forte really loud, piano still carrying==\n"
                               u"- Bring a copy for the piano accompaniment\n"
                               u"\n"
                               u"**Recital in November:**\n"
                               u"- Play the piece twice in a row for the family\n"
                               u"- Practise walking on and bowing\n"
                               u"\n"
                               u"*Listen a lot – don't just play notes, make music!*"_s,
                          false);
            // The next lesson is planned in blocks as well
            m_longPlan = data->savePlan(membership, {}, data->nextLessonDate(membership, {}),
                                        de ? u"**Vivaldi a-Moll, 1. Satz:**\n"
                                             u"- Takt 25–48 dazunehmen\n"
                                             u"- Sechzehntel weiter locker aus dem Handgelenk\n"
                                             u"- Mit Begleitung: das Tempo halten\n"
                                             u"\n"
                                             u"**Etüde:**\n"
                                             u"- Kayser Nr. 9 anfangen\n"
                                             u"- Saitenwechsel erst langsam\n"
                                             u"\n"
                                             u"Vorspiel: Reihenfolge der Stücke festlegen"_s
                                           : u"**Vivaldi A minor, 1st movement:**\n"
                                             u"- Add bars 25–48\n"
                                             u"- Semiquavers still loosely from the wrist\n"
                                             u"- With accompaniment: keep the tempo\n"
                                             u"\n"
                                             u"**Study:**\n"
                                             u"- Start Kayser no. 9\n"
                                             u"- String crossings slowly first\n"
                                             u"\n"
                                             u"Recital: decide the order of the pieces"_s,
                                        false);
            break;
        }
    }
}

// Sets a property wherever it exists (e.g. "unfolded" of the student page)
void ScreenshotTour::setAll(const char *property, const QVariant &value)
{
    for (QObject *o : m_window->findChildren<QObject *>()) {
        if (o->property(property).isValid())
            o->setProperty(property, value);
    }
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
    const QString today = Qupil::Clock::today().toString(Qt::ISODate);
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
