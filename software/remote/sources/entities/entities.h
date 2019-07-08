#ifndef ENTITIES_H
#define ENTITIES_H

#include <QObject>
#include <QList>
#include <QString>
#include <QVariant>
#include <QMap>

class Entities : public QObject
{
    Q_OBJECT

    // list of all entities
    Q_PROPERTY  (QList<QObject *>    list    READ    list    CONSTANT)

public:
    // get all entities
    QList<QObject *>                list        ();

    // get entites by type
    Q_INVOKABLE QList<QObject *>    getByType   (const QString type);

    // get entites by area
    Q_INVOKABLE QList<QObject *>    getByArea   (const QString area);

    // get entities by integration
    Q_INVOKABLE QList<QObject *>    getByIntegration (const QString integration);

    // get entity by entity_id
    Q_INVOKABLE QObject*            get         (const QString &entity_id);

    // add an entity
    Q_INVOKABLE void                add         (const QVariantMap& config, QObject *integrationObj);


    explicit Entities (QObject *parent = nullptr);
    virtual ~Entities();

private:
    QMap<QString, QObject*>         m_entities;
};

#endif // ENTITIES_H
