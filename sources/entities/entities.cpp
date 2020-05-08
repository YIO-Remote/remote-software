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

#include <QJsonArray>
#include <QLoggingCategory>
#include <QTimer>
#include <QtDebug>

#include "../config.h"
#include "../integrations/integrations.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "entities");

EntitiesInterface::~EntitiesInterface() {}

Entities *Entities::s_instance = nullptr;

Entities::Entities(QObject *parent) : QObject(parent), m_enumSupportedEntityTypes(nullptr) {
    s_instance = this;

    // Remote is special. Register class before entity creation (for use in Main.qml)
    Remote::staticInitialize();

    const QMetaObject &metaObject             = EntitiesSupported::staticMetaObject;
    int                index                  = metaObject.indexOfEnumerator("SupportedEntityTypes");
    QMetaEnum          metaEnumSupportedTypes = metaObject.enumerator(index);
    m_enumSupportedEntityTypes                = &metaEnumSupportedTypes;

    for (int i = 0; i < m_enumSupportedEntityTypes->keyCount(); i++) {
        QString name(m_enumSupportedEntityTypes->key(i));
        qCDebug(CLASS_LC()) << "Adding supported entity type:" << name.toLower();
        m_supportedEntities.append(name.toLower());
    }
}

Entities::~Entities() { s_instance = nullptr; }

QList<QObject *> Entities::list() {
    // This is ued in rare cases (until now not at all).
    // Overhead of creating this QList is justified compared to the advantage dealing with Entity* instead of QObject*
    QList<QObject *> entities;
    for (QMap<QString, Entity *>::iterator i = m_entities.begin(); i != m_entities.end(); ++i) {
        entities.append(i.value());
    }
    return entities;
}

void Entities::load() {
    QVariantMap entities = Config::getInstance()->getAllEntities();

    for (int i = 0; i < m_supportedEntities.length(); i++) {
        if (entities.contains(m_supportedEntities[i])) {
            QVariantList type = entities.value(m_supportedEntities[i]).toJsonArray().toVariantList();

            for (int k = 0; k < type.length(); k++) {
                QVariantMap           map = type[k].toMap();
                QObject *             obj = Integrations::getInstance()->get(map.value("integration").toString());
                IntegrationInterface *integration = qobject_cast<IntegrationInterface *>(obj);
                add(m_supportedEntities[i], map, integration);
            }
        }
    }
    emit entitiesLoaded();

    // when all entities are loaded, connect the integrations
    QList<QObject *> integrations = Integrations::getInstance()->list();
    for (int i = 0; i < integrations.count(); i++) {
        QObject *             obj = integrations[i];
        IntegrationInterface *ii  = qobject_cast<IntegrationInterface *>(obj);
        ii->connect();
    }
}

QList<EntityInterface *> Entities::getByType(const QString &type) {
    QList<EntityInterface *> e;
    for (QObject *value : m_entities) {
        if (value->property("type") == type) {
            e.append(qobject_cast<EntityInterface *>(m_entities.value(value->property("entity_id").toString())));
        }
    }
    return e;
}

// TODO(marton) this function might be removed
QList<EntityInterface *> Entities::getByArea(const QString &area) {
    QList<EntityInterface *> e;
    for (QMap<QString, Entity *>::iterator i = m_entities.begin(); i != m_entities.end(); ++i) {
        if (i.value()->area() == area) {
            e.append(qobject_cast<EntityInterface *>(i.value()));
        }
    }
    return e;
}

QList<EntityInterface *> Entities::getByAreaType(const QString &area, const QString &type) {
    QList<EntityInterface *> e;
    for (QMap<QString, Entity *>::iterator i = m_entities.begin(); i != m_entities.end(); ++i) {
        if (i.value()->area() == area && i.value()->type() == type) {
            e.append(qobject_cast<EntityInterface *>(i.value()));
        }
    }
    return e;
}

QList<EntityInterface *> Entities::getByIntegration(const QString &integration) {
    QList<EntityInterface *> e;
    for (QMap<QString, Entity *>::iterator i = m_entities.begin(); i != m_entities.end(); ++i) {
        if (i.value()->integration() == integration) {
            e.append(qobject_cast<EntityInterface *>(i.value()));
        }
    }
    return e;
}
void Entities::setConnected(const QString &integrationId, bool connected) {
    for (QMap<QString, Entity *>::iterator i = m_entities.begin(); i != m_entities.end(); ++i) {
        if (i.value()->integration() == integrationId) {
            i.value()->setConnected(connected);
        }
    }
}

bool Entities::isSupportedEntityType(const QString &type) {
    if (m_supportedEntities.contains(type)) {
        return true;
    } else {
        return false;
    }
    //    if (m_enumSupportedEntityTypes->keyToValue(type.toUpper().toUtf8()) > -1) {
    //        return true;
    //    } else {
    //        return false;
    //    }
}

QObject *        Entities::get(const QString &entity_id) { return m_entities.value(entity_id); }
EntityInterface *Entities::getEntityInterface(const QString &entity_id) {
    return qobject_cast<EntityInterface *>(m_entities.value(entity_id));
}

/// ADD NEW ENTITY TYPE HERE
void Entities::add(const QString &type, const QVariantMap &config, IntegrationInterface *integrationObj) {
    Entity *entity = nullptr;
    // Light entity
    if (type == "light") {
        entity = new Light(config, integrationObj, this);
    } else if (type == "blind") {
        // Blind entity
        entity = new Blind(config, integrationObj, this);
    } else if (type == "media_player") {
        // Media player entity
        entity = new MediaPlayer(config, integrationObj, this);
    } else if (type == "remote") {
        // Remote entity
        entity = new Remote(config, integrationObj, this);
    } else if (type == "weather") {
        // Weather entity
        entity = new Weather(config, integrationObj, this);
    } else if (type == "climate") {
        // Climate entity
        entity = new Climate(config, integrationObj, this);
    } else if (type == "switch") {
        // Switch entity
        entity = new Switch(config, integrationObj, this);
    }

    if (entity == nullptr) {
        qCDebug(CLASS_LC) << "Illegal entity type : " << type;
    } else {
        m_entities.insert(entity->entity_id(), entity);
    }
}

void Entities::remove(const QString &entity_id) { m_entities.remove(entity_id); }

void Entities::update(const QString &entity_id, const QVariantMap &attributes) {
    Entity *e = static_cast<Entity *>(m_entities.value(entity_id));
    if (e == nullptr)
        qCDebug(CLASS_LC) << "Entity not found : " << entity_id;
    else
        e->update(attributes);
}

QList<QObject *> Entities::mediaplayersPlaying() { return m_mediaplayersPlaying.values(); }

void Entities::addMediaplayersPlaying(const QString &entity_id) {
    QMutexLocker locker(&m_mutex);

    // check if there is a timer active to remove the media player
    if (m_mediaplayersTimers.contains(entity_id)) {
        qCDebug(CLASS_LC) << "There is a timer for:" << entity_id;
        QTimer *timer = m_mediaplayersTimers.value(entity_id);
        if (timer) {
            timer->stop();
            timer->blockSignals(true);
            m_mediaplayersTimers.remove(entity_id);
            timer->deleteLater();
        }
        qCDebug(CLASS_LC) << "Removing timer";
    }

    if (!m_mediaplayersPlaying.contains(entity_id)) {
        qCDebug(CLASS_LC) << "Getting entity object" << entity_id;
        QObject *o = get(entity_id);
        qCDebug(CLASS_LC) << "Object parent:" << o->parent();
        //        o->setParent(this);

        qCDebug(CLASS_LC) << "Object is not in the list, adding to list" << entity_id;
        m_mediaplayersPlaying.insert(entity_id, o);
        qCDebug(CLASS_LC) << "Emitting mediaplayersPlayingChanged";
        emit mediaplayersPlayingChanged();
    }
}

void Entities::removeMediaplayersPlaying(const QString &entity_id, const bool &now) {
    QMutexLocker locker(&m_mutex);

    if (m_mediaplayersPlaying.contains(entity_id)) {
        qCDebug(CLASS_LC) << "There is an object playing list" << entity_id;
        if (now) {
            qCDebug(CLASS_LC) << "Removing object" << entity_id;
            m_mediaplayersPlaying.remove(entity_id);

            if (m_mediaplayersTimers.contains(entity_id)) {
                qCDebug(CLASS_LC) << "Removing timer" << entity_id;
                m_mediaplayersTimers.remove(entity_id);
            }

            qCDebug(CLASS_LC) << "Emitting mediaplayersPlayingChanged";
            emit mediaplayersPlayingChanged();

        } else {
            // use a timer to remove the entity with a delay
            if (!m_mediaplayersTimers.contains(entity_id)) {
                qCDebug(CLASS_LC) << "No timer found for object" << entity_id;
                QTimer *timer = new QTimer();
                timer->setSingleShot(true);

                QObject *context = new QObject();

                qCDebug(CLASS_LC) << "Connecting signals" << entity_id;
                connect(timer, &QTimer::timeout, context, [=]() {
                    qCDebug(CLASS_LC) << "Timer timeout" << entity_id;
                    if (m_mediaplayersPlaying.contains(entity_id)) {
                        qCDebug(CLASS_LC) << "Removing object" << entity_id;
                        m_mediaplayersPlaying.remove(entity_id);
                    }

                    if (m_mediaplayersTimers.contains(entity_id)) {
                        qCDebug(CLASS_LC) << "Removing timer" << entity_id;
                        m_mediaplayersTimers.remove(entity_id);
                    }

                    qCDebug(CLASS_LC) << "Emitting mediaplayersPlayingChanged";
                    emit mediaplayersPlayingChanged();
                    qCDebug(CLASS_LC) << "Context deleteLater";
                    context->deleteLater();
                    qCDebug(CLASS_LC) << "Timer deleteLater";
                    timer->deleteLater();
                });

                qCDebug(CLASS_LC) << "Starting timer" << entity_id;
                timer->start(120000);

                qCDebug(CLASS_LC) << "Inserting timer to the list" << entity_id;
                m_mediaplayersTimers.insert(entity_id, timer);
            }
        }
    }
}

// void Entities::addLoadedEntity(const QString &entity) { m_loaded_entities.append(entity); }

QString Entities::getSupportedEntityTranslation(const QString &type) {
    QString translation;

    for (int i = 0; i < m_supportedEntities.length(); i++) {
        if (supportedEntities().value(i) == type) {
            translation = supportedEntitiesTranslation().value(i);
        }
    }

    return translation;
}
