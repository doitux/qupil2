// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#include "Schema.h"

namespace Qupil {

QStringList EntityDef::fieldNames() const
{
    QStringList names;
    for (const FieldDef &f : fields)
        names << f.name;
    return names;
}

bool EntityDef::hasField(const QString &field) const
{
    if (field == QLatin1String("deleted"))
        return true;
    for (const FieldDef &f : fields)
        if (f.name == field)
            return true;
    return false;
}

namespace Schema {

namespace {

FieldDef text(const char *name, const char *def = "''")
{
    return { QString::fromLatin1(name), QStringLiteral("TEXT"), QString::fromLatin1(def) };
}

FieldDef integer(const char *name, int def = 0)
{
    return { QString::fromLatin1(name), QStringLiteral("INTEGER"), QString::number(def) };
}

QList<EntityDef> buildEntities()
{
    QList<EntityDef> list;

    // Students (1.x: pupil). Dates ISO yyyy-MM-dd or empty.
    list << EntityDef{ QStringLiteral("student"), {
        text("surname"), text("forename"), text("address"), text("email"), text("phone"), text("mobile"),
        text("birthday"), text("notes"),
        // Parents: name, phone and a free note (until schema 3 the occupation, see renamedFields())
        text("father_name"), text("father_phone"), text("father_note"),
        text("mother_name"), text("mother_phone"), text("mother_note"),
        text("first_lesson"), text("instrument"), text("instrument_size"), integer("next_size"),
        integer("rental"), text("rental_desc"), text("rental_since"),
        integer("recital_interval", 5), integer("ensemble_expected", 1),
    }, {} };

    // Archived students: a read-only record of everything that was known (HTML)
    list << EntityDef{ QStringLiteral("student_archive"), {
        text("surname"), text("forename"), text("content"), text("archived_on"),
    }, {} };

    // Lessons. kind: 1 single, 2 group, 3 ensemble. weekday: 0 Monday … 6 Sunday.
    list << EntityDef{ QStringLiteral("lesson"), {
        integer("active", 1), integer("kind", 1), integer("auto_name", 1), text("name"),
        integer("irregular"), integer("weekday"), text("start_time"), text("end_time"), text("location"),
    }, {} };

    // Student in a lesson for a period (1.x: pupilatlesson). Empty end_date = ongoing.
    list << EntityDef{ QStringLiteral("membership"), {
        text("lesson_id"), text("student_id"), text("start_date"), text("end_date"), text("previous_name"),
    }, { QStringLiteral("lesson_id"), QStringLiteral("student_id") } };

    // Pieces. state: 0 planned, 1 in progress, 2 paused, 3 ready for recital, 4 finished.
    // group_key links the copies of one piece in a group lesson.
    list << EntityDef{ QStringLiteral("piece"), {
        text("membership_id"), text("group_key"), text("composer"), text("title"), text("genre"),
        integer("duration"), text("start_date"), text("end_date"), integer("state"),
    }, { QStringLiteral("membership_id"), QStringLiteral("group_key") } };

    // Lesson notes (rich text HTML). group_key links the copies of a group note.
    list << EntityDef{ QStringLiteral("note"), {
        text("membership_id"), text("group_key"), text("date"), text("content"),
    }, { QStringLiteral("membership_id"), QStringLiteral("group_key") } };

    // Activities outside lessons: continuous (weekly, e.g. orchestra) or single (e.g. a recital)
    list << EntityDef{ QStringLiteral("activity"), {
        text("student_id"), integer("continuous"), text("description"), integer("weekday"), text("time"),
        text("date"), text("end_date"), integer("single_kind"), integer("continuous_kind"),
    }, { QStringLiteral("student_id") } };

    // Sheet music library. number: the index written on the music (1.x: smlid, shown as 0007), 0 = none
    list << EntityDef{ QStringLiteral("library_item"), {
        text("author"), text("title"), text("publisher"), text("lent_to"), text("lent_since"), integer("number"),
    }, {} };

    // Events (1.x: recital). state: 0 planned, 1 finished.
    list << EntityDef{ QStringLiteral("event"), {
        text("title"), text("date"), text("time"), text("location"), text("organizer"),
        text("accompanist"), integer("state"),
    }, {} };

    list << EntityDef{ QStringLiteral("event_archive"), {
        text("title"), text("content"),
    }, {} };

    // Programme of an event: either a student's piece or an external contribution
    list << EntityDef{ QStringLiteral("event_item"), {
        text("event_id"), text("piece_id"), text("external_id"), integer("position"),
    }, { QStringLiteral("event_id") } };

    list << EntityDef{ QStringLiteral("external_piece"), {
        text("composer"), text("title"), text("genre"), integer("duration"), text("performer"),
    }, {} };

    // Reminders. mode: 0 at next start, 1 at the student's next lesson.
    list << EntityDef{ QStringLiteral("reminder"), {
        text("text"), integer("mode"), text("student_id"), integer("sound"),
    }, {} };

    // Instruments that can be chosen for students
    list << EntityDef{ QStringLiteral("instrument"), {
        text("name"), text("sizes"), integer("position"),
    }, {} };

    // Settings shared by all devices (device-only settings live in QSettings). id = key.
    list << EntityDef{ QStringLiteral("setting"), {
        text("value"),
    }, {} };

    return list;
}

} // namespace

const QList<EntityDef> &entities()
{
    static const QList<EntityDef> list = buildEntities();
    return list;
}

const EntityDef *entity(const QString &name)
{
    for (const EntityDef &e : entities())
        if (e.name == name)
            return &e;
    return nullptr;
}

const QList<RenamedField> &renamedFields()
{
    // The occupation of the parents sounded too much like collecting data: a note
    // about the parent instead, starting with what the occupation field held
    static const QList<RenamedField> list = {
        { QStringLiteral("student"), QStringLiteral("mother_job"), QStringLiteral("mother_note"), 4 },
        { QStringLiteral("student"), QStringLiteral("father_job"), QStringLiteral("father_note"), 4 },
    };
    return list;
}

QVariantMap currentFields(const QString &entity, const QVariantMap &fields)
{
    QVariantMap out = fields;
    for (const RenamedField &r : renamedFields()) {
        if (r.entity != entity || !out.contains(r.from))
            continue;
        const QVariant value = out.take(r.from);
        if (!out.contains(r.to))
            out.insert(r.to, value);
    }
    return out;
}

QStringList createStatements()
{
    QStringList sql;
    for (const EntityDef &e : entities()) {
        QStringList cols{ QStringLiteral("id TEXT PRIMARY KEY NOT NULL") };
        for (const FieldDef &f : e.fields)
            cols << QStringLiteral("%1 %2 NOT NULL DEFAULT %3").arg(f.name, f.type, f.defaultValue);
        cols << QStringLiteral("deleted INTEGER NOT NULL DEFAULT 0");
        sql << QStringLiteral("CREATE TABLE %1 (%2)").arg(e.name, cols.join(QStringLiteral(", ")));
        for (const QString &idx : e.indexes)
            sql << QStringLiteral("CREATE INDEX idx_%1_%2 ON %1 (%2)").arg(e.name, idx);
    }

    // Local key/value data of this database (device id, clock, schema version, …)
    sql << QStringLiteral("CREATE TABLE meta (key TEXT PRIMARY KEY NOT NULL, value TEXT)");

    // Which change wins: the time and device of the newest value. Field '' is the
    // whole record (when it was created); a field has its own row only when it was
    // changed after that. Devices are numbered locally (table origin).
    sql << QStringLiteral("CREATE TABLE field_clock (entity TEXT NOT NULL, row_id TEXT NOT NULL, field TEXT NOT NULL, "
                          "time INTEGER NOT NULL, origin INTEGER NOT NULL, "
                          "PRIMARY KEY (entity, row_id, field)) WITHOUT ROWID");

    // Every accepted change, local or received: one row per record and write, data
    // holds the new values ({"field": value, …}). (device id, seq) identifies a
    // change on all devices; a row can stand for several older changes in a row
    // (seq is the last of them).
    sql << QStringLiteral("CREATE TABLE change_log (origin INTEGER NOT NULL, seq INTEGER NOT NULL, time INTEGER NOT NULL, "
                          "entity TEXT NOT NULL, row_id TEXT NOT NULL, data TEXT NOT NULL, "
                          "PRIMARY KEY (origin, seq)) WITHOUT ROWID");

    // Local numbers for device ids, so the tables above stay small
    sql << QStringLiteral("CREATE TABLE origin (num INTEGER PRIMARY KEY, id TEXT NOT NULL UNIQUE)");

    // Known devices and sync partners
    sql << QStringLiteral("CREATE TABLE device (id TEXT PRIMARY KEY NOT NULL, name TEXT NOT NULL DEFAULT '', "
                          "platform TEXT NOT NULL DEFAULT '', last_seen TEXT NOT NULL DEFAULT '', "
                          "paired INTEGER NOT NULL DEFAULT 0, secret TEXT NOT NULL DEFAULT '')");
    return sql;
}

} // namespace Schema
} // namespace Qupil
