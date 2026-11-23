// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// UI test: the menu of a note and of a coupled student's tasks ("⋯", press and hold, right
// click). A tap on the card only shows the lines of a block or the hints of a task; it
// neither makes a new note nor opens an editor.

#include "UiTest.h"

#include <QCoreApplication>
#include <QJSValue>
#include <QMouseEvent>
#include <QQuickItem>
#include <QQuickWindow>

#include <memory>

#include "Clock.h"
#include "DataService.h"

using namespace Qt::StringLiterals;

void UiTest::addNoteMenuCases()
{
    // The visible item with this object name (delegates are not QObject children: by the items)
    auto named = [this](const QString &name) {
        QQuickItem *found = nullptr;
        std::function<void(QQuickItem *)> walk = [&](QQuickItem *i) {
            if (!found && i->objectName() == name && i->isVisible())
                found = i;
            for (QQuickItem *c : i->childItems())
                walk(c);
        };
        if (m_window)
            walk(m_window->contentItem());
        return found;
    };
    auto centre = [named](const QString &name) {
        return [named, name] {
            QQuickItem *i = named(name);
            return i ? i->mapToScene(QPointF(i->width() / 2, i->height() / 2)) : QPointF();
        };
    };
    // A place on the card that is neither a block nor a button: left of its "⋯"
    auto beside = [named](const QString &name) {
        return [named, name] {
            QQuickItem *i = named(name);
            return i ? i->mapToScene(QPointF(-110, i->height() / 2)) : QPointF();
        };
    };
    auto mouse = [this](QEvent::Type type, const QPointF &p, Qt::MouseButton button) {
        QMouseEvent e(type, p, m_window->mapToGlobal(p), button,
                      type == QEvent::MouseButtonPress ? Qt::MouseButtons(button) : Qt::MouseButtons(Qt::NoButton), Qt::NoModifier);
        QCoreApplication::sendEvent(m_window.data(), &e);
    };
    auto unfolded = [this](const QString &key) {
        for (QObject *o : m_window->findChildren<QObject *>(u"studentDetail"_s)) {
            QVariant v = o->property("unfolded");
            if (v.canConvert<QJSValue>())
                v = v.value<QJSValue>().toVariant();
            if (v.toMap().value(key).toBool())
                return true;
        }
        return false;
    };
    const QString date = Qupil::Clock::today().addDays(-7).toString(Qt::ISODate);
    auto count = std::make_shared<int>(0);

    {
        Case &c = add(u"note-menu"_s, u"a note: a tap only shows a block, the menu has new note, plan, edit, share, delete"_s);
        auto moreName = [this] { return u"noteMore:"_s + m_text; };
        c.steps << act(u"a note in blocks"_s, [this, date, count] {
                       m_membership = runningMembership();
                       m_text = m_data->addNote(m_membership, date, u"**UI-Menü Tonleiter**\nzweioktavig\nMetronom 60\n\nUI-Menü Noten besorgen"_s, false);
                       *count = m_data->notes(m_membership).size();
                   })
                << openStudent(u"Lena Muster"_s)
                << tapAt([beside, moreName] { return beside(moreName())(); }, u"tap the card beside its date"_s)
                << wait(500)
                << check(u"nothing opened, no new note"_s, [this, count] {
                       return popupItems().isEmpty() && m_data->notes(m_membership).size() == *count;
                   })
                << tap(u"^UI-Menü Tonleiter, noch 2 Zeilen$"_s, u"tap the block"_s)
                << check(u"the block shows its lines, nothing else opened"_s, [this, unfolded] {
                       return unfolded(m_text + u"/0"_s) && popupItems().isEmpty();
                   })
                << act(u"right click on the card"_s, [this, beside, moreName, mouse] {
                       const QPointF p = beside(moreName())();
                       mouse(QEvent::MouseButtonPress, p, Qt::RightButton);
                       mouse(QEvent::MouseButtonRelease, p, Qt::RightButton);
                   })
                << see(u"^Neue Notiz daraus \\(heute\\)$"_s) << key(Qt::Key_Escape, u"Escape closes the menu"_s)
                << gone(u"^Neue Notiz daraus \\(heute\\)$"_s)
                << act(u"press and hold on the card"_s, [this, beside, moreName, mouse] {
                       mouse(QEvent::MouseButtonPress, beside(moreName())(), Qt::LeftButton);
                   })
                << wait(1100)
                << act(u"let go"_s, [this, beside, moreName, mouse] {
                       mouse(QEvent::MouseButtonRelease, beside(moreName())(), Qt::LeftButton);
                   })
                << see(u"^Neue Notiz daraus \\(heute\\)$"_s) << key(Qt::Key_Escape, u"Escape closes the menu"_s)
                << gone(u"^Neue Notiz daraus \\(heute\\)$"_s)
                << tapAt([centre, moreName] { return centre(moreName())(); }, u"the note's ⋯"_s)
                << see(u"^Neue Notiz daraus \\(heute\\)$"_s) << see(u"^Nächste Stunde planen$"_s) << see(u"^Bearbeiten$"_s)
                << see(u"^Als Nachricht teilen$"_s) << see(u"^Löschen$"_s)
                << snap(u"note-menu"_s)
                << tap(u"^Neue Notiz daraus \\(heute\\)$"_s)
                << see(u"^Notiz speichern$"_s)
                << check(u"a new note (no delete button) with the old note's text"_s, [this] {
                       QQuickItem *text = textItem();
                       return text && text->property("text").toString().contains(u"UI-Menü Tonleiter"_s)
                              && !find(u"^Notiz löschen$"_s);
                   })
                << tap(u"^Abbrechen$"_s) << gone(u"^Notiz speichern$"_s)
                << check(u"no note was made by looking"_s, [this, count] { return m_data->notes(m_membership).size() == *count; })
                << tapAt([centre, moreName] { return centre(moreName())(); }, u"the note's ⋯"_s)
                << tap(u"^Bearbeiten$"_s) << see(u"^Notiz speichern$"_s) << see(u"^Notiz löschen$"_s)
                << tap(u"^Abbrechen$"_s) << gone(u"^Notiz speichern$"_s)
                << tapAt([centre, moreName] { return centre(moreName())(); }, u"the note's ⋯"_s)
                << tap(u"^Löschen$"_s) << see(u"^Abbrechen$"_s)
                << tap(u"^Löschen$"_s, u"confirm"_s)
                << check(u"the note is gone"_s, [this, count] { return m_data->notes(m_membership).size() == *count - 1; });
    }

    {
        Case &c = add(u"tasks-menu"_s, u"tasks of a coupled student: a tap shows the hints, the menu plans, edits and deletes"_s);
        const QString more = u"taskMore:"_s + date;
        c.steps << act(u"coupled, with tasks of last week"_s, [this, date] {
                       m_membership = runningMembership();
                       const QString student = m_data->get(u"membership"_s, m_membership).value(u"student_id"_s).toString();
                       m_text = m_data->create(u"coupling"_s, { { u"student_id"_s, student }, { u"created"_s, m_data->today() } });
                       m_data->saveTasks(m_membership, date,
                                         { QVariantMap { { u"title"_s, u"UI-Menü Tonleiter"_s },
                                                         { u"hints"_s, QStringList { u"zweioktavig"_s, u"Metronom 60"_s } } },
                                           QVariantMap { { u"title"_s, u"UI-Menü Noten besorgen"_s }, { u"kind"_s, 1 } } },
                                         false);
                   })
                << openStudent(u"Lena Muster"_s)
                << see(u"^UI-Menü Tonleiter, noch 2 Zeilen$"_s)
                << tap(u"^UI-Menü Tonleiter, noch 2 Zeilen$"_s, u"tap the task"_s)
                << see(u"^zweioktavig$"_s) << see(u"^Metronom 60$"_s)
                << check(u"no editor opened"_s, [this] { return popupItems().isEmpty(); })
                << snap(u"tasks-hints"_s)
                << tapAt(centre(more), u"the tasks' ⋯"_s)
                << see(u"^Nächste Stunde planen$"_s) << see(u"^Bearbeiten$"_s) << see(u"^Löschen$"_s)
                << gone(u"^Als Nachricht teilen$"_s)
                << snap(u"tasks-menu"_s)
                << tap(u"^Nächste Stunde planen$"_s)
                << see(u"^Planen$"_s)
                << tap(u"^UI-Menü Tonleiter$"_s, u"tick the task"_s)
                << tap(u"^Planen$"_s) << gone(u"^Planen$"_s)
                << check(u"planned: the task as a block with its hints"_s, [this] {
                       for (const QVariant &v : m_data->plans(m_membership))
                           if (v.toMap().value(u"content"_s).toString().contains(u"**UI-Menü Tonleiter**\nzweioktavig\nMetronom 60"_s))
                               return !v.toMap().value(u"content"_s).toString().contains(u"UI-Menü Noten besorgen"_s);
                       return false;
                   })
                << check(u"the tasks stay as they are"_s, [this, date] { return m_data->tasksOn(m_membership, date).size() == 2; })
                << tapAt(centre(more), u"the tasks' ⋯"_s)
                << tap(u"^Bearbeiten$"_s) << see(u"^Weitere Aufgabe$"_s)
                << tap(u"^Abbrechen$"_s) << gone(u"^Weitere Aufgabe$"_s)
                << tapAt(centre(more), u"the tasks' ⋯"_s)
                << tap(u"^Löschen$"_s) << see(u"^Abbrechen$"_s)
                << tap(u"^Löschen$"_s, u"confirm"_s)
                << check(u"the tasks of that lesson are gone"_s, [this, date] { return m_data->tasksOn(m_membership, date).isEmpty(); })
                << backIfThere(u"^Schüler$"_s)
                << act(u"remove the plan part and the coupling again"_s, [this] {
                       for (const QVariant &v : m_data->plans(m_membership)) {
                           const QVariantMap p = v.toMap();
                           QString content = p.value(u"content"_s).toString();
                           if (!content.contains(u"UI-Menü Tonleiter"_s))
                               continue;
                           content.remove(u"**UI-Menü Tonleiter**\nzweioktavig\nMetronom 60"_s);
                           if (content.trimmed().isEmpty())
                               m_data->deletePlan(p.value(u"id"_s).toString(), false);
                           else
                               m_data->savePlan(m_membership, p.value(u"id"_s).toString(), p.value(u"date"_s).toString(), content.trimmed(), false);
                       }
                       m_data->remove(u"coupling"_s, m_text);
                   });
    }
}
