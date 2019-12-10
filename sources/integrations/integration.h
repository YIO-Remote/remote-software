#ifndef INTEGRATION_H
#define INTEGRATION_H

#include <QObject>
#include "integrationinterface.h"
#include "../entities/entitiesinterface.h"

// Integration base class
class Integration : public QObject, IntegrationInterface
{
    Q_OBJECT
    Q_INTERFACES (IntegrationInterface)
public:

    Integration() {}

    enum states {
        CONNECTED=0,
        CONNECTING=1,
        DISCONNECTED=2
    };

    Q_ENUM(states)

    Q_PROPERTY(int              state           READ state          WRITE setState          NOTIFY stateChanged)
    Q_PROPERTY(QString          integrationId   READ integrationId  WRITE setIntegrationId  NOTIFY integrationIdChanged)
    Q_PROPERTY(QString          friendlyName    READ friendlyName   WRITE setFriendlyName)

    Q_INVOKABLE void            connect()       = 0;            // Must be implemented by integration
    Q_INVOKABLE void            disconnect()    = 0;            // Must be implemented by integration
    Q_INVOKABLE void            enterStandby()  {}              // Can be overriden by integration
    Q_INVOKABLE void            leaveStandby()  {}              // Can be overriden by integration
    Q_INVOKABLE void            sendCommand     (const QString& type, const QString& entity_id, int command, const QVariant& param) = 0;

    void                        setup           (const QVariantMap& config, QObject *entities)
    {
        for (QVariantMap::const_iterator iter = config.begin(); iter != config.end(); ++iter) {
            if (iter.key() == "friendly_name")
                m_friendlyName = iter.value().toString();
            else if (iter.key() == "id")
                m_integrationId =  iter.value().toString();
        }
        m_entities = qobject_cast<EntitiesInterface*>(entities);
    }

    ~Integration() {}

    // get the if the state
    int state() { return m_state; }

    // set the state
    void setState(int value)
    {
        m_state = value;
        if (m_state == CONNECTING) {
            emit connecting();
            emit stateChanged();
        } else if (m_state == CONNECTED){
            emit connected();
            emit stateChanged();
            m_entities->setConnected (m_integrationId, true);
        } else {
            emit disconnected();
            emit stateChanged();
            m_entities->setConnected (m_integrationId, false);
        }
    }

    // get the id of the integration
    QString integrationId() { return m_integrationId; }

    // set the id of the integration
    void setIntegrationId(QString value) {

        m_integrationId = value;
        emit integrationIdChanged();
    }

    // get the friendly name of the integration
    QString friendlyName() { return m_friendlyName; }

    // set the friendly name of the integration
    void setFriendlyName(QString value) { m_friendlyName = value; }

protected:
    int                 m_state = CONNECTED;
    QString             m_integrationId;
    QString             m_friendlyName;
    EntitiesInterface*  m_entities;

signals:
    void integrationIdChanged();
    void connected();
    void connecting();
    void disconnected();
    void stateChanged();

};

#endif // INTEGRATION_H
