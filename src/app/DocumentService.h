// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QList>
#include <QObject>
#include <QPair>
#include <QUrl>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

class DataService;
class QThread;
class QTimer;
class QJSEngine;
class QQmlEngine;

/*!
    Documents to print or share (PDF) and the archive records, available in
    QML as the singleton "Docs".

    The documents are HTML (as in Qupil 1.x), laid out with QTextDocument and
    written as A4 PDF into the app's documents folder; the system viewer opens
    them, from there they can be printed, shared or saved.
*/
class DocumentService : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Docs)
    QML_SINGLETON

public:
    DocumentService(DataService *data, QObject *parent);
    ~DocumentService() override;

    static DocumentService *instance();
    static DocumentService *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    // ------------------------------------------------------------ PDF (written and opened)
    /*!
        Lessons of one day with the latest notes and space for handwritten ones.
        dateIso empty: the irregular lessons.
    */
    Q_INVOKABLE QUrl dayOverview(const QString &dateIso, int notesPerLesson, int spaceLines);
    Q_INVOKABLE QUrl timetable();
    Q_INVOKABLE QUrl rentalList();
    Q_INVOKABLE QUrl programme(const QString &eventId);
    //! entity: "student_archive" or "event_archive"
    Q_INVOKABLE QUrl archiveEntry(const QString &entity, const QString &id);
    /*!
        An archive entry to show: unpacked, and an entry of Qupil 1.x in the
        compact layout (it is then saved like that, see convertLegacyArchives).
    */
    Q_INVOKABLE QString archiveHtml(const QString &entity, const QString &id);

    //! Weekdays with their next date for the daily overview: { label, date, weekday }
    Q_INVOKABLE QVariantList dayChoices() const;

    // ------------------------------------------------------------ archive
    //! Read-only record of everything known about the student; returns the archive id
    Q_INVOKABLE QString archiveStudent(const QString &studentId);
    //! The programme as it is now; returns the archive id
    Q_INVOKABLE QString archiveEvent(const QString &eventId);
    /*!
        Archive entries of Qupil 1.x get the compact layout, one after the other
        in a worker thread (about a second each). In place, in the table and in
        the change log: each device converts its own entries (also those it gets
        later from devices with older versions), nothing is sent.
    */
    void convertLegacyArchives();
    /*!
        Keeps the archive compact from now on: converts (see above) shortly after
        archive entries changed, e.g. after a sync brought entries of an older
        version, and packs entries that arrived unpacked.
    */
    void setArchiveUpkeep(bool on);
    //! A Qupil 1.x archive document in the compact layout (a whole page)
    static QString compactLegacyArchive(const QString &legacyHtml);

    // HTML (public for tests and the archive)
    QString dayOverviewHtml(const QString &dateIso, int notesPerLesson, int spaceLines) const;
    QString timetableHtml() const;
    QString rentalListHtml() const;
    QString programmeHtml(const QString &eventId) const;
    QString studentArchiveHtml(const QString &studentId) const;

    //! Writes the HTML as PDF (into the documents folder unless one is given); empty URL on failure
    QUrl writePdf(const QString &html, const QString &baseName, bool landscape, const QString &folder = {}) const;
    //! Android: copies the PDF of saveRequested to the chosen place (also content:// URIs)
    Q_INVOKABLE bool saveCopy(const QUrl &file, const QUrl &target, const QString &targetName);
    //! Android: the save dialog was cancelled
    Q_INVOKABLE void discard(const QUrl &file);

signals:
    //! Could not write or open a document
    void failed(const QString &message);
    //! iOS: the document was saved where the user chose
    void exported(const QString &message);
    /*!
        Android: the PDF is ready in a temporary file; the user chooses where it
        goes (the system's save dialog in QML), then saveCopy() or discard().
    */
    void saveRequested(const QUrl &file, const QString &name);

private:
    QUrl writeAndOpen(const QString &html, const QString &baseName, bool landscape);
    void convertNext();

    DataService *m_data;
    QList<QPair<QString, QString>> m_legacyArchives;   // entity, id still to convert
    QThread *m_converter = nullptr;
    QTimer *m_archiveScan = nullptr;
    int m_converted = 0;   // in this run
};
