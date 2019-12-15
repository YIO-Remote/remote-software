/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/

#ifndef ENTITIESINTERFACE_H
#define ENTITIESINTERFACE_H

#include <QVariant>
#include "entityinterface.h"
/// This interface is implemented by the Entities object and used by integration DLLs to access the entities
class EntitiesInterface
{
public:
    virtual ~EntitiesInterface  ();

    virtual QList<QObject *>    list        () = 0;

    // get entites by type
    virtual QList<EntityInterface *>    getByType   (const QString& type) = 0;

    // get entites by area
    virtual QList<EntityInterface *>    getByArea   (const QString& area) = 0;

    // get entities by integration
    virtual QList<EntityInterface *>    getByIntegration (const QString& integration) = 0;

    // get entity by entity_id
    virtual QObject*            get         (const QString& entity_id) = 0;

    // update an entity's attributes
    virtual void                update      (const QString& entity_id, const QVariantMap& attributes) = 0;

    // get entity interface by entity_id
    virtual EntityInterface*    getEntityInterface(const QString& entity_id) = 0;
};

QT_BEGIN_NAMESPACE
#define EntitiesInterface_iid "YIO.EntitiesInterface"
Q_DECLARE_INTERFACE(EntitiesInterface, EntitiesInterface_iid)
QT_END_NAMESPACE

#endif // ENTITIESINTERFACE_H
