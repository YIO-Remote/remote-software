#ifndef ENTITIES_H
#define ENTITIES_H

#include <QObject>
#include <QQmlComponent>
#include <QtPlugin>
#include <QList>
#include <QString>
#include <QVariant>
#include <QMap>
#include <QtDebug>

#include "entitiesinterface.h"

class Entities : public QObject , EntitiesInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "YIO.EntitiesInterface")
    Q_INTERFACES(EntitiesInterface)

    // list of all entities
    Q_PROPERTY  (QList<QObject *>    list                   READ    list                    CONSTANT)
    Q_PROPERTY  (QStringList         supported_entities     READ    supported_entities      CONSTANT)
    Q_PROPERTY  (QStringList         supported_entities_translation READ  supported_entities_translation    CONSTANT)

public:
    // get all entities
    QList<QObject *>                list                ();

    // get entites by type
    Q_INVOKABLE QList<QObject *>    getByType           (const QString& type);

    // get entites by area
    Q_INVOKABLE QList<QObject *>    getByArea           (const QString& area);

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

    explicit Entities               (QObject *parent = nullptr);
    virtual ~Entities();

private:
    QMap<QString, QObject*>     m_entities;
    QStringList                 m_supported_entities = {"light","blind"};
    QStringList                 m_supported_entities_translation = {tr("Lights"), tr("Blinds")};
};

#endif // ENTITIES_H
