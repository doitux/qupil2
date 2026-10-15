// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QObject>
#include <QUrl>
#include <QVariantList>
#include <QtQml/qqmlregistration.h>

class DataService;
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

    //! Weekdays with their next date for the daily overview: { label, date, weekday }
    Q_INVOKABLE QVariantList dayChoices() const;

    // ------------------------------------------------------------ archive
    //! Read-only record of everything known about the student; returns the archive id
    Q_INVOKABLE QString archiveStudent(const QString &studentId);
    //! The programme as it is now; returns the archive id
    Q_INVOKABLE QString archiveEvent(const QString &eventId);

    // HTML (public for tests and the archive)
    QString dayOverviewHtml(const QString &dateIso, int notesPerLesson, int spaceLines) const;
    QString timetableHtml() const;
    QString rentalListHtml() const;
    QString programmeHtml(const QString &eventId) const;
    QString studentArchiveHtml(const QString &studentId) const;

    //! Writes the HTML as PDF (into the documents folder unless one is given); empty URL on failure
    QUrl writePdf(const QString &html, const QString &baseName, bool landscape, const QString &folder = {}) const;

signals:
    //! Could not write or open a document
    void failed(const QString &message);

private:
    QUrl writeAndOpen(const QString &html, const QString &baseName, bool landscape);

    DataService *m_data;
};
