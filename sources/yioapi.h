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
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)        // returns the state of the API
    Q_PROPERTY(QString hostname READ hostname NOTIFY hostnameChanged)  // returns the hostname of the remote

    bool    running() const { return m_running; }
    QString hostname() const { return m_hostname; }

    Q_INVOKABLE void start() override;
    Q_INVOKABLE void stop() override;
    /**
     * @brief sendMessage Sends a message to all clients
     */
    Q_INVOKABLE void sendMessage(QString message) override;

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

    explicit YioAPI(QQmlApplicationEngine* engine = nullptr);
    ~YioAPI() override;

    static YioAPI* getInstance() { return s_instance; }

 signals:
    //    void closed();
    //    void messageReceived(QVariantMap message);
    void runningChanged();
    void hostnameChanged();
    void buttonPressed(QString button);
    void buttonReleased(QString button);

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void onClosed();
    void onNewConnection();
    void processMessage(QString message);
    void onClientDisconnected();

 private:
    QWebSocketServer*       m_server;
    QMap<QWebSocket*, bool> m_clients;  // websocket client, true if authentication was successful

    QList<QWebSocket*> m_subscribed_clients;
    QObject*           m_context;
    void               subscribeOnSignalEvent(const QString& event);

    bool m_running = false;

    static YioAPI*         s_instance;
    QQmlApplicationEngine* m_engine;

    QString m_token = "0";  // "c82b5fd6bea6fc3faf9a30bb864a9ee2"
    // QCryptographicHash::hash(m_token.toLocal8Bit(), QCryptographicHash::Sha512);
    QByteArray m_hash =
        "{U\xC0<$\xF7\n\xA7PA\xC3=\xBEk\xF5\xC1\xCA\x8B\t\x91\xA0\x9Et\xBA"
        "E\xE9\xA0)\xE4\x07^E\x04\x17Xg\xE4)\x04\xB7\xD4\x9D,\x19%\xD7\xA1\xDC\x84U\x83\xA2\xAA\x1D\xD7:\xBE\xF6"
        "1\xFA\x90\xED\x16\xBB";
    QString m_hostname;

    QZeroConf  m_zeroConf;
    QZeroConf* m_zeroConfBrowser;

    QStringList m_discoverableServices;
    QString     m_prevIp;

    Entities*     m_entities;
    Integrations* m_integrations;
    Config*       m_config;

    // API CALLS
    void apiSendResponse(QWebSocket* client, const int& id, const bool& success, QVariantMap response);

    void apiAuth(QWebSocket* client, const QVariantMap& map);

    void apiSystemButton(const int& id, const QVariantMap& map);
    void apiSystemReboot(QWebSocket* client, const int& id);
    void apiSystemShutdown(QWebSocket* client, const int& id);
    void apiSystemSubscribeToEvents(QWebSocket* client, const int& id);
    void apiSystemUnsubscribeFromEvents(QWebSocket* client, const int& id);

    void apiGetConfig(QWebSocket* client, const int& id);
    void apiSetConfig(QWebSocket* client, const int& id, const QVariantMap& map);

    void apiIntegrationsDiscover(QWebSocket* client, const int& id);
    void apiIntegrationsGetSupported(QWebSocket* client, const int& id);
    void apiIntegrationsGetLoaded(QWebSocket* client, const int& id);
    void apiIntegrationGetData(QWebSocket* client, const int& id, const QVariantMap& map);
    void apiIntegrationAdd(QWebSocket* client, const int& id, const QVariantMap& map);
    void apiIntegrationUpdate(QWebSocket* client, const int& id, const QVariantMap& map);
    void apiIntegrationRemove(QWebSocket* client, const int& id, const QVariantMap& map);

    void apiEntitiesGetSupported(QWebSocket* client, const int& id);
    void apiEntitiesGetLoaded(QWebSocket* client, const int& id);
    void apiEntitiesGetAvailable(QWebSocket* client, const int& id);
    void apiEntitiesAdd(QWebSocket* client, const int& id, const QVariantMap& map);
    void apiEntitiesUpdate(QWebSocket* client, const int& id, const QVariantMap& map);
    void apiEntitiesRemove(QWebSocket* client, const int& id, const QVariantMap& map);

    void apiProfilesGetAll(QWebSocket* client, const int& id);
    void apiProfilesSet(QWebSocket* client, const int& id, const QVariantMap& map);
    void apiProfilesAdd(QWebSocket* client, const int& id, const QVariantMap& map);
    void apiProfilesUpdate(QWebSocket* client, const int& id, const QVariantMap& map);
    void apiProfilesRemove(QWebSocket* client, const int& id, const QVariantMap& map);

    void apiPagesGetAll(QWebSocket* client, const int& id);
    void apiPagesAdd(QWebSocket* client, const int& id, const QVariantMap& map);
    void apiPagesUpdate(QWebSocket* client, const int& id, const QVariantMap& map);
    void apiPagesRemove(QWebSocket* client, const int& id, const QVariantMap& map);

    void apiGroupsGetAll(QWebSocket* client, const int& id);
    void apiGroupsAdd(QWebSocket* client, const int& id, const QVariantMap& map);
    void apiGroupsUpdate(QWebSocket* client, const int& id, const QVariantMap& map);
    void apiGroupsRemove(QWebSocket* client, const int& id, const QVariantMap& map);

    void apiSettingsGetAllLanguages(QWebSocket* client, const int& id);
    void apiSettingsSetLanguage(QWebSocket* client, const int& id, const QVariantMap& map);
    void apiSettingsSetAutoBrightness(QWebSocket* client, const int& id, const QVariantMap& map);
    void apiSettingsSetDarkMode(QWebSocket* client, const int& id, const QVariantMap& map);
};
