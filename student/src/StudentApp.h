// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QDateTime>
#include <QElapsedTimer>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>

#include "Booklet.h"

class QNetworkAccessManager;
class QNetworkReply;

/*!
    The student app's state: the coupled teachers (one booklet each), the
    student's own feedback and its way back to the teacher.

    Everything stays on this device in one file (student.json in the app's data
    folder). Feedback is uploaded as a whole: each upload holds the full current
    feedback of this device, so a lost or late upload never loses anything.
*/
class StudentApp : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(QVariantList teachers READ teachers NOTIFY changed)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY changed)
    Q_PROPERTY(QVariantMap current READ current NOTIFY changed)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(QString version READ version CONSTANT)
    // The commit this build was made from ("dev" for local builds), and both together:
    // "1.0.0 (abc1234)", as in Qupil
    Q_PROPERTY(QString build READ build CONSTANT)
    Q_PROPERTY(QString versionText READ versionText CONSTANT)
    //! The phone's QR code scanner can be opened (scan())
    Q_PROPERTY(bool canScan READ canScan CONSTANT)
    //! Why the last scan added nothing: "" (fine, or closed), "invalid" (not a Qupil code),
    //! "denied" (the camera is not allowed)
    Q_PROPERTY(QString scanError READ scanError NOTIFY scanErrorChanged)
    //! The on-screen keyboard's height over the window (Android: measured, the window keeps
    //! its size; elsewhere 0). The pages make room for it (Main.qml).
    Q_PROPERTY(qreal keyboardHeight READ keyboardHeight NOTIFY keyboardHeightChanged)
    // Room the status bar and the home indicator take of the window (iOS; SafeArea.h)
    Q_PROPERTY(qreal safeTop READ safeTop NOTIFY safeAreaChanged)
    Q_PROPERTY(qreal safeBottom READ safeBottom NOTIFY safeAreaChanged)

public:
    ~StudentApp() override;

    //! The one instance; QML gets the same through create() (the constructor is private,
    //! otherwise Qt 6.4 would build a second one for QML)
    static StudentApp *instance();
    static StudentApp *create(QQmlEngine *, QJSEngine *);

    //! Data folder (tests, screenshots); before the first use
    static void setDataDir(const QString &dir);
    //! Fixed "today" for screenshots and tests
    static void setToday(const QDate &today);
    static QDate today();

    QVariantList teachers() const;
    int currentIndex() const { return m_current; }
    void setCurrentIndex(int index);
    QVariantMap current() const;
    bool busy() const { return m_busy > 0; }
    QString version() const;
    QString build() const;
    QString versionText() const;
    //! Puts the app's name with version and commit on the clipboard (for bug reports)
    Q_INVOKABLE void copyVersion() const;
    bool canScan() const;
    QString scanError() const { return m_scanError; }
    qreal keyboardHeight() const { return m_keyboardHeight; }
    void setKeyboardHeight(qreal height);
    qreal safeTop() const { return m_safeTop; }
    qreal safeBottom() const { return m_safeBottom; }
    void setSafeArea(qreal top, qreal bottom);

    //! A coupling link from the QR code, a message or the clipboard.
    //! Returns "" (added or already there), "invalid" or "demo-only".
    Q_INVOKABLE QString addLink(const QString &text);
    //! The example booklet (also for Apple's review: works without any Nextcloud)
    Q_INVOKABLE void openDemo();
    Q_INVOKABLE void refresh();
    Q_INVOKABLE void removeTeacher(const QString &id);

    Q_INVOKABLE void setPracticed(const QString &taskId, int value);   // 0 none, 1 hardly, 2 some, 3 a lot
    Q_INVOKABLE void setWorks(const QString &taskId, int value);       // 0 none, 1 not yet, 2 so-so, 3 well
    Q_INVOKABLE void setDone(const QString &taskId, bool done);
    Q_INVOKABLE void askQuestion(const QString &text, const QString &taskId = {});

    //! Opens the QR code scanner; a coupling code is added like a link (addLink)
    Q_INVOKABLE void scan();

    //! The text of the clipboard, if it is a coupling link (for "Paste link")
    Q_INVOKABLE QString clipboardLink() const;
    //! Goodbyes not sent yet (removed here, the teacher not told; for the tests)
    int pendingGoodbyes() const { return int(m_leaving.size()); }

    //! Called by the system for https://qupil.de/u#… and qupil-student:…
    Q_INVOKABLE void openUrl(const QUrl &url);

signals:
    void changed();
    void busyChanged();
    void scanErrorChanged();
    void keyboardHeightChanged();
    void safeAreaChanged();
    //! A short message for the user (e.g. "coupled with …", "could not update")
    void notice(const QString &text);

private:
    explicit StudentApp(QObject *parent = nullptr);
    struct Teacher
    {
        QString id;
        Qupil::Booklet::Link link;
        QJsonObject booklet;
        QDateTime fetched;
        QString error;             // "", "offline", "gone", "key", "newer"
        bool legacy = false;       // Nextcloud before 29: old public WebDAV (found again on each load)
        bool demo = false;
        QJsonObject feedback;      // taskId -> {practiced, works, done, at}
        QJsonArray questions;      // {id, text, task, at}
        qint64 changedAt = 0;      // last local change of feedback or questions
        qint64 sentAt = 0;         // last successful upload (the state of changedAt then)
        qint64 sentState = 0;
        qint64 since = 0;          // when this device took the booklet (0: before the app said so)
        bool said = false;         // the teacher was told that this device has the booklet
        qint64 left = 0;           // removed from this device: only the goodbye is still to send
        bool sending = false;      // an upload is on its way (not kept)
    };

    void load();
    void save() const;
    Teacher *find(const QString &id);
    Teacher *currentTeacher();
    const Teacher *currentTeacher() const;
    void fetch(Teacher &teacher, bool legacy = false);
    void upload(Teacher &teacher);
    void scheduleUpload();
    void sendPending();
    bool canUpload(const Teacher &teacher) const;
    void touch(Teacher &teacher);
    void setBusy(bool on);
    void setScanError(const QString &error);
    QJsonObject feedbackFor(const Teacher &teacher, const QString &taskId) const;
    void updateFeedback(const QString &taskId, const QString &field, const QJsonValue &value);
    static QJsonObject demoBooklet();

    QList<Teacher> m_teachers;
    QList<Teacher> m_leaving;  // removed here, the teacher not told yet (kept for a month at most)
    int m_current = 0;
    int m_busy = 0;
    QString m_scanError;
    qreal m_keyboardHeight = 0;
    qreal m_safeTop = 0;
    qreal m_safeBottom = 0;
    QUrl m_lastUrl;            // a link may be handed over twice (IncomingLinks)
    QElapsedTimer m_lastUrlAt;
    QString m_deviceId;
    QNetworkAccessManager *m_net = nullptr;
    QTimer m_uploadTimer;
};
