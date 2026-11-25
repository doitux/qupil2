// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// UI test of the trial "Try the new idea" (Settings): the proposal "tasks, lesson,
// history" with invented students. One lesson is gone through as a teacher would: a new
// point, a point and a task done, a new task, a task put on pause, a note – and the
// history shows it. Then the preparation, a task taken up again, the look into the
// student's app, and a student with notes only. The real data must stay untouched.

#include "UiTest.h"

#include <QQuickItem>
#include <QQuickWindow>

#include "DataService.h"

#include <QRegularExpression>

using namespace Qt::StringLiterals;

void UiTest::addIdeaCases()
{
    auto before = std::make_shared<int>(0);
    const auto today = [this] {
        return u"^"_s + QRegularExpression::escape(m_data->formatDate(m_data->today(), true)) + u"$"_s;
    };

    const QString lessonOpen = u"^Geschafft: Mozart: Violinkonzert G-Dur, 1\\. Satz$"_s;
    Case &c = add(u"idea-demo"_s, u"The trial of the new idea: a lesson, the history, preparing, pausing; nothing is stored"_s);
    c.steps << act(u"remember the state of the real data"_s, [this, before] { *before = m_data->property("revision").toInt(); })
            << tab(u"Mehr"_s) << tap(u"^Einstellungen$"_s) << tap(u"^Neue Idee ausprobieren$"_s)
            << see(u"^Mia Muster$"_s) << snap(u"idea-start"_s)
            << tap(u"^Jonas Beispiel$"_s)
            << see(u"^Mozart: Violinkonzert G-Dur, 1\\. Satz$"_s) << see(u"^Pausiert \\(1\\)$"_s) << see(u"^Geschafft \\(3\\)$"_s)
            << snap(u"idea-student"_s)

            // The lesson
            << tap(u"^Stunde$"_s) << see(u"^Geschafft: Tonleiter A-Dur, drei Oktaven$"_s)
            << tap(u"^Mozart: Violinkonzert G-Dur, 1\\. Satz$"_s, u"unfold the Mozart"_s)
            << see(u"^Exposition bis T\\. 94 auswendig$"_s)
            << tap(u"edit:^Punkt hinzufügen$"_s) << type(u"UI-Kadenz ab T. 180"_s) << key(Qt::Key_Return, u"Enter adds the point"_s)
            << see(u"^UI-Kadenz ab T\\. 180$"_s)
            << tap(u"^Triller in T\\. 38 von oben$"_s, u"a point is done"_s)
            << snap(u"idea-lesson-points"_s)
            << tap(u"^Geschafft: Tonleiter A-Dur, drei Oktaven$"_s, u"a whole task is done"_s)
            << tap(u"^Kreutzer: Etüde Nr\\. 2$"_s, u"unfold the étude"_s)
            << tap(u"^Pausieren: Kreutzer: Etüde Nr\\. 2$"_s)
            << see(u"^Wieder aufnehmen: Kreutzer: Etüde Nr\\. 2$"_s)
            << tap(u"edit:^Weitere Aufgabe$"_s) << type(u"UI-Vibrato"_s) << key(Qt::Key_Return, u"Enter adds the task"_s)
            << see(u"^UI-Vibrato$"_s)
            << tap(u"edit:^Notiz zur Stunde"_s) << type(u"UI-Notiz zur Stunde"_s)
            << snap(u"idea-lesson-end"_s)
            << tap(u"^Speichern$"_s) << gone(u"^Geschafft: Tonleiter A-Dur, drei Oktaven$"_s)

            // The page and the history after it
            << see(u"^Geschafft \\(4\\)$"_s) << see(u"^Pausiert \\(2\\)$"_s) << see(u"^UI-Vibrato$"_s)
            << tap(today(), u"unfold the entry of the lesson"_s)
            << see(u"^UI-Kadenz ab T\\. 180$"_s) << see(u"^Triller in T\\. 38 von oben$"_s)
            << snap(u"idea-history"_s)

            // The history is put right afterwards: a point that was wrong goes, also at the task
            << tap(u"^Eintrag ändern: "_s + today().mid(1), u"the pencil of the lesson's entry"_s)
            << see(u"^Entfernen: UI-Kadenz ab T\\. 180$"_s) << snap(u"idea-entry-edit"_s)
            << tap(u"^Entfernen: UI-Kadenz ab T\\. 180$"_s) << gone(u"^Entfernen: UI-Kadenz ab T\\. 180$"_s)
            << tap(u"^Speichern$"_s) << gone(u"^UI-Kadenz ab T\\. 180$"_s)
            // A task is changed in between, without a lesson: its title, a point more
            << tap(u"^Ändern: UI-Vibrato$"_s) << see(u"edit:^Aufgabe$"_s)
            << tap(u"edit:^Aufgabe$"_s) << type(u" neu"_s)
            << tap(u"edit:^Punkt hinzufügen$"_s) << type(u"UI-Direktpunkt"_s) << key(Qt::Key_Return, u"Enter adds the point"_s)
            << see(u"^Entfernen: UI-Direktpunkt$"_s) << snap(u"idea-task-edit"_s)
            << tap(u"^Speichern$"_s) << see(u"^UI-Vibrato neu$"_s)
            << tap(u"^UI-Vibrato neu$"_s, u"unfold the task"_s) << see(u"^UI-Direktpunkt$"_s)

            // Preparing the next lesson: due, and a point only for me
            << tap(u"^Vorbereiten$"_s) << tap(u"^Dran: Mozart: Violinkonzert G-Dur, 1\\. Satz$"_s)
            << tap(u"^Mozart: Violinkonzert G-Dur, 1\\. Satz$"_s, u"unfold the Mozart"_s)
            << tap(u"edit:^Punkt hinzufügen, nur für mich$"_s) << type(u"UI-Vorhaben"_s)
            // A paused task taken up again and due: it runs and is prepared after saving
            << tap(u"^Wieder aufnehmen: Kreutzer: Etüde Nr\\. 2$"_s) << tap(u"^Dran: Kreutzer: Etüde Nr\\. 2$"_s)
            << snap(u"idea-prepare"_s)
            // (saving takes the line that is still being typed along)
            << tap(u"^Speichern$"_s) << see(u"^Vorbereitet für "_s) << see(u"^UI-Vorhaben$"_s)
            << see(u"^Pausiert \\(1\\)$"_s)
            // In the lesson it is at the top, to be ticked
            // (asked at once, while the window of the preparation is still on its way out: it
            // opens as soon as that is gone. The page shows "UI-Vorhaben" too, in the card of
            // what is prepared – only in the lesson's window it is a box to tick)
            << tap(u"^Stunde$"_s) << see(lessonOpen) << see(u"checkbox:^UI-Vorhaben$"_s)
            << see(u"^Geschafft: Kreutzer: Etüde Nr\\. 2$"_s)
            << tap(u"checkbox:^UI-Vorhaben$"_s, u"what I planned is done"_s)
            << snap(u"idea-lesson-prepared"_s)
            << tap(u"^Speichern$"_s) << gone(u"^Vorbereitet für "_s)

            // A paused task is taken up again; the student's app
            << tap(u"^Pausiert \\(1\\)$"_s) << tap(u"^Wieder aufnehmen$"_s) << gone(u"^Pausiert \\(\\d\\)$"_s)
            << tap(u"^So sieht es Jonas in der App$"_s)
            << tap(u"^Mozart: Violinkonzert G-Dur, 1\\. Satz: viel geübt$"_s)
            << snap(u"idea-pupil"_s)
            << tap(u"^Schließen$"_s) << gone(u"^Schließen$"_s)

            // Notes only: with the setting off, the lesson of a student without tasks is its note
            << tap(u"^Neue Idee ausprobieren$"_s, u"back to the trial's start"_s)
            << tap(u"^Aufgaben verwenden$"_s) << tap(u"^Paul Probe$"_s)
            << tap(u"^Stunde$"_s) << see(u"edit:^Notiz$"_s) << gone(u"edit:^Neue Aufgabe$"_s)
            << tap(u"edit:^Notiz$"_s) << type(u"UI-Nur eine Notiz"_s)
            << tap(u"^Speichern$"_s) << gone(u"^Speichern$"_s)
            << snap(u"idea-notes-only"_s)
            // Preparing such a student is a free note (no points to add), and the lesson's
            // note starts from it – as a new note is made from an older one today
            << tap(u"^Vorbereiten$"_s) << see(u"edit:^Vorbereitung$"_s) << gone(u"edit:^Weiterer Punkt"_s)
            << tap(u"edit:^Vorbereitung$"_s) << type(u"UI-Vorbereitung Menuett"_s)
            << snap(u"idea-notes-prepare"_s)
            << tap(u"^Speichern$"_s) << see(u"^Vorbereitet für "_s) << see(u"^UI-Vorbereitung Menuett$"_s)
            << tap(u"^Stunde$"_s) << see(u"edit:^Notiz$"_s)
            << check(u"the lesson's note holds what was prepared"_s, [this] { return textPlain().trimmed() == u"UI-Vorbereitung Menuett"_s; })
            << tap(u"edit:^Notiz$"_s) << type(u", gespielt"_s)
            << snap(u"idea-notes-lesson"_s)
            << tap(u"^Speichern$"_s) << gone(u"^Vorbereitet für "_s) << see(u"^UI-Vorbereitung Menuett, gespielt$"_s)
            << tap(u"^Neue Idee ausprobieren$"_s, u"back to the trial's start"_s)
            << tap(u"^Einstellungen$"_s, u"back to the settings"_s)
            << check(u"the real data is untouched"_s, [this, before] { return m_data->property("revision").toInt() == *before; })
            << backIfThere(u"^Mehr$"_s);

    {
        // A point of a task longer than its line (a phone): the field wraps and grows
        const QString longPoint = u"UI-Kadenz ab T. 180: erst nur die Griffe ohne Bogen, dann langsam mit Metronom 60, die Doppelgriffe einzeln stimmen"_s;
        Case &w = add(u"idea-point-wrap"_s, u"trial: a long point wraps in its field while it is written"_s);
        w.steps << act(u"the field for a new point"_s, [this] { m_textField = u"edit:^Punkt hinzufügen$"_s; })
                << tab(u"Mehr"_s) << tap(u"^Einstellungen$"_s) << tap(u"^Neue Idee ausprobieren$"_s) << tap(u"^Jonas Beispiel$"_s)
                << tap(u"^Ändern: Tonleiter A-Dur, drei Oktaven$"_s) << see(u"edit:^Aufgabe$"_s)
                << tap(u"edit:^Punkt hinzufügen$"_s) << type(longPoint) << wait(400)
                << check(u"the point wraps: several lines in a taller field, start and end both in it"_s, [this, longPoint] {
                       QQuickItem *t = textItem();
                       if (!t)
                           return false;
                       QRectF first, last;
                       QMetaObject::invokeMethod(t, "positionToRectangle", Q_RETURN_ARG(QRectF, first), Q_ARG(int, 0));
                       QMetaObject::invokeMethod(t, "positionToRectangle", Q_RETURN_ARG(QRectF, last), Q_ARG(int, int(longPoint.size())));
                       return t->height() > 60 && last.y() > first.y() + first.height() / 2 && first.x() >= 0
                              && last.x() <= t->width() && first.y() >= 0 && last.y() + last.height() <= t->height() + 1;
                   })
                << snap(u"idea-point-wrapped"_s)
                << key(Qt::Key_Return, u"Enter adds the point, no line break"_s) << wait(300)
                << check(u"the point is added, the field is empty and one line again"_s, [this] {
                       QQuickItem *t = textItem();
                       return t && textPlain().isEmpty() && t->height() <= 60;
                   })
                << tap(u"^Abbrechen$"_s) << gone(u"edit:^Aufgabe$"_s)
                << act(u"back to the note field"_s, [this] { m_textField = u"edit:^Notiz$"_s; })
                << tap(u"^Neue Idee ausprobieren$"_s, u"back"_s) << tap(u"^Einstellungen$"_s, u"back"_s) << backIfThere(u"^Mehr$"_s);
    }

    {
        // The arrow at the end of a task's line in the window of a lesson unfolds the task, as
        // a tap on its title does (it was only a picture)
        const auto arrow = [this] {
            QQuickItem *found = nullptr;
            std::function<void(QQuickItem *)> walk = [&](QQuickItem *i) {
                if (!found && i->objectName() == u"ideaTaskFold"_s && i->isVisible())
                    found = i;
                for (QQuickItem *c : i->childItems())
                    walk(c);
            };
            walk(m_window->contentItem());
            return found ? found->mapToScene(QPointF(found->width() / 2, found->height() / 2)) : QPointF();
        };
        Case &f = add(u"idea-fold-arrow"_s, u"trial: the arrow of a task unfolds it in the window of a lesson"_s);
        f.steps << tab(u"Mehr"_s) << tap(u"^Einstellungen$"_s) << tap(u"^Neue Idee ausprobieren$"_s) << tap(u"^Jonas Beispiel$"_s)
                << tap(u"^Vorbereiten$"_s) << see(u"^Dran: Mozart: Violinkonzert G-Dur, 1\\. Satz$"_s)
                << gone(u"edit:^Punkt hinzufügen, nur für mich$"_s)
                << tapAt(arrow, u"the arrow of the first task"_s) << see(u"edit:^Punkt hinzufügen, nur für mich$"_s)
                << tapAt(arrow, u"the arrow again"_s) << gone(u"edit:^Punkt hinzufügen, nur für mich$"_s)
                << tap(u"^Abbrechen$"_s) << gone(u"^Dran: Mozart: Violinkonzert G-Dur, 1\\. Satz$"_s)
                << tap(u"^Neue Idee ausprobieren$"_s, u"back"_s) << tap(u"^Einstellungen$"_s, u"back"_s) << backIfThere(u"^Mehr$"_s);
    }
}
