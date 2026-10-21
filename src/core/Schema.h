// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QList>
#include <QString>
#include <QStringList>
#include <QVariantMap>

namespace Qupil {

struct FieldDef
{
    QString name;
    QString type;          // TEXT or INTEGER
    QString defaultValue;  // SQL literal
};

/*!
    A synchronised entity: one table, primary key "id" (UUIDv7 text), a "deleted"
    tombstone flag and the listed fields. Every write is logged with a
    hybrid-clock time so devices can merge field by field.
*/
struct EntityDef
{
    QString name;
    QList<FieldDef> fields;
    QStringList indexes;   // columns that get an index

    QStringList fieldNames() const;
    bool hasField(const QString &field) const;
};

namespace Schema {

//! Current version of the Qupil 2 database layout
//! 2: library_item.number
//! 3: compact sync bookkeeping (device numbers, integer clock, one change per record and write)
//! 4: student.mother_note and father_note instead of mother_job and father_job (renamedFields())
constexpr int Version = 4;

const QList<EntityDef> &entities();
const EntityDef *entity(const QString &name);

//! A field that has a new name since a schema version (its values moved there)
struct RenamedField
{
    QString entity;
    QString from;
    QString to;
    int version;   // schema version that renamed it
};
const QList<RenamedField> &renamedFields();

/*!
    The fields of a change with today's names: changes written by older builds
    (other devices, old sync files) still use the old ones. A value already
    under the new name stays.
*/
QVariantMap currentFields(const QString &entity, const QVariantMap &fields);

//! All CREATE statements for a new database
QStringList createStatements();

} // namespace Schema
} // namespace Qupil
