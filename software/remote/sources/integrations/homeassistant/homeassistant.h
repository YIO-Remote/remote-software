#ifndef HOMEASSISTANT_H
#define HOMEASSISTANT_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QVariant>
#include <QtWebSockets/QWebSocket>
#include <QTimer>
#include <QThread>

#include "../integration.h"
#include "../integrationinterface.h"

class HomeAssistant : public Integration, IntegrationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "YIO.IntegrationInterface" FILE "homeassistant.json")
    Q_INTERFACES(IntegrationInterface)

public:
    explicit HomeAssistant() {}

    Q_INVOKABLE void initialize	    (int integrationId, const QVariantMap& config, QObject *entities, QObject *notifications) override;
    Q_INVOKABLE void connect	    () override;
    Q_INVOKABLE void disconnect	    () override;

signals:
    void connectSignal              ();
    void disconnectSignal           ();
    void sendCommandSignal          (const QString& type, const QString& entity_id, const QString& command, const QVariant& param);


public slots:
    void sendCommand                (const QString& type, const QString& entity_id, const QString& command, const QVariant& param) override;
    void stateHandler               (int state);

private:
    void updateEntity               (const QString& entity_id, const QVariantMap& attr) override {}

    QThread                         m_thread;
};


class HomeAssistantThread : public QObject
{
    Q_OBJECT

public:
    HomeAssistantThread             (const QVariantMap &config, QObject *entities, QObject *notifications);

signals:
    void stateChanged               (int state);

public slots:
    void connect                    ();
    void disconnect                 ();

    void sendCommand                (const QString& type, const QString& entity_id, const QString& command, const QVariant& param);

    void onTextMessageReceived	    (const QString &message);
    void onStateChanged             (QAbstractSocket::SocketState state);
    void onError                    (QAbstractSocket::SocketError error);

    void onTimeout                  ();


private:
    void webSocketSendCommand	    (const QString& domain, const QString& service, const QString& entity_id, QVariantMap *data);
    int  convertBrightnessToPercentage (float value);

    void updateEntity               (const QString& entity_id, const QVariantMap& attr);
    void updateLight                (Entity* entity, const QVariantMap& attr);
    void updateBlind                (Entity* entity, const QVariantMap& attr);
    void updateMediaPlayer          (Entity* entity, const QVariantMap& attr);

    void setState                   (int state);

    EntitiesInterface*              m_entities;
    NotificationsInterface*         m_notifications;

    QString                         m_ip;
    QString                         m_token;
    QWebSocket*                     m_socket;
    QTimer                          m_websocketReconnect;
    int                             m_tries;
    int                             m_webSocketId;
    bool                            m_userDisconnect = false;

    int                             m_state = 0;

};


#endif // HOMEASSISTANT_H
