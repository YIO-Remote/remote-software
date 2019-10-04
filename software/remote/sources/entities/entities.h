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
    Q_PROPERTY  (QList<QObject *>    list                   READ    list                    CONSTANT)
    Q_PROPERTY  (QStringList         supported_entities     READ    supported_entities      CONSTANT)
    Q_PROPERTY  (QStringList         supported_entities_translation READ  supported_entities_translation    CONSTANT)
    Q_PROPERTY  (QStringList         loaded_entities        READ    loaded_entities         CONSTANT)

public:
    // get all entities
    QList<QObject *>                list                ();

    // load all entites from config file
    Q_INVOKABLE void                load                ();

    // get entites by type
    Q_INVOKABLE QList<QObject *>    getByType           (const QString& type);

    // get entites by area
    Q_INVOKABLE QList<QObject *>    getByArea           (const QString& area);

    // get entites by area and type
    Q_INVOKABLE QList<QObject *>    getByAreaType       (const QString& area, const QString& type);

    // get entities by integration
    Q_INVOKABLE QList<QObject *>    getByIntegration    (const QString& integration);

    // get entity by entity_id
    Q_INVOKABLE QObject*            get                 (const QString& entity_id);

    // add an entity
    Q_INVOKABLE void                add                 (const QVariantMap& config, QObject *integrationObj);

    // update an entity
    Q_INVOKABLE void                update              (const QString& entity_id, const QVariantMap& attributes);

    QStringList                     supported_entities  () { return m_supported_entities; }
    QStringList                     supported_entities_translation () { return m_supported_entities_translation; }
    QStringList                     loaded_entities     () { return m_loaded_entities; }

    Q_INVOKABLE void                addLoadedEntity     (const QString& entity);
    Q_INVOKABLE QString             getSupportedEntityTranslation (const QString& type);

//    explicit Entities               (QObject *parent = nullptr);
//    virtual ~Entities();

    explicit Entities(QObject *parent = nullptr);
    virtual ~Entities();

    static Entities*       getInstance     ()
    { return s_instance; }

private:
    QMap<QString, QObject*>     m_entities;
    QStringList                 m_supported_entities = {"light","blind","media_player"};
    QStringList                 m_supported_entities_translation = {tr("Lights"), tr("Blinds"), tr("Media")};
    QStringList                 m_loaded_entities;

    static Entities*            s_instance;
};

#endif // ENTITIES_H
