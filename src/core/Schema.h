// SPDX-FileCopyrightText: 2006-2026 Felix Hammer
// SPDX-License-Identifier: GPL-3.0-or-later WITH LicenseRef-Qupil-AppStore-exception
#pragma once

#include <QList>
#include <QString>
#include <QStringList>

namespace Qupil {

struct FieldDef
{
    QString name;
    QString type;          // TEXT or INTEGER
    QString defaultValue;  // SQL literal
};

/*!
    A synchronised entity: one table, primary key "id" (UUIDv7 text), a "deleted"
    tombstone flag and the listed fields. Every change to a field is logged with
    a hybrid-clock timestamp so devices can merge field by field.
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
constexpr int Version = 1;

const QList<EntityDef> &entities();
const EntityDef *entity(const QString &name);

//! All CREATE statements for a new database
QStringList createStatements();

} // namespace Schema
} // namespace Qupil
