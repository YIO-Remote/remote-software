#ifndef ENTITY_H
#define ENTITY_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVariant>

//#include "../integrations/integrationinterface.h"

class Entity : public QObject
{
    Q_OBJECT

public:
    Q_PROPERTY	(QString        type READ type CONSTANT)
    Q_PROPERTY	(QString        friendly_name READ friendly_name CONSTANT)
    Q_PROPERTY	(QString        entity_id READ entity_id CONSTANT)
    Q_PROPERTY	(QString        area READ area CONSTANT)
    Q_PROPERTY	(QString        integration READ integration CONSTANT)
    Q_PROPERTY	(QObject*       integrationObj READ integrationObj)
    Q_PROPERTY	(bool           favorite READ favorite WRITE setFavorite NOTIFY favoriteChanged)
    Q_PROPERTY	(QStringList   	supported_features READ supported_features NOTIFY featureChanged)
    Q_PROPERTY  (QVariant       attributes READ attributes NOTIFY attributesChanged)		// all dynamic attributes component type specific, for example: state, brightness, etc.

    // send command to the integration
    Q_INVOKABLE	void		command		(const QString &command, const QVariant& param);

    // get the data of an entity to save in config.json
    Q_INVOKABLE QVariantMap getDataToSave   ();

    // update an entity with attributes from integration hub
    virtual     bool        update          (const QVariantMap& attributes);

    // Creation from config.json content and integration hub
    explicit Entity (const QString& type, const QVariantMap& config, QObject *integrationObj);
    virtual ~Entity ();

    QString			type()                  { return m_type; }
    QString			area()                  { return m_area; }
    QString			friendly_name()         { return m_friendly_name; }
    QString			entity_id()             { return objectName(); }
    QString			integration()           { return m_integration; }
    QObject*		integrationObj()        { return m_integrationObj; }
    QStringList     supported_features()    { return m_supported_features; }
    bool			favorite()              { return m_favorite; }
    QVariant        attributes()            { return m_attributes; }

    void			setFavorite             (bool value);

signals:
    void featureChanged();
    void favoriteChanged();
    void attributesChanged();

protected:
    //    IntegrationInterface*   m_integrationIf;
    QObject*                m_integrationObj;
    QString                 m_type;
    QString                 m_area;
    QString                 m_friendly_name;
    QString                 m_integration;
    bool                    m_favorite;
    QStringList             m_supported_features;
    QVariant                m_attributes;
};

#endif // ENTITY_H
