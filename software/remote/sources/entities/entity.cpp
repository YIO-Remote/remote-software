#include "entity.h"

#include <QtDebug>

Entity::Entity(const QString& type, const QVariantMap& config, QObject *integrationObj) :
    m_integrationObj(integrationObj),
    m_type(type),
    m_area(config.value("area").toString()),
    m_friendly_name(config.value("friendly_name").toString()),
    m_integration(config.value("integration").toString()),
    m_favorite(config.value("favorite").toBool()),
    m_supported_features(config.value("supported_features").toStringList()),
    m_attributes(config.value("attributes"))
{
    setObjectName(config.value("entity_id").toString());
}

Entity::~Entity()
{
}

bool Entity::command(const QString &command, const QVariant& param)
{
    QVariant returnedValue;
    QMetaObject::invokeMethod(m_integrationObj, "sendCommand",
                              Q_RETURN_ARG(QVariant, returnedValue),
                              Q_ARG(QVariant, QVariant(m_type)), Q_ARG(QVariant, QVariant(entity_id())), Q_ARG(QVariant, QVariant(command)), Q_ARG(QVariant, param));
    return returnedValue.toBool();
}

QVariantMap Entity::getDataToSave()
{
    QVariantMap map;

    map["type"] = m_type;
    map["entity_id"] = entity_id();
    map["favorite"] = m_favorite;
    map["friendly_name"] = m_friendly_name;
    map["integration"] = m_integration;
    map["area"] = m_area;
    map["supported_features"] = m_supported_features;
    map["attributes"] = m_attributes;

    return map;
}

bool Entity::update(const QVariantMap &attributes)
{
}

void Entity::setFavorite(bool value)
{
    if (m_favorite != value) {
        m_favorite = value;
        emit favoriteChanged();
    }
}
