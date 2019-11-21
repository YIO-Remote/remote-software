#ifndef ENTITY_H
#define ENTITY_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

//#include "../integrations/integrationinterface.h"
#include "entityinterface.h"

class Entity : public QObject, EntityInterface
{
    Q_OBJECT
    Q_INTERFACES(EntityInterface)

public:
    // Creation from config.json content and integration hub
    explicit Entity (const QString& type, const QVariantMap& config, QObject *integrationObj, QObject *parent = nullptr);
    virtual ~Entity ();


    Q_PROPERTY	(QString        type READ type CONSTANT)
    Q_PROPERTY	(QString        friendly_name READ friendly_name CONSTANT)
    Q_PROPERTY	(QString        entity_id READ entity_id CONSTANT)
    Q_PROPERTY	(QString        area READ area CONSTANT)
    Q_PROPERTY	(QString        integration READ integration CONSTANT)
    Q_PROPERTY	(QObject*       integrationObj READ integrationObj NOTIFY integrationObjChanged)
    Q_PROPERTY	(bool           favorite READ favorite WRITE setFavorite NOTIFY favoriteChanged)
    Q_PROPERTY	(QStringList   	supported_features READ supported_features NOTIFY featureChanged)

    // Q_PROPERTY  (QVariant       attributes READ attributes NOTIFY attributesChanged)		// all dynamic attributes component type specific, for example: state, brightness, etc.

    // send command to the integration
    Q_INVOKABLE	void            command             (const QString &command, const QVariant& param);

    // get the data of an entity to save in config.json
    Q_INVOKABLE QVariantMap     getDataToSave       ();

    // update an entity with attributes from integration hub, return true in case of change, will be overriden
    Q_INVOKABLE bool            update              (const QVariantMap& attributes);
    Q_INVOKABLE bool            updateAttrByName    (const QString& name, const QVariant& value);
    Q_INVOKABLE bool            updateAttrByIndex   (int attrIndex, const QVariant& value);

    // Attribute name and index, will be overloaded
    Q_INVOKABLE QString         getAttrName         (int attrIndex);
    Q_INVOKABLE int             getAttrIndex        (const QString& attrName);

    // Only for C++ integrations, overriden
    virtual     void*           getSpecificInterface();     // light, blind, media player */

    QString                     type()                  { return m_type; }
    QString                     area()                  { return m_area; }
    QString                     friendly_name()         { return m_friendly_name; }
    QString                     entity_id()             { return objectName(); }
    QString                     integration()           { return m_integration; }
    QObject*                    integrationObj()        { return m_integrationObj; }
    QStringList                 supported_features()    { return m_supported_features; }
    bool                        favorite()              { return m_favorite; }

    void                        setFavorite             (bool value);

signals:
    void featureChanged();
    void favoriteChanged();
    void attributesChanged();
    void integrationObjChanged();

protected:
    QObject*                m_integrationObj;
    QString                 m_type;
    QString                 m_area;
    QString                 m_friendly_name;
    QString                 m_integration;
    bool                    m_favorite;
    QStringList             m_supported_features;
};

#endif // ENTITY_H
