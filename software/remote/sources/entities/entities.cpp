#include "entities.h"
#include "entity.h"

#include <QtDebug>

Entities::Entities(QObject *parent) : QObject(parent) {}

Entities::~Entities() {}

QList<QObject *> Entities::list()
{
    return m_entities.values();
}

QList<QObject *> Entities::getByType(const QString type)
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

QObject *Entities::get(const QString &entity_id)
{
    return m_entities.value(entity_id);
}

void Entities::add(const QVariantMap &config, QObject *integrationObj)
{
    m_entities.insert(config.value("entity_id").toString(), new Entity(config.value("type").toString(), config, integrationObj));
}
