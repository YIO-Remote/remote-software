/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
 *
 * This file is part of the YIO-Remote software project.
 *
 * YIO-Remote software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * YIO-Remote software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with YIO-Remote software. If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *****************************************************************************/
#pragma once

#include <QCryptographicHash>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>

#include "../qtzeroconf/qzeroconf.h"
#include "config.h"
#include "entities/entities.h"
#include "integrations/integrations.h"
#include "yio-interface/yioapiinterface.h"

class YioAPI : public YioAPIInterface {
    Q_OBJECT
    Q_INTERFACES(YioAPIInterface)

 public:
    static YioAPI* getInstance();

    Q_PROPERTY(bool running READ running NOTIFY runningChanged)        // returns the state of the API
    Q_PROPERTY(QString hostname READ hostname NOTIFY hostnameChanged)  // returns the hostname of the remote

    bool    running() const { return m_running; }
    QString hostname() const { return m_hostname; }

    /**
     * @brief Enables message payload logging
     * @details Warning: this is only intended for debugging. It's very verbose and exposes access tokens in the log!
     */
    void setLogMsgPayload(bool enabled) { m_logMsgPayload = enabled; }
    bool isLogMsgPayload() const { return m_logMsgPayload; }

    Q_INVOKABLE void start() override;
    Q_INVOKABLE void stop() override;
    /**
     * @brief sendMessage Sends a message to all clients
     */
    Q_INVOKABLE void sendMessage(QString message) override;

    void    setWebSocketPort(quint16 port) { m_port = port; }
    quint16 webSocketPort() { return m_port; }

    // CONFIG MANIPULATION METHODS
    QVariantMap getConfig() override;
    bool        setConfig(QVariantMap config);

    bool addEntity(QVariantMap entity) override;
    bool updatEntity(QVariantMap entity) override;
    bool removeEntity(QString entityId) override;

    Q_INVOKABLE bool addIntegration(QVariantMap integration);
    bool             updateIntegration(QVariantMap integration);
    bool             removeIntegration(QString integrationId);

    // NETWORK SERVICES DISCOVERY
    void             discoverNetworkServices() override;
    Q_INVOKABLE void discoverNetworkServices(QString mdns) override;

 signals:
    void runningChanged();
    void hostnameChanged();
    void buttonPressed(QString button);
    void buttonReleased(QString button);

 public slots:
    void onClosed();
    void onNewConnection();
    void processMessage(const QString& message);
    void onClientDisconnected();

 private:
    YioAPI();
    ~YioAPI() override;

    void subscribeOnSignalEvent(const QString& event);

    // API CALLS
    /**
     * @brief Helper method to send a success response without additional payload
     */
    void apiSendSuccessResponse(QWebSocket* client, int id);
    /**
     * @brief Helper method to send an error response without additional payload
     */
    void apiSendErrorResponse(QWebSocket* client, int id);
    void apiSendResponse(QWebSocket* client, int id, bool success, QVariantMap* response);

    void apiAuth(QWebSocket* client, const QVariantMap& map);

    void apiSystemButton(int id, const QVariantMap& map);
    void apiSystemReboot(QWebSocket* client, int id);
    void apiSystemShutdown(QWebSocket* client, int id);
    void apiSystemSubscribeToEvents(QWebSocket* client, int id);
    void apiSystemUnsubscribeFromEvents(QWebSocket* client, int id);

    void apiGetConfig(QWebSocket* client, int id);
    void apiSetConfig(QWebSocket* client, int id, const QVariantMap& map);

    void apiIntegrationsDiscover(QWebSocket* client, int id);
    void apiIntegrationsGetSupported(QWebSocket* client, int id);
    void apiIntegrationsGetLoaded(QWebSocket* client, int id);
    void apiIntegrationGetData(QWebSocket* client, int id, const QVariantMap& map);
    void apiIntegrationAdd(QWebSocket* client, int id, const QVariantMap& map);
    void apiIntegrationUpdate(QWebSocket* client, int id, const QVariantMap& map);
    void apiIntegrationRemove(QWebSocket* client, int id, const QVariantMap& map);

    void apiEntitiesGetSupported(QWebSocket* client, int id);
    void apiEntitiesGetLoaded(QWebSocket* client, int id);
    void apiEntitiesGetAvailable(QWebSocket* client, int id);
    void apiEntitiesAdd(QWebSocket* client, int id, const QVariantMap& map);
    void apiEntitiesUpdate(QWebSocket* client, int id, const QVariantMap& map);
    void apiEntitiesRemove(QWebSocket* client, int id, const QVariantMap& map);

    void apiProfilesGetAll(QWebSocket* client, int id);
    void apiProfilesSet(QWebSocket* client, int id, const QVariantMap& map);
    void apiProfilesAdd(QWebSocket* client, int id, const QVariantMap& map);
    void apiProfilesUpdate(QWebSocket* client, int id, const QVariantMap& map);
    void apiProfilesRemove(QWebSocket* client, int id, const QVariantMap& map);

    void apiPagesGetAll(QWebSocket* client, int id);
    void apiPagesAdd(QWebSocket* client, int id, const QVariantMap& map);
    void apiPagesUpdate(QWebSocket* client, int id, const QVariantMap& map);
    void apiPagesRemove(QWebSocket* client, int id, const QVariantMap& map);

    void apiGroupsGetAll(QWebSocket* client, int id);
    void apiGroupsAdd(QWebSocket* client, int id, const QVariantMap& map);
    void apiGroupsUpdate(QWebSocket* client, int id, const QVariantMap& map);
    void apiGroupsRemove(QWebSocket* client, int id, const QVariantMap& map);

    void apiSettingsGetAllLanguages(QWebSocket* client, int id);
    void apiSettingsSetLanguage(QWebSocket* client, int id, const QVariantMap& map);
    void apiSettingsSetAutoBrightness(QWebSocket* client, int id, const QVariantMap& map);
    void apiSettingsSetDarkMode(QWebSocket* client, int id, const QVariantMap& map);

    void apiLoggerControl(QWebSocket* client, int id, const QVariantMap& map);

 private:
    quint16                 m_port;
    bool                    m_running = false;
    bool                    m_logMsgPayload = false;
    QWebSocketServer*       m_server;
    QMap<QWebSocket*, bool> m_clients;  // websocket client, true if authentication was successful

    QList<QWebSocket*> m_subscribed_clients;
    QObject*           m_context;

    QString m_token = "0";  // "c82b5fd6bea6fc3faf9a30bb864a9ee2"
    // QCryptographicHash::hash(m_token.toLocal8Bit(), QCryptographicHash::Sha512);
    QByteArray m_hash =
        "{U\xC0<$\xF7\n\xA7PA\xC3=\xBEk\xF5\xC1\xCA\x8B\t\x91\xA0\x9Et\xBA"
        "E\xE9\xA0)\xE4\x07^E\x04\x17Xg\xE4)\x04\xB7\xD4\x9D,\x19%\xD7\xA1\xDC\x84U\x83\xA2\xAA\x1D\xD7:\xBE\xF6"
        "1\xFA\x90\xED\x16\xBB";
    QString m_hostname;

    QZeroConf m_zeroConf;

    QStringList m_discoverableServices;
    QString     m_prevIp;

    Entities*     m_entities;
    Integrations* m_integrations;
    Config*       m_config;
};
