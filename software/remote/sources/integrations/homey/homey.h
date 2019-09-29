#ifndef HOMEY_H
#define HOMEY_H

#include <QObject>
#include <QString>
#include <QColor>
#include <QVariant>
#include <QtWebSockets/QWebSocket>
#include <QTimer>
#include <QThread>

#include "../integration.h"
#include "../integrationinterface.h"

class Homey : public Integration, IntegrationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "YIO.IntegrationInterface" FILE "homey.json")
    Q_INTERFACES(IntegrationInterface)

public:
    explicit Homey() {}

    Q_INVOKABLE void initialize	    (int integrationId, const QVariantMap& config, QObject *entities, QObject *notifications, QObject* api, QObject *configObj) override;
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


class HomeyThread : public QObject
{
    Q_OBJECT

public:
    HomeyThread                     (const QVariantMap &config, QObject *entities, QObject *notifications, QObject* api, QObject *configObj);

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
    void webSocketSendCommand	    (QVariantMap data);
    int  convertBrightnessToPercentage (float value);

    void updateEntity               (const QString& entity_id, const QVariantMap& attr);
    void updateLight                (Entity* entity, const QVariantMap& attr);
    void updateBlind                (Entity* entity, const QVariantMap& attr);
    void updateMediaPlayer          (Entity* entity, const QVariantMap& attr);

    void setState                   (int state);

    EntitiesInterface*              m_entities;
    NotificationsInterface*         m_notifications;
    YioAPIInterface*                m_api;
    ConfigInterface*                m_config;

    QString                         m_ip;
    QString                         m_token;
    QWebSocket*                     m_socket;
    QTimer*                         m_websocketReconnect;
    int                             m_tries;
    int                             m_webSocketId;
    bool                            m_userDisconnect = false;

    int                             m_state = 0;

};


#endif // HOMEY_H
