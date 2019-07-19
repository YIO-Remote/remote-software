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

    void sendCommand                (const QString& type, const QString& id, const QString& command, const QVariant& param) override;

private:
    void webSocketSendCommand	    (const QString& domain, const QString& service, const QString& entity_id, QVariantMap *data);
    int  convertBrightnessToPercentage (float value);


    void updateEntity               (const QString& entity_id, const QVariantMap& attr) override;
    void updateLight                (Entity* entity, const QVariantMap& attr);
    void updateBlind                (Entity* entity, const QVariantMap& attr);
    void updateMediaPlayer          (Entity* entity, const QVariantMap& attr);

    EntitiesInterface*              m_entities;
    NotificationsInterface*         m_notifications;

    QString                         m_ip;
    QString                         m_token;
    QWebSocket                      m_socket;
    QTimer                          m_websocketReconnect;
    int                             m_tries;
    int                             m_webSocketId;

};

#endif // HOMEASSISTANT_H
