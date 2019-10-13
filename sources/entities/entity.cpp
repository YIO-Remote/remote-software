#include "entity.h"
#include "../config.h"

#include <QtDebug>
#include <QTimer>

Entity::Entity(const QString& type, const QVariantMap& config, QObject *integrationObj, QObject *parent) :
    QObject(parent),
    m_integrationObj(integrationObj),
    m_type(type),
    m_area(config.value("area").toString()),
    m_friendly_name(config.value("friendly_name").toString()),
    m_integration(config.value("type").toString()), //integration
    m_supported_features(config.value("supported_features").toStringList()),
    m_attributes(config.value("attributes"))
{
    setObjectName(config.value("entity_id").toString());

    QVariantMap c = Config::getInstance()->read();
    QString p = QString::number(Config::getInstance()->profile());

    QVariantList f = c.value("ui_config").toMap().value("profiles").toMap().value(p).toMap().value("favorites").toJsonArray().toVariantList();

    m_favorite = false;

    for (int i = 0; i < f.length(); i++)
    {
        if (f[i].toString() == config.value("entity_id").toString()) {
            m_favorite = true;
        }
    }
}

Entity::~Entity()
{
}

void Entity::command(const QString &command, const QVariant& param)
{
    QVariant returnedValue;
    QMetaObject::invokeMethod(m_integrationObj, "sendCommand", Qt::AutoConnection,
                              Q_ARG(QString, m_type),
                              Q_ARG(QString, QString(entity_id())),
                              Q_ARG(QString, command),
                              Q_ARG(QVariant, param));
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
//    map["attributes"] = m_attributes;

    return map;
}

bool Entity::update(const QVariantMap &attributes)
{
}

void Entity::setFavorite(bool value)
{
        QTimer::singleShot(1000, this, [=](){

            QVariantMap c = Config::getInstance()->read();
            QString p = QString::number(Config::getInstance()->profile());

            QVariantList f = c.value("ui_config").toMap().value("profiles").toMap().value(p).toMap().value("favorites").toJsonArray().toVariantList();

            if (value && f.indexOf(entity_id()) == -1) {
                f.append(entity_id());
            }

            if (!value && f.indexOf(entity_id()) > -1) {
                f.removeAt(f.indexOf(entity_id()));
            }

            // save to config
            QVariantMap r = c.value("ui_config").toMap().value("profiles").toMap().value(p).toMap();
            r.insert("favorites", f);

            QVariantMap r2 = c.value("ui_config").toMap().value("profiles").toMap();
            r2.insert(p, r);

            QVariantMap r3 = c.value("ui_config").toMap();
            r3.insert("profiles", r2);

            c.insert("ui_config", r3);

            Config::getInstance()->readWrite(c);

            // write to config file
            Config::getInstance()->getInstance()->writeConfig();

            m_favorite = value;
            emit favoriteChanged();
        });
}
