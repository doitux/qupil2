// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// UI test: every editor sheet with the on-screen keyboard (simulated: App.keyboardHeight,
// the height the platforms report). For each editor and window size: the first and the
// last text field get the cursor, the keyboard comes and goes. Checked: Save (or Close)
// stays whole on screen and above the keyboard, the cursor stays in view, the sheet does
// not run under the keyboard or off the top, and it moves one way only while the keyboard
// comes (no jumping). A new editor that is not listed here fails the case "dialogs-all".

#include "UiTest.h"

#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QRegularExpression>
#include <QTimer>
#include <QUrl>

#include <algorithm>
#include <memory>

#include "AppController.h"
#include "DataService.h"

using namespace Qt::StringLiterals;

namespace {

struct Sample
{
    qint64 ms;
    qreal top, bottom, cursor;   // scene coordinates; cursor: -1 when there is none
};

struct DialogRun
{
    QPointer<QQuickItem> holder;   // the Item the editor is made in (gone with it)
    QPointer<QObject> editor;      // the Sheet (a Popup)
    QPointer<QQuickItem> field;    // the field with the cursor
    QList<Sample> samples;
    QElapsedTimer clock;
    QTimer timer;
    QString problems;              // what went wrong, for the report
};

qreal effectiveOpacity(QQuickItem *item)
{
    qreal o = 1;
    for (QQuickItem *p = item; p; p = p->parentItem())
        o *= p->opacity();
    return o;
}

void collect(QQuickItem *item, const std::function<void(QQuickItem *)> &f)
{
    if (!item)
        return;
    f(item);
    for (QQuickItem *child : item->childItems())
        collect(child, f);
}

//! Text fields of the sheet a person can type in, top to bottom
QList<QQuickItem *> fieldsOf(QObject *editor)
{
    QList<QQuickItem *> out;
    auto *content = editor ? editor->property("contentItem").value<QQuickItem *>() : nullptr;
    collect(content, [&out](QQuickItem *i) {
        if ((i->inherits("QQuickTextInput") || i->inherits("QQuickTextEdit")) && i->isVisible() && i->isEnabled()
            && !i->property("readOnly").toBool() && i->width() > 20 && effectiveOpacity(i) > 0.5)
            out << i;
    });
    std::stable_sort(out.begin(), out.end(), [](QQuickItem *a, QQuickItem *b) {
        return a->mapToScene(QPointF()).y() < b->mapToScene(QPointF()).y();
    });
    return out;
}

//! The visible button with this text in the sheet (Save in the header or at the bottom)
QQuickItem *buttonOf(QObject *editor, const QString &text)
{
    QQuickItem *found = nullptr;
    auto *content = editor ? editor->property("contentItem").value<QQuickItem *>() : nullptr;
    collect(content, [&](QQuickItem *i) {
        if (!found && i->inherits("QQuickAbstractButton") && i->property("text").toString() == text && i->isVisible()
            && effectiveOpacity(i) > 0.5 && i->height() > 10)
            found = i;
    });
    return found;
}

//! The sheet's own scrolling area (the outermost Flickable in it)
QQuickItem *flickOf(QObject *editor)
{
    QQuickItem *found = nullptr;
    int best = 1 << 30;
    auto *content = editor ? editor->property("contentItem").value<QQuickItem *>() : nullptr;
    collect(content, [&](QQuickItem *i) {
        if (!i->inherits("QQuickFlickable") || i->inherits("QQuickListView") || !i->isVisible())
            return;
        int depth = 0;
        for (QQuickItem *p = i; p && p != content; p = p->parentItem())
            ++depth;
        if (depth < best) {
            best = depth;
            found = i;
        }
    });
    return found;
}

QRectF sceneRect(QQuickItem *i)
{
    return i ? i->mapRectToScene(QRectF(0, 0, i->width(), i->height())) : QRectF();
}

QString rectText(const QRectF &r)
{
    return u"%1,%2 %3×%4"_s.arg(qRound(r.x())).arg(qRound(r.y())).arg(qRound(r.width())).arg(qRound(r.height()));
}

} // namespace

void UiTest::addDialogCases()
{
    // A check is polled until its time is up: its note only once
    auto noteOnce = [this](const QString &text) {
        if (!m_results.isEmpty() && !m_results.last().note.contains(text))
            m_results.last().note += text;
    };
    struct Dialog
    {
        QString type;      // the QML type in qml/editors
        QString label;     // for the case name
        QString function;  // what opens it
        std::function<QVariantList()> args;
    };
    // What a failed case leaves (it stops at the failed step): the next case tidies up first
    struct Leftover
    {
        QPointer<QQuickItem> holder;
        QPointer<QObject> editor;
        QSize original;
    };
    auto left = std::make_shared<Leftover>();
    auto tidy = [this, left] {
        m_app->setKeyboardHeight(0);
        if (left->editor && left->editor->property("visible").toBool())
            QMetaObject::invokeMethod(left->editor, "close");
        // Kept (hidden) until Qupil quits: deleting an editor right after it closed crashed
        // Qt 6.4 in QQuickShape (an icon still being drawn)
        if (left->holder)
            left->holder->setVisible(false);
        left->holder = nullptr;
        left->editor = nullptr;
        if (left->original.isValid() && m_window->size() != left->original)
            m_window->resize(left->original);
    };
    // Sample data the editors open with (made in "dialogs-prepare", gone in "dialogs-cleanup")
    auto ids = std::make_shared<QHash<QString, QString>>();
    auto id = [ids](const char *key) { return QVariant(ids->value(QString::fromLatin1(key))); };
    const QList<Dialog> dialogs = {
        { u"NoteEditor"_s, u"note"_s, u"edit"_s, [id] { return QVariantList { id("membership"), QString() }; } },
        { u"NoteEditor"_s, u"plan"_s, u"editPlan"_s, [id] { return QVariantList { id("membership"), QString() }; } },
        { u"TaskEditor"_s, u"tasks"_s, u"edit"_s, [id] { return QVariantList { id("membership"), id("today") }; } },
        { u"PlanFinish"_s, u"plan-finish"_s, u"start"_s, [id] { return QVariantList { id("membership"), id("plan") }; } },
        { u"PlanFinish"_s, u"plan-from-note"_s, u"startFromNote"_s, [id] { return QVariantList { id("membership"), id("note") }; } },
        { u"BookletSheet"_s, u"booklet"_s, u"show"_s, [id] { return QVariantList { id("student") }; } },
        { u"PieceEditor"_s, u"piece"_s, u"edit"_s, [id] { return QVariantList { id("membership"), QString() }; } },
        { u"NewStudent"_s, u"student"_s, u"start"_s, [] { return QVariantList {}; } },
        { u"LessonEditor"_s, u"lesson"_s, u"edit"_s, [] { return QVariantList { QString(), 1 }; } },
        { u"ActivityEditor"_s, u"activity"_s, u"edit"_s, [id] { return QVariantList { id("student"), QString(), false }; } },
        { u"ReminderEditor"_s, u"reminder"_s, u"edit"_s, [] { return QVariantList { QString(), QString(), QString() }; } },
        { u"EventEditor"_s, u"event"_s, u"edit"_s, [] { return QVariantList { QString() }; } },
        { u"ExternalEditor"_s, u"external"_s, u"start"_s, [id] { return QVariantList { id("event") }; } },
        { u"ReadyPiecesPicker"_s, u"ready-pieces"_s, u"start"_s, [id] { return QVariantList { id("event") }; } },
        { u"LibraryItemEditor"_s, u"library"_s, u"edit"_s, [] { return QVariantList { QString() }; } },
        { u"PairingSheet"_s, u"pairing"_s, u"start"_s, [] { return QVariantList {}; } },
        { u"DayOverviewSheet"_s, u"day-overview"_s, u"start"_s, [] { return QVariantList {}; } },
        { u"CsvImportSheet"_s, u"csv-import"_s, u"picked"_s, [id] { return QVariantList { QUrl(id("csv").toString()) }; } },
    };

    // Every editor sheet in qml/editors has to be in the list above
    {
        Case &c = add(u"dialogs-all"_s, u"every editor sheet is in the keyboard test"_s);
        c.steps << check(u"no editor left out"_s, [this, dialogs, noteOnce] {
            QStringList missing, seen;
            QStringList paths;
            QDirIterator it(u":/"_s, { u"*.qml"_s }, QDir::Files, QDirIterator::Subdirectories);
            while (it.hasNext()) {
                const QString path = it.next();
                if (path.contains(u"/editors/"_s))
                    paths << path;
            }
            for (const QString &path : std::as_const(paths)) {
                QFile f(path);
                if (!f.open(QIODevice::ReadOnly))
                    continue;
                const QString text = QString::fromUtf8(f.readAll());
                if (!QRegularExpression(u"^Sheet \\{"_s, QRegularExpression::MultilineOption).match(text).hasMatch())
                    continue;
                const QString type = QFileInfo(path).completeBaseName();
                seen << type;
                if (std::none_of(dialogs.begin(), dialogs.end(), [&type](const Dialog &d) { return d.type == type; }))
                    missing << type;
            }
            if (seen.isEmpty()) {
                noteOnce(u"No editor found in the resources. "_s);
                return false;
            }
            if (!missing.isEmpty())
                noteOnce(u"Not in the keyboard test (UiTestDialogs.cpp): %1. "_s.arg(missing.join(u", "_s)));
            return missing.isEmpty();
        }, 100);
    }

    {
        Case &c = add(u"dialogs-prepare"_s, u"sample data for the editors"_s);
        c.steps << act(u"plan, note, coupling, event, CSV file"_s, [this, ids] {
            const QString membership = runningMembership();
            const QString student = m_data->get(u"membership"_s, membership).value(u"student_id"_s).toString();
            const QString today = m_data->today();
            ids->insert(u"membership"_s, membership);
            ids->insert(u"student"_s, student);
            ids->insert(u"today"_s, today);
            ids->insert(u"plan"_s, m_data->savePlan(membership, {}, today,
                                                    u"**UI-Etüde Nr. 8**\nTakt 1–16 langsam\nTakt 17–32\n\nUI-Tonleiter G-Dur"_s, false));
            ids->insert(u"note"_s, m_data->addNote(membership, u"2026-01-05"_s,
                                                   u"**UI-Sonatine**\n1. Satz\n2. Satz\n\nUI-Bogenhaltung"_s, false));
            // Coupled (as another device of the teacher would sync it): tasks and the sheet's name field
            ids->insert(u"coupling"_s, m_data->create(u"coupling"_s, {
                { u"student_id"_s, student }, { u"server"_s, u"https://cloud.example.org"_s },
                { u"folder"_s, u"Qupil Übeheft/uitest-dialogs"_s }, { u"share_id"_s, u"1"_s },
                { u"token"_s, u"UiTestToken123"_s }, { u"password"_s, u"Qp-7UiTestPassword"_s },
                { u"key"_s, QString::fromLatin1(QByteArray(32, '\x05').toBase64()) }, { u"created"_s, today } }));
            const QVariantList events = m_data->events();
            ids->insert(u"event"_s, events.isEmpty() ? QString() : events.first().toMap().value(u"id"_s).toString());
            const QString csv = m_folder + u"/dialogs.csv"_s;
            QFile f(csv);
            if (f.open(QIODevice::WriteOnly))
                f.write("Vorname;Nachname;Instrument;Telefon\nUi;Eins;Violine;0123\nUi;Zwei;Cello;0456\n");
            ids->insert(u"csv"_s, QUrl::fromLocalFile(csv).toString());
        })
                << check(u"the sample data is there"_s, [ids] {
                       return !ids->value(u"membership"_s).isEmpty() && !ids->value(u"plan"_s).isEmpty()
                              && !ids->value(u"note"_s).isEmpty() && !ids->value(u"event"_s).isEmpty();
                   }, 100);
        c.keepScreen = true;
    }

    // Window sizes: desktop goes through phones and tablets (the window can change its
    // size); phones and tablets test at their own size
    QList<QSize> sizes;
    // (QUPIL_UITEST_OWN_SIZE=1: only the window as it is, e.g. one run per size)
    if (m_app->desktop() && m_window && m_window->width() < 600 && !qEnvironmentVariableIsSet("QUPIL_UITEST_OWN_SIZE"))
        sizes = { QSize(375, 667), QSize(390, 844), QSize(844, 390), QSize(1180, 820) };
    else
        sizes = { QSize() };   // as it is

    for (const Dialog &d : dialogs) {
        auto run = std::make_shared<DialogRun>();
        Case &c = add(u"dialogs-%1"_s.arg(d.label), u"%1 (%2) with the keyboard: Save and the cursor stay in view, no jumping"_s
                                                       .arg(d.type, d.function));
        c.keepScreen = true;
        c.steps << act(u"tidy up after the case before"_s, tidy) << wait(300);
        for (const QSize &size : sizes) {
            const QString where = size.isEmpty() ? QString() : u" (%1×%2)"_s.arg(size.width()).arg(size.height());
            if (!size.isEmpty()) {
                c.steps << act(u"window%1"_s.arg(where), [this, left, size] {
                    if (!left->original.isValid())
                        left->original = m_window->size();
                    m_window->resize(size);
                })
                        << check(u"window has the size%1"_s.arg(where), [this, size] {
                               return m_window->size() == size;
                           }, 3000)
                        << wait(400);
            }
            // 1 = the first field, 2 = the last field (each time also without the keyboard after it)
            for (int field = 1; field <= 2; ++field) {
                const QString which = field == 1 ? u"first field"_s : u"last field"_s;
                const QString tag = u"%1, %2"_s.arg(which, where.isEmpty() ? u"window as it is"_s : where.trimmed());
                c.steps << act(u"open %1 (%2)"_s.arg(d.type, tag), [this, run, d, left] {
                    run->problems.clear();
                    QQmlEngine *engine = qmlEngine(m_window->contentItem());
                    if (!engine) {
                        const auto all = m_window->contentItem()->childItems();
                        for (QQuickItem *i : all)
                            if ((engine = qmlEngine(i)))
                                break;
                    }
                    if (!run->editor && engine) {
                        QQmlComponent component(engine);
                        component.setData(u"import QtQuick\nimport Qupil\nItem { property alias editor: e\n%1 { id: e } }\n"_s
                                              .arg(d.type).toUtf8(),
                                          QUrl(u"qrc:/qt/qml/Qupil/uitest-dialogs.qml"_s));
                        QObject *o = component.beginCreate(engine->rootContext());
                        if (auto *item = qobject_cast<QQuickItem *>(o)) {
                            item->setParentItem(m_window->contentItem());
                            item->setParent(m_window->contentItem());
                            component.completeCreate();
                            run->holder = item;
                            run->editor = item->property("editor").value<QObject *>();
                            left->holder = item;
                            left->editor = run->editor;
                        } else {
                            run->problems = component.errorString();
                            delete o;
                        }
                    }
                    if (!run->editor)
                        return;
                    QVariantList args = d.args();
                    QGenericArgument a[4];
                    for (int i = 0; i < args.size() && i < 4; ++i)
                        a[i] = Q_ARG(QVariant, args[i]);
                    QMetaObject::invokeMethod(run->editor, d.function.toLatin1().constData(), a[0], a[1], a[2], a[3]);
                    // Some editors open themselves only when it makes sense (start() of the plan)
                    if (!run->editor->property("visible").toBool())
                        QMetaObject::invokeMethod(run->editor, "open");
                })
                        << check(u"the editor is open (%1)"_s.arg(tag), [run, noteOnce] {
                               if (!run->editor) {
                                   noteOnce(u"Could not make the editor: %1. "_s.arg(run->problems));
                                   return false;
                               }
                               return run->editor->property("opened").toBool();
                           }, 4000)
                        << wait(300);
                {
                    c.steps << act(u"cursor in the %1"_s.arg(which), [run, field] {
                        const QList<QQuickItem *> fields = fieldsOf(run->editor);
                        run->field = fields.isEmpty() ? nullptr : (field == 1 ? fields.first() : fields.last());
                        if (run->field)
                            run->field->forceActiveFocus(Qt::MouseFocusReason);
                    })
                            << wait(200)
                            << act(u"note the sheet and the cursor, the keyboard comes"_s, [this, run] {
                                   run->samples.clear();
                                   run->clock.start();
                                   QObject::disconnect(&run->timer, nullptr, nullptr, nullptr);
                                   run->timer.setInterval(15);
                                   QObject::connect(&run->timer, &QTimer::timeout, &run->timer, [run] {
                                       if (!run->editor)
                                           return;
                                       const qreal top = run->editor->property("y").toReal();
                                       const qreal bottom = top + run->editor->property("height").toReal();
                                       qreal cursor = -1;
                                       if (run->field) {
                                           const QRectF r = run->field->property("cursorRectangle").toRectF();
                                           cursor = run->field->mapToScene(r.bottomLeft()).y();
                                       }
                                       run->samples << Sample { run->clock.elapsed(), top, bottom, cursor };
                                   });
                                   run->timer.start();
                                   if (run->field) {
                                       const qreal h = m_window->height() > m_window->width() ? m_window->height() * 0.4
                                                                                              : m_window->height() * 0.5;
                                       m_app->setKeyboardHeight(std::round(h));
                                   }
                               })
                            << wait(900)
                            << act(u"stop noting"_s, [run] { run->timer.stop(); })
                            << check(u"with the keyboard: everything in view (%1)"_s.arg(tag), [this, run, noteOnce] {
                                   if (!run->field)
                                       return true;   // no text field in this sheet
                                   QStringList bad;
                                   const qreal kbTop = m_window->height() - m_app->keyboardHeight();
                                   const qreal top = run->editor->property("y").toReal();
                                   const qreal bottom = top + run->editor->property("height").toReal();
                                   if (top < -0.5)
                                       bad << u"sheet off the top (y %1)"_s.arg(qRound(top));
                                   if (bottom > kbTop + 1)
                                       bad << u"sheet under the keyboard (bottom %1, keyboard at %2)"_s.arg(qRound(bottom)).arg(qRound(kbTop));
                                   const QString acceptText = run->editor->property("acceptText").toString();
                                   const bool showAccept = run->editor->property("showAccept").toBool();
                                   for (const QString &text : showAccept ? QStringList { acceptText, run->editor->property("rejectText").toString() }
                                                                         : QStringList { run->editor->property("rejectText").toString() }) {
                                       QQuickItem *b = buttonOf(run->editor, text);
                                       const QRectF r = sceneRect(b);
                                       if (!b)
                                           bad << u"„%1“ not shown"_s.arg(text);
                                       else if (r.top() < -0.5 || r.bottom() > kbTop + 1 || r.left() < -0.5 || r.right() > m_window->width() + 0.5)
                                           bad << u"„%1“ cut off or under the keyboard (%2)"_s.arg(text, rectText(r));
                                   }
                                   if (!run->field->hasActiveFocus())
                                       bad << u"the field lost the cursor"_s;
                                   const QRectF cursor = run->field->mapRectToScene(run->field->property("cursorRectangle").toRectF());
                                   QRectF view = sceneRect(flickOf(run->editor));
                                   if (view.isEmpty())
                                       view = QRectF(0, top, m_window->width(), bottom - top);
                                   view.setBottom(std::min(view.bottom(), kbTop));
                                   if (cursor.top() < view.top() - 1 || cursor.bottom() > view.bottom() + 1)
                                       bad << u"the cursor is not in view (cursor %1, view %2)"_s.arg(rectText(cursor), rectText(view));
                                   if (!bad.isEmpty())
                                       noteOnce(bad.join(u"; "_s) + u". "_s);
                                   return bad.isEmpty();
                               }, 100)
                            << check(u"the sheet moves one way, the cursor does not jump (%1)"_s.arg(tag), [run, noteOnce] {
                                   if (!run->field)
                                       return true;
                                   const QList<Sample> &s = run->samples;
                                   if (s.size() < 5) {
                                       noteOnce(u"Only %1 samples. "_s.arg(s.size()));
                                       return false;
                                   }
                                   QStringList bad;
                                   auto oneWay = [&](const char *what, auto value) {
                                       const qreal total = value(s.last()) - value(s.first());
                                       qreal back = 0;
                                       for (int i = 1; i < s.size(); ++i) {
                                           const qreal step = value(s[i]) - value(s[i - 1]);
                                           if (step * total < 0 || (std::abs(total) < 1 && std::abs(step) > 1))
                                               back = std::max(back, std::abs(step));
                                       }
                                       if (back > 2)
                                           bad << u"%1 went back by %2 px (moved %3 in all)"_s.arg(QLatin1StringView(what)).arg(qRound(back)).arg(qRound(total));
                                   };
                                   oneWay("top of the sheet", [](const Sample &x) { return x.top; });
                                   oneWay("bottom of the sheet", [](const Sample &x) { return x.bottom; });
                                   if (s.first().cursor >= 0)
                                       oneWay("cursor", [](const Sample &x) { return x.cursor; });
                                   if (!bad.isEmpty()) {
                                       noteOnce(bad.join(u"; "_s) + u". "_s);
                                       for (const Sample &x : s)
                                           qInfo("dialogs: %lld ms top %.1f bottom %.1f cursor %.1f", qlonglong(x.ms), x.top, x.bottom, x.cursor);
                                   }
                                   return bad.isEmpty();
                               }, 100)
                            << snap(u"dialog-%1-%2-%3"_s.arg(d.label, field == 1 ? u"first"_s : u"last"_s,
                                                              size.isEmpty() ? u"own"_s : u"%1x%2"_s.arg(size.width()).arg(size.height())))
                            << act(u"keyboard down"_s, [this] { m_app->setKeyboardHeight(0); })
                            << wait(800);
                }
                c.steps << check(u"without the keyboard: Save and Cancel whole on screen (%1)"_s.arg(tag), [this, run, noteOnce] {
                    QStringList bad;
                    const qreal top = run->editor->property("y").toReal();
                    const qreal bottom = top + run->editor->property("height").toReal();
                    if (top < -0.5 || bottom > m_window->height() + 1)
                        bad << u"sheet not on screen (%1 … %2)"_s.arg(qRound(top)).arg(qRound(bottom));
                    const bool showAccept = run->editor->property("showAccept").toBool();
                    QStringList texts { run->editor->property("rejectText").toString() };
                    if (showAccept)
                        texts.prepend(run->editor->property("acceptText").toString());
                    for (const QString &text : texts) {
                        QQuickItem *b = buttonOf(run->editor, text);
                        const QRectF r = sceneRect(b);
                        if (!b)
                            bad << u"„%1“ not shown"_s.arg(text);
                        else if (r.top() < -0.5 || r.bottom() > std::min<qreal>(m_window->height(), bottom) + 1
                                 || r.left() < -0.5 || r.right() > m_window->width() + 0.5)
                            bad << u"„%1“ cut off (%2)"_s.arg(text, rectText(r));
                    }
                    if (!bad.isEmpty())
                        noteOnce(bad.join(u"; "_s) + u". "_s);
                    return bad.isEmpty();
                }, 1500);
                c.steps << act(u"close"_s, [this, run] {
                    m_app->setKeyboardHeight(0);
                    if (run->editor)
                        QMetaObject::invokeMethod(run->editor, "close");
                })
                        << check(u"closed"_s, [run] { return !run->editor || !run->editor->property("visible").toBool(); }, 3000)
                        << wait(250);
            }
        }
        c.steps << act(u"remove the editor, window as before"_s, tidy)
                << wait(400);
    }

    {
        Case &c = add(u"dialogs-cleanup"_s, u"the editors' sample data goes again"_s);
        c.steps << act(u"remove plan, note, coupling"_s, [this, ids, tidy] {
            tidy();
            m_data->deletePlan(ids->value(u"plan"_s), false);
            m_data->deleteNote(ids->value(u"note"_s), false);
            m_data->remove(u"coupling"_s, ids->value(u"coupling"_s));
            QFile::remove(m_folder + u"/dialogs.csv"_s);
        })
                << check(u"not coupled any more"_s, [this, ids] { return !m_data->isCoupled(ids->value(u"student"_s)); }, 1000);
    }
}
