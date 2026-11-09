// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QElapsedTimer>
#include <QList>
#include <QObject>
#include <QPointer>
#include <QQuickItem>
#include <QRectF>
#include <QString>
#include <QStringList>

#include <functional>

class AppController;
class DataService;
class LanguageManager;
class QQuickItem;
class QQuickWindow;

/*!
    qupil --ui-test <folder>: goes through the important flows of the app the way a
    person would – it finds buttons and fields by the names Qupil gives them for
    screen readers, taps them and types – and checks what came out (on the screen
    and in the data). Runs on every platform (desktop, Android emulator, iOS
    simulator) with sample data in its own data folder.

    Each case starts from the "Today" page. A failed case leaves a screenshot and
    the list of what was on the screen, the run goes on with the next case. QML
    warnings count as failures of the case they came up in. The result is in
    results.md and results.json in the folder; the exit code is 0 only when every
    case passed.

    --ui-test-phase restart: the second half of the restart case (after the first
    run quit with an unsaved note): the editor has to come back with the text.
    Needs the same --data-dir as the first run.

    --ui-test-phase only:pages,today,…: just these cases (one long run split into several
    starts, e.g. in the Android emulator).

    --ui-test-phase draft: only types an unsaved note and stays open, for a test run
    that sends Qupil to the background and lets the system end it (iOS simulator);
    the restart half then checks the note as above.
*/
class UiTest : public QObject
{
    Q_OBJECT

public:
    UiTest(QQuickWindow *window, QObject *theme, QObject *nav, QObject *metronome, AppController *app,
           DataService *data, LanguageManager *languages, const QString &folder, const QString &phase,
           QObject *parent = nullptr);
    ~UiTest() override;

    void start();

    //! An element of the screen as screen readers see it
    struct Node
    {
        QString name;
        QString role;
        QRectF rect;   // in window coordinates
        bool tab = false;   // a page of the navigation (NavButton.qml)
        QPointer<QQuickItem> item;
        QRectF full;        // the whole item, also where it is scrolled out of sight
    };

private:
    struct Step
    {
        QString what;                        // for the report
        std::function<bool()> run;           // done when true; polled until the timeout
        int timeoutMs = 8000;
    };
    struct Case
    {
        QString name;
        QString title;
        QList<Step> steps;
        bool keepScreen = false;   // the next case (or quitting) starts from what this one left
    };
    struct Result
    {
        QString name;
        QString title;
        bool passed = true;
        QString failure;
        QString note;
        QStringList warnings;
        qint64 ms = 0;
    };

    // ---- the cases
    void addCases();
    void addRestartPhase();
    void addDraftPhase();
    Case &add(const QString &name, const QString &title);

    // ---- steps
    Step tap(const QString &pattern, const QString &what = {});
    Step tapAt(std::function<QPointF()> where, const QString &what);
    Step see(const QString &pattern);
    Step backIfThere(const QString &pattern);
    Step tapUntil(const QString &pattern, const QString &expect);
    Step tapIfThere(const QString &pattern, int withinMs, const QString &what);
    //! Opens the list of a ComboBox (taps again if a tap only closed the keyboard)
    Step openList(const QString &pattern);
    Step gone(const QString &pattern);
    Step type(const QString &text);
    Step key(int key, const QString &what);
    Step check(const QString &what, std::function<bool()> test, int timeoutMs = 4000);
    Step act(const QString &what, std::function<void()> action);
    Step wait(int ms);
    Step snap(const QString &name);
    Step tab(const QString &name);
    Step clearField();
    QList<Step> openStudent(const QString &name);

    // ---- the screen
    QList<Node> nodes() const;
    bool find(const QString &pattern, Node *found = nullptr) const;
    void click(const QPointF &point);
    void sendKey(int key, const QString &text = {});
    QString dump() const;
    //! Texts on the screen too faint to read on what is behind them (contrast ratio below 2:1)
    QStringList unreadable() const;
    void writeDump(const QString &name) const;
    void shot(const QString &name);
    QVariant windowProperty(const char *name) const;
    bool busy() const;
    bool scrollIntoView(const Node &node);
    bool editorOpen() const;
    bool compact() const;
    void resetToToday();
    QString runningMembership() const;
    QString latestNote(const QString &membership) const;
    QStringList allLessons() const;

    // ---- running
    void next();
    void finishCase();
    void report();
    static void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message);

    QPointer<QQuickWindow> m_window;
    QPointer<QObject> m_theme;
    QPointer<QObject> m_nav;
    QPointer<QObject> m_metronome;
    AppController *m_app;
    DataService *m_data;
    LanguageManager *m_languages;
    QString m_folder;
    QString m_phase;
    QList<Case> m_cases;
    QList<Result> m_results;
    int m_case = -1;
    int m_step = 0;
    QElapsedTimer m_stepTimer;
    QElapsedTimer m_caseTimer;
    bool m_stepStarted = false;
    // what the cases share
    QString m_membership;   // the running lesson's first student at 14:20 (sample data)
    int m_count = 0;
    QStringList m_lessons;
    QString m_other;              // a second lesson of the same student (case "together")
    QStringList m_cleanup;        // "entity:id" to remove at the end of a case
    QStringList m_unreadable;     // found at a snap or the end of the case
};
