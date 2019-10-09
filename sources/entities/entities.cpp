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

Entities* Entities::s_instance = NULL;

Entities::Entities(QObject *parent) : QObject(parent)
{
    s_instance = this;
}

Entities::~Entities()
{
    s_instance = NULL;
}

QList<QObject *> Entities::list()
{
    return m_entities.values();
}

void Entities::load()
{
    QVariantMap c = Config::getInstance()->read();
    QVariant entities = c.value("entities");

    for (int i=0; i < m_supported_entities.length(); i++)
    {
        if (entities.toMap().contains(m_supported_entities[i])) {

            QVariantList type = entities.toMap().value(m_supported_entities[i]).toJsonArray().toVariantList();

            for (int k=0; k < type.length(); k++)
            {
                QVariantMap map = type[k].toMap();
                QObject* obj = Integrations::getInstance()->getByType(map.value("integration").toString());

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
    QList<QObject *> e;
    foreach (QObject *value, m_entities)
    {
        if (value->property("integration") == integration) {
            e.append(m_entities.value(value->property("entity_id").toString()));
        }
    }
    return e;
}

QObject *Entities::get(const QString& entity_id)
{
    return m_entities.value(entity_id);
}

void Entities::add(const QString& type, const QVariantMap& config, QObject *integrationObj)
{
    Entity *entity;
    // Light entity
    if (type == "light") {
        entity = new Light(config, integrationObj);
    }
    // Blind entity
    if (type == "blind") {
        entity = new Blind(config, integrationObj);
    }
    // Media player entity
    if (type == "media_player") {
        entity = new MediaPlayer(config, integrationObj);
    }  
    // Remote entity
    if (type == "remote") {
        entity = new Remote(config, integrationObj);
    }
    m_entities.insert(entity->entity_id(), entity);
}

void Entities::update(const QString &entity_id, const QVariantMap& attributes)
{
    Entity *e = (Entity*)m_entities.value(entity_id);
    e->update(attributes);
}

QList<QObject *> Entities::mediaplayersPlaying()
{
    return m_mediaplayersPlaying.values();
}

void Entities::addMediaplayersPlaying(const QString &entity_id)
{
    QObject *o = m_entities.value(entity_id);
    if (!m_mediaplayersPlaying.contains(entity_id)) {
        m_mediaplayersPlaying.insert(entity_id, o);
        emit mediaplayersPlayingChanged();;
        emit mediaplayerAdded();
    }
}

void Entities::removeMediaplayersPlaying(const QString &entity_id)
{
    if (m_mediaplayersPlaying.contains(entity_id)) {
        m_mediaplayersPlaying.remove(entity_id);
        emit mediaplayersPlayingChanged();
        emit mediaplayersPlayingChanged();
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
