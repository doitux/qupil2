// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QObject>
#include <QString>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

#include <memory>

class QQmlEngine;
class QJSEngine;

namespace Qupil {
class Database;
class Store;
}

/*!
    Owns the user's database and everything around its lifecycle:
    first start (take over Qupil 1.x data, restore a backup or start empty),
    backups (manual and automatic) and replacing the data at runtime.

    Available in QML as the singleton "App". Create exactly one instance in
    main() before the QML engine loads.
*/
class AppController : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(App)
    QML_SINGLETON

    Q_PROPERTY(Phase phase READ phase NOTIFY phaseChanged FINAL)
    //! Why the existing data could not be opened (shown on the setup screen)
    Q_PROPERTY(QString startupError READ startupError NOTIFY phaseChanged FINAL)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged FINAL)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged FINAL)
    //! Step key of a running import: lookup, students, lessons, pieces, notes, …
    Q_PROPERTY(QString progressStep READ progressStep NOTIFY progressChanged FINAL)
    //! Qupil 1.x data folders found on this computer: [{ path, url, students }]
    Q_PROPERTY(QVariantList legacyFolders READ legacyFolders NOTIFY legacyFoldersChanged FINAL)
    //! Summary of the last import or restore, empty when dismissed
    Q_PROPERTY(QVariantMap lastTakeOver READ lastTakeOver NOTIFY lastTakeOverChanged FINAL)
    Q_PROPERTY(QVariantMap info READ info NOTIFY infoChanged FINAL)
    //! Desktop systems can pick folders and show hidden 1.x data folders
    Q_PROPERTY(bool desktop READ desktop CONSTANT FINAL)
    //! iOS has no "save as" dialog: backups go to the app's Documents (Files app)
    Q_PROPERTY(bool saveDialogAvailable READ saveDialogAvailable CONSTANT FINAL)
    Q_PROPERTY(QUrl backupFolder READ backupFolder CONSTANT FINAL)

public:
    enum Phase {
        Setup,  //!< no data yet (or unreadable): welcome / take-over screen
        Ready   //!< database open, normal UI
    };
    Q_ENUM(Phase)

    //! dataDir holds qupil.db and the backups folder
    AppController(const QString &dataDir, QObject *parent);
    ~AppController() override;

    static AppController *instance();
    static AppController *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    Phase phase() const { return m_phase; }
    QString startupError() const { return m_startupError; }
    bool busy() const { return m_busy; }
    int progress() const { return m_progress; }
    QString progressStep() const { return m_progressStep; }
    QVariantList legacyFolders() const { return m_legacyFolders; }
    QVariantMap lastTakeOver() const { return m_lastTakeOver; }
    QVariantMap info() const;
    bool desktop() const;
    bool saveDialogAvailable() const;
    QUrl backupFolder() const;

    Qupil::Database *database() const { return m_db.get(); }
    Qupil::Store *store() const { return m_store.get(); }
    QString databasePath() const;

    /*!
        Takes over data from a Qupil 1.x data folder, a 1.x database file or a
        backup (1.x or 2). Runs in the background; existing data is backed up
        first and then replaced. Ends with lastTakeOverChanged or takeOverFailed.
    */
    Q_INVOKABLE void takeOver(const QUrl &source);
    //! Starts with an empty database (an unreadable old file is kept as a backup)
    Q_INVOKABLE bool startFresh();
    //! Takes over invented sample data (in the UI language) to try Qupil out
    Q_INVOKABLE void loadDemo();
    //! Leaves the setup screen after the import summary
    Q_INVOKABLE void finishSetup();
    Q_INVOKABLE void dismissTakeOver();

    //! "Qupil-Backup-2026-10-15.db"
    Q_INVOKABLE QString suggestedBackupName() const;
    /*!
        Writes a backup. With an empty target it goes to the documents folder
        (on iOS visible in the Files app). Returns { ok, file, message }.
    */
    Q_INVOKABLE QVariantMap createBackup(const QUrl &target = {});
    //! Automatic daily backup into the backups folder, keeps the last 14
    Q_INVOKABLE bool autoBackupIfDue();

    //! Readable path for a picked URL (content:// URIs show their file name)
    Q_INVOKABLE QString displayPath(const QUrl &url) const;
    //! Looks again for Qupil 1.x data folders on this computer
    Q_INVOKABLE void refreshLegacyFolders();

signals:
    void phaseChanged();
    void busyChanged();
    void progressChanged();
    void legacyFoldersChanged();
    void lastTakeOverChanged();
    void infoChanged();
    void takeOverFailed(const QString &message);
    //! The whole database was replaced (import / restore): reload everything
    void dataReplaced();

private:
    struct Outcome;
    static Outcome runTakeOver(AppController *self, const QString &source, const QString &workDir);
    void finishTakeOver(const Outcome &outcome);

    bool openDatabase(QString *error);
    bool safetyBackup(const QString &reason);
    void rotate(const QString &prefix, int keep);
    void setPhase(Phase phase);
    void setBusy(bool busy);
    void setProgress(int percent, const QString &step);

    QString m_dataDir;
    std::unique_ptr<Qupil::Database> m_db;
    std::unique_ptr<Qupil::Store> m_store;
    Phase m_phase = Setup;
    QString m_startupError;
    bool m_busy = false;
    int m_progress = 0;
    QString m_progressStep;
    QVariantList m_legacyFolders;
    QVariantMap m_lastTakeOver;
    bool m_demoPending = false;
};
