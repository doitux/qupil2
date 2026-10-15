// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QByteArray>
#include <QList>
#include <QString>
#include <QStringList>

#include <optional>

#include "Store.h"

namespace Qupil {

/*!
    A folder on a server (Nextcloud via WebDAV) or, in tests, in memory.
    Calls block; FolderSync runs in a worker thread.
*/
class RemoteFolder
{
public:
    virtual ~RemoteFolder() = default;
    //! Creates the folder (and does nothing if it exists)
    virtual bool makeFolder(const QString &path) = 0;
    //! Names of files and folders (folders end with "/"); nullopt on error, empty if missing
    virtual std::optional<QStringList> list(const QString &path) = 0;
    //! nullopt on error or if the file does not exist (see lastErrorWasNotFound)
    virtual std::optional<QByteArray> read(const QString &path) = 0;
    virtual bool write(const QString &path, const QByteArray &data) = 0;
    virtual QString lastError() const = 0;
    virtual bool lastErrorWasNotFound() const = 0;
};

/*!
    Sync through a shared folder, end-to-end encrypted with the sync password.

    Qupil/
      key.json                         salt and a check value for the password
      devices/<device>.bin             name and last sync of every device
      changes/<device>/<first>-<last>.bin   the change log of each device in batches

    Each device only writes its own folder and reads all others, so nothing is
    ever overwritten by two devices. Everything except key.json is encrypted.
*/
class FolderSync
{
public:
    enum class Status {
        Ok,
        NoPassword,       //!< the folder is set up, the password is needed on this device
        WrongPassword,
        Failed            //!< network or server problem, see error()
    };

    struct Stats
    {
        int uploaded = 0;
        int downloaded = 0;
        int applied = 0;
        QString maxHlc;
        QStringList devices;   //!< names of the other devices
    };

    FolderSync(Store *store, RemoteFolder *remote, const QString &root = QStringLiteral("Qupil"));

    //! Is the folder set up already (key.json exists)? nullopt on error
    std::optional<bool> isSetUp();
    /*!
        First device: creates key.json for this password. Other devices: checks
        the password. Returns the key to keep on this device (empty on failure).
    */
    QByteArray unlock(const QString &password, Status *status);
    //! Uploads this device's new changes and applies those of the others
    Status run(const QByteArray &key, const QString &deviceName, Stats *stats);

    QString error() const { return m_error; }

    //! Changes per file (also the unit of upload)
    static constexpr int BatchSize = 2000;

private:
    bool fail(const QString &message);
    QString path(const QString &relative) const;

    Store *m_store;
    RemoteFolder *m_remote;
    QString m_root;
    QString m_error;
};

//! Changes as compact, compressed bytes (sync files and Wi-Fi messages)
namespace SyncFormat {
QByteArray encode(const QList<Change> &changes);
std::optional<QList<Change>> decode(const QByteArray &data);
} // namespace SyncFormat

} // namespace Qupil
