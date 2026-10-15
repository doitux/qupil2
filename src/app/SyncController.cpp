// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "SyncController.h"

#include <QDateTime>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QJSEngine>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPointer>
#include <QQmlEngine>
#include <QSettings>
#include <QSysInfo>
#include <QThread>
#include <QUrlQuery>

#include "AppController.h"
#include "Crypto.h"
#include "Database.h"
#include "FolderSync.h"
#include "Store.h"
#include "WebDavFolder.h"

using namespace Qt::StringLiterals;
using Qupil::FolderSync;

namespace {

SyncController *s_instance = nullptr;

constexpr int kPollMs = 2000;
constexpr int kMaxPolls = 600;            // 20 minutes for the login in the browser
constexpr int kPeriodicMs = 10 * 60 * 1000;
constexpr int kAfterChangeMs = 15 * 1000;

struct Account
{
    QUrl server;
    QString loginName;
    QString userId;
    QString password;
};

//! Runs fn in a worker thread and delivers its result to done() in the GUI thread
template<typename Result, typename Work, typename Done>
void inWorker(QObject *context, Work work, Done done)
{
    QPointer<QObject> guard(context);   // the app may be closing while the worker runs
    QThread *thread = QThread::create([guard, work, done] {
        Result result = work();
        if (guard)
            QMetaObject::invokeMethod(guard.data(), [result, done] { done(result); }, Qt::QueuedConnection);
    });
    QObject::connect(thread, &QThread::finished, thread, &QObject::deleteLater);
    thread->start();
}

} // namespace

SyncController::SyncController(AppController *app, QObject *parent)
    : QObject(parent)
    , m_app(app)
{
    Q_ASSERT_X(!s_instance, "SyncController", "only one instance allowed");
    s_instance = this;
    loadSettings();

    m_pollTimer.setInterval(kPollMs);
    connect(&m_pollTimer, &QTimer::timeout, this, &SyncController::pollLogin);
    m_periodic.setInterval(kPeriodicMs);
    connect(&m_periodic, &QTimer::timeout, this, [this] {
        if (m_autoSync)
            syncNow();
    });
    m_periodic.start();
    m_soon.setSingleShot(true);
    m_soon.setInterval(kAfterChangeMs);
    connect(&m_soon, &QTimer::timeout, this, &SyncController::syncNow);

    // After local changes, when the app comes to the front, when data is ready
    connect(app->store(), &Qupil::Store::dataChanged, this, [this] {
        if (!m_applyingRemote && m_autoSync)
            scheduleSoon();
    });
    connect(qGuiApp, &QGuiApplication::applicationStateChanged, this, [this](Qt::ApplicationState state) {
        if (state == Qt::ApplicationActive && m_autoSync)
            QTimer::singleShot(1500, this, &SyncController::syncNow);
    });
    connect(app, &AppController::phaseChanged, this, [this] {
        if (m_app->phase() == AppController::Ready && m_autoSync)
            QTimer::singleShot(3000, this, &SyncController::syncNow);
    });
    // After an import or restore the data on this device is a different one:
    // pause until the user decides how it goes together with the folder
    connect(app, &AppController::dataReplaced, this, [this] {
        if (!m_replacingData && connected() && !m_key.isEmpty())
            forgetKey(qtTrId("sync.paused_after_replace"));
    });
    connect(app, &AppController::busyChanged, this, [this] {
        if (!m_app->busy() && m_syncAgain) {
            m_syncAgain = false;
            QTimer::singleShot(500, this, &SyncController::syncNow);
        }
    });
    if (connected())
        QTimer::singleShot(3000, this, [this] {
            if (m_key.isEmpty())
                checkFolder();
            else if (m_autoSync)
                syncNow();
        });
}

SyncController::~SyncController()
{
    if (s_instance == this)
        s_instance = nullptr;
}

SyncController *SyncController::instance()
{
    return s_instance;
}

SyncController *SyncController::create(QQmlEngine *, QJSEngine *)
{
    Q_ASSERT_X(s_instance, "SyncController", "create the instance in main() before loading QML");
    QJSEngine::setObjectOwnership(s_instance, QJSEngine::CppOwnership);
    return s_instance;
}

// ---------------------------------------------------------------------------- settings

void SyncController::loadSettings()
{
    QSettings s;
    s.beginGroup(u"sync"_s);
    m_server = QUrl(s.value(u"server"_s).toString());
    m_loginName = s.value(u"loginName"_s).toString();
    m_userId = s.value(u"userId"_s, m_loginName).toString();
    m_appPassword = s.value(u"appPassword"_s).toString();
    m_key = QByteArray::fromBase64(s.value(u"key"_s).toByteArray());
    if (m_key.size() != Qupil::Crypto::KeySize)
        m_key.clear();
    m_autoSync = s.value(u"auto"_s, true).toBool();
    m_lastSync = s.value(u"lastSync"_s).toString();
    m_devices = s.value(u"devices"_s).toStringList();
    m_folderExists = s.value(u"folderExists"_s, false).toBool();
    QString fallback = QSysInfo::machineHostName();
    if (fallback.isEmpty() || fallback == u"localhost"_s)
        fallback = QSysInfo::prettyProductName();
    m_deviceName = s.value(u"deviceName"_s, fallback).toString();
}

void SyncController::saveSettings()
{
    QSettings s;
    s.beginGroup(u"sync"_s);
    s.setValue(u"server"_s, m_server.toString());
    s.setValue(u"loginName"_s, m_loginName);
    s.setValue(u"userId"_s, m_userId);
    s.setValue(u"appPassword"_s, m_appPassword);
    s.setValue(u"key"_s, m_key.toBase64());
    s.setValue(u"auto"_s, m_autoSync);
    s.setValue(u"lastSync"_s, m_lastSync);
    s.setValue(u"devices"_s, m_devices);
    s.setValue(u"folderExists"_s, m_folderExists);
    s.setValue(u"deviceName"_s, m_deviceName);
}

void SyncController::setAutoSync(bool on)
{
    if (on == m_autoSync)
        return;
    m_autoSync = on;
    saveSettings();
    emit autoSyncChanged();
}

void SyncController::setDeviceName(const QString &name)
{
    const QString n = name.trimmed();
    if (n.isEmpty() || n == m_deviceName)
        return;
    m_deviceName = n;
    saveSettings();
    emit deviceNameChanged();
}

void SyncController::setBusy(bool busy)
{
    if (busy == m_busy)
        return;
    m_busy = busy;
    emit busyChanged();
}

void SyncController::setError(const QString &error)
{
    m_error = error;
    emit statusChanged();
}

void SyncController::scheduleSoon()
{
    if (connected() && !m_key.isEmpty())
        m_soon.start();
}

void SyncController::forgetKey(const QString &reason)
{
    Qupil::Crypto::wipe(m_key);
    m_key.clear();
    saveSettings();
    emit accountChanged();
    if (!reason.isEmpty()) {
        setError(reason);
        emit message(reason);
    }
}

// ---------------------------------------------------------------------------- login (Nextcloud Login Flow v2)

void SyncController::connectNextcloud(const QString &serverAddress)
{
    QString address = serverAddress.trimmed();
    if (address.isEmpty())
        return;
    if (!address.contains(u"://"_s))
        address = u"https://"_s + address;
    // Accept pasted addresses like https://cloud.example.org/index.php/apps/files
    const qsizetype index = address.indexOf(u"/index.php"_s);
    if (index > 0)
        address = address.left(index);
    while (address.endsWith(u'/'))
        address.chop(1);
    m_server = QUrl(address);
    setError(QString());

    QNetworkRequest request(QUrl(address + u"/index.php/login/v2"_s));
    request.setRawHeader("User-Agent", ("Qupil (" + m_deviceName + u')').toUtf8());
    request.setTransferTimeout(30000);
    m_loginReply = m_network.post(request, QByteArray());
    emit loggingInChanged();
    connect(m_loginReply, &QNetworkReply::finished, this, [this, reply = m_loginReply.data()] {
        reply->deleteLater();
        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const QJsonObject o = QJsonDocument::fromJson(reply->readAll()).object();
        const QString login = o.value(u"login"_s).toString();
        m_pollEndpoint = QUrl(o.value(u"poll"_s).toObject().value(u"endpoint"_s).toString());
        m_pollToken = o.value(u"poll"_s).toObject().value(u"token"_s).toString().toUtf8();
        if (status != 200 || login.isEmpty() || !m_pollEndpoint.isValid()) {
            //: %1 = server address
            const QString text = qtTrId("sync.error.server").arg(m_server.host().isEmpty() ? m_server.toString() : m_server.host());
            setError(text);
            emit message(text);
            m_loginReply = nullptr;
            emit loggingInChanged();
            return;
        }
        m_loginReply = nullptr;
        m_pollCount = 0;
        m_pollTimer.start();
        emit loggingInChanged();
        QDesktopServices::openUrl(QUrl(login));
    });
}

void SyncController::pollLogin()
{
    if (++m_pollCount > kMaxPolls) {
        cancelLogin();
        setError(qtTrId("sync.error.login_timeout"));
        return;
    }
    QNetworkRequest request(m_pollEndpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    request.setTransferTimeout(15000);
    QNetworkReply *reply = m_network.post(request, "token=" + QUrl::toPercentEncoding(QString::fromUtf8(m_pollToken)));
    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        reply->deleteLater();
        if (!m_pollTimer.isActive())
            return;
        if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() != 200)
            return;   // 404 until the user has logged in
        const QJsonObject o = QJsonDocument::fromJson(reply->readAll()).object();
        const QString loginName = o.value(u"loginName"_s).toString();
        const QString appPassword = o.value(u"appPassword"_s).toString();
        QUrl server(o.value(u"server"_s).toString());
        if (loginName.isEmpty() || appPassword.isEmpty())
            return;
        m_pollTimer.stop();
        emit loggingInChanged();
        fetchUserId(loginName, appPassword, server.isValid() ? server : m_server);
    });
}

void SyncController::cancelLogin()
{
    m_pollTimer.stop();
    if (m_loginReply)
        m_loginReply->abort();
    m_loginReply = nullptr;
    emit loggingInChanged();
}

// The WebDAV path uses the user id, which can differ from the login name (e.g. e-mail login)
void SyncController::fetchUserId(const QString &loginName, const QString &appPassword, const QUrl &server)
{
    QUrl url = server;
    url.setPath(url.path() + u"/ocs/v1.php/cloud/user"_s);
    url.setQuery(u"format=json"_s);
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", "Basic " + (loginName + u':' + appPassword).toUtf8().toBase64());
    request.setRawHeader("OCS-APIRequest", "true");
    request.setTransferTimeout(30000);
    QNetworkReply *reply = m_network.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply, loginName, appPassword, server] {
        reply->deleteLater();
        const QJsonObject o = QJsonDocument::fromJson(reply->readAll()).object();
        const QString id = o.value(u"ocs"_s).toObject().value(u"data"_s).toObject().value(u"id"_s).toString();
        m_server = server;
        m_loginName = loginName;
        m_userId = id.isEmpty() ? loginName : id;
        m_appPassword = appPassword;
        m_key.clear();
        saveSettings();
        emit accountChanged();
        checkFolder();
    });
}

void SyncController::disconnectNextcloud()
{
    cancelLogin();
    Qupil::Crypto::wipe(m_key);
    m_appPassword.clear();
    m_loginName.clear();
    m_userId.clear();
    m_lastSync.clear();
    m_devices.clear();
    m_folderExists = false;
    saveSettings();
    emit accountChanged();
    emit folderExistsChanged();
    setError(QString());
}

// ---------------------------------------------------------------------------- password and sync

void SyncController::checkFolder()
{
    if (!connected())
        return;
    const Account account { m_server, m_loginName, m_userId, m_appPassword };
    struct Result { std::optional<bool> exists; QString error; int http = 0; };
    m_checking = true;
    setBusy(true);
    inWorker<Result>(this, [account] {
        WebDavFolder folder(account.server, account.loginName, account.userId, account.password);
        FolderSync sync(nullptr, &folder);
        Result r;
        r.exists = sync.isSetUp();
        r.error = sync.error();
        r.http = folder.lastStatus();
        return r;
    }, [this](const Result &r) {
        m_checking = false;
        setBusy(false);
        if (!r.exists) {
            setError(r.http == 401 ? qtTrId("sync.error.access_denied") : r.error);
            return;
        }
        m_folderExists = *r.exists;
        saveSettings();
        emit folderExistsChanged();
    });
}

void SyncController::setPassword(const QString &password, JoinMode joinMode)
{
    if (!connected() || password.isEmpty() || m_busy)
        return;
    const Account account { m_server, m_loginName, m_userId, m_appPassword };
    struct Result { QByteArray key; FolderSync::Status status = FolderSync::Status::Failed; QString error; bool existed = false; int http = 0; };
    setBusy(true);
    setError(QString());
    inWorker<Result>(this, [account, password, joinMode] {
        WebDavFolder folder(account.server, account.loginName, account.userId, account.password);
        FolderSync sync(nullptr, &folder);
        Result r;
        const auto existed = sync.isSetUp();
        r.existed = existed.value_or(false);
        r.key = joinMode == StartOver ? sync.startOver(password, &r.status) : sync.unlock(password, &r.status);
        r.error = sync.error();
        r.http = folder.lastStatus();
        return r;
    }, [this, joinMode](const Result &r) {
        setBusy(false);
        if (r.status == FolderSync::Status::WrongPassword) {
            emit message(qtTrId("sync.wrong_password"));
            return;
        }
        if (r.key.isEmpty()) {
            const QString text = r.http == 401 ? qtTrId("sync.error.access_denied") : r.error;
            setError(text);
            emit message(qtTrId("sync.failed").arg(text));
            return;
        }
        m_key = r.key;
        m_folderExists = true;
        saveSettings();
        if (r.existed && joinMode == ReplaceLocal && !m_app->awaitingSync()) {
            m_replacingData = true;
            const bool replaced = m_app->replaceWithEmpty();
            m_replacingData = false;
            if (!replaced) {
                forgetKey(QString());
                emit message(qtTrId("sync.failed").arg(qtTrId("sync.error.replace")));
                return;
            }
        }
        emit accountChanged();
        emit folderExistsChanged();
        syncNow();
    });
}

void SyncController::syncNow()
{
    if (!connected() || m_key.isEmpty() || m_app->phase() != AppController::Ready)
        return;
    if (m_busy || m_app->busy()) {
        m_syncAgain = true;   // after the running sync or the import
        return;
    }
    m_soon.stop();
    const Account account { m_server, m_loginName, m_userId, m_appPassword };
    const QByteArray key = m_key;
    const QString databasePath = m_app->databasePath();
    const QString deviceName = m_deviceName;
    struct Result { FolderSync::Status status = FolderSync::Status::Failed; FolderSync::Stats stats; QString error; int http = 0; };
    setBusy(true);
    m_app->beginSync();
    inWorker<Result>(this, [account, key, databasePath, deviceName] {
        Result r;
        // Own connection: the GUI keeps working while the sync runs
        Qupil::Database db;
        QString error;
        if (!db.open(databasePath, &error)) {
            r.status = FolderSync::Status::Failed;
            r.error = error;
            return r;
        }
        Qupil::Store store(&db);
        WebDavFolder folder(account.server, account.loginName, account.userId, account.password);
        FolderSync sync(&store, &folder);
        r.status = sync.run(key, deviceName, &r.stats);
        r.error = sync.error();
        r.http = folder.lastStatus();
        db.close();
        return r;
    }, [this](const Result &r) {
        setBusy(false);
        if (r.status == FolderSync::Status::Ok) {
            m_lastSync = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
            m_devices = r.stats.devices;
            m_error.clear();
            saveSettings();
            if (r.stats.applied > 0) {
                // Our clock must not fall behind what other devices wrote
                if (!r.stats.maxHlc.isEmpty())
                    m_app->database()->clock().observe(r.stats.maxHlc);
                m_applyingRemote = true;   // no new sync because of these changes
                m_app->store()->notifyReset();
                m_applyingRemote = false;
            }
            if (m_app->awaitingSync()) {
                m_app->syncArrived();
                emit firstSyncDone();
            }
            emit statusChanged();
        } else if (r.status == FolderSync::Status::WrongPassword) {
            // Another device started over with a new password
            forgetKey(qtTrId("sync.password_changed"));
        } else if (r.status == FolderSync::Status::NoPassword) {
            // The folder was removed on the server
            m_folderExists = false;
            emit folderExistsChanged();
            forgetKey(qtTrId("sync.folder_missing"));
        } else {
            setError(r.http == 401 ? qtTrId("sync.error.access_denied") : r.error);
        }
        if (m_syncAgain) {
            m_syncAgain = false;
            QTimer::singleShot(500, this, &SyncController::syncNow);
        }
        m_app->endSync();   // last: may finish an import that waited
    });
}
