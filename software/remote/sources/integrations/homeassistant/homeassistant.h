#ifndef HOMEASSISTANT_H
#define HOMEASSISTANT_H

//#include <QtPlugin>
#include <QObject>
#include <QString>
#include <QColor>
#include <QVariant>
#include <QtWebSockets/QWebSocket>
#include <QTimer>

#include "../integration.h"
#include "../integrationinterface.h"

class HomeAssistant : public Integration, IntegrationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "YIO.IntegrationInterface" FILE "homeassistant.json")
    Q_INTERFACES(IntegrationInterface)

public:
    explicit HomeAssistant();

    Q_INVOKABLE void initialize	    (int integrationId, const QVariantMap& config, QObject *entities, QObject *notifications) override;
    Q_INVOKABLE void connect	    () override;
    Q_INVOKABLE void disconnect	    () override;

public slots:
    void onTextMessageReceived	    (const QString &message);
    void onStateChanged             (QAbstractSocket::SocketState state);
    void onError                    (QAbstractSocket::SocketError error);

    void onTimeout                  ();

    void sendCommand                (QString type, QString id, QString command, QVariant param) override;

private:
    void webSocketSendCommand	    (QString domain, QString service, QString entity_id, QVariantMap *data);
    int  convertBrightnessToPercentage (float value);


    void updateEntity               (QString entity_id, QVariantMap attr) override;
    void updateLight                (Entity* entity, QVariantMap& attr);
    void updateBlind                (Entity* entity, QVariantMap& attr);
    void updateMediaPlayer          (Entity* entity, QVariantMap& attr);

    EntitiesInterface*              m_entities;
    NotificationsInterface*         m_notifications;

    QString                         m_ip;
    QString                         m_token;
    QWebSocket                      m_socket;
    QTimer                          m_websocketReconnect;
    int                             m_tries;
    int                             m_webSocketId;
    bool                            m_userDisconnect = false;

};

#endif // HOMEASSISTANT_H
