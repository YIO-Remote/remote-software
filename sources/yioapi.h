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

#ifndef YIOAPI_H_
#define YIOAPI_H_

#include <QObject>
#include <QQmlApplicationEngine>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketServer>
#include <QCryptographicHash>
#include <QLoggingCategory>

#include "yioapiinterface.h"
#include "../qtzeroconf/qzeroconf.h"
#include "config.h"
#include "integrations/integrations.h"
#include "logger.h"

class YioAPI : public YioAPIInterface
{
    Q_OBJECT
    Q_INTERFACES(YioAPIInterface)

 public:
    // returns the state of the API
    Q_PROPERTY(bool running       READ running        NOTIFY runningChanged)
    // returns the hostname of the remote
    Q_PROPERTY(QString hostname   READ hostname       NOTIFY hostnameChanged)

    Q_INVOKABLE void start() override;
    Q_INVOKABLE void stop() override;
    /**
     * @brief sendMessage Sends a message to all clients
     */
    Q_INVOKABLE void sendMessage(QString message) override;


    // CONFIG MANIPULATION METHODS
    Q_INVOKABLE QVariantMap getConfig() override;
    Q_INVOKABLE bool        setConfig(QVariantMap config);
    Q_INVOKABLE bool        addEntityToConfig(QVariantMap entity) override;


    // NETWORK SERVICES DISCOVERY
    Q_PROPERTY(QVariantList discoveredServices        READ discoveredServices   NOTIFY discoveredServicesChanged)
    Q_INVOKABLE void discoverNetworkServices() override;
    Q_INVOKABLE void discoverNetworkServices(QString mdns) override;

    QVariantList discoveredServices();


    bool running() const { return m_running; }

    QString hostname() const { return m_hostname; }

    explicit YioAPI(QQmlApplicationEngine *engine = nullptr);
    ~YioAPI() override;

    static YioAPI*  getInstance() { return s_instance; }

 signals:
    void closed();
    void messageReceived(QVariantMap message);
    void runningChanged();
    void hostnameChanged();
    void discoveredServicesChanged();
    void buttonPressed(QString button);
    void buttonReleased(QString button);

 public slots: //NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void onNewConnection();
    void processMessage(QString message);
    void onClientDisconnected();

 private:
    QLoggingCategory             m_log;
    QWebSocketServer*            m_server;
    QMap<QWebSocket *, bool>     m_clients;  // websocket client, true if authentication was successful

    bool                         m_running = false;

    static YioAPI*               s_instance;
    QQmlApplicationEngine*       m_engine;

    QString                      m_token = "0";  // "c82b5fd6bea6fc3faf9a30bb864a9ee2"
    // QCryptographicHash::hash(m_token.toLocal8Bit(), QCryptographicHash::Sha512);
    QByteArray                   m_hash = "{U\xC0<$\xF7\n\xA7PA\xC3=\xBEk\xF5\xC1\xCA\x8B\t\x91\xA0\x9Et\xBA""E\xE9\xA0)\xE4\x07^E\x04\x17Xg\xE4)\x04\xB7\xD4\x9D,\x19%\xD7\xA1\xDC\x84U\x83\xA2\xAA\x1D\xD7:\xBE\xF6""1\xFA\x90\xED\x16\xBB"; //NOLINT
    QString                      m_hostname;

    QZeroConf                    m_qzero_conf;
    QZeroConf*                   m_qzero_conf_browser;

    QStringList                  m_discoverableServices;
    // name as string, <ip address as string, mdns name as string>  "192.169.100.1", 496
    QMap<QString, QVariantMap>   m_discoveredServices;
};

#endif  // YIOAPI_H_
