#include "entities.h"
#include "entity.h"

#include "light.h"
#include "blind.h"
#include "mediaplayer.h"
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
    QVariantList entities = c.value("entities").toJsonArray().toVariantList();

    for (int i=0; i<entities.length(); i++)
    {
        for (int k=0; k<m_supported_entities.length(); k++)
        {
            if (entities[i].toMap().value("type").toString() == m_supported_entities[k]) {
                QVariantList data = entities[i].toMap().value("data").toJsonArray().toVariantList();
                for (int j=0; i < data.length(); j++)
                {
                    QVariant d = data[j];
                    qDebug() << "DATA" << d;

                    ////                    add(en, Integrations::getInstance()->getByType(en.value("integration").toString()));
                    ////                    addLoadedEntity(m_supported_entities[k]);
                }
            }
        }
    }


    //    for (var i=0; i<config.read.entities.length; i++) {
    //        for (var k=0; k<entities.supported_entities.length; k++) {
    //            if (config.read.entities[i].type == entities.supported_entities[k]) {
    //                for (var j=0; j < config.read.entities[i].data.length; j++) {
    //                    const en = config.read.entities[i].data[j];
    ////                        entities.add(en, integration[en.integration].obj);
    ////                        var obj = integrations.getByType(en.integration);
    //                    entities.add(en, obj);
    //                }

    //                // store which entity type was loaded. Not all supported entities are loaded.
    //                entities.addLoadedEntity(entities.supported_entities[k]);
    //            }
    //        }
    //    }
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

void Entities::add(const QVariantMap& config, QObject *integrationObj)
{
    Entity *entity;
    // Light entity
    if (config.value("type").toString() == "light") {
        entity = new Light(config, integrationObj);
    }
    // Blind entity
    if (config.value("type").toString() == "blind") {
        entity = new Blind(config, integrationObj);
    }
    // Media player entity
    if (config.value("type").toString() == "media_player") {
        entity = new MediaPlayer(config, integrationObj);
    }
    m_entities.insert(entity->entity_id(), entity);
}

void Entities::update(const QString &entity_id, const QVariantMap& attributes)
{
    Entity *e = (Entity*)m_entities.value(entity_id);
    e->update(attributes);
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
