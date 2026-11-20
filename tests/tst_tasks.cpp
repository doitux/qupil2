// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
//
// The practice book's data in the teacher's Qupil: tasks of a lesson, open tasks moving on
// to the next lesson, group copies, the student's feedback, the coupling. Invented data.

#include <QTemporaryDir>
#include <QTest>

#include "Clock.h"
#include "DataService.h"
#include "Database.h"
#include "Store.h"

using namespace Qt::StringLiterals;
using namespace Qupil;

class TestTasks : public QObject
{
    Q_OBJECT

    QTemporaryDir m_dir;
    Database *m_db = nullptr;
    Store *m_store = nullptr;
    DataService *m_data = nullptr;
    QString m_student, m_lesson, m_membership;

    QString addStudent(const QString &forename)
    {
        return m_store->create(u"student"_s, { { u"forename"_s, forename }, { u"surname"_s, u"Test"_s } });
    }
    QString join(const QString &lesson, const QString &student)
    {
        return m_store->create(u"membership"_s, { { u"lesson_id"_s, lesson }, { u"student_id"_s, student },
                                                  { u"start_date"_s, u"2026-09-01"_s } });
    }
    static QVariantMap task(const QString &title, const QStringList &hints = {}, int kind = 0, const QString &id = {})
    {
        return { { u"id"_s, id }, { u"title"_s, title }, { u"hints"_s, hints }, { u"kind"_s, kind } };
    }

private slots:
    void init()
    {
        delete m_data;
        delete m_store;
        delete m_db;
        QFile::remove(m_dir.filePath(u"q.db"_s));
        m_db = new Database;
        QVERIFY(m_db->open(m_dir.filePath(u"q.db"_s)));
        m_store = new Store(m_db);
        m_data = new DataService(m_store, nullptr);
        m_student = addStudent(u"Mia"_s);
        m_lesson = m_store->create(u"lesson"_s, { { u"kind"_s, 1 }, { u"weekday"_s, 1 }, { u"start_time"_s, u"15:00"_s },
                                                  { u"end_time"_s, u"15:45"_s } });
        m_membership = join(m_lesson, m_student);
    }
    void cleanupTestCase()
    {
        delete m_data;
        delete m_store;
        delete m_db;
    }

    void saveAndEdit()
    {
        QVERIFY(m_data->saveTasks(m_membership, u"2026-11-17"_s,
                                  { task(u"Bourrée Takt 17–24 langsam"_s, { u"Metronom 60"_s, u"  Bogen   locker  "_s, u""_s }),
                                    task(u"Noten Suzuki 3 besorgen"_s, {}, 1) }, false));
        QVariantList list = m_data->tasksOn(m_membership, u"2026-11-17"_s);
        QCOMPARE(list.size(), 2);
        QVariantMap first = list.at(0).toMap();
        QCOMPARE(first.value(u"title"_s).toString(), u"Bourrée Takt 17–24 langsam"_s);
        QCOMPARE(first.value(u"hints"_s).toStringList(), (QStringList { u"Metronom 60"_s, u"Bogen locker"_s }));
        QCOMPARE(first.value(u"since"_s).toString(), u"2026-11-17"_s);
        QCOMPARE(list.at(1).toMap().value(u"kind"_s).toInt(), 1);

        // Edited: the second goes, a new one comes first; empty ones are not saved
        const QString id = first.value(u"id"_s).toString();
        QVERIFY(m_data->saveTasks(m_membership, u"2026-11-17"_s,
                                  { task(u"Tonleiter G-Dur"_s), task(u"Bourrée"_s, { u"Metronom 66"_s }, 0, id), task(u""_s) },
                                  false));
        list = m_data->tasksOn(m_membership, u"2026-11-17"_s);
        QCOMPARE(list.size(), 2);
        QCOMPARE(list.at(0).toMap().value(u"title"_s).toString(), u"Tonleiter G-Dur"_s);
        QCOMPARE(list.at(1).toMap().value(u"id"_s).toString(), id);   // the same record: feedback stays with it
        QCOMPARE(list.at(1).toMap().value(u"hints"_s).toStringList(), QStringList { u"Metronom 66"_s });

        // Only hints, no title: the first line becomes the title
        QVERIFY(m_data->saveTasks(m_membership, u"2026-11-24"_s, { task(u""_s, { u"Etüde Nr. 3"_s, u"langsam"_s }) }, false));
        const QVariantMap t = m_data->tasksOn(m_membership, u"2026-11-24"_s).value(0).toMap();
        QCOMPARE(t.value(u"title"_s).toString(), u"Etüde Nr. 3"_s);
        QCOMPARE(t.value(u"hints"_s).toStringList(), QStringList { u"langsam"_s });
        // Newest lesson first
        QCOMPARE(m_data->tasks(m_membership).value(0).toMap().value(u"date"_s).toString(), u"2026-11-24"_s);
    }

    void openTasksMoveOn()
    {
        QVERIFY(m_data->saveTasks(m_membership, u"2026-11-10"_s, { task(u"Tonleiter"_s), task(u"Noten besorgen"_s, {}, 1) }, false));
        const QVariantList before = m_data->tasksOn(m_membership, u"2026-11-10"_s);
        const QString scale = before.at(0).toMap().value(u"id"_s).toString();
        const QString music = before.at(1).toMap().value(u"id"_s).toString();
        QVERIFY(m_data->closeTask(music, true));

        // The next lesson: only the open one is offered
        const QVariantList open = m_data->openTasksBefore(m_membership, u"2026-11-17"_s);
        QCOMPARE(open.size(), 1);
        QCOMPARE(open.at(0).toMap().value(u"id"_s).toString(), scale);

        // Kept in the next lesson: it moves there and remembers where it came from
        QVERIFY(m_data->saveTasks(m_membership, u"2026-11-17"_s, { task(u"Tonleiter"_s, {}, 0, scale), task(u"Etüde"_s) }, false));
        QCOMPARE(m_data->tasksOn(m_membership, u"2026-11-10"_s).size(), 1);   // the closed one stays
        const QVariantMap moved = m_data->tasksOn(m_membership, u"2026-11-17"_s).value(0).toMap();
        QCOMPARE(moved.value(u"id"_s).toString(), scale);
        QCOMPARE(moved.value(u"since"_s).toString(), u"2026-11-10"_s);
        // Moved once more: still the first lesson
        QVERIFY(m_data->saveTasks(m_membership, u"2026-11-24"_s, { task(u"Tonleiter"_s, {}, 0, scale) }, false));
        QCOMPARE(m_data->tasksOn(m_membership, u"2026-11-24"_s).value(0).toMap().value(u"since"_s).toString(), u"2026-11-10"_s);
        QCOMPARE(m_data->openTasksBefore(m_membership, u"2026-12-01"_s).size(), 2);   // Etüde and Tonleiter

        // Closed in the editor
        QVERIFY(m_data->saveTasks(m_membership, u"2026-11-24"_s,
                                  { QVariantMap { { u"id"_s, scale }, { u"title"_s, u"Tonleiter"_s }, { u"closed"_s, true } } }, false));
        QVERIFY(!m_data->tasksOn(m_membership, u"2026-11-24"_s).value(0).toMap().value(u"closed"_s).toString().isEmpty());
        QVERIFY(m_data->closeTask(scale, false));
        QVERIFY(m_data->tasksOn(m_membership, u"2026-11-24"_s).value(0).toMap().value(u"closed"_s).toString().isEmpty());
    }

    void groupCopies()
    {
        const QString group = m_store->create(u"lesson"_s, { { u"kind"_s, 2 }, { u"weekday"_s, 3 }, { u"start_time"_s, u"16:00"_s } });
        const QString a = join(group, addStudent(u"Ben"_s));
        const QString b = join(group, addStudent(u"Cleo"_s));
        QVERIFY(m_data->saveTasks(a, u"2026-11-19"_s, { task(u"Kanon"_s, { u"Einsatz zählen"_s }) }, true));
        const QVariantMap ta = m_data->tasksOn(a, u"2026-11-19"_s).value(0).toMap();
        const QVariantMap tb = m_data->tasksOn(b, u"2026-11-19"_s).value(0).toMap();
        QVERIFY(ta.value(u"id"_s) != tb.value(u"id"_s));   // each their own (own feedback)
        QCOMPARE(ta.value(u"shared"_s).toInt(), 2);
        // Changed for all
        QVERIFY(m_data->saveTasks(a, u"2026-11-19"_s, { task(u"Kanon in D"_s, {}, 0, ta.value(u"id"_s).toString()) }, true));
        QCOMPARE(m_data->tasksOn(b, u"2026-11-19"_s).value(0).toMap().value(u"title"_s).toString(), u"Kanon in D"_s);
        // Deleted for one only
        QVERIFY(m_data->deleteTask(tb.value(u"id"_s).toString(), false));
        QCOMPARE(m_data->tasksOn(b, u"2026-11-19"_s).size(), 0);
        QCOMPARE(m_data->tasksOn(a, u"2026-11-19"_s).size(), 1);
    }

    void feedback()
    {
        QVERIFY(m_data->saveTasks(m_membership, u"2026-11-17"_s, { task(u"Tonleiter"_s) }, false));
        const QString id = m_data->tasksOn(m_membership, u"2026-11-17"_s).value(0).toMap().value(u"id"_s).toString();
        const QString other = addStudent(u"Ole"_s);
        const QString otherMembership = join(m_lesson, other);
        QVERIFY(m_data->saveTasks(otherMembership, u"2026-11-17"_s, { task(u"Fremd"_s) }, false));
        const QString foreign = m_data->tasksOn(otherMembership, u"2026-11-17"_s).value(0).toMap().value(u"id"_s).toString();

        // Not coupled: nothing taken
        QVERIFY(!m_data->applyFeedback(m_student, { { id, QVariantMap { { u"practiced"_s, 3 } } } }, {}));
        QVERIFY(m_store->create(u"coupling"_s, { { u"student_id"_s, m_student }, { u"created"_s, u"2026-11-01"_s } }) != QString());
        QVERIFY(m_data->isCoupled(m_student));

        const QVariantMap answer { { u"practiced"_s, 3 }, { u"works"_s, 2 }, { u"done"_s, false }, { u"at"_s, 2000.0 } };
        QVERIFY(m_data->applyFeedback(m_student, { { id, answer }, { foreign, answer } },
                                      { QVariantMap { { u"id"_s, u"q1"_s }, { u"text"_s, u"Wie schnell?"_s } } }));
        QCOMPARE(m_data->tasksOn(m_membership, u"2026-11-17"_s).value(0).toMap().value(u"feedback"_s).toMap().value(u"practiced"_s).toInt(), 3);
        // Another student's task is not touched through this coupling
        QVERIFY(m_data->tasksOn(otherMembership, u"2026-11-17"_s).value(0).toMap().value(u"feedback"_s).toMap().isEmpty());
        QVERIFY(m_data->coupling(m_student).value(u"questions"_s).toString().contains(u"Wie schnell?"_s));
        // An older answer does not win
        QVERIFY(m_data->applyFeedback(m_student, { { id, QVariantMap { { u"practiced"_s, 1 }, { u"at"_s, 1000.0 } } } }, {}));
        QCOMPARE(m_data->tasksOn(m_membership, u"2026-11-17"_s).value(0).toMap().value(u"feedback"_s).toMap().value(u"practiced"_s).toInt(), 3);

        // Ended: no longer coupled
        QVERIFY(m_store->update(u"coupling"_s, m_data->coupling(m_student).value(u"id"_s).toString(), { { u"ended"_s, u"2026-11-20"_s } }));
        QVERIFY(!m_data->isCoupled(m_student));
    }
};

QTEST_MAIN(TestTasks)
#include "tst_tasks.moc"
