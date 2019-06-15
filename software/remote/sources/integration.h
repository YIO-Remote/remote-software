#ifndef INTEGRATION_H
#define INTEGRATION_H

#include <QObject>
#include <QQuickItem>

// Integration base class

class Integration : public QQuickItem
{
    Q_OBJECT

public:
    Q_PROPERTY(bool         connected       READ connected      WRITE setConnected      NOTIFY connectedChanged)
    Q_PROPERTY(int          integrationId   READ integrationId  WRITE setIntegrationId  NOTIFY integrationIdChanged)
    Q_PROPERTY(QString      type            READ type           WRITE setType)
    Q_PROPERTY(QString      friendlyName    READ friendlyName   WRITE setFriendlyName)

    Integration() {}
    ~Integration() {}

    // get the if the integration is connected
    bool connected() { return m_connected; }

    // set the connected state of the integration
    void setConnected(bool value)
    {
        m_connected = value;
        emit connectedChanged();
    }

    // get the id of the integration
    int integrationId() { return m_integrationId; }

    // set the id of the integration
    void setIntegrationId(int value) {

        m_integrationId = value;
        emit integrationIdChanged();
    }

    // get the type of the integration
    QString type() { return m_type; }

    // set the type of the integration
    void setType(QString value) { m_type = value; }

    // get the friendly name of the integration
    QString friendlyName() { return m_friendlyName; }

    // set the friendly name of the integration
    void setFriendlyName(QString value) { m_friendlyName = value; }

private:
    bool        m_connected = false;
    int         m_integrationId;
    QString     m_type;
    QString     m_friendlyName;

signals:
    void connectedChanged();
    void integrationIdChanged();

};

#endif // INTEGRATION_H
