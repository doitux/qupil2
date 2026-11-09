// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "UiTest.h"

#include <cmath>
#include <QAccessible>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QQuickItem>
#include <QQuickWindow>
#include <QRegularExpression>
#include <QSysInfo>
#include <QTimer>

#include <algorithm>
#include <cstdio>

#include "AppController.h"
#include "Clock.h"
#include "DataService.h"
#include "LanguageManager.h"

using namespace Qt::StringLiterals;

namespace {

constexpr int kPollMs = 80;
constexpr int kSettleMs = 250;   // after an action: animations and layouts settle

UiTest *s_test = nullptr;
QtMessageHandler s_previous = nullptr;
QStringList s_warnings;

QString roleName(QAccessible::Role role)
{
    switch (role) {
    case QAccessible::Button: return u"button"_s;
    case QAccessible::CheckBox: return u"checkbox"_s;
    case QAccessible::RadioButton: return u"radio"_s;
    case QAccessible::StaticText: return u"text"_s;
    case QAccessible::EditableText: return u"edit"_s;
    case QAccessible::PageTab: return u"tab"_s;
    case QAccessible::List: return u"list"_s;
    case QAccessible::ListItem: return u"item"_s;
    case QAccessible::Link: return u"link"_s;
    case QAccessible::MenuItem: return u"menuitem"_s;
    case QAccessible::ComboBox: return u"combobox"_s;
    case QAccessible::Slider: return u"slider"_s;
    case QAccessible::Dialog: return u"dialog"_s;
    case QAccessible::Heading: return u"heading"_s;
    default: return u"%1"_s.arg(int(role));
    }
}

} // namespace

UiTest::UiTest(QQuickWindow *window, QObject *theme, QObject *nav, QObject *metronome, AppController *app,
               DataService *data, LanguageManager *languages, const QString &folder, const QString &phase,
               QObject *parent)
    : QObject(parent)
    , m_window(window)
    , m_theme(theme)
    , m_nav(nav)
    , m_metronome(metronome)
    , m_app(app)
    , m_data(data)
    , m_languages(languages)
    , m_folder(folder)
    , m_phase(phase)
{
    QDir().mkpath(m_folder);
    s_test = this;
    s_previous = qInstallMessageHandler(messageHandler);
    // No QAccessible::setActive(): the items' accessible names are read directly (nodes()),
    // and on Android switching it on calls into Android's thread while it waits for Qupil
}

UiTest::~UiTest()
{
    if (s_test == this) {
        qInstallMessageHandler(s_previous);
        s_test = nullptr;
    }
}

void UiTest::messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    const bool fromQml = (context.file && QByteArray(context.file).endsWith(".qml")) || message.contains(u".qml:"_s);
    // (QtInfoMsg comes after QtFatalMsg in the enum: console.info is no warning)
    if ((type == QtWarningMsg || type == QtCriticalMsg || type == QtFatalMsg) && fromQml)
        s_warnings << message;
    if (s_previous)
        s_previous(type, context, message);
}

void UiTest::start()
{
    // The cases depend on the layout (phone or wider): made once the window has its size
    QTimer::singleShot(600, this, [this] {
        if (m_phase == u"restart"_s)
            addRestartPhase();
        else if (m_phase == u"draft"_s)
            addDraftPhase();
        else
            addCases();
        // "only:a,b,c": just these cases (a long run split into several starts)
        if (m_phase.startsWith(u"only:"_s)) {
            const QStringList keep = m_phase.mid(5).split(u',', Qt::SkipEmptyParts);
            m_cases.removeIf([&keep](const Case &c) { return !keep.contains(c.name); });
        }
        // First the reminders Qupil shows at the start (a few seconds after it): closed
        // as a person would, so they do not take the taps of the first cases
        if (m_phase != u"restart"_s) {
            Case start { u"start"_s, u"close the reminders shown at the start"_s, {} };
            for (int i = 0; i < 3; ++i)
                start.steps << tapIfThere(u"^OK$"_s, i == 0 ? 6000 : 1500, u"close a reminder (if any)"_s);
            m_cases.prepend(start);
        }
        next();
    });
}

// ------------------------------------------------------------------------ the screen

namespace {
//! WCAG relative luminance and contrast ratio (1 … 21)
double luminance(const QColor &c)
{
    const auto channel = [](double v) { return v <= 0.03928 ? v / 12.92 : std::pow((v + 0.055) / 1.055, 2.4); };
    return 0.2126 * channel(c.redF()) + 0.7152 * channel(c.greenF()) + 0.0722 * channel(c.blueF());
}
double contrast(const QColor &a, const QColor &b)
{
    const double la = luminance(a), lb = luminance(b);
    return (std::max(la, lb) + 0.05) / (std::min(la, lb) + 0.05);
}
//! The last painted, filled rectangle under \a point in this part of the scene
QColor topFilledAt(QQuickItem *root, const QPointF &point)
{
    if (!root || !root->isVisible() || root->opacity() < 0.3 || root->inherits("QQuickText"))
        return {};
    QColor found;
    if (root->inherits("QQuickRectangle") && root->width() >= 8 && root->height() >= 8) {
        const QColor c = root->property("color").value<QColor>();
        if (c.alphaF() >= 0.5 && root->mapRectToScene(QRectF(0, 0, root->width(), root->height())).contains(point))
            found = c;
    }
    QList<QQuickItem *> children = root->childItems();
    std::stable_sort(children.begin(), children.end(), [](QQuickItem *a, QQuickItem *b) { return a->z() < b->z(); });
    for (QQuickItem *child : std::as_const(children)) {
        const QColor c = topFilledAt(child, point);
        if (c.isValid())
            found = c;
    }
    return found;
}

//! What a text is painted on: going up from the text, whatever was painted before it at
//! its middle – siblings below it (a control's background, a card) or the parent itself
QColor backgroundOf(QQuickItem *item, const QColor &window)
{
    const QPointF middle = item->mapToScene(QPointF(item->width() / 2, item->height() / 2));
    for (QQuickItem *child = item, *p = item->parentItem(); p; child = p, p = p->parentItem()) {
        QList<QQuickItem *> siblings = p->childItems();
        std::stable_sort(siblings.begin(), siblings.end(), [](QQuickItem *a, QQuickItem *b) { return a->z() < b->z(); });
        QColor found;
        for (QQuickItem *s : std::as_const(siblings)) {
            if (s == child)
                break;
            const QColor c = topFilledAt(s, middle);
            if (c.isValid())
                found = c;
        }
        if (found.isValid())
            return found;
        if (p->inherits("QQuickRectangle") && p->isVisible() && p->opacity() >= 0.3) {
            const QColor c = p->property("color").value<QColor>();
            if (c.alphaF() >= 0.5)
                return c;
        }
    }
    return window;
}
} // namespace

QStringList UiTest::unreadable() const
{
    QStringList found;
    if (!m_window)
        return found;
    const QRectF bounds(QPointF(0, 0), QSizeF(m_window->size()));
    std::function<void(QQuickItem *, QRectF, qreal, int)> walk = [&](QQuickItem *item, QRectF clip, qreal opacity, int depth) {
        if (!item || depth > 80 || !item->isVisible())
            return;
        opacity *= item->opacity();
        if (opacity < 0.5)   // fading in or out, or deliberately faint (disabled)
            return;
        const QRectF rect = item->mapRectToScene(QRectF(0, 0, item->width(), item->height()));
        if (item->clip())
            clip = clip.intersected(rect);
        if (item->inherits("QQuickText")) {
            const QString text = item->property("text").toString().simplified();
            const QColor color = item->property("color").value<QColor>();
            const QRectF shown = rect.intersected(clip);
            if (!text.isEmpty() && color.alphaF() >= 0.5 && shown.width() > 2 && shown.height() > 2) {
                const QColor behind = backgroundOf(item, m_window->color());
                const double ratio = contrast(color, behind);
                if (ratio < 2.0)
                    found << u"\"%1\" (%2 on %3, %4:1)"_s.arg(text.left(40), color.name(), behind.name())
                                 .arg(ratio, 0, 'f', 1);
            }
        }
        for (QQuickItem *child : item->childItems())
            walk(child, clip, opacity, depth + 1);
    };
    walk(m_window->contentItem(), bounds, 1.0, 0);
    found.removeDuplicates();
    return found;
}

QList<UiTest::Node> UiTest::nodes() const
{
    // The items themselves (not the window's accessibility tree, which leaves out popups):
    // every visible item with a name for screen readers, in paint order
    QList<Node> list;
    if (!m_window)
        return list;
    const QRectF bounds(QPointF(0, 0), QSizeF(m_window->size()));
    std::function<void(QQuickItem *, QRectF, int, bool)> walk = [&](QQuickItem *item, QRectF clip, int depth,
                                                                    bool scrolled) {
        if (!item || depth > 80 || !item->isVisible() || item->opacity() < 0.01)
            return;
        const QRectF rect = item->mapRectToScene(QRectF(0, 0, item->width(), item->height()));
        // Inside a Flickable whose own frame is on the screen: may be scrolled to
        if (item->inherits("QQuickFlickable") && rect.intersected(clip).height() > 1)
            scrolled = true;
        if (item->clip())
            clip = clip.intersected(rect);
        QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(item);
        const bool button = item->inherits("QQuickAbstractButton");
        if (iface || button) {
            QString name = iface ? iface->text(QAccessible::Name).simplified() : QString();
            // A button without its own name is read by its text (as screen readers do)
            if (name.isEmpty() && button)
                name = item->property("text").toString().simplified();
            const QRectF shown = rect.intersected(clip);
            const bool hidden = iface && iface->state().invisible;
            const bool onScreen = shown.width() > 1 && shown.height() > 1;
            // Scrolled out of sight (a smaller window): still there, a tap scrolls to it first
            if (!name.isEmpty() && !hidden && (onScreen || scrolled))
                list << Node { name, iface ? roleName(iface->role()) : u"button"_s, onScreen ? shown : QRectF(),
                               QByteArray(item->metaObject()->className()).startsWith("NavButton"), item, rect };
        }
        QList<QQuickItem *> children = item->childItems();
        std::stable_sort(children.begin(), children.end(), [](QQuickItem *a, QQuickItem *b) { return a->z() < b->z(); });
        for (QQuickItem *child : children)
            walk(child, clip, depth + 1, scrolled);
    };
    walk(m_window->contentItem(), bounds, 0, false);
    return list;
}

bool UiTest::find(const QString &pattern, Node *found) const
{
    // "edit:^Vorname$" – only elements of that role (edit, button, …)
    QString role;
    QString text = pattern;
    static const QRegularExpression rolePrefix(u"^([a-z]+):(.*)$"_s);
    const QRegularExpressionMatch m = rolePrefix.match(pattern);
    if (m.hasMatch()) {
        role = m.captured(1);
        text = m.captured(2);
    }
    const QRegularExpression regex(text);
    const QList<Node> all = nodes();
    // The last match is on top (editors and dialogs come after the page in the tree)
    // On the screen first, then scrolled out of sight
    for (const bool onScreen : { true, false }) {
        for (auto it = all.crbegin(); it != all.crend(); ++it) {
            if (onScreen == it->rect.isEmpty())
                continue;
            // The navigation's tabs only by "tab:…" (a page's back button has the name of its page)
            const bool roleOk = role == u"tab"_s ? it->tab : !it->tab && (role.isEmpty() || it->role == role);
            if (roleOk && regex.match(it->name).hasMatch()) {
                if (found)
                    *found = *it;
                return true;
            }
        }
    }
    return false;
}

QString UiTest::dump() const
{
    QString text;
    for (const Node &n : nodes()) {
        const QRectF r = n.rect.isEmpty() ? n.full : n.rect;
        text += u"%1 %2 %3 %4 %5  %6%7\n"_s.arg(n.tab ? u"tab"_s : n.role, -9)
                    .arg(int(r.x()), 5).arg(int(r.y()), 5)
                    .arg(int(r.width()), 5).arg(int(r.height()), 5)
                    .arg(n.name, n.rect.isEmpty() ? u"  (scrolled out of sight)"_s : QString());
    }
    return text;
}

void UiTest::click(const QPointF &point)
{
    if (!m_window)
        return;
    const QPointF global = m_window->mapToGlobal(point);
    QMouseEvent press(QEvent::MouseButtonPress, point, global, Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    QCoreApplication::sendEvent(m_window.data(), &press);
    QMouseEvent release(QEvent::MouseButtonRelease, point, global, Qt::LeftButton, Qt::NoButton, Qt::NoModifier);
    QCoreApplication::sendEvent(m_window.data(), &release);
}

void UiTest::sendKey(int key, const QString &text)
{
    if (!m_window)
        return;
    QKeyEvent press(QEvent::KeyPress, key, Qt::NoModifier, text);
    QCoreApplication::sendEvent(m_window.data(), &press);
    QKeyEvent release(QEvent::KeyRelease, key, Qt::NoModifier, text);
    QCoreApplication::sendEvent(m_window.data(), &release);
}

void UiTest::writeDump(const QString &name) const
{
    QFile f(QDir(m_folder).filePath(name + u".txt"_s));
    if (f.open(QIODevice::WriteOnly | QIODevice::Text))
        f.write(dump().toUtf8());
}

void UiTest::shot(const QString &name)
{
    if (!m_window)
        return;
    const QImage image = m_window->grabWindow();
    if (!image.isNull())
        image.save(QDir(m_folder).filePath(name + u".png"_s));
}

bool UiTest::busy() const
{
    // A page stack in its transition takes no input
    if (!m_window)
        return false;
    for (QObject *o : m_window->findChildren<QObject *>()) {
        if (o->inherits("QQuickStackView") && o->property("busy").toBool())
            return true;
    }
    return false;
}

bool UiTest::scrollIntoView(const Node &node)
{
    if (!node.item)
        return false;
    for (QQuickItem *p = node.item->parentItem(); p; p = p->parentItem()) {
        if (!p->inherits("QQuickFlickable"))
            continue;
        auto *content = p->property("contentItem").value<QQuickItem *>();
        if (!content)
            return false;
        const QRectF r = node.item->mapRectToItem(content, QRectF(0, 0, node.item->width(), node.item->height()));
        const qreal maxY = std::max<qreal>(0, p->property("contentHeight").toReal() - p->height());
        const qreal y = std::clamp<qreal>(r.center().y() - p->height() / 2, 0, maxY);
        if (qAbs(p->property("contentY").toReal() - y) < 1)
            return false;
        p->setProperty("contentY", y);
        return true;
    }
    return false;
}

QVariant UiTest::windowProperty(const char *name) const
{
    return m_window ? m_window->property(name) : QVariant();
}

bool UiTest::compact() const
{
    return windowProperty("compact").toBool();
}

bool UiTest::editorOpen() const
{
    return m_nav && m_nav->property("openSheets").toInt() > 0;
}

void UiTest::resetToToday()
{
    if (!m_window)
        return;
    QMetaObject::invokeMethod(m_window.data(), "closeEditors");
    // Back on every page (as the system back button), so no page stays open on a stack
    for (int page = 4; page >= 0; --page) {
        m_window->setProperty("currentPage", page);
        for (int i = 0; i < 8; ++i) {
            QVariant handled;
            QMetaObject::invokeMethod(m_window.data(), "goBack", Q_RETURN_ARG(QVariant, handled));
            if (!handled.toBool())
                break;
        }
    }
    if (m_theme)
        m_theme->setProperty("mode", u"light"_s);
    if (m_languages)
        m_languages->setLanguage(u"de"_s);
    if (m_app->testMode())
        m_app->setTestMode(false);
}

// ------------------------------------------------------------------------ steps

UiTest::Case &UiTest::add(const QString &name, const QString &title)
{
    m_cases << Case { name, title, {} };
    return m_cases.last();
}

UiTest::Step UiTest::tap(const QString &pattern, const QString &what)
{
    return Step { what.isEmpty() ? u"tap \"%1\""_s.arg(pattern) : what, [this, pattern] {
                      Node node;
                      if (busy() || !find(pattern, &node))
                          return false;
                      // Partly out of sight: scrolled into view first, as a person would
                      if ((node.rect.isEmpty() || node.rect.height() < node.full.height() - 1) && scrollIntoView(node))
                          return false;
                      if (node.rect.isEmpty())
                          return false;
                      click(node.rect.center());
                      return true;
                  } };
}

UiTest::Step UiTest::tapAt(std::function<QPointF()> where, const QString &what)
{
    return Step { what, [this, where] {
                      const QPointF p = where();
                      if (p.isNull())
                          return false;
                      click(p);
                      return true;
                  } };
}

UiTest::Step UiTest::tapIfThere(const QString &pattern, int withinMs, const QString &what)
{
    auto since = std::make_shared<QElapsedTimer>();
    Step inner = tap(pattern);
    return Step { what, [since, inner, withinMs] {
                      if (!since->isValid())
                          since->start();
                      if (inner.run() || since->elapsed() > withinMs) {
                          since->invalidate();
                          return true;
                      }
                      return false;
                  },
                  withinMs + 2000 };
}

UiTest::Step UiTest::tapUntil(const QString &pattern, const QString &expect)
{
    // Taps again when nothing happened (at most three times) and notes it: a tap that
    // only closed the keyboard, for example, is worth knowing about
    auto taps = std::make_shared<int>(0);
    auto since = std::make_shared<QElapsedTimer>();
    return Step { u"tap \"%1\" until \"%2\""_s.arg(pattern, expect), [this, pattern, expect, taps, since] {
                      if (*taps > 0 && find(expect)) {
                          if (*taps > 1)
                              m_results.last().note += u"%1 taps needed for \"%2\". "_s.arg(*taps).arg(pattern);
                          *taps = 0;
                          since->invalidate();
                          return true;
                      }
                      if (*taps > 0 && since->isValid() && since->elapsed() < 2000)
                          return false;
                      if (*taps >= 3 || busy())
                          return false;
                      Node node;
                      if (!find(pattern, &node) || node.rect.isEmpty())
                          return false;
                      click(node.rect.center());
                      ++*taps;
                      since->start();
                      return false;
                  },
                  10000 };
}

UiTest::Step UiTest::openList(const QString &pattern)
{
    auto taps = std::make_shared<int>(0);
    auto since = std::make_shared<QElapsedTimer>();
    return Step { u"open the list of \"%1\""_s.arg(pattern), [this, pattern, taps, since] {
                      Node node;
                      if (!find(pattern, &node) || !node.item)
                          return false;
                      QObject *popup = node.item->property("popup").value<QObject *>();
                      if (popup && popup->property("opened").toBool()) {
                          if (*taps > 1)
                              m_results.last().note += u"%1 taps needed for \"%2\". "_s.arg(*taps).arg(pattern);
                          return true;
                      }
                      if ((*taps > 0 && since->isValid() && since->elapsed() < 1500) || *taps >= 3 || busy()
                          || node.rect.isEmpty())
                          return false;
                      click(node.rect.center());
                      ++*taps;
                      since->start();
                      return false;
                  },
                  8000 };
}

UiTest::Step UiTest::backIfThere(const QString &pattern)
{
    // The back button of a page carries the name of the page it goes back to
    return Step { u"back (\"%1\") if the page has one"_s.arg(pattern), [this, pattern] {
                      if (busy())
                          return false;
                      Node node;
                      if (find(pattern, &node) && !node.rect.isEmpty())
                          click(node.rect.center());
                      return true;
                  } };
}

UiTest::Step UiTest::see(const QString &pattern)
{
    return Step { u"see \"%1\""_s.arg(pattern), [this, pattern] { return find(pattern); } };
}

UiTest::Step UiTest::gone(const QString &pattern)
{
    return Step { u"no more \"%1\""_s.arg(pattern), [this, pattern] { return !find(pattern); } };
}

UiTest::Step UiTest::type(const QString &text)
{
    // Into a field that has the focus (a tap on it may still be on its way). On phones and
    // tablets the on-screen keyboard comes first: once it is there it may reset what it
    // thinks the field holds, and keys typed before would be lost. Then the field must hold
    // the text; if not, it is typed once more.
    auto since = std::make_shared<QElapsedTimer>();
    auto tries = std::make_shared<int>(0);
    return Step { u"type \"%1\""_s.arg(text), [this, text, since, tries] {
                      QQuickItem *focus = m_window ? m_window->activeFocusItem() : nullptr;
                      if (!focus || !(focus->inherits("QQuickTextInput") || focus->inherits("QQuickTextEdit")))
                          return false;
                      if (!since->isValid())
                          since->start();
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
                      if (*tries == 0 && !QGuiApplication::inputMethod()->isVisible() && since->elapsed() < 3000)
                          return false;
                      if (*tries == 0 && since->elapsed() < 800)
                          return false;   // the keyboard settles
#endif
                      const QString plain = text;
                      const auto holds = [focus, plain] {
                          QString value = focus->property("text").toString();
                          // Rich text: compare without the markup
                          value.remove(QRegularExpression(u"<[^>]*>"_s));
                          return value.contains(plain);
                      };
                      if (*tries > 0) {
                          if (holds()) {
                              *tries = 0;
                              since->invalidate();
                              return true;
                          }
                          if (since->elapsed() < 1000 || *tries >= 2)
                              return false;
                      }
                      for (const QChar c : text) {
                          if (c == u'\n')
                              sendKey(Qt::Key_Return, u"\r"_s);
                          else
                              sendKey(0, QString(c));
                      }
                      ++*tries;
                      since->restart();
                      return false;
                  } };
}

UiTest::Step UiTest::clearField()
{
    return Step { u"clear the field"_s, [this] {
                      QQuickItem *focus = m_window ? m_window->activeFocusItem() : nullptr;
                      if (!focus || !(focus->inherits("QQuickTextInput") || focus->inherits("QQuickTextEdit")))
                          return false;
                      for (int i = 0; i < 40 && !focus->property("text").toString().isEmpty(); ++i)
                          sendKey(Qt::Key_Backspace);
                      return focus->property("text").toString().isEmpty();
                  } };
}

// Students page: search the student by name and open the page (independent of what an
// earlier case left in the search field or how far the list is scrolled)
QList<UiTest::Step> UiTest::openStudent(const QString &name)
{
    return { tab(u"Schüler"_s), tap(u"edit:^Schüler suchen$"_s), clearField(),
             type(name.section(u' ', 0, 0)),
             tapUntil(u"^%1$"_s.arg(QRegularExpression::escape(name)), u"^Unterricht wählen$"_s) };
}

UiTest::Step UiTest::key(int k, const QString &what)
{
    return Step { what, [this, k] {
                      sendKey(k);
                      return true;
                  } };
}

UiTest::Step UiTest::check(const QString &what, std::function<bool()> test, int timeoutMs)
{
    return Step { what, test, timeoutMs };
}

UiTest::Step UiTest::act(const QString &what, std::function<void()> action)
{
    return Step { what, [action] {
                      action();
                      return true;
                  } };
}

UiTest::Step UiTest::wait(int ms)
{
    auto since = std::make_shared<QElapsedTimer>();
    return Step { u"wait %1 ms"_s.arg(ms), [since, ms] {
                      if (!since->isValid())
                          since->start();
                      if (since->elapsed() < ms)
                          return false;
                      since->invalidate();
                      return true;
                  },
                  ms + 2000 };
}

UiTest::Step UiTest::snap(const QString &name)
{
    return Step { u"screenshot %1"_s.arg(name), [this, name] {
                      shot(name);
                      // Every screenshot is also a check that all text on it can be read
                      for (const QString &t : unreadable())
                          if (!m_unreadable.contains(t))
                              m_unreadable << t;
                      return true;
                  } };
}

// ------------------------------------------------------------------------ running

void UiTest::next()
{
    if (m_case < 0 || m_case >= m_cases.size() || m_step >= m_cases[m_case].steps.size()) {
        if (m_case >= 0 && m_case < m_cases.size())
            finishCase();
        ++m_case;
        m_step = 0;
        m_stepStarted = false;
        if (m_case >= m_cases.size()) {
            report();
            return;
        }
        qInfo("ui test: %s", qPrintable(m_cases[m_case].name));
        // After a restart the editor is open with the draft: the first case starts from there
        if (m_case == 0 ? m_phase != u"restart"_s : !m_cases[m_case - 1].keepScreen)
            resetToToday();
        s_warnings.clear();
        m_unreadable.clear();
        m_results << Result { m_cases[m_case].name, m_cases[m_case].title };
        m_caseTimer.start();
        QTimer::singleShot(kSettleMs * 2, this, &UiTest::next);
        return;
    }
    Step &step = m_cases[m_case].steps[m_step];
    if (!m_stepStarted) {
        m_stepStarted = true;
        m_stepTimer.start();
    }
    if (step.run()) {
        ++m_step;
        m_stepStarted = false;
        QTimer::singleShot(kSettleMs, this, &UiTest::next);
        return;
    }
    if (m_stepTimer.elapsed() > step.timeoutMs) {
        Result &r = m_results.last();
        r.passed = false;
        r.failure = u"step %1: %2"_s.arg(m_step + 1).arg(step.what);
        const QString name = u"fail-%1"_s.arg(m_cases[m_case].name);
        shot(name);
        QFile file(QDir(m_folder).filePath(name + u".txt"_s));
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
            file.write(dump().toUtf8());
        m_step = m_cases[m_case].steps.size();   // on to the next case
        m_stepStarted = false;
        QTimer::singleShot(kSettleMs, this, &UiTest::next);
        return;
    }
    QTimer::singleShot(kPollMs, this, &UiTest::next);
}

void UiTest::finishCase()
{
    Result &r = m_results.last();
    if (r.passed) {
        // What the screen looked like at the end (for comparing platforms)
        shot(u"case-%1"_s.arg(r.name));
        for (const QString &t : unreadable())
            if (!m_unreadable.contains(t))
                m_unreadable << t;
        if (!m_unreadable.isEmpty()) {
            r.passed = false;
            r.failure = u"text not readable: "_s + m_unreadable.join(u"; "_s);
        }
    }
    r.ms = m_caseTimer.elapsed();
    r.warnings = s_warnings;
    if (!s_warnings.isEmpty() && r.passed) {
        r.passed = false;
        r.failure = u"QML warnings"_s;
    }
    qInfo("ui test: %s %s%s", qPrintable(r.name), r.passed ? "passed" : "FAILED",
          r.passed ? "" : qPrintable(u" – "_s + r.failure));
}

void UiTest::report()
{
    int failed = 0;
    QJsonArray cases;
    QString md = u"# Qupil UI test\n\n- platform: %1 (%2), Qt %3\n- window: %4 × %5\n\n"_s
                     .arg(QSysInfo::prettyProductName(), QGuiApplication::platformName(), QString::fromLatin1(qVersion()))
                     .arg(m_window ? m_window->width() : 0)
                     .arg(m_window ? m_window->height() : 0);
    md += u"| | Case | Time | Failure / note |\n|---|---|---|---|\n"_s;
    for (const Result &r : m_results) {
        if (!r.passed)
            ++failed;
        cases.append(QJsonObject { { u"name"_s, r.name }, { u"title"_s, r.title }, { u"passed"_s, r.passed },
                                   { u"note"_s, r.note },
                                   { u"failure"_s, r.failure }, { u"ms"_s, r.ms },
                                   { u"warnings"_s, QJsonArray::fromStringList(r.warnings) } });
        md += u"| %1 | %2 – %3 | %4 s | %5 |\n"_s.arg(r.passed ? u"✅"_s : u"❌"_s, r.name, r.title)
                  .arg(r.ms / 1000.0, 0, 'f', 1)
                  .arg(QString(r.failure.isEmpty() ? r.note : r.failure).replace(u'|', u'/'));
    }
    for (const Result &r : m_results) {
        if (r.warnings.isEmpty())
            continue;
        md += u"\n## QML warnings in %1\n\n```\n%2\n```\n"_s.arg(r.name, r.warnings.join(u'\n'));
    }
    md += u"\n%1 of %2 cases passed.\n"_s.arg(m_results.size() - failed).arg(m_results.size());
    const QJsonObject result { { u"platform"_s, QSysInfo::prettyProductName() },
                               { u"qpa"_s, QGuiApplication::platformName() },
                               { u"qt"_s, QString::fromLatin1(qVersion()) },
                               { u"failed"_s, failed },
                               { u"cases"_s, cases } };
    QFile json(QDir(m_folder).filePath(u"results.json"_s));
    if (json.open(QIODevice::WriteOnly))
        json.write(QJsonDocument(result).toJson());
    QFile text(QDir(m_folder).filePath(u"results.md"_s));
    if (text.open(QIODevice::WriteOnly | QIODevice::Text))
        text.write(md.toUtf8());
    qInfo("ui test: %lld of %lld cases passed (%s)", qlonglong(m_results.size() - failed),
          qlonglong(m_results.size()), qPrintable(QDir::toNativeSeparators(m_folder)));
    // "draft": Qupil stays open with the unsaved note, the system ends it from outside
    if (m_phase != u"draft"_s)
        QCoreApplication::exit(failed == 0 ? EXIT_SUCCESS : 1);
}

// ------------------------------------------------------------------------ the cases

QString UiTest::runningMembership() const
{
    if (!m_window)
        return {};
    for (QObject *o : m_window->findChildren<QObject *>(u"scheduleOverview"_s)) {
        const QVariantList students = o->property("running").toMap().value(u"students"_s).toList();
        if (!students.isEmpty())
            return students.first().toMap().value(u"membership_id"_s).toString();
    }
    return {};
}

QStringList UiTest::allLessons() const
{
    QStringList ids;
    for (int day = 0; day <= 6; ++day)   // 0 = Monday
        for (const QVariant &l : m_data->lessonsOn(day))
            ids << l.toMap().value(u"id"_s).toString();
    for (const QVariant &l : m_data->irregularLessons())
        ids << l.toMap().value(u"id"_s).toString();
    ids.removeDuplicates();
    return ids;
}

QString UiTest::latestNote(const QString &membership) const
{
    const QVariantList notes = m_data->notes(membership);
    return notes.isEmpty() ? QString() : m_data->plainText(notes.first().toMap().value(u"content"_s).toString());
}

// Tab bar at the bottom (phone) or side (desktop): the tab, not a button of the page with the same name
UiTest::Step UiTest::tab(const QString &name)
{
    Step step = tap(u"tab:^%1$"_s.arg(QRegularExpression::escape(name)));
    step.what = u"tab \"%1\""_s.arg(name);
    return step;
}

void UiTest::addRestartPhase()
{
    Case &c = add(u"restart-draft"_s, u"after a restart the unsaved note is in the editor again"_s);
    c.keepScreen = true;
    c.steps << see(u"^Notiz speichern$"_s)
            << tapIfThere(u"^OK$"_s, 3000, u"close the reminders shown at the start (if any)"_s)
            << check(u"the draft is kept"_s, [this] { return m_app->keptDraft(u"note"_s).contains(u"Vor dem Neustart"_s); })
            << snap(u"restart-draft"_s)
            << tap(u"^Abbrechen$"_s)
            << gone(u"^Notiz speichern$"_s)
            << check(u"the draft is gone after cancelling"_s, [this] { return m_app->keptDraft(u"note"_s).isEmpty(); });
}

void UiTest::addDraftPhase()
{
    Case &c = add(u"draft-prepare"_s, u"an unsaved note, then the system ends Qupil (from outside)"_s);
    c.keepScreen = true;
    c.steps << tapIfThere(u"^OK$"_s, 3000, u"close the reminders shown at the start (if any)"_s)
            << tap(u"^Notiz$"_s) << see(u"^Notiz speichern$"_s)
            << tap(u"edit:^Notiz$"_s) << type(u"Vor dem Neustart getippt"_s);
}

void UiTest::addCases()
{
    // Every page, with what it shows to screen readers (for comparing platforms)
    {
        Case &c = add(u"pages"_s, u"every page opens"_s);
        const char *const tabs[] = { "Schüler", "Veranstaltungen", "Mehr", "Stundenplan" };
        c.steps << see(u"tab:^Stundenplan$"_s);
        for (const char *t : tabs)
            c.steps << tab(QString::fromUtf8(t)) << wait(200)
                    << check(u"page %1 shown"_s.arg(QString::fromUtf8(t)), [this, t] {
                           const int pages[] = { 1, 2, 4, 0 };
                           const char *const names[] = { "Schüler", "Veranstaltungen", "Mehr", "Stundenplan" };
                           for (int i = 0; i < 4; ++i)
                               if (qstrcmp(names[i], t) == 0)
                                   return windowProperty("currentPage").toInt() == pages[i];
                           return false;
                       })
                    << act(u"dump"_s, [this, t] {
                           QFile f(QDir(m_folder).filePath(u"screen-%1.txt"_s.arg(QString::fromUtf8(t))));
                           if (f.open(QIODevice::WriteOnly | QIODevice::Text))
                               f.write(dump().toUtf8());
                       })
                    << snap(u"page-%1"_s.arg(QString::fromUtf8(t)));
    }
    // Today: the running lesson, another day of the week and back
    {
        Case &c = add(u"today"_s, u"today with the running lesson, switching days"_s);
        c.steps << see(u"^Lena Muster$"_s) << see(u"^Notiz$"_s)
                << check(u"a lesson is running"_s, [this] { return !runningMembership().isEmpty(); })
                << tap(u"^Mittwoch$"_s) << gone(u"^Lena Muster$"_s)
                << tap(u"^Dienstag$"_s) << see(u"^Lena Muster$"_s);
    }
    // A note for the running lesson, typed and saved
    {
        Case &c = add(u"note-save"_s, u"write and save a note for the running lesson"_s);
        c.steps << act(u"remember the lesson"_s, [this] { m_membership = runningMembership(); })
                << tap(u"^Notiz$"_s) << see(u"^Notiz speichern$"_s)
                << tap(u"edit:^Notiz$"_s) << type(u"UI-Test: G-Dur-Tonleiter zweioktavig"_s)
                << snap(u"note-typed"_s)
                << tap(u"^Notiz speichern$"_s) << gone(u"^Notiz speichern$"_s)
                << check(u"the note is saved"_s, [this] { return latestNote(m_membership).contains(u"G-Dur-Tonleiter zweioktavig"_s); })
                << check(u"no draft left"_s, [this] { return m_app->keptDraft(u"note"_s).isEmpty(); });
    }
    // Typed and cancelled: nothing saved, no draft
    {
        Case &c = add(u"note-cancel"_s, u"cancel a note: nothing saved, no draft"_s);
        c.steps << act(u"remember the lesson"_s, [this] { m_membership = runningMembership(); })
                << tap(u"^Notiz$"_s) << see(u"^Notiz speichern$"_s)
                << tap(u"edit:^Notiz$"_s) << type(u"Wird verworfen"_s)
                << tap(u"^Abbrechen$"_s) << gone(u"^Notiz speichern$"_s)
                << check(u"not saved"_s, [this] { return !latestNote(m_membership).contains(u"Wird verworfen"_s); })
                << check(u"no draft"_s, [this] { return m_app->keptDraft(u"note"_s).isEmpty(); });
    }
    // Students: search, open one, back
    {
        Case &c = add(u"student-search"_s, u"search a student, open and close the student page"_s);
        c.steps << tab(u"Schüler"_s) << tap(u"edit:^Schüler suchen$"_s) << type(u"Lena"_s)
                << see(u"^Lena Muster$"_s) << gone(u"^Jonas Becker$"_s)
                << snap(u"student-search"_s)
                << tapUntil(u"^Lena Muster$"_s, u"^Unterricht wählen$"_s)
                << snap(u"student-page"_s);
        // Narrow: the student page instead of the list, with a way back; wide: side by side
        c.steps << backIfThere(u"^Schüler$"_s) << see(u"edit:^Schüler suchen$"_s);
    }
    // A new student
    {
        Case &c = add(u"student-new"_s, u"add a student"_s);
        c.steps << tab(u"Schüler"_s) << tap(u"^Neuer Schüler$"_s) << see(u"edit:^Vorname$"_s)
                << tap(u"edit:^Vorname$"_s) << type(u"Uwe"_s)
                << tap(u"edit:^Nachname$"_s) << type(u"Testmann"_s)
                << snap(u"student-new"_s)
                << tap(u"^Speichern$"_s) << gone(u"edit:^Vorname$"_s)
                << check(u"the student is there"_s, [this] { return m_data->students(u"Testmann"_s).size() == 1; });
    }
    // Events: open one and back
    {
        Case &c = add(u"event-open"_s, u"open an event and go back"_s);
        c.steps << tab(u"Veranstaltungen"_s) << tap(u"^Herbstvorspiel$"_s) << see(u"^Veranstaltung bearbeiten$"_s)
                << snap(u"event-page"_s);
        c.steps << backIfThere(u"^Veranstaltungen$"_s) << see(u"^Herbstvorspiel$"_s);
    }
    // Metronome: start and stop
    {
        Case &c = add(u"metronome"_s, u"start and stop the metronome"_s);
        // Without a sound output (some test machines) the metronome cannot start and says so
        c.steps << tab(u"Schüler"_s) << tab(u"Metronom"_s) << see(u"^Im Takt tippen$"_s) << tap(u"^Start$"_s)
                << check(u"running, or a message that there is no sound output"_s, [this] {
                       return m_metronome && (m_metronome->property("running").toBool()
                                              || !m_metronome->property("error").toString().isEmpty());
                   })
                << act(u"note"_s, [this] {
                       if (m_metronome && !m_metronome->property("running").toBool())
                           m_results.last().note = u"no sound: "_s + m_metronome->property("error").toString();
                   })
                << tap(u"^(Stopp|Start)$"_s, u"tap Stopp (or Start again)"_s);
    }
    // Appearance: dark and back to light
    {
        Case &c = add(u"appearance"_s, u"switch to dark and back to light"_s);
        c.steps << tab(u"Mehr"_s) << tap(u"^Einstellungen$"_s) << tap(u"^Darstellung"_s)
                << act(u"dump"_s, [this] { writeDump(u"screen-appearance"_s); })
                << tap(u"^Dunkel$"_s)
                << check(u"dark"_s, [this] { return m_theme && m_theme->property("dark").toBool(); })
                << snap(u"appearance-dark"_s)
                << tap(u"^Hell$"_s)
                << check(u"light"_s, [this] { return m_theme && !m_theme->property("dark").toBool(); });
    }
    // Language: English and back to German
    {
        Case &c = add(u"language"_s, u"switch the language to English and back"_s);
        c.steps << tab(u"Mehr"_s) << tap(u"^Einstellungen$"_s) << tap(u"^Darstellung"_s)
                << tap(u"combobox:^Sprache$"_s) << wait(400)
                << tap(u"^English$"_s) << see(u"tab:^Schedule$"_s) << see(u"combobox:^Language$"_s)
                << snap(u"language-english"_s)
                << tap(u"combobox:^Language$"_s) << tap(u"^Deutsch$"_s) << see(u"tab:^Stundenplan$"_s);
    }
    // Reminders page opens with the sample reminders
    {
        Case &c = add(u"reminders"_s, u"open the reminders"_s);
        c.steps << tab(u"Mehr"_s) << tap(u"^Erinnerungen"_s) << gone(u"^Vorspielkandidaten"_s)
                << snap(u"reminders"_s) << act(u"dump"_s, [this] { writeDump(u"screen-reminders"_s); })
                << tap(u"^Mehr$"_s, u"back to More"_s) << see(u"^Vorspielkandidaten"_s);
    }
    // Test mode: seven taps on the version, then off again
    {
        Case &c = add(u"test-mode"_s, u"switch the test mode on (7 taps on the version) and off"_s);
        c.steps << tab(u"Mehr"_s) << tap(u"^Einstellungen$"_s) << tap(u"^Über Qupil"_s) << gone(u"^Darstellung"_s)
                << act(u"dump"_s, [this] { writeDump(u"screen-about"_s); });
        for (int i = 0; i < 7; ++i)
            c.steps << tap(u"^Version "_s, u"tap the version (%1)"_s.arg(i + 1));
        c.steps << check(u"test mode on"_s, [this] { return m_app->testMode(); }, 15000)
                << check(u"sample data of the test mode ready"_s, [this] { return !m_app->busy(); }, 30000)
                << snap(u"test-mode"_s)
                << act(u"test mode off"_s, [this] { m_app->setTestMode(false); })
                << check(u"test mode off"_s, [this] { return !m_app->testMode(); }, 15000);
    }
    // A plan for the next lesson, from the student page
    {
        Case &c = add(u"plan-save"_s, u"plan the next lesson of a student"_s);
        c.steps << act(u"remember the lesson"_s, [this] {
                       m_membership = runningMembership();
                       m_count = m_data->plans(m_membership).size();
                   })
                << openStudent(u"Lena Muster"_s) << tap(u"^Planung$"_s)
                << tap(u"edit:^Notiz$"_s) << type(u"UI-Test-Plan: Vibrato auf der A-Saite"_s)
                << snap(u"plan-typed"_s)
                << tap(u"speichern$"_s, u"tap save"_s) << gone(u"^Abbrechen$"_s)
                << check(u"the plan is saved"_s, [this] {
                       // (a plan for that lesson may exist already: then it holds the text too)
                       for (const QVariant &p : m_data->plans(m_membership))
                           if (m_data->plainText(p.toMap().value(u"content"_s).toString()).contains(u"Vibrato auf der A-Saite"_s))
                               return true;
                       return false;
                   });
    }
    // A piece for a student
    {
        Case &c = add(u"piece-new"_s, u"add a piece for a student"_s);
        c.steps << act(u"count"_s, [this] {
                       m_membership = runningMembership();
                       m_count = m_data->pieces(m_membership).size();
                   })
                << openStudent(u"Lena Muster"_s) << tap(u"^Werke$"_s) << tap(u"^Werk$"_s)
                << act(u"dump"_s, [this] { writeDump(u"screen-piece"_s); })
                << tap(u"edit:^Titel$"_s) << type(u"UI-Test-Menuett"_s)
                << tap(u"edit:^Komponist"_s) << type(u"Testkomponist"_s)
                << tap(u"^Speichern$"_s) << gone(u"edit:^Titel$"_s)
                << check(u"the piece is saved"_s, [this] { return m_data->pieces(m_membership).size() == m_count + 1; });
    }
    // A new lesson (the editor's defaults): then it is in the schedule; removed again, so the
    // lesson running at 14:20 stays the same for the other cases
    {
        Case &c = add(u"lesson-new"_s, u"add a lesson from the schedule"_s);
        c.steps << act(u"remember the lessons"_s, [this] { m_lessons = allLessons(); })
                << tap(u"^Neu$"_s) << tap(u"menuitem:^Neuer Unterricht$"_s) << see(u"combobox:^Wochentag$"_s)
                << snap(u"lesson-new"_s)
                << tap(u"^Speichern$"_s) << gone(u"combobox:^Wochentag$"_s)
                << check(u"one lesson more"_s, [this] { return allLessons().size() == m_lessons.size() + 1; })
                << act(u"remove it again"_s, [this] {
                       for (const QString &id : allLessons())
                           if (!m_lessons.contains(id))
                               m_data->deleteLesson(id);
                   })
                << check(u"removed"_s, [this] { return allLessons().size() == m_lessons.size(); });
    }
    // Individual lessons on two days kept together: same notes, plans and pieces
    {
        Case &c = add(u"together"_s, u"two individual lessons of a student kept together"_s);
        c.steps << act(u"a second individual lesson for the running student, on Monday, with a note and a piece"_s, [this] {
                       m_membership = runningMembership();
                       m_cleanup.clear();
                       const QString student = m_data->get(u"membership"_s, m_membership).value(u"student_id"_s).toString();
                       const QString runningLesson = m_data->get(u"membership"_s, m_membership).value(u"lesson_id"_s).toString();
                       m_data->saveLesson({}, { { u"kind"_s, 1 }, { u"auto_name"_s, 1 }, { u"irregular"_s, false },
                                                { u"weekday"_s, 0 }, { u"start_time"_s, u"15:00"_s }, { u"end_time"_s, u"15:45"_s },
                                                { u"location"_s, u"Raum 20"_s } },
                                          { student });
                       const QString lesson = m_data->lastId();
                       m_cleanup << u"lesson:"_s + lesson;
                       m_other.clear();
                       for (const QVariant &v : m_data->togetherCandidates(student, runningLesson))
                           if (v.toMap().value(u"lesson_id"_s).toString() == lesson)
                               m_other = v.toMap().value(u"id"_s).toString();
                       const QString monday = Qupil::Clock::today().addDays(1 - Qupil::Clock::today().dayOfWeek()).toString(Qt::ISODate);
                       m_cleanup << u"note:"_s + m_data->addNote(m_other, monday, u"UI-Test: Notiz vom Montag"_s, false);
                       m_cleanup << u"piece:"_s + m_data->addPiece(m_other, { { u"title"_s, u"UI-Test-Werk vom Montag"_s },
                                                                               { u"state"_s, 1 } }, false);
                   })
                << check(u"the second lesson is there, not kept together yet"_s, [this] {
                       return !m_other.isEmpty() && m_data->together(m_membership).size() == 1;
                   })
                << openStudent(u"Lena Muster"_s)
                << see(u"^Zweimal Einzelunterricht pro Woche$"_s) << snap(u"together-offer"_s)
                << tap(u"button:^Gemeinsam führen$"_s)
                << check(u"kept together"_s, [this] { return m_data->together(m_membership).size() == 2; })
                << gone(u"^Zweimal Einzelunterricht pro Woche$"_s)
                << check(u"one entry in the lesson chooser, \"… · gemeinsam\""_s, [this] {
                       const QString student = m_data->get(u"membership"_s, m_membership).value(u"student_id"_s).toString();
                       const QVariantList choices = m_data->student(student).value(u"choices"_s).toList();
                       return std::any_of(choices.cbegin(), choices.cend(), [&](const QVariant &v) {
                           const QVariantMap c = v.toMap();
                           return c.value(u"ids"_s).toStringList().size() == 2 && c.value(u"togetherText"_s).toString().endsWith(u"gemeinsam"_s);
                       });
                   })
                << check(u"Tuesday shows Monday's note and piece"_s, [this] {
                       const auto has = [](const QVariantList &list, const QString &field, const QString &text) {
                           return std::any_of(list.cbegin(), list.cend(),
                                              [&](const QVariant &v) { return v.toMap().value(field).toString().contains(text); });
                       };
                       return has(m_data->notes(m_membership), u"content"_s, u"Notiz vom Montag"_s)
                              && has(m_data->pieces(m_membership), u"title"_s, u"UI-Test-Werk vom Montag"_s);
                   })
                << see(u"Notiz vom Montag"_s) << snap(u"together-student"_s)
                << check(u"the next lesson is Monday's; a plan for it belongs to Monday"_s, [this] {
                       const QString next = m_data->nextLessonDate(m_membership);
                       if (QDate::fromString(next, Qt::ISODate).dayOfWeek() != 1)
                           return false;
                       const QString plan = m_data->savePlan(m_membership, {}, next, u"UI-Test: Plan für Montag"_s, false);
                       m_cleanup << u"plan:"_s + plan;
                       return m_data->get(u"plan"_s, plan).value(u"membership_id"_s).toString() == m_other;
                   })
                << tap(u"^Unterricht wählen$"_s) << see(u"menuitem:^Beide Stunden gemeinsam führen$"_s)
                << snap(u"together-menu"_s)
                << tap(u"menuitem:^Beide Stunden gemeinsam führen$"_s)
                << check(u"separate again"_s, [this] { return m_data->together(m_membership).size() == 1; })
                << see(u"^Zweimal Einzelunterricht pro Woche$"_s)
                << check(u"Monday's note is Monday's again"_s, [this] {
                       const QVariantList notes = m_data->notes(m_membership);
                       return std::none_of(notes.cbegin(), notes.cend(), [](const QVariant &v) {
                           return v.toMap().value(u"content"_s).toString().contains(u"Notiz vom Montag"_s);
                       });
                   })
                << act(u"remove the lesson again"_s, [this] {
                       for (const QString &entry : std::as_const(m_cleanup)) {
                           const QString entity = entry.section(u':', 0, 0);
                           const QString id = entry.section(u':', 1);
                           if (entity == u"note"_s)
                               m_data->deleteNote(id, false);
                           else if (entity == u"plan"_s)
                               m_data->deletePlan(id, false);
                           else if (entity == u"piece"_s)
                               m_data->remove(u"piece"_s, id);
                       }
                       for (const QString &entry : std::as_const(m_cleanup))
                           if (entry.startsWith(u"lesson:"_s))
                               m_data->deleteLesson(entry.section(u':', 1));
                       m_cleanup.clear();
                   })
                << check(u"the student has one lesson again"_s, [this] {
                       const QString student = m_data->get(u"membership"_s, m_membership).value(u"student_id"_s).toString();
                       return m_data->togetherCandidates(student, {}).size() == 1;
                   });
    }
    // A second individual lesson for a student: the lesson editor offers to keep them together (on by default)
    {
        Case &c = add(u"together-new"_s, u"new lesson for a student who has one: kept together"_s);
        c.steps << act(u"remember the lessons"_s, [this] {
                       m_membership = runningMembership();
                       m_lessons = allLessons();
                   })
                << tap(u"^Neu$"_s) << tap(u"menuitem:^Neuer Unterricht$"_s) << see(u"combobox:^Wochentag$"_s)
                << tap(u"combobox:^Wochentag$"_s) << tap(u"^Montag$"_s)
                << tap(u"^Schüler auswählen$"_s) << tap(u"edit:^Schüler suchen$"_s) << type(u"Lena"_s)
                // (iOS: the first tap may only close the keyboard)
                << tapUntil(u"^Muster, Lena"_s, u"^Gemeinsam mit Montag führen$|^Gemeinsam mit Dienstag führen$"_s)
                << act(u"dump"_s, [this] { writeDump(u"screen-together-new"_s); })
                << snap(u"together-new"_s)
                << tap(u"^Speichern$"_s) << gone(u"combobox:^Wochentag$"_s)
                << check(u"kept together with the running lesson"_s, [this] { return m_data->together(m_membership).size() == 2; })
                << act(u"remove it again"_s, [this] {
                       for (const QString &id : allLessons())
                           if (!m_lessons.contains(id))
                               m_data->deleteLesson(id);
                   })
                << check(u"removed"_s, [this] { return allLessons().size() == m_lessons.size(); })
                << check(u"the running lesson on its own again"_s, [this] { return m_data->together(m_membership).size() == 1; });
    }
    // A reminder
    {
        Case &c = add(u"reminder-new"_s, u"add a reminder"_s);
        c.steps << act(u"count"_s, [this] { m_count = m_data->reminders().size(); })
                << tab(u"Mehr"_s) << tap(u"^Erinnerungen"_s) << tap(u"^Hinzufügen$"_s)
                << act(u"dump"_s, [this] { writeDump(u"screen-reminder"_s); })
                << tap(u"edit:"_s, u"tap the first field"_s) << type(u"UI-Test-Erinnerung"_s)
                << tap(u"^Speichern$"_s) << gone(u"^Speichern$"_s)
                << check(u"the reminder is saved"_s, [this] { return m_data->reminders().size() == m_count + 1; })
                << see(u"^UI-Test-Erinnerung"_s);
    }
    // A reminder only for one lesson: the ensemble (from the lesson view), and one for an individual lesson
    {
        Case &c = add(u"lesson-reminder"_s, u"reminders for one lesson"_s);
        c.steps << act(u"the ensemble later today and the running lesson"_s, [this] {
                       m_membership = runningMembership();
                       m_other.clear();
                       m_cleanup.clear();
                       for (const QVariant &v : m_data->lessonsOn(Qupil::Clock::today().dayOfWeek() - 1))
                           if (v.toMap().value(u"name"_s).toString().contains(u"Streicherensemble"_s))
                               m_other = v.toMap().value(u"id"_s).toString();
                       m_count = m_data->reminders().size();
                   })
                << check(u"there is an ensemble today"_s, [this] { return !m_other.isEmpty(); })
                << tap(u"Streicherensemble"_s) << see(u"^Erinnerung einrichten$"_s) << snap(u"lesson-view"_s)
                << tap(u"^Erinnerung einrichten$"_s) << see(u"^Nur bei folgendem Unterricht erinnern$"_s)
                << tap(u"edit:"_s, u"tap the first field"_s) << type(u"UI-Test: Vorspiel besprechen"_s)
                << snap(u"lesson-reminder-editor"_s)
                // The list of lessons: the entry under the finger has to stay readable
                << openList(u"combobox:^Unterricht$"_s) << wait(300) << key(Qt::Key_Down, u"highlight the next lesson"_s)
                << wait(200) << snap(u"lesson-reminder-choices"_s) << key(Qt::Key_Escape, u"close the list"_s) << wait(400)
                << see(u"^Nur bei folgendem Unterricht erinnern$"_s)
                << tap(u"^Speichern$"_s) << gone(u"^Speichern$"_s)
                << check(u"saved for the ensemble (mode 3)"_s, [this] {
                       for (const QVariant &v : m_data->reminders()) {
                           const QVariantMap r = v.toMap();
                           if (r.value(u"text"_s).toString() == u"UI-Test: Vorspiel besprechen"_s) {
                               m_cleanup << r.value(u"id"_s).toString();
                               return r.value(u"mode"_s).toInt() == 3 && r.value(u"lesson_id"_s).toString() == m_other;
                           }
                       }
                       return false;
                   })
                << check(u"at the ensemble, not at the running individual lesson"_s, [this] {
                       const auto has = [](const QVariantMap &lesson) {
                           const QVariantList list = lesson.value(u"reminders"_s).toList();
                           return std::any_of(list.cbegin(), list.cend(), [](const QVariant &r) {
                               return r.toMap().value(u"text"_s).toString() == u"UI-Test: Vorspiel besprechen"_s;
                           });
                       };
                       const QString running = m_data->get(u"membership"_s, m_membership).value(u"lesson_id"_s).toString();
                       return has(m_data->lesson(m_other)) && !has(m_data->lesson(running));
                   })
                << see(u"^UI-Test: Vorspiel besprechen$"_s)
                << snap(u"lesson-view-reminder"_s)
                << act(u"a reminder only for the running individual lesson"_s, [this] {
                       const QString running = m_data->get(u"membership"_s, m_membership).value(u"lesson_id"_s).toString();
                       m_cleanup << m_data->create(u"reminder"_s, { { u"text"_s, u"UI-Test: nur im Einzelunterricht"_s }, { u"mode"_s, 3 },
                                                                    { u"lesson_id"_s, running }, { u"student_id"_s, QString() },
                                                                    { u"sound"_s, 0 } });
                   })
                << openStudent(u"Lena Muster"_s)
                << see(u"UI-Test: nur im Einzelunterricht$"_s)
                << gone(u"UI-Test: Vorspiel besprechen$"_s)
                << snap(u"student-lesson-reminder"_s)
                << act(u"remove them again"_s, [this] {
                       for (const QString &id : std::as_const(m_cleanup))
                           m_data->remove(u"reminder"_s, id);
                       m_cleanup.clear();
                   })
                << check(u"removed"_s, [this] { return m_data->reminders().size() == m_count; });
    }
    // Birthdays and the data page open
    {
        Case &c = add(u"more-pages"_s, u"birthdays and data pages open"_s);
        c.steps << tab(u"Mehr"_s) << tap(u"^Geburtstage"_s) << see(u"geworden"_s)
                << tap(u"^Mehr$"_s, u"back to More"_s) << tap(u"^Einstellungen$"_s) << tap(u"^Daten und Backups"_s)
                << see(u"^Backup erstellen$"_s);
    }
    // Last: a note left unsaved when Qupil quits (--ui-test-phase restart checks it)
    {
        Case &c = add(u"restart-prepare"_s, u"an unsaved note when Qupil quits"_s);
        c.keepScreen = true;
        c.steps << tap(u"^Notiz$"_s) << see(u"^Notiz speichern$"_s)
                << tap(u"edit:^Notiz$"_s) << type(u"Vor dem Neustart getippt"_s);
    }
}
