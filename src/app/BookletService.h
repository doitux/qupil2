// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QObject>
#include <QSet>
#include <QTimer>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

class AppController;
class DataService;
class SyncController;
class QJSEngine;
class QQmlEngine;

/*!
    The practice book in the teacher's Nextcloud, available in QML as "Booklet"
    (docs/student-app.md).

    Coupling a student makes a folder "Qupil Übeheft/<random>" in the Nextcloud of the
    sync, shares it by a link with a password (the student app may read and add files),
    and keeps folder, link and a new key in the coupling record (it syncs to the
    teacher's other devices). The QR code holds all of it.

    After each sync and shortly after changes, every coupled student's booklet is written
    again (heft.qupil, sealed) and the student's answers are collected (fb-*.qupil): into
    the tasks and the coupling, older answer files deleted.
*/
class BookletService : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Booklet)
    QML_SINGLETON

    //! The Nextcloud of the sync is connected (the practice book needs it)
    Q_PROPERTY(bool available READ available NOTIFY availableChanged FINAL)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged FINAL)

public:
    BookletService(AppController *app, DataService *data, SyncController *sync, QObject *parent);
    ~BookletService() override;

    static BookletService *instance();
    static BookletService *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    bool available() const;
    bool busy() const { return !m_running.isEmpty(); }

    //! Couples the student: folder, link, key; coupled(studentId, error) when done ("" = fine)
    Q_INVOKABLE void couple(const QString &studentId);
    //! Ends the coupling: the link goes (the student app says so), the folder is deleted
    Q_INVOKABLE void endCoupling(const QString &studentId);
    //! The link of the QR code (https://qupil.de/u#…), "" when not coupled
    Q_INVOKABLE QString link(const QString &studentId) const;
    //! The QR code of the link for a Shape in QML: { path (SVG, one unit per module), size }
    Q_INVOKABLE QVariantMap qrCode(const QString &studentId) const;
    //! Writes the booklets and collects the answers now (otherwise after each sync)
    Q_INVOKABLE void publishAll();

signals:
    void availableChanged();
    void busyChanged();
    void coupled(const QString &studentId, const QString &error);
    void ended(const QString &studentId, const QString &error);
    //! New answers or questions came in for these students
    void answersArrived(const QStringList &studentIds);

private:
    void setRunning(const QString &job, bool running);

    AppController *m_app;
    DataService *m_data;
    SyncController *m_sync;
    QSet<QString> m_running;
    QTimer m_soon;
    bool m_publishing = false;
};
