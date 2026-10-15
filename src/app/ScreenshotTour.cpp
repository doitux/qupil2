// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "ScreenshotTour.h"

#include <QCoreApplication>
#include <QDir>
#include <QImage>
#include <QQuickWindow>
#include <QTimer>

#include <cstdio>
#include <iterator>

#include "AppController.h"

using namespace Qt::StringLiterals;

namespace {

constexpr int kSettleMs = 450;      // animations and layouts settle
constexpr int kPollMs = 100;
constexpr int kTimeoutMs = 30000;

const char *const kPages[] = { "today", "students", "events", "metronome", "more" };

} // namespace

ScreenshotTour::ScreenshotTour(QQuickWindow *window, QObject *theme, AppController *app, const QString &folder,
                               QObject *parent)
    : QObject(parent)
    , m_window(window)
    , m_theme(theme)
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

    // Setup screen
    m_steps << size(1180, 800) << mode("light") << snap(u"01-setup-light"_s)
            << mode("dark") << snap(u"02-setup-dark"_s) << mode("light");
    // Sample data through the real 1.x import
    m_steps << Step { [this] { m_app->loadDemo(); },
                      [this] { return !m_app->busy() && !m_app->lastTakeOver().isEmpty(); } }
            << snap(u"03-imported-light"_s)
            << Step { [this] { m_app->finishSetup(); }, [this] { return m_app->phase() == AppController::Ready; } };
    addPages(u"1-desktop-light"_s);
    m_steps << mode("dark");
    addPages(u"2-desktop-dark"_s);
    m_steps << mode("light") << size(390, 844);
    addPages(u"3-phone-light"_s);
    m_steps << mode("dark");
    addPages(u"4-phone-dark"_s);
}

void ScreenshotTour::addPages(const QString &prefix)
{
    for (int i = 0; i < int(std::size(kPages)); ++i) {
        const QString name = u"%1-%2-%3"_s.arg(prefix).arg(i).arg(QLatin1StringView(kPages[i]));
        m_steps << Step { [this, i] { m_window->setProperty("currentPage", i); }, {} }
                << Step { [this, name] { shot(name); }, {} };
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
