#include "entities.h"
#include "entity.h"

#include "light.h"
#include "blind.h"
#include "mediaplayer.h"
#include "remote.h"
#include "weather.h"
#include "../config.h"
#include "../integrations/integrations.h"
#include "remote.h"

#include <QJsonArray>
#include <QtDebug>
#include <QTimer>

EntitiesInterface::~EntitiesInterface()
{
}


Entities* Entities::s_instance = nullptr;

Entities::Entities(QObject *parent) : QObject(parent)
{
    s_instance = this;

    // Remote is special. Register class before entity creation (for use in Main.qml)
    Remote::staticInitialize();
}

Entities::~Entities()
{
    s_instance = nullptr;
}

QList<QObject *> Entities::list()
{
    // This is ued in rare cases (until now not at all).
    // Overhead of creating this QList is justified compared to the advantage dealing with Entity* instead of QObject*
    QList<QObject*> entities;
    for (QMap<QString, Entity*>::iterator i = m_entities.begin(); i != m_entities.end(); ++i) {
        entities.append(i.value());
    }
    return entities;
}

void Entities::load()
{
    QVariantMap entities = Config::getInstance()->getAllEntities();

    for (int i=0; i < m_supported_entities.length(); i++)
    {
        if (entities.contains(m_supported_entities[i])) {

            QVariantList type = entities.value(m_supported_entities[i]).toJsonArray().toVariantList();

            for (int k=0; k < type.length(); k++)
            {
                QVariantMap map = type[k].toMap();
                QObject* obj = Integrations::getInstance()->get(map.value("integration").toString());
                IntegrationInterface *integration = qobject_cast<IntegrationInterface*>(obj);
                add(m_supported_entities[i], map, integration);
                addLoadedEntity(m_supported_entities[i]);
            }

        }
    }
}

QList<EntityInterface *> Entities::getByType(const QString& type)
{
    QList<EntityInterface *> e;
    foreach (QObject *value, m_entities)
    {
        if (value->property("type") == type) {
            e.append(qobject_cast<EntityInterface*>(m_entities.value(value->property("entity_id").toString())));
        }
    }
    return e;
}

QList<EntityInterface *> Entities::getByArea(const QString& area)
{
    QList<EntityInterface *> e;
    for (QMap<QString, Entity*>::iterator i = m_entities.begin(); i != m_entities.end(); ++i) {
        if (i.value()->area() == area) {
            e.append(qobject_cast<EntityInterface*>(i.value()));
        }
    }
    return e;
}

QList<EntityInterface *> Entities::getByAreaType(const QString &area, const QString &type)
{
    QList<EntityInterface *> e;
    for (QMap<QString, Entity*>::iterator i = m_entities.begin(); i != m_entities.end(); ++i) {
        if (i.value()->area() == area && i.value()->type() == type) {
            e.append(qobject_cast<EntityInterface*>(i.value()));
        }
    }
    return e;
}

QList<EntityInterface *> Entities::getByIntegration(const QString& integration)
{
    QList<EntityInterface *> e;
    for (QMap<QString, Entity*>::iterator i = m_entities.begin(); i != m_entities.end(); ++i) {
        if (i.value()->integration() == integration) {
            e.append(qobject_cast<EntityInterface*>(i.value()));
        }
    }
    return e;
}
void Entities::setConnected (const QString& integrationId, bool connected)
{
    for (QMap<QString, Entity*>::iterator i = m_entities.begin(); i != m_entities.end(); ++i) {
        if (i.value()->integration() == integrationId) {
            i.value()->setConnected(connected);
        }
    }
}

QObject *Entities::get(const QString& entity_id)
{
    return m_entities.value(entity_id);
}
EntityInterface* Entities::getEntityInterface (const QString& entity_id)
{
    return qobject_cast<EntityInterface*>(m_entities.value(entity_id));
}

void Entities::add(const QString& type, const QVariantMap& config, IntegrationInterface *integrationObj)
{
    Entity *entity = nullptr;
    // Light entity
    if (type == "light") {
        entity = new Light(config, integrationObj, this);
    }
    // Blind entity
    else if (type == "blind") {
        entity = new Blind(config, integrationObj, this);
    }
    // Media player entity
    else if (type == "media_player") {
        entity = new MediaPlayer(config, integrationObj, this);
    }
    // Remote entity
    else if (type == "remote") {
        entity = new Remote(config, integrationObj, this);
    }
    // Weather entity
    else if (type == "weather") {
        entity = new Weather(config, integrationObj, this);
    }
    if (entity == nullptr)
        qDebug() << "Illegal entity type : " << type;
    else
        m_entities.insert(entity->entity_id(), entity);
}

void Entities::update(const QString &entity_id, const QVariantMap& attributes)
{
    Entity *e = static_cast<Entity*>(m_entities.value(entity_id));
    if (e == nullptr)
        qDebug() << "Entity not found : " << entity_id;
    else
        e->update(attributes);
}

QList<QObject *> Entities::mediaplayersPlaying()
{
    return m_mediaplayersPlaying.values();
}

void Entities::addMediaplayersPlaying(const QString &entity_id)
{
    // check if there is a timer active to remove the media player
    QTimer* timer = m_mediaplayersTimers.value(entity_id);
    if (timer) {
        timer->stop();
    }

    QObject *o = m_entities.value(entity_id);

    if (!m_mediaplayersPlaying.contains(entity_id)) {
        m_mediaplayersPlaying.insert(entity_id, o);
        emit mediaplayersPlayingChanged();
    }
}

void Entities::removeMediaplayersPlaying(const QString &entity_id)
{
    if (m_mediaplayersPlaying.contains(entity_id)) {

        // use a timer to remove the entity with a delay
        QTimer* timer = new QTimer();
        timer->setSingleShot(true);
        connect(timer, &QTimer::timeout, this, [=](){
            m_mediaplayersPlaying.remove(entity_id);
            emit mediaplayersPlayingChanged();
        });
        timer->start(120000);

        m_mediaplayersTimers.insert(entity_id, timer);
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
