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

#pragma once

#include <QList>
#include <QLoggingCategory>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QQmlComponent>
#include <QString>
#include <QVariant>
#include <QtDebug>

#include "entity.h"
#include "yio-interface/entities/entitiesinterface.h"
#include "yio-interface/integrationinterface.h"

class Entities : public QObject, public EntitiesInterface {
    Q_OBJECT
    //    Q_PLUGIN_METADATA(IID "YIO.EntitiesInterface")
    Q_INTERFACES(EntitiesInterface)

    // list of all entities
    Q_PROPERTY(QList<QObject*> list READ list CONSTANT)
    Q_PROPERTY(QStringList supported_entities READ supported_entities CONSTANT)
    Q_PROPERTY(QStringList supported_entities_translation READ supported_entities_translation CONSTANT)
    Q_PROPERTY(QStringList loaded_entities READ loaded_entities CONSTANT)

    Q_PROPERTY(QList<QObject*> mediaplayersPlaying READ mediaplayersPlaying NOTIFY mediaplayersPlayingChanged)

 public:
    // get all entities
    QList<QObject*> list();

    // load all entites from config file
    Q_INVOKABLE void load();

    // get entity by entity_id
    Q_INVOKABLE QObject* get(const QString& entity_id);

    // update an entity
    Q_INVOKABLE void update(const QString& entity_id, const QVariantMap& attributes);

    // add an entity
    void add(const QString& type, const QVariantMap& config, IntegrationInterface* integrationObj);

    // get entites by type
    QList<EntityInterface*> getByType(const QString& type);

    // get entites by area
    QList<EntityInterface*> getByArea(const QString& area);

    // get entites by area and type
    QList<EntityInterface*> getByAreaType(const QString& area, const QString& type);

    // get entities by integration
    QList<EntityInterface*> getByIntegration(const QString& integration);

    // get entity interface
    EntityInterface* getEntityInterface(const QString& entity_id);

    // set connected
    void setConnected(const QString& integrationId, bool connected);

    QStringList supported_entities() { return m_supported_entities; }
    QStringList supported_entities_translation() { return m_supported_entities_translation; }
    QStringList loaded_entities() { return m_loaded_entities; }

    QList<QObject*> mediaplayersPlaying();
    void            addMediaplayersPlaying(const QString& entity_id);
    void            removeMediaplayersPlaying(const QString& entity_id);

    Q_INVOKABLE void addLoadedEntity(const QString& entity);
    Q_INVOKABLE QString getSupportedEntityTranslation(const QString& type);

    explicit Entities(QObject* parent = nullptr);
    virtual ~Entities();

    static Entities* getInstance() { return s_instance; }

 signals:
    void mediaplayersPlayingChanged();
    void entitiesLoaded();

 private:
    QMap<QString, Entity*> m_entities;
    /// ADD NEW ENTITY TYPE HERE
    QStringList m_supported_entities = {"light", "blind", "media_player", "remote", "weather", "climate", "switch"};
    QStringList m_supported_entities_translation = {tr("Lights"), tr("Blinds"),  tr("Media"),
                                                    tr("Remote"), tr("Climate"), tr("Switch")};
    QStringList m_loaded_entities;

    QMap<QString, QObject*> m_mediaplayersPlaying;
    QMap<QString, QTimer*>  m_mediaplayersTimers;

    static Entities* s_instance;

    QLoggingCategory m_log;

    QMutex m_mutex;
};
