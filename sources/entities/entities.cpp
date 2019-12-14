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

#include "entities.h"
#include "entity.h"

#include "light.h"
#include "blind.h"
#include "mediaplayer.h"
#include "remote.h"
#include "../config.h"
#include "../integrations/integrations.h"

#include <QJsonArray>
#include <QtDebug>
#include <QTimer>

EntitiesInterface::~EntitiesInterface()
{
}


Entities* Entities::s_instance = nullptr;

Entities::Entities(QObject *parent) : QObject(parent)
{
    s_instance = this;
}

Entities::~Entities()
{
    s_instance = nullptr;
}

QList<QObject *> Entities::list()
{
    return m_entities.values();
}

void Entities::load()
{
    QVariantMap c = Config::getInstance()->config();
    QVariant entities = c.value("entities");

    for (int i=0; i < m_supported_entities.length(); i++)
    {
        if (entities.toMap().contains(m_supported_entities[i])) {

            QVariantList type = entities.toMap().value(m_supported_entities[i]).toJsonArray().toVariantList();

            for (int k=0; k < type.length(); k++)
            {
                QVariantMap map = type[k].toMap();
                QObject* obj = Integrations::getInstance()->get(map.value("integration").toString());

                add(m_supported_entities[i], map, obj);
                addLoadedEntity(m_supported_entities[i]);
            }

        }
    }
}

QList<QObject *> Entities::getByType(const QString& type)
{
    QList<QObject *> e;
    foreach (QObject *value, m_entities)
    {
        if (value->property("type") == type) {
            e.append(m_entities.value(value->property("entity_id").toString()));
        }
    }
    return e;
}

QList<QObject *> Entities::getByArea(const QString& area)
{
    QList<QObject *> e;
    foreach (QObject *value, m_entities)
    {
        if (value->property("area") == area) {
            e.append(m_entities.value(value->property("entity_id").toString()));
        }
    }
    return e;
}

QList<QObject *> Entities::getByAreaType(const QString &area, const QString &type)
{
    QList<QObject *> e;
    foreach (QObject *value, m_entities)
    {
        if (value->property("area") == area && value->property("type") == type) {
            e.append(m_entities.value(value->property("entity_id").toString()));
        }
    }
    return e;
}

QList<QObject *> Entities::getByIntegration(const QString& integration)
{
    qDebug() << "CALLED";

    QList<QObject *> e;
    foreach (QObject *value, m_entities)
    {
        if (value->property("integration") == integration) {
            e.append(m_entities.value(value->property("entity_id").toString()));

            qDebug() << e;
        }
    }
    return e;
}

QObject *Entities::get(const QString& entity_id)
{
    return m_entities.value(entity_id);
}
EntityInterface* Entities::getEntityInterface (const QString& entity_id)
{
    return qobject_cast<EntityInterface*>(m_entities.value(entity_id));
}

void Entities::add(const QString& type, const QVariantMap& config, QObject *integrationObj)
{
    Entity *entity = nullptr;
    // Light entity
    if (type == "light") {
        entity = new Light(config, integrationObj, this);
    }
    // Blind entity
    if (type == "blind") {
        entity = new Blind(config, integrationObj, this);
    }
    // Media player entity
    if (type == "media_player") {
        entity = new MediaPlayer(config, integrationObj, this);
    }
    // Remote entity
    if (type == "remote") {
        entity = new Remote(config, integrationObj, this);
    }
    if (entity == nullptr)
        qDebug() << "Illegal entity type : " << type;
    else
        m_entities.insert(entity->entity_id(), entity);
}

void Entities::update(const QString &entity_id, const QVariantMap& attributes)
{
    Entity *e = static_cast<Entity*>(m_entities.value(entity_id));
    if (e == nullptr)
        qDebug() << "Entity not found : " << entity_id;
    else
        e->update(attributes);
}

QList<QObject *> Entities::mediaplayersPlaying()
{
    return m_mediaplayersPlaying.values();
}

void Entities::addMediaplayersPlaying(const QString &entity_id)
{
    // check if there is a timer active to remove the media player
    QTimer* timer = m_mediaplayersTimers.value(entity_id);
    if (timer) {
        timer->stop();
    }

    QObject *o = m_entities.value(entity_id);

    if (!m_mediaplayersPlaying.contains(entity_id)) {
        m_mediaplayersPlaying.insert(entity_id, o);
        emit mediaplayersPlayingChanged();
    }
}

void Entities::removeMediaplayersPlaying(const QString &entity_id)
{
    if (m_mediaplayersPlaying.contains(entity_id)) {

        // use a timer to remove the entity with a delay
        QTimer* timer = new QTimer();
        timer->setSingleShot(true);
        connect(timer, &QTimer::timeout, this, [=](){
            m_mediaplayersPlaying.remove(entity_id);
            emit mediaplayersPlayingChanged();
        });
        timer->start(120000);

        m_mediaplayersTimers.insert(entity_id, timer);
    }
}

void Entities::addLoadedEntity(const QString &entity)
{
    m_loaded_entities.append(entity);
}

QString Entities::getSupportedEntityTranslation(const QString &type)
{
    QString translation;

    for (int i=0; i<m_supported_entities.length(); i++) {
        if (supported_entities().value(i) == type) {
            translation = supported_entities_translation().value(i);
        }
    }

    return translation;
}
