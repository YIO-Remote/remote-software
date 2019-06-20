#ifndef INTEGRATION_H
#define INTEGRATION_H

#include <QObject>
#include <QQuickItem>

// Integration base class

class Integration : public QQuickItem
{
    Q_OBJECT

public:

    enum states {
        CONNECTED=0,
        CONNECTING=1,
        DISCONNECTED=2
    };

    Q_ENUM(states)

    Q_PROPERTY(states       state           READ state          WRITE setState)
    Q_PROPERTY(int          integrationId   READ integrationId  WRITE setIntegrationId  NOTIFY integrationIdChanged)
    Q_PROPERTY(QString      type            READ type           WRITE setType)
    Q_PROPERTY(QString      friendlyName    READ friendlyName   WRITE setFriendlyName)

    Integration() {}
    ~Integration() {}

    // get the if the state
    enum states state() { return m_state; }

    // set the state
    void setState(states value)
    {
        m_state = value;
        if (m_state == CONNECTED) {
            emit connected();
        } else if (m_state == CONNECTING){
            emit connecting();
        } else {
            emit disconnected();
        }
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
    states      m_state = CONNECTED;
    int         m_integrationId;
    QString     m_type;
    QString     m_friendlyName;

signals:
    void integrationIdChanged();
    void connected();
    void connecting();
    void disconnected();

};

#endif // INTEGRATION_H
