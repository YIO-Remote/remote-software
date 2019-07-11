#include <QtDebug>
#include <QJsonDocument>
#include <QJsonArray>

#include "openhab.h"
#include "../../entities/entity.h"

OpenHAB::OpenHAB()
{
}

void OpenHAB::initialize(int integrationId, const QVariantMap& config, QObject* entities)
{
    setIntegrationId(integrationId);
    for (QVariantMap::const_iterator iter = config.begin(); iter != config.end(); ++iter) {
        if (iter.key() == "data") {
            QVariantMap map = iter.value().toMap();
            m_ip = map.value("ip").toString();
        }
        else if (iter.key() == "type")
            setType(iter.value().toString());
        else if (iter.key() == "friendly_name")
            setFriendlyName(iter.value().toString());
    }
    m_entities = qobject_cast<EntitiesInterface *>(entities);
}

void OpenHAB::connect()
{
    setState(CONNECTING);

}

void OpenHAB::disconnect()
{

    setState(DISCONNECTED);

}

void OpenHAB::sendCommand(const QString& type, const QString& entity_id, const QString& command, const QVariant& param)
{
    if (type == "light") {
    }
    if (type == "blind") {
    }
}

void OpenHAB::updateEntity(const QString& entity_id, const QVariantMap& attr)
{
    Entity* entity = (Entity*)m_entities->get(entity_id);
    if (entity) {
        if (entity->type() == "light") {
            updateLight(entity, attr);
        }
    }
}

void OpenHAB::updateLight(Entity* entity, const QVariantMap& attr)
{
    QVariantMap attributes;

    m_entities->update(entity->entity_id(), attributes);
}
