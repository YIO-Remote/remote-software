#include "entities.h"
#include "entity.h"

Entities::Entities(QObject *parent) : QObject(parent) {}

Entities::~Entities() {}

QList<QObject *> Entities::list()
{
    return m_entities.values();
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
    entity = new Entity(config.value("type").toString(), config, integrationObj);
    m_entities.insert(entity->entity_id(), entity);
}
