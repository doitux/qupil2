// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "UiTest.h"

#include <cmath>
#include <QAccessible>
#include <QClipboard>
#include <QGuiApplication>
#include <QInputMethodEvent>
#include <QSet>
#include <QTest>
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

#ifdef Q_OS_IOS
// AudioSession_ios.mm
bool qupilPlaybackAudioSessionSet();
#endif

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
        else if (m_phase == u"select"_s)
            addSelectPhase();
        else if (m_phase == u"metronome"_s)
            addMetronomePhase();
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
QColor backgroundOf(QQuickItem *item, const QPointF &middle, const QColor &window)
{
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
            // Only texts that are mostly on the screen (not one cut off at the edge of a list),
            // looked at in the middle of what is shown
            if (!text.isEmpty() && color.alphaF() >= 0.5 && shown.width() > 2 && shown.height() >= rect.height() * 0.6) {
                const QColor behind = backgroundOf(item, shown.center(), m_window->color());
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

void UiTest::mouseClicks(const QPointF &point, int count)
{
    // count clicks 90 ms apart with timestamps, so Qt sees a double or triple click (QTest
    // spaces its clicks out on purpose so they never form one)
    if (!m_window)
        return;
    const QPointF global = m_window->mapToGlobal(point);
    static ulong t = 1000000;
    t += 5000;
    const auto send = [this, point, global](QEvent::Type type, Qt::MouseButtons buttons, ulong at) {
        QMouseEvent e(type, point, global, Qt::LeftButton, buttons, Qt::NoModifier);
        e.setTimestamp(at);
        QCoreApplication::sendEvent(m_window.data(), &e);
    };
    for (int i = 0; i < count; ++i) {
        send(QEvent::MouseButtonPress, Qt::LeftButton, t);
        if (i == 1)
            send(QEvent::MouseButtonDblClick, Qt::LeftButton, t);
        send(QEvent::MouseButtonRelease, Qt::NoButton, t + 10);
        t += 90;
    }
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
        // A row that scrolls sideways (the tabs of the student page)
        if (p->property("contentWidth").toReal() > p->width() + 1 && p->property("contentHeight").toReal() <= p->height() + 1) {
            const qreal maxX = std::max<qreal>(0, p->property("contentWidth").toReal() - p->width());
            const qreal x = std::clamp<qreal>(r.center().x() - p->width() / 2, 0, maxX);
            if (qAbs(p->property("contentX").toReal() - x) < 1)
                return false;
            p->setProperty("contentX", x);
            return true;
        }
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
                      if ((node.rect.isEmpty() || node.rect.height() < node.full.height() - 1
                           || node.rect.width() < node.full.width() - 1)
                          && scrollIntoView(node))
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

UiTest::Step UiTest::tapUntilGone(const QString &pattern)
{
    auto taps = std::make_shared<int>(0);
    auto since = std::make_shared<QElapsedTimer>();
    return Step { u"tap \"%1\" until it is gone"_s.arg(pattern), [this, pattern, taps, since] {
                      Node node;
                      const bool there = find(pattern, &node);
                      if (*taps > 0 && !there) {
                          if (*taps > 1)
                              m_results.last().note += u"%1 taps needed for \"%2\". "_s.arg(*taps).arg(pattern);
                          return true;
                      }
                      if (!there || node.rect.isEmpty() || (*taps > 0 && since->elapsed() < 1500) || *taps >= 3 || busy())
                          return false;
                      click(node.rect.center());
                      ++*taps;
                      since->start();
                      return false;
                  },
                  10000 };
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

UiTest::Step UiTest::typeTime(const QString &time)
{
    // A time field ("99:99" input mask): from the start, the digits overwrite what is there
    auto sent = std::make_shared<bool>(false);
    return Step { u"type the time %1"_s.arg(time), [this, time, sent] {
                      QQuickItem *focus = m_window ? m_window->activeFocusItem() : nullptr;
                      if (!focus || !focus->inherits("QQuickTextInput"))
                          return false;
                      if (!*sent) {
                          // Not the Home key: on macOS it does not go to the start of the field
                          focus->setProperty("cursorPosition", 0);
                          for (const QChar c : time)
                              if (c.isDigit())
                                  sendKey(0, QString(c));
                          *sent = true;
                      }
                      if (focus->property("text").toString() != time)
                          return false;
                      *sent = false;
                      return true;
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

void UiTest::touchTap(const QPointF &scenePoint, int holdMs)
{
    // A finger, not the mouse: Qt Quick treats touch differently (the system selects text)
    static QPointingDevice *finger = QTest::createTouchDevice();
    if (!m_window)
        return;
    const QPoint p = scenePoint.toPoint();
    QTest::touchEvent(m_window, finger).press(0, p, m_window);
    if (holdMs <= 0) {
        QTest::touchEvent(m_window, finger).release(0, p, m_window);
        return;
    }
    QPointer<QQuickWindow> window(m_window);
    QTimer::singleShot(holdMs, this, [window, p] {
        if (window)
            QTest::touchEvent(window, finger).release(0, p, window);
    });
}

QPointF UiTest::wordInNote(const QString &word) const
{
    // Middle of a word in the note field, in window coordinates (null if not there)
    Node n;
    if (!find(u"edit:^Notiz$"_s, &n) || !n.item)
        return {};
    QString plain;
    QMetaObject::invokeMethod(n.item, "getText", Q_RETURN_ARG(QString, plain), Q_ARG(int, 0),
                              Q_ARG(int, n.item->property("length").toInt()));
    const int at = plain.indexOf(word);
    if (at < 0)
        return {};
    QRectF a, b;
    QMetaObject::invokeMethod(n.item, "positionToRectangle", Q_RETURN_ARG(QRectF, a), Q_ARG(int, at));
    QMetaObject::invokeMethod(n.item, "positionToRectangle", Q_RETURN_ARG(QRectF, b), Q_ARG(int, at + int(word.size())));
    return n.item->mapToScene(QPointF((a.left() + b.right()) / 2, (a.top() + a.bottom()) / 2));
}

// ---------------------------------------------------------------- text by finger (TextTouch)

QQuickItem *UiTest::textItem() const
{
    Node n;
    return find(m_textField, &n) ? n.item : nullptr;
}

QString UiTest::textPlain() const
{
    QQuickItem *t = textItem();
    if (!t)
        return {};
    QString plain;
    QMetaObject::invokeMethod(t, "getText", Q_RETURN_ARG(QString, plain), Q_ARG(int, 0),
                              Q_ARG(int, t->property("length").toInt()));
    return plain;
}

QPointF UiTest::textPoint(const QString &word, int part) const
{
    // In window coordinates: 0 the middle of the word, 1 its start, 2 its end, 3 an empty
    // place in the field after the whole text (below the last line), 4 just right of the
    // word's end (on its line)
    QQuickItem *t = textItem();
    if (!t)
        return {};
    const QString plain = textPlain();
    const int at = part == 3 ? int(plain.size()) : int(plain.indexOf(word));
    if (at < 0)
        return {};
    const int end = part == 3 ? at : at + int(word.size());
    QRectF a, b;
    QMetaObject::invokeMethod(t, "positionToRectangle", Q_RETURN_ARG(QRectF, a), Q_ARG(int, at));
    QMetaObject::invokeMethod(t, "positionToRectangle", Q_RETURN_ARG(QRectF, b), Q_ARG(int, end));
    const qreal y = (a.top() + a.bottom()) / 2;
    QPointF p;
    switch (part) {
    case 1: p = { a.left() + 3, y }; break;
    case 2: p = { b.left() - 3, y }; break;
    case 4: p = { b.left() + 8, y }; break;
    case 3: {
        // Below the last line (the field is taller than its text), else right of its end
        const qreal below = b.bottom() + b.height();
        p = below < t->height() - 8 ? QPointF(b.left(), below) : QPointF(std::min(b.left() + 40, t->width() - 20), y);
        break;
    }
    default: p = { (a.left() + b.left()) / 2, y }; break;
    }
    return t->mapToScene(p);
}

QString UiTest::textSelection() const
{
    QQuickItem *t = textItem();
    return t ? t->property("selectedText").toString() : QString();
}

QString UiTest::textMarkup() const
{
    QQuickItem *t = textItem();
    if (!t || !t->parentItem())
        return {};
    for (QObject *child : t->parentItem()->children()) {
        if (QByteArray(child->metaObject()->className()).contains("TextFormatter")) {
            QString markup;
            QMetaObject::invokeMethod(child, "markup", Q_RETURN_ARG(QString, markup));
            return markup;
        }
    }
    return {};
}

QPointF UiTest::handlePoint(bool end) const
{
    // The knob of a selection handle (TextTouch), in window coordinates
    QQuickItem *t = textItem();
    if (!t)
        return {};
    const auto handles = t->findChildren<QQuickItem *>(end ? u"textTouchEnd"_s : u"textTouchStart"_s);
    if (handles.isEmpty() || !handles.first()->isVisible())
        return {};
    QQuickItem *h = handles.first();
    const QRectF line = h->property("line").toRectF();
    const qreal knob = 14;
    return h->mapToScene(QPointF(h->width() / 2, end ? line.height() + knob / 2 : 22 + knob / 2));
}

void UiTest::finger(int state, const QPointF &scenePoint)
{
    static QPointingDevice *device = QTest::createTouchDevice();
    if (!m_window)
        return;
    const QPoint p = scenePoint.toPoint();
    if (state == 0)
        QTest::touchEvent(m_window, device).press(0, p, m_window);
    else if (state == 1)
        QTest::touchEvent(m_window, device).move(0, p, m_window);
    else
        QTest::touchEvent(m_window, device).release(0, p, m_window);
}

UiTest::Step UiTest::fingerTaps(const QString &word, int count, int part)
{
    // count taps 80 ms apart; what is selected right after the last touch (before the finger
    // lifts) goes to m_atPress
    return Step { u"%1× tap on \"%2\""_s.arg(count).arg(word), [this, word, count, part] {
                      const QPointF p = textPoint(word, part);
                      if (p.isNull())
                          return false;
                      m_fingerDone = false;
                      auto left = std::make_shared<int>(count);
                      auto one = std::make_shared<std::function<void()>>();
                      *one = [this, p, left, one] {
                          finger(0, p);
                          if (--*left == 0)
                              m_atPress = textSelection();
                          finger(2, p);
                          if (*left > 0)
                              QTimer::singleShot(80, this, *one);
                          else
                              m_fingerDone = true;
                      };
                      (*one)();
                      return true;
                  } }
        ;
}

UiTest::Step UiTest::fingerLong(const QString &word, int part)
{
    // Press, hold 700 ms (what is selected then: m_atPress), lift
    return Step { u"long press on \"%1\""_s.arg(word), [this, word, part] {
                      const QPointF p = textPoint(word, part);
                      if (p.isNull())
                          return false;
                      m_fingerDone = false;
                      m_atPress.clear();
                      finger(0, p);
                      QTimer::singleShot(700, this, [this, p] {
                          m_atPress = textSelection();
                          m_menuAtPress = menuShown();
                          finger(2, p);
                          m_fingerDone = true;
                      });
                      return true;
                  } };
}

UiTest::Step UiTest::fingerDrag(std::function<QPointF()> from, std::function<QPointF()> to, int ms)
{
    // A finger from one point to another in ~20 steps
    return Step { u"drag with a finger"_s, [this, from, to, ms] {
                      const QPointF a = from(), b = to();
                      if (a.isNull() || b.isNull())
                          return false;
                      m_fingerDone = false;
                      finger(0, a);
                      const int steps = 20;
                      for (int i = 1; i <= steps; ++i) {
                          QTimer::singleShot(ms * i / steps, this, [this, a, b, i] {
                              const QPointF p = a + (b - a) * (qreal(i) / steps);
                              finger(1, p);
                              if (i == steps) {
                                  finger(2, p);
                                  m_fingerDone = true;
                              }
                          });
                      }
                      return true;
                  } };
}

UiTest::Step UiTest::fingerDone()
{
    return Step { u"the finger lifted"_s, [this] { return m_fingerDone; } };
}

bool UiTest::menuShown() const
{
    Node n;
    return find(u"^(Kopieren|Einsetzen|Auswählen|Alles auswählen)$"_s, &n);
}

QString UiTest::menuItems() const
{
    QStringList items;
    for (const QString &label : { u"Ausschneiden"_s, u"Kopieren"_s, u"Auswählen"_s, u"Einsetzen"_s, u"Alles auswählen"_s }) {
        Node n;
        if (find(u"^"_s + label + u"$"_s, &n))
            items << label;
    }
    return items.join(u", "_s);
}

QString UiTest::noteSelection() const
{
    Node n;
    return find(u"edit:^Notiz$"_s, &n) && n.item ? n.item->property("selectedText").toString() : QString();
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
                      // Not while a page slides in: the page below is still there and its
                      // colours would count as the background of the new page's texts
                      if (busy())
                          return false;
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
        // (not in the middle of a page transition, see snap())
        for (const QString &t : busy() ? QStringList() : unreadable())
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
    // "draft": Qupil stays open with the unsaved note, the system ends it from outside;
    // "select": stays open for the touch tests from outside; "metronome": stays open and
    // plays, the platform test puts Qupil into the background
    if (m_phase != u"draft"_s && m_phase != u"select"_s && m_phase != u"metronome"_s)
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
            << check(u"the tuning tone's pitch is kept (442 Hz)"_s,
                     [this] { return m_metronome && m_metronome->property("pitch").toInt() == 442; })
            << snap(u"restart-draft"_s)
            << tap(u"^Abbrechen$"_s)
            << gone(u"^Notiz speichern$"_s)
            << check(u"the draft is gone after cancelling"_s, [this] { return m_app->keptDraft(u"note"_s).isEmpty(); });
}

void UiTest::addDraftPhase()
{
    Case &c = add(u"draft-prepare"_s, u"an unsaved note, then the system ends Qupil (from outside)"_s);
    c.keepScreen = true;
    c.steps << act(u"tuning tone at 442 Hz"_s, [this] { if (m_metronome) m_metronome->setProperty("pitch", 442); })
            << tapIfThere(u"^OK$"_s, 3000, u"close the reminders shown at the start (if any)"_s)
            << tap(u"^Notiz$"_s) << see(u"^Notiz speichern$"_s)
            << tap(u"edit:^Notiz$"_s) << type(u"Vor dem Neustart getippt"_s);
}

void UiTest::addSelectPhase()
{
    // Three lines: a word at the start of the first line (the system's menu for the
    // selection comes up right where the format buttons are), a phrase, words further down
    static const QString html = u"<p>Tonleitern G-Dur und D-Dur langsam üben</p>"
                                "<p>Etüde Nr. 3 bis Takt 16 mit Metronom</p>"
                                "<p>Vortragsstück: Bogeneinteilung im Mittelteil</p>"_s;
    Case &c = add(u"select-prepare"_s, u"a note with three lines, positions logged for touch tests from outside"_s);
    c.keepScreen = true;
    c.steps << tap(u"^Notiz$"_s) << see(u"^Notiz speichern$"_s)
            << act(u"three lines of text"_s, [this] {
                   Node n;
                   if (!find(u"edit:^Notiz$"_s, &n) || !n.item)
                       return;
                   n.item->setProperty("text", html);
                   n.item->forceActiveFocus();
                   n.item->setProperty("cursorPosition", n.item->property("length"));
                   // What the keyboard sends to the field (iOS replaced a word once)
                   n.item->installEventFilter(this);
                   m_traceTimer.setInterval(200);
                   connect(&m_traceTimer, &QTimer::timeout, this, &UiTest::traceSelection, Qt::UniqueConnection);
                   m_traceTimer.start();
               })
            << check(u"the text is there"_s, [this] {
                   Node n;
                   return find(u"edit:^Notiz$"_s, &n) && n.item && n.item->property("length").toInt() > 100;
               })
            << snap(u"select-prepare"_s);
}

void UiTest::addMetronomePhase()
{
    // The metronome plays on; every second Qupil logs "metronome: beat <n> session <kind>"
    // – from outside the test sends Qupil into the background and checks that the beats go
    // on (iOS: only with the audio session "Playback")
    Case &c = add(u"metronome-prepare"_s, u"the metronome plays, its beats logged for the background test from outside"_s);
    c.keepScreen = true;
    c.steps << tab(u"Metronom"_s) << see(u"^Im Takt tippen$"_s) << tap(u"^Start$"_s)
            << check(u"running"_s, [this] { return m_metronome && m_metronome->property("running").toBool(); })
            << act(u"log the beats"_s, [this] {
                   // Counted from the beats the sound output has played (processedUSecs)
                   connect(m_metronome, SIGNAL(beatChanged()), this, SLOT(countBeat()));
                   m_traceTimer.setInterval(1000);
                   // (no Qt::UniqueConnection: not allowed with a lambda, the connection fails)
                   connect(&m_traceTimer, &QTimer::timeout, this, [this] {
                       if (!m_metronome)
                           return;
#ifdef Q_OS_IOS
                       const char *session = qupilPlaybackAudioSessionSet() ? "playback" : "other";
#else
                       const char *session = "-";
#endif
                       qInfo("metronome: beat %lld running %d session %s", qlonglong(m_beatsHeard),
                             m_metronome->property("running").toBool() ? 1 : 0, session);
                   });
                   m_traceTimer.start();
               });
}

bool UiTest::eventFilter(QObject *watched, QEvent *event)
{
    // Select phase: every input method event to the note, and Qupil's commit of the input
    // method, in the log ("select: ime …")
    if (event->type() == QEvent::InputMethod) {
        const auto *e = static_cast<QInputMethodEvent *>(event);
        qInfo("select: ime commit '%s' preedit '%s' replace %d %d cursor %d",
              qPrintable(e->commitString()), qPrintable(e->preeditString()), e->replacementStart(),
              e->replacementLength(), watched->property("cursorPosition").toInt());
    }
    return QObject::eventFilter(watched, event);
}

void UiTest::countBeat()
{
    ++m_beatsHeard;
}

void UiTest::traceSelection()
{
    // Lines "select: …" in the log, each only when it changed. Coordinates are global and in
    // logical pixels (iOS: points; Android: times "dpr" for the screen's pixels)
    Node n;
    if (!m_window || !find(u"edit:^Notiz$"_s, &n) || !n.item)
        return;
    QQuickItem *text = n.item;
    const auto log = [this](const QString &key, const QString &line) {
        if (m_traced.value(key) == line)
            return;
        m_traced.insert(key, line);
        qInfo("select: %s", qPrintable(line));
    };
    const auto global = [](QQuickItem *item, QPointF p) { return item->mapToGlobal(p); };
    log(u"dpr"_s, u"dpr %1"_s.arg(m_window->effectiveDevicePixelRatio()));
    const QRectF field(global(text, { 0, 0 }), QSizeF(text->width(), text->height()));
    log(u"field"_s, u"field %1 %2 %3 %4"_s.arg(field.x(), 0, 'f', 1).arg(field.y(), 0, 'f', 1)
                       .arg(field.right(), 0, 'f', 1).arg(field.bottom(), 0, 'f', 1));
    const int length = text->property("length").toInt();
    QString plain;
    QMetaObject::invokeMethod(text, "getText", Q_RETURN_ARG(QString, plain), Q_ARG(int, 0), Q_ARG(int, length));
    for (const QString &word : { u"Tonleitern"_s, u"G-Dur"_s, u"D-Dur"_s, u"Etüde"_s, u"Metronom"_s,
                                 u"Bogeneinteilung"_s, u"Mittelteil"_s }) {
        const int at = plain.indexOf(word);
        if (at < 0)
            continue;
        QRectF a, b;
        QMetaObject::invokeMethod(text, "positionToRectangle", Q_RETURN_ARG(QRectF, a), Q_ARG(int, at));
        QMetaObject::invokeMethod(text, "positionToRectangle", Q_RETURN_ARG(QRectF, b), Q_ARG(int, at + int(word.size())));
        const QPointF p = global(text, a.topLeft()), q = global(text, b.bottomRight());
        log(u"word "_s + word, u"word %1 %2 %3 %4 %5"_s.arg(word).arg(p.x(), 0, 'f', 1).arg(p.y(), 0, 'f', 1)
                                   .arg(q.x(), 0, 'f', 1).arg(q.y(), 0, 'f', 1));
    }
    for (const QString &button : { u"Fett"_s, u"Kursiv"_s, u"Unterstrichen"_s, u"Textfarbe"_s }) {
        Node b;
        if (!find(u"^"_s + button + u"$"_s, &b) || !b.item)
            continue;
        const QPointF p = global(b.item, { b.item->width() / 2, b.item->height() / 2 });
        log(u"button "_s + button, u"button %1 %2 %3"_s.arg(button).arg(p.x(), 0, 'f', 1).arg(p.y(), 0, 'f', 1));
    }
    const int from = text->property("selectionStart").toInt(), to = text->property("selectionEnd").toInt();
    log(u"sel"_s, u"sel %1 %2 %3"_s.arg(from).arg(to).arg(text->property("selectedText").toString()));
    log(u"cursor"_s, u"cursor %1"_s.arg(text->property("cursorPosition").toInt()));
    log(u"preedit"_s, u"preedit '%1'"_s.arg(text->property("preeditText").toString()));
    // Qupil's own text menu (TextTouch) and where its items and the handles are
    log(u"menu"_s, u"menu "_s + menuItems());
    for (const QString &label : { u"Ausschneiden"_s, u"Kopieren"_s, u"Einsetzen"_s, u"Auswählen"_s }) {
        Node b;
        if (!find(u"^"_s + label + u"$"_s, &b) || !b.item)
            continue;
        const QPointF p = global(b.item, { b.item->width() / 2, b.item->height() / 2 });
        log(u"button "_s + label, u"button %1 %2 %3"_s.arg(label).arg(p.x(), 0, 'f', 1).arg(p.y(), 0, 'f', 1));
    }
    for (bool end : { false, true }) {
        const QPointF p = handlePoint(end);
        const QString name = end ? u"end"_s : u"start"_s;
        if (p.isNull())
            log(u"handle "_s + name, u"handle %1 -"_s.arg(name));
        else {
            const QPointF g = m_window->mapToGlobal(p);
            log(u"handle "_s + name, u"handle %1 %2 %3"_s.arg(name).arg(g.x(), 0, 'f', 1).arg(g.y(), 0, 'f', 1));
        }
    }
    for (QObject *child : text->parentItem() ? text->parentItem()->children() : QObjectList()) {
        if (QByteArray(child->metaObject()->className()).contains("TextFormatter")) {
            QString markup;
            QMetaObject::invokeMethod(child, "markup", Q_RETURN_ARG(QString, markup));
            log(u"markup"_s, u"markup "_s + QString(markup).replace(u'\n', u" / "_s));
        }
    }
}

// Text by finger – the rules of TextTouch.qml (docs/text-touch.md), each with real touch
// events, on every platform. Three lines of a note; each case opens and cancels the editor.
void UiTest::addTextTouchCases()
{
#ifdef Q_OS_IOS
    // TextTouch is off on iOS for now (the keyboard replaced words, TextTouch.qml: active);
    // the platform run from outside checks there that the text stays intact
    return;
#endif
    static const QString html = u"<p>Tonleitern G-Dur und D-Dur langsam üben</p>"
                                "<p>Etüde Nr. 3 bis Takt 16 mit Metronom</p>"
                                "<p>Vortragsstück: Bogeneinteilung im Mittelteil</p>"_s;
    const auto open = [this](const QString &text) {
        return QList<Step> {
            act(u"the note field"_s, [this] { m_textField = u"edit:^Notiz$"_s; }),
            tap(u"^Notiz$"_s), see(u"^Notiz speichern$"_s),
            act(u"the text"_s, [this, text] {
                if (QQuickItem *t = textItem()) {
                    t->setProperty("text", text);
                    t->forceActiveFocus();
                }
            }),
            check(u"the text is there"_s, [this] { return !textPoint(u"Metronom"_s).isNull(); }),
            wait(600),
        };
    };
    const QList<Step> close = { tap(u"^Abbrechen$"_s), gone(u"^Notiz speichern$"_s) };
    const auto fettChecked = [this] {
        Node n;
        return find(u"^Fett$"_s, &n) && n.item && n.item->property("checked").toBool();
    };
    const auto cursorIn = [this](const QString &word) {
        QQuickItem *t = textItem();
        const int at = int(textPlain().indexOf(word));
        const int c = t ? t->property("cursorPosition").toInt() : -1;
        return t && at >= 0 && c >= at && c <= at + int(word.size()) && textSelection().isEmpty();
    };

    {
        Case &c = add(u"text-tap"_s, u"finger: a tap places the cursor; a tap on the cursor shows and hides the menu"_s);
        c.steps << open(html)
                << fingerTaps(u"Metronom"_s, 1) << fingerDone()
                << check(u"cursor in \"Metronom\", nothing selected, no menu"_s,
                         [cursorIn, this] { return cursorIn(u"Metronom"_s) && !menuShown(); })
                << wait(600) << fingerTaps(u"Metronom"_s, 1) << fingerDone()
                << check(u"a tap on the cursor: menu with Auswählen, Alles auswählen"_s, [this] {
                       return menuItems().contains(u"Auswählen"_s) && menuItems().contains(u"Alles auswählen"_s)
                              && !menuItems().contains(u"Kopieren"_s);
                   })
                << snap(u"text-menu-cursor"_s)
                << wait(600) << fingerTaps(u"Metronom"_s, 1) << fingerDone()
                << check(u"again: the menu goes"_s, [this] { return !menuShown(); })
                << wait(600) << fingerTaps(u"Tonleitern"_s, 1) << fingerDone()
                << check(u"a tap elsewhere moves the cursor"_s, [cursorIn] { return cursorIn(u"Tonleitern"_s); })
                // B with the cursor in a word: the whole word (the keyboard may hold a part of
                // it as text being composed)
                << wait(1000) << tap(u"^Fett$"_s)
                << check(u"B: the whole word bold"_s, [this] { return textMarkup().contains(u"**Tonleitern**"_s); })
                << close;
    }
    {
        Case &c = add(u"text-double-tap"_s, u"finger: a double tap selects the word at the second touch; B and B again"_s);
        c.steps << open(html)
                << fingerTaps(u"G-Dur"_s, 2) << fingerDone()
                << check(u"\"G-Dur\" selected already at the second touch"_s, [this] { return m_atPress == u"G-Dur"_s; })
                << check(u"and after the finger lifted, with handles and the menu"_s, [this] {
                       return textSelection() == u"G-Dur"_s && !handlePoint(true).isNull() && !handlePoint(false).isNull()
                              && menuItems().startsWith(u"Ausschneiden, Kopieren"_s);
                   })
                << snap(u"text-double-tap"_s)
                << tap(u"^Fett$"_s)
                << check(u"bold, and B shows it"_s, [this, fettChecked] { return textMarkup().contains(u"**G-Dur**"_s) && fettChecked(); })
                << tap(u"^Fett$"_s)
                << check(u"B again: normal"_s, [this, fettChecked] { return !textMarkup().contains(u"**"_s) && !fettChecked(); })
                << close;
    }
    {
        Case &c = add(u"text-triple-tap"_s, u"finger: a triple tap selects the paragraph (without its end); B, B again"_s);
        c.steps << open(html)
                << fingerTaps(u"Metronom"_s, 3) << fingerDone()
                << check(u"line 2 selected at the third touch"_s,
                         [this] { return m_atPress == u"Etüde Nr. 3 bis Takt 16 mit Metronom"_s; })
                << check(u"still after the finger lifted"_s,
                         [this] { return textSelection() == u"Etüde Nr. 3 bis Takt 16 mit Metronom"_s && menuShown(); })
                << tap(u"^Fett$"_s)
                << check(u"the paragraph bold, B shows it"_s, [this, fettChecked] {
                       return textMarkup().contains(u"**Etüde Nr. 3 bis Takt 16 mit Metronom**"_s) && fettChecked();
                   })
                << tap(u"^Fett$"_s)
                << check(u"B again: normal"_s, [this, fettChecked] { return !textMarkup().contains(u"**"_s) && !fettChecked(); })
                << close;
    }
    {
        Case &c = add(u"text-long-press"_s, u"finger: a long press selects the word while the finger is down"_s);
        c.steps << open(html)
                << fingerLong(u"Metronom"_s) << fingerDone()
                << check(u"\"Metronom\" selected while the finger was down, menu there"_s,
                         [this] { return m_atPress == u"Metronom"_s && m_menuAtPress; })
                << check(u"still selected after it lifted"_s, [this] { return textSelection() == u"Metronom"_s && menuShown(); })
                << wait(600) << fingerLong(QString(), 3) << fingerDone()
                << check(u"behind the text: the cursor there, menu with Alles auswählen"_s, [this] {
                       QQuickItem *t = textItem();
                       return t && textSelection().isEmpty() && t->property("cursorPosition").toInt() == t->property("length").toInt()
                              && menuItems().contains(u"Alles auswählen"_s);
                   })
                // Two slow taps on a word (the cursor there, then its menu), then a long press
                // on it: the word, not the menu switched off
                << wait(600) << fingerTaps(u"Mittelteil"_s, 1) << fingerDone() << wait(700)
                << fingerTaps(u"Mittelteil"_s, 1) << fingerDone()
                << check(u"a tap on the cursor: menu"_s, [this] { return textSelection().isEmpty() && menuShown(); })
                << wait(1200) << fingerLong(u"Mittelteil"_s) << fingerDone()
                << check(u"then a long press: the word, menu"_s, [this] { return textSelection() == u"Mittelteil"_s && menuShown(); })
                << close;
    }
    {
        // From the end handle on its knob to behind "D-Dur" (the knob hangs below the line)
        const auto endKnobTo = [this](const QString &word) {
            return [this, word] {
                const QPointF knob = handlePoint(true), wordEnd = textPoint(u"G-Dur"_s, 2);
                return textPoint(word, 2) + (knob - wordEnd);
            };
        };
        const auto startKnobTo = [this](const QString &word) {
            return [this, word] {
                const QPointF knob = handlePoint(false), wordStart = textPoint(u"G-Dur"_s, 1);
                return textPoint(word, 1) + (knob - wordStart);
            };
        };
        Case &c = add(u"text-handles"_s, u"finger: the handles make the selection longer at either end"_s);
        c.steps << open(html)
                << fingerTaps(u"G-Dur"_s, 2) << fingerDone() << wait(300)
                << fingerDrag([this] { return handlePoint(true); }, endKnobTo(u"D-Dur"_s)) << fingerDone()
                << check(u"the end to \"D-Dur\""_s, [this] { return textSelection() == u"G-Dur und D-Dur"_s; })
                << check(u"menu back after the drag"_s, [this] { return menuShown(); })
                << fingerDrag([this] { return handlePoint(false); }, startKnobTo(u"Tonleitern"_s)) << fingerDone()
                << check(u"the start to \"Tonleitern\""_s, [this] { return textSelection() == u"Tonleitern G-Dur und D-Dur"_s; })
                << snap(u"text-handles"_s)
                << close;
    }
    {
        Case &c = add(u"text-tap-selection"_s, u"finger: a tap into the selection hides/shows the menu, outside ends it"_s);
        c.steps << open(html)
                << fingerTaps(u"Metronom"_s, 2) << fingerDone()
                << check(u"selected, menu"_s, [this] { return textSelection() == u"Metronom"_s && menuShown(); })
                << wait(600) << fingerTaps(u"Metronom"_s, 1) << fingerDone()
                << check(u"a tap into it: menu gone, still selected"_s, [this] { return textSelection() == u"Metronom"_s && !menuShown(); })
                << wait(600) << fingerTaps(u"Metronom"_s, 1) << fingerDone()
                << check(u"again: menu back"_s, [this] { return textSelection() == u"Metronom"_s && menuShown(); })
                << wait(600) << fingerTaps(u"Tonleitern"_s, 1) << fingerDone()
                << check(u"a tap outside: selection and menu gone"_s, [this] { return textSelection().isEmpty() && !menuShown(); })
                // The last word, then a tap right behind it: outside too (the cursor there)
                << wait(600) << fingerTaps(u"Mittelteil"_s, 2) << fingerDone()
                << check(u"the last word selected"_s, [this] { return textSelection() == u"Mittelteil"_s; })
                << wait(600) << fingerTaps(u"Mittelteil"_s, 1, 4) << fingerDone()
                << check(u"a tap behind it: the selection ends, the cursor behind the word, no menu"_s, [this] {
                       QQuickItem *t = textItem();
                       return t && textSelection().isEmpty() && !menuShown()
                              && t->property("cursorPosition").toInt() == t->property("length").toInt();
                   })
                << close;
    }
    {
        Case &c = add(u"text-menu"_s, u"finger: Kopieren, Einsetzen, Alles auswählen, Ausschneiden"_s);
        c.steps << open(html)
                << fingerTaps(u"Metronom"_s, 2) << fingerDone()
                << tap(u"^Kopieren$"_s)
                << check(u"copied, menu gone"_s, [this] {
                       return QGuiApplication::clipboard()->text() == u"Metronom"_s && !menuShown();
                   })
                << wait(600) << fingerTaps(QString(), 1, 3) << fingerDone()
                << wait(600) << fingerTaps(QString(), 1, 3) << fingerDone()
                << check(u"a tap on the cursor at the end: Einsetzen"_s, [this] { return menuItems().contains(u"Einsetzen"_s); })
                << tap(u"^Einsetzen$"_s)
                << check(u"pasted at the end"_s, [this] { return textPlain().endsWith(u"MittelteilMetronom"_s) && !menuShown(); })
                // the cursor is at the end after pasting: one tap on it brings the menu
                << wait(600) << fingerTaps(QString(), 1, 3) << fingerDone()
                << tap(u"^Alles auswählen$"_s)
                << check(u"everything selected, menu stays"_s, [this] {
                       return !textSelection().isEmpty() && textSelection().size() >= textPlain().size() - 2
                              && menuItems().contains(u"Ausschneiden"_s);
                   })
                << tap(u"^Ausschneiden$"_s)
                << check(u"cut: the field is empty, the text in the clipboard"_s, [this] {
                       return textPlain().isEmpty() && QGuiApplication::clipboard()->text().contains(u"Tonleitern"_s);
                   })
                << close;
    }
    {
        // Fifty lines: the field is longer than the sheet, also on a big iPad without a keyboard
        QString lines;
        for (int i = 1; i <= 50; ++i)
            lines += u"<p>Zeile %1 mit etwas Text zum Blättern</p>"_s.arg(i);
        auto before = std::make_shared<qreal>(0);
        const auto flickOf = [this]() -> QQuickItem * {
            for (QQuickItem *p = textItem(); p; p = p->parentItem())
                if (p->property("flicking").isValid() && p->property("contentY").isValid())
                    return p;
            return nullptr;
        };
        Case &c = add(u"text-wipe"_s, u"finger: wiping over the text scrolls and selects nothing"_s);
        c.steps << open(lines + html)
                << act(u"where the sheet is scrolled to"_s, [before, flickOf] {
                       if (QQuickItem *f = flickOf())
                           *before = f->property("contentY").toReal();
                   })
                << fingerDrag([this] { return textPoint(u"Zeile 9 "_s); }, [this] { return textPoint(u"Zeile 2 "_s); }, 250)
                << fingerDone() << wait(800)
                << check(u"scrolled, nothing selected"_s, [this, before, flickOf] {
                       QQuickItem *f = flickOf();
                       return f && std::abs(f->property("contentY").toReal() - *before) > 40 && textSelection().isEmpty();
                   })
                << close;
    }
    {
        Case &c = add(u"text-field"_s, u"finger: the same in a one-line field (title of a piece)"_s);
        c.steps << act(u"the title field"_s, [this] { m_textField = u"edit:^Titel$"_s; })
                << openStudent(u"Lena Muster"_s) << tap(u"^Werke$"_s) << tap(u"^Werk$"_s)
                << tap(u"edit:^Titel$"_s) << type(u"Menuett in G-Dur"_s) << wait(600)
                << fingerTaps(u"G-Dur"_s, 2) << fingerDone()
                << check(u"double tap: \"G-Dur\", menu"_s, [this] { return textSelection() == u"G-Dur"_s && menuItems().contains(u"Kopieren"_s); })
                << wait(600) << fingerLong(u"Menuett"_s) << fingerDone()
                << check(u"long press: \"Menuett\""_s, [this] { return textSelection() == u"Menuett"_s; })
                << tap(u"^Abbrechen$"_s) << gone(u"edit:^Titel$"_s)
                << act(u"back to the note field"_s, [this] { m_textField = u"edit:^Notiz$"_s; })
                << backIfThere(u"^Schüler$"_s);
    }
    {
        // A mouse: the field selects by itself; double and triple click with the same rules
        Case &c = add(u"text-mouse"_s, u"mouse: double click at the start of a paragraph takes the word; B, B again; triple click"_s);
        c.steps << open(html)
                << act(u"double click at the start of line 1"_s, [this] { mouseClicks(textPoint(u"Tonleitern"_s, 1), 2); })
                << check(u"\"Tonleitern\", not the paragraph"_s, [this] { return textSelection() == u"Tonleitern"_s; })
                << tap(u"^Fett$"_s)
                << check(u"bold, B shows it"_s, [this, fettChecked] { return textMarkup().contains(u"**Tonleitern**"_s) && fettChecked(); })
                << tap(u"^Fett$"_s)
                << check(u"B again: normal"_s, [this, fettChecked] { return !textMarkup().contains(u"**"_s) && !fettChecked(); })
                << wait(600)
                << act(u"triple click in line 2"_s, [this] { mouseClicks(textPoint(u"Metronom"_s), 3); })
                << check(u"the paragraph"_s, [this] { return textSelection() == u"Etüde Nr. 3 bis Takt 16 mit Metronom"_s; })
                << close;
    }
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
    // Touch: a double tap on a word selects it (Qt leaves it to the system otherwise), then "B"
    // (TextTouch: not on iOS for now)
#ifndef Q_OS_IOS
    {
        Case &c = add(u"note-touch-select"_s, u"double tap with a finger selects a word, B makes it bold"_s);
        c.steps << tap(u"^Notiz$"_s) << see(u"^Notiz speichern$"_s)
                << act(u"two lines of text"_s, [this] {
                       Node n;
                       if (find(u"edit:^Notiz$"_s, &n) && n.item)
                           n.item->setProperty("text", u"<p>Tonleitern G-Dur und D-Dur</p><p>Etüde mit Metronom</p>"_s);
                   })
                << check(u"the text is there"_s, [this] { return !wordInNote(u"Metronom"_s).isNull(); })
                << act(u"double tap on \"Metronom\" (90 ms apart)"_s, [this] {
                       touchTap(wordInNote(u"Metronom"_s));
                       QTimer::singleShot(90, this, [this] { touchTap(wordInNote(u"Metronom"_s)); });
                   })
                << check(u"\"Metronom\" is selected"_s, [this] { return noteSelection() == u"Metronom"_s; })
                << snap(u"note-touch-select"_s)
                << tap(u"^Fett$"_s)
                << check(u"bold"_s, [this] {
                       Node n;
                       if (!find(u"edit:^Notiz$"_s, &n) || !n.item)
                           return false;
                       for (QObject *child : n.item->parentItem()->children()) {
                           if (QByteArray(child->metaObject()->className()).contains("TextFormatter")) {
                               QString markup;
                               QMetaObject::invokeMethod(child, "markup", Q_RETURN_ARG(QString, markup));
                               return markup.contains(u"**Metronom**"_s);
                           }
                       }
                       return false;
                   })
                << wait(600)   // apart from the next tap: no double tap
                << act(u"a single tap on \"Tonleitern\""_s, [this] { touchTap(wordInNote(u"Tonleitern"_s)); })
                << check(u"a single tap only places the cursor"_s, [this] { return noteSelection().isEmpty(); })
                << wait(600)
                // a word with a hyphen is one word
                << act(u"double tap on \"G-Dur\""_s, [this] {
                       touchTap(wordInNote(u"G-Dur"_s));
                       QTimer::singleShot(90, this, [this] { touchTap(wordInNote(u"G-Dur"_s)); });
                   })
                << check(u"\"G-Dur\" is selected (not just \"Dur\")"_s, [this] { return noteSelection() == u"G-Dur"_s; })
                << wait(600);
#ifdef Q_OS_ANDROID
        // Android: a long press selects the word too (when the finger lifts)
        c.steps << act(u"long press on \"Etüde\" (1 s)"_s, [this] { touchTap(wordInNote(u"Etüde"_s), 1000); })
                << wait(1500)
                << check(u"\"Etüde\" is selected"_s, [this] { return noteSelection() == u"Etüde"_s; });
#endif
        c.steps << tap(u"^Abbrechen$"_s) << gone(u"^Notiz speichern$"_s);
    }
#endif
    addTextTouchCases();

    // Students: search, open one, back
    {
        Case &c = add(u"student-search"_s, u"search a student, open and close the student page"_s);
        // (a case before may have left a search in the field)
        c.steps << tab(u"Schüler"_s) << tap(u"edit:^Schüler suchen$"_s) << clearField() << type(u"Lena"_s)
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
                // The beat display follows the sound (the beat the output has played): it
                // must move through the bar
                << check(u"the beat moves (what the sound output played)"_s, [this, seen = std::make_shared<QSet<int>>()] {
                       if (!m_metronome || !m_metronome->property("running").toBool())
                           return true;
                       seen->insert(m_metronome->property("beat").toInt());
                       return seen->size() >= 3;
                   }, 6000)
#ifdef Q_OS_IOS
                // The session for playing (background, silent switch) – Qt sets "Ambient"
                // when the sound output opens (AudioSession_ios.mm)
                << wait(1500)
                << check(u"iOS: audio session \"Playback\" while it plays"_s, [this] {
                       return !m_metronome || !m_metronome->property("running").toBool() || qupilPlaybackAudioSessionSet();
                   })
#endif
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
    // The calendar of a date field in the lower part of an editor (iPad in landscape): it
    // has to be on the screen as a whole, every day reachable
    {
        Case &c = add(u"date-calendar"_s, u"the calendar of a date field is on the screen as a whole"_s);
        const auto calendarInside = [this] {
            if (!m_window)
                return false;
            for (QObject *popup : m_window->findChildren<QObject *>(u"dateCalendar"_s)) {
                if (!popup->property("opened").toBool())
                    continue;
                // Where the calendar really is (the popup's y is only what it asked for)
                auto *content = popup->property("contentItem").value<QQuickItem *>();
                if (!content)
                    continue;
                const QRectF r = content->mapRectToScene(QRectF(0, 0, content->width(), content->height()));
                return r.height() > 100 && r.top() >= 0 && r.bottom() <= m_window->height()
                       && r.left() >= 0 && r.right() <= m_window->width();
            }
            return false;
        };
        c.steps << openStudent(u"Lena Muster"_s) << tap(u"^Werke$"_s) << tap(u"^Werk$"_s)
                << tap(u"^Start "_s) << check(u"the calendar for the start date: all of it on the screen"_s, calendarInside)
                << snap(u"date-calendar"_s)
                << key(Qt::Key_Escape, u"close the calendar"_s)
                << tap(u"^Abbrechen$"_s) << gone(u"edit:^Titel$"_s) << backIfThere(u"^Schüler$"_s);
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
    // Any length of lesson: type start and end and save right away (iOS: no Return, the
    // keyboard stays open; the times must still count)
    {
        Case &c = add(u"lesson-times"_s, u"a lesson of 11 minutes, times typed and saved at once"_s);
        c.steps << act(u"remember the lessons"_s, [this] { m_lessons = allLessons(); })
                << tap(u"^Neu$"_s) << tap(u"menuitem:^Neuer Unterricht$"_s) << see(u"combobox:^Wochentag$"_s)
                << tap(u"edit:^Start$"_s) << typeTime(u"11:00"_s)
                // Tab goes from the start to the end
                << act(u"press Tab"_s, [this] { sendKey(Qt::Key_Tab, u"\t"_s); })
                << check(u"the end has the focus"_s, [this] {
                       QQuickItem *focus = m_window ? m_window->activeFocusItem() : nullptr;
                       QAccessibleInterface *a = focus ? QAccessible::queryAccessibleInterface(focus) : nullptr;
                       return a && a->text(QAccessible::Name) == u"Ende"_s;
                   })
                // An end before the start cannot be saved: a hint, the editor stays
                << typeTime(u"10:30"_s) << see(u"^Das Ende muss nach dem Beginn liegen\\.$"_s)
                << snap(u"lesson-times-wrong"_s)
                << tap(u"^Speichern$"_s) << see(u"combobox:^Wochentag$"_s)
                << check(u"nothing saved"_s, [this] { return allLessons().size() == m_lessons.size(); })
                // Save put the focus into the end field: type the right end
                << typeTime(u"11:11"_s) << gone(u"^Das Ende muss nach dem Beginn"_s)
                << snap(u"lesson-times"_s)
                << tap(u"^Speichern$"_s) << gone(u"combobox:^Wochentag$"_s)
                << check(u"saved 11:00 to 11:11"_s, [this] {
                       for (const QString &id : allLessons()) {
                           if (m_lessons.contains(id))
                               continue;
                           const QVariantMap l = m_data->get(u"lesson"_s, id);
                           m_data->deleteLesson(id);
                           return l.value(u"start_time"_s).toString() == u"11:00"_s
                                  && l.value(u"end_time"_s).toString() == u"11:11"_s;
                       }
                       return false;
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
                // The page of lessons: by day, the bar of days, the search
                << tapUntil(u"^Unterricht$"_s, u"edit:^Schüler oder Unterricht suchen$"_s) << wait(300)
                << snap(u"lesson-picker"_s)
                << tap(u"^Freitag$"_s) << wait(300) << snap(u"lesson-picker-friday"_s)
                << tap(u"edit:^Schüler oder Unterricht suchen$"_s) << type(u"Lena"_s)
                << see(u"^Lena Muster · Di 14:00$"_s) << snap(u"lesson-picker-search"_s)
                << clearField() << type(u"Streicherensemble"_s)
                << tapUntilGone(u"^Streicherensemble · Di 17:00$"_s)
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
    // Sheet music lent to a student: returned on the student's page, and undone
    {
        Case &c = add(u"loan-return"_s, u"return sheet music from the student's page"_s);
        c.steps << act(u"the running student's loan"_s, [this] {
                       m_membership = runningMembership();
                       const QString student = m_data->get(u"membership"_s, m_membership).value(u"student_id"_s).toString();
                       const QVariantList loans = m_data->loans(student);
                       m_other = loans.isEmpty() ? QString() : loans.first().toMap().value(u"id"_s).toString();
                       m_lessons = { loans.isEmpty() ? QString() : loans.first().toMap().value(u"title"_s).toString(),
                                     loans.isEmpty() ? QString() : loans.first().toMap().value(u"lent_since"_s).toString() };
                   })
                << check(u"the student has sheet music"_s, [this] { return !m_other.isEmpty(); })
                << openStudent(u"Lena Muster"_s)
                << tap(u"^Leihnoten$"_s)
                << see(u"^Zurückgegeben: "_s) << snap(u"loans"_s)
                << act(u"tap \"Zurückgegeben\" of the first one"_s, [this] {
                       Node node;
                       if (find(u"^Zurückgegeben: "_s + QRegularExpression::escape(m_lessons.value(0)) + u"$"_s, &node))
                           click(node.rect.center());
                   })
                << check(u"back in the library"_s, [this] {
                       return m_data->get(u"library_item"_s, m_other).value(u"lent_to"_s).toString().isEmpty();
                   })
                << see(u"ist zurück in der Bibliothek$"_s) << snap(u"loan-returned"_s)
                << tap(u"^Rückgängig$"_s)
                << check(u"lent again, from the same day"_s, [this] {
                       const QVariantMap item = m_data->get(u"library_item"_s, m_other);
                       const QString student = m_data->get(u"membership"_s, m_membership).value(u"student_id"_s).toString();
                       return item.value(u"lent_to"_s).toString() == student && item.value(u"lent_since"_s).toString() == m_lessons.value(1);
                   })
                << see(u"^Zurückgegeben: "_s);
    }
    // Birthdays and the data page open
    {
        Case &c = add(u"more-pages"_s, u"birthdays and data pages open"_s);
        c.steps << tab(u"Mehr"_s) << tap(u"^Geburtstage"_s) << see(u"geworden"_s)
                << tap(u"^Mehr$"_s, u"back to More"_s) << tap(u"^Einstellungen$"_s) << tap(u"^Daten und Backups"_s)
                << see(u"^Backup erstellen$"_s);
    }
    // Volume of the alert sounds: a slider, on iPad and iPhone the way to the system's setting
    {
        Case &c = add(u"alert-volume"_s, u"volume of alert sounds (iOS: hint to the system setting)"_s);
        c.steps << tab(u"Mehr"_s) << tap(u"^Einstellungen$"_s) << tap(u"^Hinweise und Klänge"_s)
                << see(u"^Meldung am Ende des Unterrichts"_s);
#ifdef Q_OS_IOS
        c.steps << see(u"^Die Lautstärke stellt das i(Pad|Phone) ein: Einstellungen › "_s)
                << gone(u"slider:^Lautstärke"_s);
#else
        c.steps << see(u"slider:^Lautstärke"_s) << gone(u"^Die Lautstärke stellt das "_s);
#endif
        c.steps << snap(u"alert-volume"_s);
    }
    // Last: a note left unsaved when Qupil quits (--ui-test-phase restart checks it)
    {
        Case &c = add(u"restart-prepare"_s, u"an unsaved note when Qupil quits"_s);
        c.keepScreen = true;
        c.steps << act(u"tuning tone at 442 Hz"_s, [this] { if (m_metronome) m_metronome->setProperty("pitch", 442); })
                << tap(u"^Notiz$"_s) << see(u"^Notiz speichern$"_s)
                << tap(u"edit:^Notiz$"_s) << type(u"Vor dem Neustart getippt"_s);
    }
}
