// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QPointer>
#include <QStringList>
#include <QTimer>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

class AppController;
class QJSEngine;
class QQmlEngine;

/*!
    Sync with Nextcloud, available in QML as the singleton "Sync".

    Connecting uses Nextcloud's login in the browser (Login Flow v2): Qupil gets
    an app password, never the user's own password. The data is encrypted with
    the sync password before it leaves the device (see FolderSync).
    Syncs on start, every few minutes and shortly after changes.
*/
class SyncController : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Sync)
    QML_SINGLETON

    Q_PROPERTY(bool connected READ connected NOTIFY accountChanged FINAL)
    Q_PROPERTY(QString server READ server NOTIFY accountChanged FINAL)
    Q_PROPERTY(QString user READ user NOTIFY accountChanged FINAL)
    //! Waiting for the login in the browser
    Q_PROPERTY(bool loggingIn READ loggingIn NOTIFY loggingInChanged FINAL)
    //! Connected, but this device does not know the sync password yet
    Q_PROPERTY(bool needsPassword READ needsPassword NOTIFY accountChanged FINAL)
    //! The sync folder exists already (enter password) or not (choose password)
    Q_PROPERTY(bool folderExists READ folderExists NOTIFY folderExistsChanged FINAL)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged FINAL)
    //! Looking whether the sync folder exists (right after connecting)
    Q_PROPERTY(bool checking READ checking NOTIFY busyChanged FINAL)
    Q_PROPERTY(QString lastSync READ lastSync NOTIFY statusChanged FINAL)
    Q_PROPERTY(QString error READ error NOTIFY statusChanged FINAL)
    Q_PROPERTY(QStringList devices READ devices NOTIFY statusChanged FINAL)
    Q_PROPERTY(bool autoSync READ autoSync WRITE setAutoSync NOTIFY autoSyncChanged FINAL)
    //! Name of this device as other devices see it
    Q_PROPERTY(QString deviceName READ deviceName WRITE setDeviceName NOTIFY deviceNameChanged FINAL)

public:
    SyncController(AppController *app, QObject *parent);
    ~SyncController() override;

    static SyncController *instance();
    static SyncController *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    bool connected() const { return !m_appPassword.isEmpty(); }
    QString server() const { return m_server.toString(); }
    QString user() const { return m_loginName; }
    bool loggingIn() const { return m_pollTimer.isActive() || m_loginReply; }
    bool needsPassword() const { return connected() && m_key.isEmpty(); }
    bool folderExists() const { return m_folderExists; }
    bool busy() const { return m_busy; }
    bool checking() const { return m_checking; }
    QString lastSync() const { return m_lastSync; }
    QString error() const { return m_error; }
    QStringList devices() const { return m_devices; }
    bool autoSync() const { return m_autoSync; }
    void setAutoSync(bool on);
    QString deviceName() const { return m_deviceName; }
    void setDeviceName(const QString &name);

    //! Starts the login in the browser for this server (address with or without https://)
    Q_INVOKABLE void connectNextcloud(const QString &serverAddress);
    Q_INVOKABLE void cancelLogin();
    //! Forgets the account and the key on this device (data stays)
    Q_INVOKABLE void disconnectNextcloud();
    //! What happens to the data on this device when it joins an existing sync folder
    enum JoinMode {
        Merge,            //!< keep both (fine for data that only exists here)
        ReplaceLocal,     //!< back up and replace this device's data with the folder's
        StartOver         //!< keep the old folder aside, start a new one with this device's data
    };
    Q_ENUM(JoinMode)

    /*!
        First device: chooses the password; other devices: checks it.
        joinMode only matters when the folder exists already.
    */
    Q_INVOKABLE void setPassword(const QString &password, JoinMode joinMode = Merge);
    Q_INVOKABLE void syncNow();

signals:
    void accountChanged();
    void loggingInChanged();
    void folderExistsChanged();
    void busyChanged();
    void statusChanged();
    void autoSyncChanged();
    void deviceNameChanged();
    //! Wrong sync password, login failed, …
    void message(const QString &text);
    //! The first sync of a device that waited for data is done
    void firstSyncDone();

private:
    void pollLogin();
    void fetchUserId(const QString &loginName, const QString &appPassword, const QUrl &server);
    void checkFolder();
    void loadSettings();
    void saveSettings();
    void setBusy(bool busy);
    void setError(const QString &error);
    void scheduleSoon();
    void forgetKey(const QString &reason);

    AppController *m_app;
    QNetworkAccessManager m_network;
    QPointer<QNetworkReply> m_loginReply;
    QTimer m_pollTimer;
    QTimer m_periodic;
    QTimer m_soon;
    QUrl m_pollEndpoint;
    QByteArray m_pollToken;
    int m_pollCount = 0;

    QUrl m_server;
    QString m_loginName;
    QString m_userId;
    QString m_appPassword;
    QByteArray m_key;
    bool m_folderExists = false;
    bool m_busy = false;
    bool m_checking = false;
    bool m_syncAgain = false;
    bool m_applyingRemote = false;
    bool m_replacingData = false;
    bool m_autoSync = true;
    QString m_lastSync;
    QString m_error;
    QStringList m_devices;
    QString m_deviceName;
};
