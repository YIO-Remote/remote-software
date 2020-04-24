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
#include <QMap>
#include <QMutex>
#include <QObject>
#include <QQmlComponent>
#include <QString>
#include <QTimer>
#include <QVariant>

#include "entity.h"
#include "yio-interface/entities/entitiesinterface.h"
#include "yio-interface/integrationinterface.h"

class Entities : public QObject, public EntitiesInterface {
    Q_OBJECT
    Q_INTERFACES(EntitiesInterface)

    // list of all entities
    Q_PROPERTY(QList<QObject*> list READ list CONSTANT)
    Q_PROPERTY(QStringList supportedEntities READ supportedEntities CONSTANT)
    Q_PROPERTY(QStringList supportedEntitiesTranslation READ supportedEntitiesTranslation CONSTANT)

    Q_PROPERTY(QList<QObject*> mediaplayersPlaying READ mediaplayersPlaying NOTIFY mediaplayersPlayingChanged)

 public:
    /// ADD NEW ENTITY TYPE HERE
    enum SupportedEntityTypes { LIGHT, BLIND, MEDIA_PLAYER, REMOTE, WEATHER, CLIMATE, SWITCH };
    Q_ENUM(SupportedEntityTypes)

    // get all entities
    QList<QObject*> list();

    // load all entites from config file
    Q_INVOKABLE void load();

    // get entity by entity_id
    Q_INVOKABLE QObject* get(const QString& entity_id);

    // update an entity
    Q_INVOKABLE void update(const QString& entity_id, const QVariantMap& attributes) override;

    // add an entity
    void add(const QString& type, const QVariantMap& config, IntegrationInterface* integrationObj);

    // remove an entity
    void remove(const QString& entity_id);

    // get entites by type
    QList<EntityInterface*> getByType(const QString& type) override;

    // get entites by area
    QList<EntityInterface*> getByArea(const QString& area) override;

    // get entites by area and type
    QList<EntityInterface*> getByAreaType(const QString& area, const QString& type);

    // get entities by integration
    QList<EntityInterface*> getByIntegration(const QString& integration) override;

    // get entity interface
    EntityInterface* getEntityInterface(const QString& entity_id) override;

    // set connected
    void setConnected(const QString& integrationId, bool connected) override;

    QStringList supportedEntities() override { return m_supportedEntities; }
    QStringList supportedEntitiesTranslation() { return m_supportedEntitiesTranslation; }
    bool        isSupportedEntityType(const QString& type) override;

    // active media player handling for mini media player
    QList<QObject*> mediaplayersPlaying();
    void            addMediaplayersPlaying(const QString& entity_id);
    void            removeMediaplayersPlaying(const QString& entity_id);

    Q_INVOKABLE QString getSupportedEntityTranslation(const QString& type);

    explicit Entities(QObject* parent = nullptr);
    ~Entities() override;

    static Entities* getInstance() { return s_instance; }

 signals:
    void mediaplayersPlayingChanged();
    void entitiesLoaded();

 private:
    QMap<QString, Entity*> m_entities;
    QStringList            m_supportedEntities;
    QStringList            m_supportedEntitiesTranslation = {tr("Lights"), tr("Blinds"),  tr("Media"),
                                                  tr("Remote"), tr("Climate"), tr("Switch")};

    QMap<QString, QObject*> m_mediaplayersPlaying;
    QMap<QString, QTimer*>  m_mediaplayersTimers;

    static Entities* s_instance;

    QMutex m_mutex;

 protected:
    QMetaEnum* m_enumSupportedEntityTypes;
};
