#ifndef ENTITIES_H
#define ENTITIES_H

#include <QObject>
#include <QQmlComponent>
#include <QList>
#include <QString>
#include <QVariant>
#include <QMap>
#include <QtDebug>

#include "entitiesinterface.h"

class Entities : public QObject , EntitiesInterface
{
    Q_OBJECT
//    Q_PLUGIN_METADATA(IID "YIO.EntitiesInterface")
    Q_INTERFACES(EntitiesInterface)

    // list of all entities
    Q_PROPERTY  (QList<QObject*>            list                   READ    list                    CONSTANT)
    Q_PROPERTY  (QStringList                supported_entities     READ    supported_entities      CONSTANT)
    Q_PROPERTY  (QStringList                supported_entities_translation READ  supported_entities_translation    CONSTANT)
    Q_PROPERTY  (QStringList                loaded_entities        READ    loaded_entities         CONSTANT)

    Q_PROPERTY  (QList<QObject *>           mediaplayersPlaying    READ    mediaplayersPlaying     NOTIFY mediaplayersPlayingChanged)

public:
    // get all entities
    QList<QObject *>                list                ();

    // load all entites from config file
    Q_INVOKABLE void                load                ();

    // get entity by entity_id
    Q_INVOKABLE QObject*            get                 (const QString& entity_id);

    // add an entity
    Q_INVOKABLE void                add                 (const QString& type, const QVariantMap& config, QObject *integrationObj);

    // update an entity
    Q_INVOKABLE void                update              (const QString& entity_id, const QVariantMap& attributes);

    // get entites by type
    QList<EntityInterface *>        getByType           (const QString& type);

    // get entites by area
    QList<EntityInterface *>        getByArea           (const QString& area);

    // get entites by area and type
    QList<EntityInterface *>        getByAreaType       (const QString& area, const QString& type);

    // get entities by integration
    QList<EntityInterface *>        getByIntegration    (const QString& integration);

    // get entity interface
    EntityInterface*                getEntityInterface  (const QString& entity_id);

    QStringList                     supported_entities  () { return m_supported_entities; }
    QStringList                     supported_entities_translation () { return m_supported_entities_translation; }
    QStringList                     loaded_entities     () { return m_loaded_entities; }

    QList<QObject *>                mediaplayersPlaying ();
    void                            addMediaplayersPlaying (const QString &entity_id);
    void                            removeMediaplayersPlaying (const QString &entity_id);

    Q_INVOKABLE void                addLoadedEntity     (const QString& entity);
    Q_INVOKABLE QString             getSupportedEntityTranslation (const QString& type);

    explicit Entities(QObject *parent = nullptr);
    virtual ~Entities();

    static Entities*       getInstance     ()
    { return s_instance; }

signals:
    void mediaplayersPlayingChanged();

private:
    QMap<QString, QObject*>     m_entities;
    QStringList                 m_supported_entities = {"light","blind","media_player","remote", "weather" };
    QStringList                 m_supported_entities_translation = {tr("Lights"), tr("Blinds"), tr("Media"), tr("Remote")};
    QStringList                 m_loaded_entities;

    QMap<QString, QObject*>     m_mediaplayersPlaying;
    QMap<QString, QTimer*>      m_mediaplayersTimers;

    static Entities*            s_instance;
};

#endif // ENTITIES_H
