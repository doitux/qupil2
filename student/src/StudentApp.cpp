// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "StudentApp.h"

#include "Crypto.h"
#include "QrScanner.h"

#include <QClipboard>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSaveFile>
#include <QStandardPaths>
#include <QThread>
#include <QUuid>

using namespace Qt::StringLiterals;
namespace Booklet = Qupil::Booklet;

namespace {

QString s_dataDir;
QDate s_today;

QString dataDir()
{
    if (!s_dataDir.isEmpty())
        return s_dataDir;
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
}

QString stateFile()
{
    return dataDir() + u"/student.json"_s;
}

qint64 nowMs()
{
    return QDateTime::currentMSecsSinceEpoch();
}

QString newId()
{
    return QUuid::createUuid().toString(QUuid::WithoutBraces).remove(u'-');
}

} // namespace

StudentApp::StudentApp(QObject *parent)
    : QObject(parent)
    , m_net(new QNetworkAccessManager(this))
{
    m_uploadTimer.setSingleShot(true);
    m_uploadTimer.setInterval(2500);
    connect(&m_uploadTimer, &QTimer::timeout, this, &StudentApp::sendPending);
    load();
    // Going to the background: send what is still waiting, refresh when coming back
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, [this](Qt::ApplicationState state) {
        if (state == Qt::ApplicationSuspended || state == Qt::ApplicationInactive)
            sendPending();
        else if (state == Qt::ApplicationActive)
            refresh();
    });
}

StudentApp::~StudentApp() = default;

StudentApp *StudentApp::instance()
{
    static StudentApp *app = new StudentApp(qGuiApp);
    return app;
}

StudentApp *StudentApp::create(QQmlEngine *, QJSEngine *)
{
    StudentApp *app = instance();
    QJSEngine::setObjectOwnership(app, QJSEngine::CppOwnership);
    return app;
}

void StudentApp::setDataDir(const QString &dir)
{
    s_dataDir = dir;
}

void StudentApp::setToday(const QDate &today)
{
    s_today = today;
}

QDate StudentApp::today()
{
    return s_today.isValid() ? s_today : QDate::currentDate();
}

QString StudentApp::version() const
{
    return QCoreApplication::applicationVersion();
}

// ---------------------------------------------------------------------------- state on disk

void StudentApp::load()
{
    QFile file(stateFile());
    QJsonObject root;
    if (file.open(QIODevice::ReadOnly))
        root = QJsonDocument::fromJson(file.readAll()).object();
    m_deviceId = root.value(u"device"_s).toString();
    if (m_deviceId.isEmpty())
        m_deviceId = newId().left(16);
    m_current = root.value(u"current"_s).toInt();
    for (const QJsonValue &v : root.value(u"teachers"_s).toArray()) {
        const QJsonObject o = v.toObject();
        Teacher t;
        t.demo = o.value(u"demo"_s).toBool();
        if (!t.demo) {
            const auto link = Booklet::decodeLink(o.value(u"link"_s).toString());
            if (!link)
                continue;
            t.link = *link;
            t.id = link->id();
        } else {
            t.id = u"demo"_s;
        }
        t.booklet = t.demo ? demoBooklet() : o.value(u"booklet"_s).toObject();
        t.fetched = QDateTime::fromString(o.value(u"fetched"_s).toString(), Qt::ISODate);
        t.feedback = o.value(u"feedback"_s).toObject();
        t.questions = o.value(u"questions"_s).toArray();
        t.changedAt = qint64(o.value(u"changedAt"_s).toDouble());
        t.sentAt = qint64(o.value(u"sentAt"_s).toDouble());
        t.sentState = qint64(o.value(u"sentState"_s).toDouble());
        m_teachers.append(t);
    }
    if (m_current >= m_teachers.size())
        m_current = 0;
}

void StudentApp::save() const
{
    QJsonArray teachers;
    for (const Teacher &t : m_teachers) {
        QJsonObject o{{u"demo"_s, t.demo},
                      {u"fetched"_s, t.fetched.toString(Qt::ISODate)},
                      {u"feedback"_s, t.feedback},
                      {u"questions"_s, t.questions},
                      {u"changedAt"_s, double(t.changedAt)},
                      {u"sentAt"_s, double(t.sentAt)},
                      {u"sentState"_s, double(t.sentState)}};
        if (!t.demo) {
            o.insert(u"link"_s, Booklet::encodeLink(t.link));
            o.insert(u"booklet"_s, t.booklet);
        }
        teachers.append(o);
    }
    const QJsonObject root{{u"device"_s, m_deviceId}, {u"current"_s, m_current}, {u"teachers"_s, teachers}};
    QDir().mkpath(dataDir());
    QSaveFile file(stateFile());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson(QJsonDocument::Compact));
        file.commit();
    }
}

// ---------------------------------------------------------------------------- what QML sees

StudentApp::Teacher *StudentApp::find(const QString &id)
{
    for (Teacher &t : m_teachers)
        if (t.id == id)
            return &t;
    return nullptr;
}

StudentApp::Teacher *StudentApp::currentTeacher()
{
    return m_current >= 0 && m_current < m_teachers.size() ? &m_teachers[m_current] : nullptr;
}

const StudentApp::Teacher *StudentApp::currentTeacher() const
{
    return m_current >= 0 && m_current < m_teachers.size() ? &m_teachers[m_current] : nullptr;
}

QVariantList StudentApp::teachers() const
{
    QVariantList list;
    for (const Teacher &t : m_teachers) {
        list.append(QVariantMap{{u"id"_s, t.id},
                                {u"teacher"_s, t.booklet.value(u"teacher"_s).toString()},
                                {u"student"_s, t.booklet.value(u"student"_s).toString()},
                                {u"instrument"_s, t.booklet.value(u"instrument"_s).toString()},
                                {u"updated"_s, t.fetched},
                                {u"error"_s, t.error},
                                {u"demo"_s, t.demo},
                                {u"empty"_s, t.booklet.isEmpty()}});
    }
    return list;
}

void StudentApp::setCurrentIndex(int index)
{
    if (index == m_current || index < 0 || index >= m_teachers.size())
        return;
    m_current = index;
    save();
    emit changed();
}

QJsonObject StudentApp::feedbackFor(const Teacher &teacher, const QString &taskId) const
{
    // The teacher's Qupil sends back what it received (also from the student's other devices);
    // whatever is newer wins
    const QJsonObject mine = teacher.feedback.value(taskId).toObject();
    const QJsonObject theirs = teacher.booklet.value(u"feedback"_s).toObject().value(taskId).toObject();
    if (theirs.isEmpty())
        return mine;
    if (mine.isEmpty())
        return theirs;
    return mine.value(u"at"_s).toDouble() >= theirs.value(u"at"_s).toDouble() ? mine : theirs;
}

QVariantMap StudentApp::current() const
{
    const Teacher *t = currentTeacher();
    if (!t)
        return {};
    const QJsonObject &b = t->booklet;
    QVariantMap out = b.toVariantMap();
    out.remove(u"feedback"_s);
    out.insert(u"id"_s, t->id);
    out.insert(u"demo"_s, t->demo);
    out.insert(u"updated"_s, t->fetched);
    out.insert(u"error"_s, t->error);
    out.insert(u"empty"_s, b.isEmpty());

    // Tasks by lesson, newest first; each with the student's feedback
    QMap<QString, QVariantList> byDate;
    for (const QJsonValue &v : b.value(u"tasks"_s).toArray()) {
        QVariantMap task = v.toObject().toVariantMap();
        const QJsonObject fb = feedbackFor(*t, task.value(u"id"_s).toString());
        task.insert(u"practiced"_s, fb.value(u"practiced"_s).toInt());
        task.insert(u"works"_s, fb.value(u"works"_s).toInt());
        task.insert(u"done"_s, fb.value(u"done"_s).toBool());
        byDate[task.value(u"date"_s).toString()].append(task);
    }
    QVariantList groups;
    for (auto it = byDate.constEnd(); it != byDate.constBegin();) {
        --it;
        groups.append(QVariantMap{{u"date"_s, it.key()}, {u"tasks"_s, it.value()}});
    }
    out.insert(u"groups"_s, groups);

    // Next lessons as dates (QML formats them in the user's language)
    QVariantList next;
    for (const QJsonValue &v : b.value(u"next"_s).toArray()) {
        QVariantMap lesson = v.toObject().toVariantMap();
        // Dates stay ISO strings; QML reads them at local noon, so no time zone can shift the day
        lesson.insert(u"daysAway"_s, today().daysTo(QDate::fromString(lesson.value(u"date"_s).toString(), Qt::ISODate)));
        next.append(lesson);
    }
    out.insert(u"next"_s, next);
    QVariantList events;
    for (const QJsonValue &v : b.value(u"events"_s).toArray()) {
        events.append(v.toObject().toVariantMap());
    }
    out.insert(u"events"_s, events);

    out.insert(u"questions"_s, t->questions.toVariantList());
    const bool pending = t->changedAt > t->sentState;
    out.insert(u"sendState"_s, t->demo ? u"demo"_s
                               : pending ? u"pending"_s
                               : t->sentAt > 0 ? u"sent"_s : u"none"_s);
    out.insert(u"sentAt"_s, t->sentAt > 0 ? QDateTime::fromMSecsSinceEpoch(t->sentAt) : QDateTime());
    return out;
}

// ---------------------------------------------------------------------------- coupling

QString StudentApp::addLink(const QString &text)
{
    const auto link = Booklet::decodeLink(text);
    if (!link)
        return u"invalid"_s;
    const QString id = link->id();
    int index = -1;
    for (int i = 0; i < m_teachers.size(); ++i)
        if (m_teachers[i].id == id)
            index = i;
    if (index < 0) {
        Teacher t;
        t.id = id;
        t.link = *link;
        m_teachers.append(t);
        index = int(m_teachers.size()) - 1;
    } else {
        m_teachers[index].link = *link;   // a new password or key wins
    }
    m_current = index;
    save();
    emit changed();
    fetch(m_teachers[index]);
    return {};
}

void StudentApp::openDemo()
{
    int index = -1;
    for (int i = 0; i < m_teachers.size(); ++i)
        if (m_teachers[i].demo)
            index = i;
    if (index < 0) {
        Teacher t;
        t.id = u"demo"_s;
        t.demo = true;
        t.booklet = demoBooklet();
        t.fetched = QDateTime::currentDateTime();
        m_teachers.append(t);
        index = int(m_teachers.size()) - 1;
    }
    m_current = index;
    save();
    emit changed();
}

void StudentApp::removeTeacher(const QString &id)
{
    for (int i = 0; i < m_teachers.size(); ++i) {
        if (m_teachers[i].id == id) {
            m_teachers.removeAt(i);
            if (m_current >= m_teachers.size())
                m_current = std::max(0, int(m_teachers.size()) - 1);
            save();
            emit changed();
            return;
        }
    }
}

void StudentApp::openUrl(const QUrl &url)
{
    // Android hands a link over on its own main thread while the app is running (only at
    // the app's start it comes on Qt's thread): go on where the app's state lives. Without
    // this the link opened the app and nothing else happened.
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, [this, url] { openUrl(url); }, Qt::QueuedConnection);
        return;
    }
    // Handed over twice (iOS at the app's start: by Qt and by the scene's start options)
    if (url == m_lastUrl && m_lastUrlAt.isValid() && m_lastUrlAt.elapsed() < 5000)
        return;
    m_lastUrl = url;
    m_lastUrlAt.start();
    const QString text = url.toString();
    const bool ok = Booklet::decodeLink(text) && addLink(text).isEmpty();
    // For the tests on a device (adb logcat); never the link itself, it is the key
    qInfo("Qupil student: link received, %s", ok ? "teacher added" : "not a coupling link");
    if (ok) {
        setScanError({});
        emit notice(qtTrId("s.coupled"));
    }
}

bool StudentApp::canScan() const
{
    return QrScanner::available();
}

void StudentApp::setScanError(const QString &error)
{
    if (m_scanError == error)
        return;
    m_scanError = error;
    emit scanErrorChanged();
}

void StudentApp::scan()
{
    setScanError({});
    QrScanner::scan({ qtTrId("s.add.scan.prompt"), qtTrId("s.add.scan.cancel") }, [this](const QrScanner::Result &result) {
        if (result.denied) {
            setScanError(u"denied"_s);
        } else if (result.text.isEmpty()) {
            // closed without a code
        } else if (addLink(result.text).isEmpty()) {
            emit notice(qtTrId("s.coupled"));
        } else {
            setScanError(u"invalid"_s);
        }
        qInfo("Qupil student: scanner closed, %s", result.denied ? "camera refused" : result.text.isEmpty() ? "no code"
                  : m_scanError.isEmpty() ? "teacher added" : "not a coupling code");
    });
}

QString StudentApp::clipboardLink() const
{
    const QString text = QGuiApplication::clipboard() ? QGuiApplication::clipboard()->text() : QString();
    return Booklet::decodeLink(text) ? text.trimmed() : QString();
}

// ---------------------------------------------------------------------------- network

void StudentApp::setBusy(bool on)
{
    m_busy += on ? 1 : -1;
    emit busyChanged();
}

void StudentApp::refresh()
{
    for (Teacher &t : m_teachers)
        if (!t.demo)
            fetch(t);
    sendPending();
}

void StudentApp::fetch(Teacher &teacher, bool legacy)
{
    const QString id = teacher.id;
    const QString file = QString::fromLatin1(Booklet::BookletFile);
    QNetworkRequest request(legacy ? Booklet::legacyFileUrl(teacher.link, file) : Booklet::fileUrl(teacher.link, file));
    request.setRawHeader("Authorization", legacy ? Booklet::legacyAuthorization(teacher.link)
                                                 : Booklet::authorization(teacher.link));
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
    request.setTransferTimeout(20000);
    setBusy(true);
    QNetworkReply *reply = m_net->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, id, legacy] {
        reply->deleteLater();
        setBusy(false);
        Teacher *t = find(id);
        if (!t)
            return;
        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (reply->error() != QNetworkReply::NoError) {
            // Nextcloud before 29 has no new public WebDAV: try the old one once
            if (!legacy && (status == 404 || status == 405 || status == 501)) {
                fetch(*t, true);
                return;
            }
            t->error = (status == 404 || status == 401 || status == 403) ? u"gone"_s : u"offline"_s;
        } else {
            const auto content = Booklet::open(t->link.key, Booklet::Purpose::Booklet, reply->readAll());
            if (!content) {
                t->error = u"key"_s;
            } else if (content->value(u"format"_s).toInt() > Booklet::Format) {
                t->error = u"newer"_s;
            } else {
                t->booklet = *content;
                t->legacy = legacy;
                t->fetched = QDateTime::currentDateTime();
                t->error.clear();
            }
        }
        save();
        emit changed();
    });
}

void StudentApp::touch(Teacher &teacher)
{
    teacher.changedAt = std::max(nowMs(), teacher.changedAt + 1);
    save();
    emit changed();
    scheduleUpload();
}

void StudentApp::scheduleUpload()
{
    m_uploadTimer.start();
}

void StudentApp::sendPending()
{
    m_uploadTimer.stop();
    for (Teacher &t : m_teachers)
        if (!t.demo && t.changedAt > t.sentState)
            upload(t);
}

void StudentApp::upload(Teacher &teacher)
{
    const qint64 state = teacher.changedAt;
    const QJsonObject content{{u"device"_s, m_deviceId},
                              {u"written"_s, QDateTime::currentDateTimeUtc().toString(Qt::ISODate)},
                              {u"feedback"_s, teacher.feedback},
                              {u"questions"_s, teacher.questions}};
    const QString name = Booklet::feedbackFileName(m_deviceId, nowMs());
    QNetworkRequest request(teacher.legacy ? Booklet::legacyFileUrl(teacher.link, name) : Booklet::fileUrl(teacher.link, name));
    request.setRawHeader("Authorization", teacher.legacy ? Booklet::legacyAuthorization(teacher.link)
                                                         : Booklet::authorization(teacher.link));
    request.setRawHeader("X-Requested-With", "XMLHttpRequest");
    request.setHeader(QNetworkRequest::ContentTypeHeader, u"application/octet-stream"_s);
    request.setTransferTimeout(20000);
    const QString id = teacher.id;
    setBusy(true);
    QNetworkReply *reply = m_net->put(request, Booklet::seal(teacher.link.key, Booklet::Purpose::Feedback, content));
    connect(reply, &QNetworkReply::finished, this, [this, reply, id, state] {
        reply->deleteLater();
        setBusy(false);
        Teacher *t = find(id);
        if (!t)
            return;
        if (reply->error() == QNetworkReply::NoError) {
            t->sentState = std::max(t->sentState, state);
            t->sentAt = nowMs();
            save();
            emit changed();
        }
        // Otherwise it stays pending and goes with the next change, refresh or app switch
    });
}

// ---------------------------------------------------------------------------- feedback

void StudentApp::updateFeedback(const QString &taskId, const QString &field, const QJsonValue &value)
{
    Teacher *t = currentTeacher();
    if (!t || taskId.isEmpty())
        return;
    QJsonObject entry = feedbackFor(*t, taskId);
    entry.insert(field, value);
    entry.insert(u"at"_s, double(nowMs()));
    t->feedback.insert(taskId, entry);
    touch(*t);
}

void StudentApp::setPracticed(const QString &taskId, int value)
{
    updateFeedback(taskId, u"practiced"_s, std::clamp(value, 0, 3));
}

void StudentApp::setWorks(const QString &taskId, int value)
{
    updateFeedback(taskId, u"works"_s, std::clamp(value, 0, 3));
}

void StudentApp::setDone(const QString &taskId, bool done)
{
    updateFeedback(taskId, u"done"_s, done);
}

void StudentApp::askQuestion(const QString &text, const QString &taskId)
{
    Teacher *t = currentTeacher();
    const QString trimmed = text.trimmed();
    if (!t || trimmed.isEmpty())
        return;
    t->questions.append(QJsonObject{{u"id"_s, newId().left(12)},
                                    {u"text"_s, trimmed.left(1000)},
                                    {u"task"_s, taskId},
                                    {u"at"_s, double(nowMs())}});
    touch(*t);
}

// ---------------------------------------------------------------------------- example

QJsonObject StudentApp::demoBooklet()
{
    // Invented data only (the example and Apple's review); dates around today
    const QDate d = today();
    const QDate lastLesson = d.addDays(-((d.dayOfWeek() - 2 + 7) % 7 == 0 ? 7 : (d.dayOfWeek() - 2 + 7) % 7));
    const QDate nextLesson = lastLesson.addDays(7);
    const auto iso = [](const QDate &date) { return date.toString(Qt::ISODate); };
    const auto task = [&](const QString &id, const QDate &date, const QString &title, const QStringList &hints,
                          const QString &kind = u"practice"_s, const QDate &since = {}) {
        QJsonObject o{{u"id"_s, id}, {u"date"_s, iso(date)}, {u"title"_s, title},
                      {u"hints"_s, QJsonArray::fromStringList(hints)}, {u"kind"_s, kind}};
        if (since.isValid())
            o.insert(u"since"_s, iso(since));
        return o;
    };
    const bool de = QLocale().language() == QLocale::German;
    const auto t = [de](const char *german, const char *english) {
        return QString::fromUtf8(de ? german : english);
    };
    QJsonArray tasks{
        task(u"demo-1"_s, lastLesson, t("Bourrée Takt 17–24 langsam (Metronom 60)", "Bourrée bars 17–24 slowly (metronome 60)"),
             {t("Rechter Ellenbogen locker, nicht hochziehen", "Right elbow relaxed, do not lift it"),
              t("Linkes Handgelenk gerade lassen", "Keep the left wrist straight"),
              t("Finger rund, nicht durchdrücken", "Round fingers, do not press them flat"),
              t("Auf die Haltung achten: Schultern unten", "Mind your posture: shoulders down")}),
        task(u"demo-2"_s, lastLesson, t("Lagenwechsel 1.–3. Lage, täglich 5 Min.", "Shifting 1st–3rd position, 5 min a day"),
             {t("Daumen wandert mit", "The thumb moves along"),
              t("Erst hörbar gleiten, dann ohne Gleiten", "Slide audibly first, then without sliding")}),
        task(u"demo-3"_s, lastLesson, t("Noten „Suzuki 3“ besorgen", "Get the music \"Suzuki 3\""), {},
             u"todo"_s, lastLesson.addDays(-14)),
    };
    QJsonObject feedback{{u"demo-2"_s, QJsonObject{{u"practiced"_s, 3}, {u"works"_s, 3}, {u"at"_s, 1.0}}}};
    return QJsonObject{
        {u"format"_s, Qupil::Booklet::Format},
        {u"teacher"_s, t("Frau Rau", "Ms Rau")},
        {u"student"_s, u"Jonas"_s},
        {u"instrument"_s, t("Viola", "Viola")},
        {u"allow"_s, QJsonObject{{u"feedback"_s, true}, {u"questions"_s, true}}},
        {u"next"_s, QJsonArray{
             QJsonObject{{u"date"_s, iso(nextLesson)}, {u"start"_s, u"15:30"_s}, {u"end"_s, u"16:30"_s},
                         {u"place"_s, t("Musikschule, Raum 6", "Music school, room 6")},
                         {u"changed"_s, t("Raum 6 statt Raum 4", "Room 6 instead of room 4")}},
             QJsonObject{{u"date"_s, iso(nextLesson.addDays(7))}, {u"start"_s, u"15:30"_s}, {u"end"_s, u"16:30"_s},
                         {u"place"_s, t("Musikschule, Raum 4", "Music school, room 4")},
                         {u"cancelled"_s, true}, {u"reason"_s, t("Fortbildung", "Training day")}}}},
        {u"tasks"_s, tasks},
        {u"feedback"_s, feedback},
        {u"pieces"_s, QJsonArray{
             QJsonObject{{u"title"_s, u"Bourrée"_s}, {u"composer"_s, u"J. S. Bach"_s},
                         {u"since"_s, iso(d.addMonths(-2))}, {u"ready"_s, true}},
             QJsonObject{{u"title"_s, t("Etüde Nr. 4", "Study no. 4")}, {u"composer"_s, u"F. Wohlfahrt"_s},
                         {u"since"_s, iso(d.addMonths(-1))}}}},
        {u"events"_s, QJsonArray{
             QJsonObject{{u"date"_s, iso(d.addDays(21))}, {u"time"_s, u"18:00"_s},
                         {u"name"_s, t("Wintervorspiel", "Winter recital")}, {u"place"_s, t("Aula", "Hall")},
                         {u"contribution"_s, u"Bourrée"_s}}}},
    };
}
