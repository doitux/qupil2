// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QList>
#include <QObject>
#include <QPointer>
#include <QString>

#include <functional>

class AppController;
class QQuickWindow;

/*!
    qupil --screenshots <folder>: walks through the app with sample data and
    saves a PNG of every screen (light and dark, desktop and phone size).
    Runs on a temporary data folder; used by CI to show what a build looks like.
*/
class ScreenshotTour : public QObject
{
    Q_OBJECT

public:
    ScreenshotTour(QQuickWindow *window, QObject *theme, QObject *nav, AppController *app, const QString &folder,
                   QObject *parent = nullptr);

    void start();

private:
    struct Step
    {
        std::function<void()> action;
        std::function<bool()> ready;   // wait until true (polled), may be empty
    };

    void run();
    void shot(const QString &name);
    void addPages(const QString &prefix);
    void addDetails(const QString &prefix);
    void pickExamples();

    QPointer<QQuickWindow> m_window;
    QPointer<QObject> m_theme;
    QPointer<QObject> m_nav;
    QString m_student, m_membership, m_lesson, m_piece, m_event;
    AppController *m_app;
    QString m_folder;
    QList<Step> m_steps;
    int m_index = 0;
    int m_waited = 0;
    int m_saved = 0;
};
