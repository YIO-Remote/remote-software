#ifndef HOMEASSISTANT_H
#define HOMEASSISTANT_H

#include <QtPlugin>
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

    Q_INVOKABLE void initialize	    (int integrationId, const QVariantMap& config, QObject *entities) override;
    Q_INVOKABLE void connect	    () override;
    Q_INVOKABLE void disconnect	    () override;
    Q_INVOKABLE bool sendCommand    (const QString& type, const QString& id, const QString& command, const QVariant& param) override;

signals:
    void notify();

public slots:
    void onTextMessageReceived	    (const QString &message);
    void onStateChanged             (QAbstractSocket::SocketState state);
    void onError                    (QAbstractSocket::SocketError error);

    void onTimeout                  ();

private:
    void updateEntity               (const QString& entity_id, const QString& state, const QVariantMap& attributes);
    void webSocketSendCommand	    (const QString& domain, const QString& service, const QString& entity_id, QVariantMap *data);

    EntitiesInterface*  m_entities;

    QString 			m_ip;
    QString			    m_token;
    QWebSocket			m_socket;
    QTimer			    m_websocketReconnect;
    int				    m_tries;
    int				    m_webSocketId;

};

#endif // HOMEASSISTANT_H
