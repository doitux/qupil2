// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QDate>
#include <QDateTime>
#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

class QQmlEngine;
class QJSEngine;

namespace Qupil {
class Store;
}

/*!
    Everything the screens read and change, available in QML as the singleton "Data".

    Reads return plain lists/maps for QML. Bind them to "revision" so they
    re-evaluate after any change, also changes that come in from other devices:
        property var lessons: { Data.revision; return Data.lessonsOn(day) }
    All writes go through the Store, so they are recorded for sync.

    Group lessons: notes and pieces exist once per student; copies made
    "for all students" share a group_key and are edited and deleted together
    when forAll is true.
*/
class DataService : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Data)
    QML_SINGLETON

    Q_PROPERTY(int revision READ revision NOTIFY changed FINAL)

public:
    DataService(Qupil::Store *store, QObject *parent);
    ~DataService() override;

    static DataService *instance();
    static DataService *create(QQmlEngine *qmlEngine, QJSEngine *jsEngine);

    int revision() const { return m_revision; }

    // ------------------------------------------------------------ lessons and schedule
    //! Active lessons with a fixed time on a weekday (0 = Monday), with their students
    Q_INVOKABLE QVariantList lessonsOn(int weekday) const;
    Q_INVOKABLE QVariantList irregularLessons() const;
    //! Number of lessons per weekday, Monday first
    Q_INVOKABLE QVariantList lessonCountPerWeekday() const;
    Q_INVOKABLE QVariantMap lesson(const QString &id) const;
    //! Name generated from type, time, location and students (Qupil 1.x rule)
    Q_INVOKABLE QString autoLessonName(int kind, bool irregular, const QString &start, const QString &end,
                                       const QString &location, const QStringList &studentIds) const;
    /*!
        Creates (empty id) or saves a lesson incl. its students. Students who leave
        keep their notes and pieces under the old lesson name. Returns an error text
        or an empty string; the new id is in lastId.
    */
    Q_INVOKABLE QString saveLesson(const QString &id, const QVariantMap &fields, const QStringList &studentIds);
    Q_INVOKABLE bool deleteLesson(const QString &id);
    Q_PROPERTY(QString lastId READ lastId FINAL)
    QString lastId() const { return m_lastId; }

    // ------------------------------------------------------------ students
    //! All students, sorted by surname; filter matches names and instrument
    Q_INVOKABLE QVariantList students(const QString &filter = {}) const;
    Q_INVOKABLE QVariantMap student(const QString &id) const;
    Q_INVOKABLE QString createStudent(const QVariantMap &fields);
    Q_INVOKABLE bool updateStudent(const QString &id, const QVariantMap &fields);
    //! Removes the student with lessons history, notes, pieces and activities
    Q_INVOKABLE bool deleteStudent(const QString &id);

    // ------------------------------------------------------------ notes and pieces
    Q_INVOKABLE QVariantList notes(const QString &membershipId) const;
    Q_INVOKABLE QString addNote(const QString &membershipId, const QString &date, const QString &html, bool forAll);
    Q_INVOKABLE bool updateNote(const QString &id, const QString &date, const QString &html, bool forAll);
    Q_INVOKABLE bool deleteNote(const QString &id, bool forAll);

    //! stateFilter -1 = all
    Q_INVOKABLE QVariantList pieces(const QString &membershipId, int stateFilter = -1) const;
    //! Count per state (index = state) plus "all" at index 5
    Q_INVOKABLE QVariantList pieceCounts(const QString &membershipId) const;
    Q_INVOKABLE QString addPiece(const QString &membershipId, const QVariantMap &fields, bool forAll);
    Q_INVOKABLE bool updatePiece(const QString &id, const QVariantMap &fields, bool forAll);
    Q_INVOKABLE bool deletePiece(const QString &id, bool forAll);
    //! How many students currently share notes/pieces of this membership's lesson
    Q_INVOKABLE int groupSize(const QString &membershipId) const;
    Q_INVOKABLE QStringList composers() const;

    Q_INVOKABLE QVariantList activities(const QString &studentId) const;
    Q_INVOKABLE QVariantList loans(const QString &studentId) const;

    // ------------------------------------------------------------ events
    Q_INVOKABLE QVariantList events() const;
    Q_INVOKABLE QVariantMap event(const QString &id) const;
    //! Pieces "ready to perform" not yet in this event, one entry per group piece
    Q_INVOKABLE QVariantList readyPieces(const QString &eventId) const;
    Q_INVOKABLE QString saveEvent(const QString &id, const QVariantMap &fields);
    Q_INVOKABLE bool deleteEvent(const QString &id);
    Q_INVOKABLE bool addToProgramme(const QString &eventId, const QStringList &pieceIds);
    Q_INVOKABLE bool addExternal(const QString &eventId, const QVariantMap &fields);
    Q_INVOKABLE bool removeFromProgramme(const QString &itemId);
    Q_INVOKABLE bool reorderProgramme(const QString &eventId, const QStringList &itemIds);
    /*!
        Planned (0) or done (1). When done, optionally enters the event as an
        activity for every performer and marks the performed pieces finished.
    */
    Q_INVOKABLE bool setEventState(const QString &id, int state, bool addActivities, bool finishPieces);

    // ------------------------------------------------------------ notices
    //! Birthdays from today on (days ahead), with age and days until
    Q_INVOKABLE QVariantList birthdays(int daysAhead) const;
    //! mode 0: shown on the start page; 1: every lesson; 2: lessons of one student
    Q_INVOKABLE QVariantList reminders() const;
    Q_INVOKABLE QVariantList recitalCandidates() const;

    // ------------------------------------------------------------ settings and lists
    Q_INVOKABLE QVariant setting(const QString &key, const QVariant &fallback = {}) const;
    Q_INVOKABLE bool setSetting(const QString &key, const QVariant &value);
    //! JSON list settings: genres, locations, instrument_sizes
    Q_INVOKABLE QStringList listSetting(const QString &key) const;
    Q_INVOKABLE QStringList instrumentNames() const;
    //! Default lists in the UI language if this database has none yet
    Q_INVOKABLE void ensureDefaults();

    // ------------------------------------------------------------ generic
    Q_INVOKABLE QVariantMap get(const QString &entity, const QString &id) const;
    //! Membership with its lesson and the students currently in that lesson
    Q_INVOKABLE QVariantMap membershipInfo(const QString &membershipId) const;
    Q_INVOKABLE QString create(const QString &entity, const QVariantMap &fields);
    Q_INVOKABLE bool update(const QString &entity, const QString &id, const QVariantMap &fields);
    Q_INVOKABLE bool remove(const QString &entity, const QString &id);

    // ------------------------------------------------------------ text helpers
    //! Note HTML for display: only the body, colours readable on the current background
    Q_INVOKABLE QString noteHtml(const QString &html, bool dark) const;
    Q_INVOKABLE QString plainText(const QString &html, int maxLength = 0) const;
    //! "Wed, 07.10.2026" style date in the UI language
    Q_INVOKABLE QString formatDate(const QString &iso, bool withWeekday = false) const;
    Q_INVOKABLE QString today() const;
    //! "Wednesday, 14 October" in the UI language
    Q_INVOKABLE QString dayHeading(const QDateTime &when) const;
    Q_INVOKABLE int ageOn(const QString &birthday, const QString &onDate = {}) const;

signals:
    void changed();

private:
    QVariantList rows(const QString &sql, const QVariantList &binds = {}) const;
    QVariant scalar(const QString &sql, const QVariantList &binds = {}) const;
    QVariantList activeMembers(const QString &lessonId) const;
    QVariantMap lessonSummary(const QVariantMap &row, bool withDetails) const;
    QStringList groupCopies(const QString &entity, const QString &id) const;
    QStringList activeMembershipsOfSameLesson(const QString &membershipId) const;
    QString performers(const QString &groupKey) const;
    bool endMembership(const QString &membershipId, const QString &lessonName);
    void bump();

    Qupil::Store *m_store;
    int m_revision = 0;
    QString m_lastId;
};
