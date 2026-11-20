// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception

#include "BookletService.h"

#include <QJSEngine>
#include <QJsonArray>
#include <QJsonDocument>
#include <QPointer>
#include <QThread>

#include "AppController.h"
#include "Booklet.h"
#include "BookletInbox.h"
#include "Clock.h"
#include "Crypto.h"
#include "DataService.h"
#include "NextcloudShares.h"
#include "QrCode.h"
#include "SyncController.h"
#include "WebDavFolder.h"

using namespace Qt::StringLiterals;
using namespace Qupil;

namespace {

BookletService *s_instance = nullptr;

//! Where the students' folders are, next to the sync folder "Qupil" (not in it)
const QString kRoot = u"Qupil Übeheft"_s;
constexpr int kSoonMs = 20 * 1000;

//! Runs fn in a worker thread and delivers its result to done() in the GUI thread
template<typename Result, typename Work, typename Done>
void inWorker(QObject *context, Work work, Done done)
{
    QPointer<QObject> guard(context);
    QThread *thread = QThread::create([guard, work, done] {
        Result result = work();
        if (guard)
            QMetaObject::invokeMethod(guard.data(), [result, done] { done(result); }, Qt::QueuedConnection);
    });
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

//! Random text from an alphabet without look-alikes
QString randomText(int length)
{
    static const QString alphabet = u"abcdefghijkmnpqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ23456789"_s;
    const QByteArray bytes = Crypto::randomBytes(length);
    QString out;
    for (const char b : bytes)
        out += alphabet.at(uchar(b) % alphabet.size());
    return out;
}

//! A link password any password policy takes: upper and lower case, digits, a sign
QString linkPassword()
{
    return u"Qp-7"_s + randomText(24);
}

Booklet::Link linkOf(const QVariantMap &coupling)
{
    Booklet::Link link;
    link.server = coupling.value(u"server"_s).toString();
    link.token = coupling.value(u"token"_s).toString();
    link.password = coupling.value(u"password"_s).toString();
    link.key = QByteArray::fromBase64(coupling.value(u"key"_s).toString().toLatin1());
    return link;
}

QString serverText(const QUrl &server)
{
    QString s = server.toString(QUrl::RemoveUserInfo | QUrl::StripTrailingSlash);
    return s;
}

} // namespace

BookletService::BookletService(AppController *app, DataService *data, SyncController *sync, QObject *parent)
    : QObject(parent)
    , m_app(app)
    , m_data(data)
    , m_sync(sync)
{
    Q_ASSERT_X(!s_instance, "BookletService", "only one instance allowed");
    s_instance = this;
    m_soon.setSingleShot(true);
    m_soon.setInterval(kSoonMs);
    connect(&m_soon, &QTimer::timeout, this, &BookletService::publishAll);
    connect(m_sync, &SyncController::accountChanged, this, &BookletService::availableChanged);
    connect(m_app, &AppController::testModeChanged, this, &BookletService::availableChanged);
    // After each sync: what the other devices changed goes into the booklets too
    connect(m_sync, &SyncController::statusChanged, this, [this] {
        if (!m_sync->busy() && m_sync->error().isEmpty())
            m_soon.start(2000);
    });
    // Tasks changed here: written again shortly after (several changes in a row once)
    connect(m_data, &DataService::changed, this, [this] {
        if (!m_publishing && available())
            m_soon.start(kSoonMs);
    });
}

BookletService::~BookletService()
{
    if (s_instance == this)
        s_instance = nullptr;
}

BookletService *BookletService::instance()
{
    return s_instance;
}

BookletService *BookletService::create(QQmlEngine *, QJSEngine *)
{
    Q_ASSERT_X(s_instance, "BookletService", "create the instance in main() before loading QML");
    QJSEngine::setObjectOwnership(s_instance, QJSEngine::CppOwnership);
    return s_instance;
}

bool BookletService::available() const
{
    return m_sync->connected() && !m_app->testMode();
}

void BookletService::setRunning(const QString &job, bool running)
{
    const bool before = busy();
    if (running)
        m_running.insert(job);
    else
        m_running.remove(job);
    if (before != busy())
        emit busyChanged();
}

// ---------------------------------------------------------------------------- coupling

void BookletService::couple(const QString &studentId)
{
    if (!available()) {
        emit coupled(studentId, u"no-nextcloud"_s);
        return;
    }
    if (m_data->isCoupled(studentId)) {
        emit coupled(studentId, QString());
        return;
    }
    const QString job = u"couple:"_s + studentId;
    if (m_running.contains(job))
        return;
    setRunning(job, true);
    const SyncController::AccountInfo account = m_sync->account();
    struct Result
    {
        std::optional<NextcloudShares::Share> share;
        QString folder, password, error;
    };
    inWorker<Result>(this, [account] {
        Result r;
        WebDavFolder dav(account.server, account.loginName, account.userId, account.appPassword);
        r.folder = kRoot + u'/' + randomText(12);
        if (!dav.makeFolder(kRoot) || !dav.makeFolder(r.folder)) {
            r.error = dav.lastStatus() == 401 ? u"access"_s : u"offline"_s;
            return r;
        }
        NextcloudShares api(account.server, account.loginName, account.appPassword);
        r.password = linkPassword();
        r.share = api.createLink(u'/' + r.folder, r.password);
        if (!r.share) {
            r.error = api.lastError() == u"links-disabled"_s ? u"links-disabled"_s : u"link"_s;
            dav.remove(r.folder);
        }
        return r;
    }, [this, studentId, job, account](const Result &r) {
        setRunning(job, false);
        if (!r.share) {
            emit coupled(studentId, r.error);
            return;
        }
        const QString id = m_data->create(u"coupling"_s, {
            { u"student_id"_s, studentId }, { u"server"_s, serverText(account.server) }, { u"folder"_s, r.folder },
            { u"share_id"_s, r.share->id }, { u"token"_s, r.share->token }, { u"password"_s, r.password },
            { u"key"_s, QString::fromLatin1(Crypto::randomBytes(Crypto::KeySize).toBase64()) },
            { u"created"_s, Clock::today().toString(Qt::ISODate) }, { u"receives"_s, r.share->canReceive() ? 1 : 0 } });
        emit coupled(studentId, id.isEmpty() ? u"save"_s : QString());
        publishAll();
    });
}

void BookletService::endCoupling(const QString &studentId)
{
    const QVariantMap c = m_data->coupling(studentId);
    if (c.isEmpty()) {
        emit ended(studentId, QString());
        return;
    }
    const QString couplingId = c.value(u"id"_s).toString();
    const auto markEnded = [this, couplingId] {
        m_data->update(u"coupling"_s, couplingId, { { u"ended"_s, Clock::today().toString(Qt::ISODate) } });
    };
    if (!available()) {
        // Without the Nextcloud the link cannot go: ended here, the next device with it ends it
        markEnded();
        emit ended(studentId, u"no-nextcloud"_s);
        return;
    }
    const QString job = u"end:"_s + studentId;
    setRunning(job, true);
    const SyncController::AccountInfo account = m_sync->account();
    const QString shareId = c.value(u"share_id"_s).toString();
    const QString folder = c.value(u"folder"_s).toString();
    inWorker<bool>(this, [account, shareId, folder] {
        NextcloudShares api(account.server, account.loginName, account.appPassword);
        const bool removed = shareId.isEmpty() || api.remove(shareId);
        if (removed && !folder.isEmpty()) {
            WebDavFolder dav(account.server, account.loginName, account.userId, account.appPassword);
            dav.remove(folder);
        }
        return removed;
    }, [this, studentId, job, markEnded](bool removed) {
        setRunning(job, false);
        if (removed)
            markEnded();
        emit ended(studentId, removed ? QString() : u"offline"_s);
    });
}

QString BookletService::link(const QString &studentId) const
{
    const QVariantMap c = m_data->coupling(studentId);
    if (c.isEmpty())
        return {};
    const Booklet::Link l = linkOf(c);
    return l.isValid() ? Booklet::encodeLink(l) : QString();
}

QVariantMap BookletService::qrCode(const QString &studentId) const
{
    const QString text = link(studentId);
    if (text.isEmpty())
        return {};
    const QrCode::Code code = QrCode::encode(text);
    if (code.isEmpty())
        return {};
    return { { u"path"_s, QrCode::toSvgPath(code) }, { u"size"_s, code.size + 8 } };
}

// ---------------------------------------------------------------------------- booklets and answers

void BookletService::publishAll()
{
    m_soon.stop();
    if (!available() || m_running.contains(u"publish"_s))
        return;
    // Everything from the database in the GUI thread; only the network in the worker
    struct Student
    {
        QString id, folder;
        QByteArray key, sealed;
    };
    QList<Student> students;
    for (const QVariant &v : m_data->couplings()) {
        const QVariantMap c = v.toMap();
        const Booklet::Link l = linkOf(c);
        if (!l.isValid() || c.value(u"folder"_s).toString().isEmpty())
            continue;
        const QString studentId = c.value(u"student_id"_s).toString();
        const QJsonObject content = m_data->bookletContent(studentId);
        if (content.isEmpty())
            continue;
        students << Student { studentId, c.value(u"folder"_s).toString(), l.key,
                              Booklet::seal(l.key, Booklet::Purpose::Booklet, content) };
    }
    if (students.isEmpty())
        return;
    setRunning(u"publish"_s, true);
    const SyncController::AccountInfo account = m_sync->account();
    struct Answers
    {
        QString studentId;
        QByteArray key;
        QHash<QString, QByteArray> files;
        bool ok = false;
    };
    inWorker<QList<Answers>>(this, [account, students] {
        WebDavFolder dav(account.server, account.loginName, account.userId, account.appPassword);
        QList<Answers> out;
        for (const Student &s : students) {
            Answers a { s.id, s.key, {}, false };
            a.ok = dav.write(s.folder + u'/' + QString::fromLatin1(Booklet::BookletFile), s.sealed);
            if (const auto names = dav.list(s.folder)) {
                for (const QString &name : *names) {
                    const QString file = name.section(u'/', -1);
                    if (!Booklet::isFeedbackFileName(file))
                        continue;
                    if (const auto data = dav.read(s.folder + u'/' + file))
                        a.files.insert(file, *data);
                }
            }
            out << a;
        }
        return out;
    }, [this, account, students](const QList<Answers> &all) {
        QStringList arrived;
        QStringList toDelete;
        m_publishing = true;   // the answers change data: no new round for that
        for (const Answers &a : all) {
            if (a.files.isEmpty())
                continue;
            // What this teacher's Qupil has so far, so only newer answers count
            QJsonObject known;
            QJsonObject feedback;
            for (const QVariant &v : m_data->studentTasks(a.studentId)) {
                const QVariantMap t = v.toMap();
                if (!t.value(u"feedback"_s).toMap().isEmpty())
                    feedback.insert(t.value(u"id"_s).toString(), QJsonObject::fromVariantMap(t.value(u"feedback"_s).toMap()));
            }
            known.insert(u"feedback"_s, feedback);
            known.insert(u"questions"_s, QJsonDocument::fromJson(m_data->coupling(a.studentId).value(u"questions"_s).toString().toUtf8()).array());
            const Booklet::Inbox inbox = Booklet::collect(a.key, a.files, known);
            if (inbox.changed && m_data->applyFeedback(a.studentId, inbox.feedback.toVariantMap(), inbox.questions.toVariantList()))
                arrived << a.studentId;
            const QString folder = [&] {
                for (const Student &s : students)
                    if (s.id == a.studentId)
                        return s.folder;
                return QString();
            }();
            for (const QString &file : inbox.superseded)
                toDelete << folder + u'/' + file;
        }
        m_publishing = false;
        if (!arrived.isEmpty())
            emit answersArrived(arrived);
        if (toDelete.isEmpty()) {
            setRunning(u"publish"_s, false);
            return;
        }
        inWorker<bool>(this, [account, toDelete] {
            WebDavFolder dav(account.server, account.loginName, account.userId, account.appPassword);
            for (const QString &path : toDelete)
                dav.remove(path);
            return true;
        }, [this](bool) { setRunning(u"publish"_s, false); });
    });
}
