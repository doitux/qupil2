// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QObject>
#include <QString>
#include <QUrl>
#include <QVariantList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

#include <functional>
#include <memory>

class QQmlEngine;
class QJSEngine;

namespace Qupil {
class Database;
class Store;
namespace TakeOver {
struct Outcome;
}
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
    //! The data was saved by a newer Qupil: an older copy was started (old shortcut, old folder)
    Q_PROPERTY(bool dataTooNew READ dataTooNew NOTIFY phaseChanged FINAL)
    //! Folder this Qupil was started from (desktop), e.g. an old unpacked ZIP
    Q_PROPERTY(QString appFolder READ appFolder CONSTANT FINAL)
    //! Qupil installed with the Windows setup when this is another copy: { path, version }, else empty
    Q_PROPERTY(QVariantMap installedApp READ installedApp CONSTANT FINAL)
    //! Runs from the Microsoft Store package (MSIX), not from the setup or a folder
    Q_PROPERTY(bool storePackage READ storePackage CONSTANT FINAL)
    //! Store package only: Qupil from the Windows setup is still installed: { version }, else empty
    Q_PROPERTY(QVariantMap setupInstallation READ setupInstallation NOTIFY setupInstallationChanged FINAL)
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
    //! Version and build, e.g. "2.0.0 (0c34f48)" – shows which test build is installed
    Q_PROPERTY(QString versionText READ versionText CONSTANT FINAL)
    //! The commit this build was made from ("dev" for local builds)
    Q_PROPERTY(QString build READ build CONSTANT FINAL)
    //! Qt's "save as" dialog works here (not on iOS: see exportPickerAvailable)
    Q_PROPERTY(bool saveDialogAvailable READ saveDialogAvailable CONSTANT FINAL)
    //! The system's own "Save to Files" picker chooses where a backup goes (iOS)
    Q_PROPERTY(bool exportPickerAvailable READ exportPickerAvailable CONSTANT FINAL)
    //! The system's own picker opens files as a copy (iOS): use pickFile instead of Qt's file dialog
    Q_PROPERTY(bool importPickerAvailable READ importPickerAvailable CONSTANT FINAL)
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
    bool dataTooNew() const;
    QString appFolder() const;
    QVariantMap installedApp() const;
    //! Starts the installed Qupil (installedApp) and closes this copy
    Q_INVOKABLE bool startInstalled();
    bool storePackage() const;
    QVariantMap setupInstallation() const;
    //! Runs the uninstaller of the setup version (its data stays); setupRemoval tells the outcome
    Q_INVOKABLE bool removeSetupInstallation();
    //! Screenshot tour only: show the setup screen as if the data came from a newer Qupil
    void simulateNewerData(bool on);
    bool busy() const { return m_busy; }
    int progress() const { return m_progress; }
    QString progressStep() const { return m_progressStep; }
    QVariantList legacyFolders() const { return m_legacyFolders; }
    QVariantMap lastTakeOver() const { return m_lastTakeOver; }
    QVariantMap info() const;
    bool desktop() const;
    QString versionText() const;
    QString build() const;
    //! Puts text on the clipboard (e.g. the version for a bug report)
    Q_INVOKABLE void copyText(const QString &text) const;
    //! Qupil's license (GPL 3 with the app store permission) and those of the bundled parts
    Q_INVOKABLE QString licenseText() const;
    bool saveDialogAvailable() const;
    bool exportPickerAvailable() const;
    bool importPickerAvailable() const;
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
    //! Starts empty and waits for the data of another device (Nextcloud or Wi-Fi)
    Q_INVOKABLE bool startForSync();
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
    /*!
        iOS: writes a backup and lets the user choose where it goes ("Save to
        Files"). The result comes with backupExported.
    */
    Q_INVOKABLE void exportBackup();
    /*!
        iOS: lets the user choose a file with the system's picker, which brings
        a local copy. filePicked(purpose, file, error) follows (file empty when
        cancelled); purpose tells the caller its own pick apart ("backup",
        "legacy", "csv", "sound").
    */
    Q_INVOKABLE void pickFile(const QString &purpose);
    //! Automatic daily backup into the backups folder, keeps the last 14
    Q_INVOKABLE bool autoBackupIfDue();

    //! Readable path for a picked URL (content:// URIs show their file name)
    Q_INVOKABLE QString displayPath(const QUrl &url) const;
    //! Folder of a picked URL as far as it can be told ("Download" for content:// URIs)
    QString displayFolder(const QUrl &url) const;
    //! Looks again for Qupil 1.x data folders on this computer
    Q_INVOKABLE void refreshLegacyFolders();

    // ------------------------------------------------------------ for the sync
    //! A sync writes from its own connection: the data is not replaced meanwhile (counted)
    void beginSync();
    void endSync();
    bool syncRunning() const { return m_syncRunning > 0; }
    /*!
        Replaces the data on this device with an empty database that waits for
        the first sync. The old data is kept as a backup.
    */
    bool replaceWithEmpty();
    //! Empty data that waits for its first sync (no default lists yet)
    bool awaitingSync() const;
    //! The first sync is done: from now on this is normal data
    void syncArrived();
    /*!
        Devices in this database's change log. Another device (or the Nextcloud
        folder) that knows one of them has the same data, synchronised before:
        joining it needs no question. Empty after a restored backup until the user
        decided once how that data goes together with the other devices.
    */
    QStringList syncHistory() const;
    //! The user decided how the data here goes together with another device or the folder
    void joinDecided();

signals:
    void phaseChanged();
    void setupInstallationChanged();
    //! After removeSetupInstallation: the setup version is gone (true) or still there (false)
    void setupRemoval(bool removed);
    void busyChanged();
    void progressChanged();
    void legacyFoldersChanged();
    void lastTakeOverChanged();
    void infoChanged();
    void takeOverFailed(const QString &message);
    //! Result of exportBackup: { ok, cancelled, message }
    void backupExported(const QVariantMap &result);
    //! Result of pickFile: a local copy, nothing (cancelled) or an error message ready to show
    void filePicked(const QString &purpose, const QUrl &file, const QString &error);
    //! The whole database was replaced (import / restore): reload everything
    void dataReplaced();

private:
    void finishTakeOver(const Qupil::TakeOver::Outcome &outcome);
    bool startEmpty(bool awaitingSync);

    bool openDatabase(QString *error);
    int upgradeNotes();
    int upgradeArchives();
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
    int m_dataVersion = 0;   //!< schema version of a file that could not be opened
    bool m_busy = false;
    int m_progress = 0;
    QString m_progressStep;
    QVariantList m_legacyFolders;
    QVariantMap m_lastTakeOver;
    bool m_demoPending = false;
    int m_syncRunning = 0;
    std::function<void()> m_afterSync;   // a take-over that waits for the sync to finish
};
