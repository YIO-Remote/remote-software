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

#include "yioapi.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QNetworkInterface>
#include <QTimer>
#include <QtDebug>

#include "entities/entities.h"
#include "launcher.h"
#include "standbycontrol.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "api");

YioAPI *YioAPI::s_instance = nullptr;

YioAPI::YioAPI(QQmlApplicationEngine *engine) : m_engine(engine) {
    s_instance     = this;
    m_integrations = Integrations::getInstance();
    m_config       = Config::getInstance();
}

YioAPI::~YioAPI() { s_instance = nullptr; }

void YioAPI::start() {
    m_server = new QWebSocketServer(QStringLiteral("YIO API"), QWebSocketServer::NonSecureMode, this);

    // start websocket server on port 946(YIO)
    if (m_server->listen(QHostAddress::Any, 946)) {
        connect(m_server, &QWebSocketServer::newConnection, this, &YioAPI::onNewConnection);
        connect(m_server, &QWebSocketServer::closed, this, &YioAPI::closed);
        m_running = true;
        emit runningChanged();
    }

    QString macAddr;
    for (QNetworkInterface interface : QNetworkInterface::allInterfaces()) {
        if (!(interface.flags() & QNetworkInterface::IsLoopBack)) {
            macAddr = interface.hardwareAddress();
            break;
        }
    }

    macAddr.replace(":", "");
    m_hostname = "";
    m_hostname.append("YIO-Remote-").append(macAddr);
    qCDebug(CLASS_LC) << "NAME" << m_hostname;
    emit hostnameChanged();

    m_zeroConf.startServicePublish(m_hostname.toUtf8(), "_yio-remote._tcp", "local", 946);

    qCDebug(CLASS_LC) << "YIO api started";
}

void YioAPI::stop() {
    m_server->close();
    m_clients.clear();
    m_running = false;
    m_zeroConf.stopServicePublish();
    emit runningChanged();
}

void YioAPI::sendMessage(QString message) {
    QMap<QWebSocket *, bool>::iterator i;
    for (i = m_clients.begin(); i != m_clients.end(); i++) {
        if (i.value()) {
            QWebSocket *socket = i.key();
            socket->sendTextMessage(message);
        }
    }
}

QVariantMap YioAPI::getConfig() { return m_config->getConfig(); }

bool YioAPI::setConfig(QVariantMap config) {
    m_config->setConfig(config);
    if (!m_config->isValid()) {
        return false;
    }
    return m_config->writeConfig();
}

bool YioAPI::addEntityToConfig(QVariantMap entity) {
    // check the input if it's OK
    if (!entity.contains(Config::KEY_AREA) && !entity.contains(Config::KEY_ENTITY_ID) &&
        !entity.contains(Config::KEY_FRIENDLYNAME) && !entity.contains(Config::KEY_INTEGRATION) &&
        !entity.contains(Config::KEY_SUPPORTED_FEATURES) && !entity.contains(Config::KEY_TYPE)) {
        return false;
    }

    // get the config
    QVariantMap  c = getConfig();
    QVariantList e = c.value("entities").toJsonArray().toVariantList();

    // check what is the type of the new entity
    QString entityType = entity.value("type").toString();

    // find the entities key and insert the new entity
    for (int i = 0; i < e.length(); i++) {
        if (e[i].toMap().value("type").toString() == entityType) {
            // get the data key array
            QVariantMap  r  = e[i].toMap();
            QVariantList rl = r.value("data").toJsonArray().toVariantList();

            // add the entity
            rl.append(entity);

            r.insert("data", rl);

            e[i] = r;

            // put the entity back to the config
            c.insert("entities", e);

            // add it to the entity registry
            //            Entities::getInstance()->add(entity, );
        }
    }

    // write the config back
    m_config->setConfig(c);
    if (!m_config->isValid()) {
        return false;
    }

    return m_config->writeConfig();
}

bool YioAPI::removeIntegration(QString integrationId) {
    QObject *integration     = m_integrations->get(integrationId);
    QString  integrationType = m_integrations->getType(integrationId);

    QVariantMap config = getConfig();

    // unload all entities connected to the integration
    QList<EntityInterface *> entities = Entities::getInstance()->getByIntegration(integrationType);
    for (int i = 0; i < entities.length(); i++) {
        // remove entity from config and database
        Entities::getInstance()->remove(entities[i]->entity_id());
    }

    // remove all entities connected to the integration from the config
    QVariantMap configEntities = config.value("entities").toMap();

    // remove integration from database
    if (integration) {
        m_integrations->remove(integrationId);
    } else {
        return false;
    }

    // remove integration from config file
    QVariantMap  configIntegrations         = config.value("integrations").toMap();
    QVariantMap  configIntegrationsType     = configIntegrations.value(integrationType).toMap();
    QVariantList configIntegrationsTypeData = configIntegrationsType.value("data").toList();

    // iterate through the data and remove the integration config
    for (int i = 0; i < configIntegrationsTypeData.length(); i++) {
        QVariantMap item = configIntegrationsTypeData[i].toMap();
        if (item.value("id").toString() == integrationId) {
            configIntegrationsTypeData.removeAt(i);
            break;
        }
    }

    // write the config
    configIntegrationsType.insert("data", configIntegrationsTypeData);
    configIntegrations.insert(integrationType, configIntegrationsType);
    config.insert("integrations", configIntegrations);

    if (!setConfig(config)) {
        return false;
    }

    return true;
}

void YioAPI::discoverNetworkServices() {
    // FIXME this is not multi threading safe!
    // m_discoveredServices gets cleared if one of the discoverNetworkServices is called.
    // At the moment this works since we only have the dock integration using this functionality.
    // As soon as we have another integration plugin this can fail unpredictably since most integration plugins are
    // running in their own thread. I.e. during startup and service discovery these methods will be called in parallel!
    // Possible solution: use an identifier for discoverNetworkServices(), discoverNetworkServices(mdns) and
    // discoveredServices(), e.g. the mdns name itself: map with mdns -> QZeroConf
    // Possible solution 2: use a worker thread with signal / slot queuing
    m_discoveredServices.clear();

    // retrieve all configured mdns records from the integration plugins
    m_discoverableServices = Integrations::getInstance()->getMDNSList();

    for (int i = 0; i < m_discoverableServices.length(); i++) {
        // FIXME memory leak
        m_zeroConfBrowser = new QZeroConf;
        connect(m_zeroConfBrowser, &QZeroConf::serviceAdded, this, [=](QZeroConfService item) {
            QVariantMap txt;

            QMap<QByteArray, QByteArray> txtInfo = item->txt();

            QMap<QByteArray, QByteArray>::iterator qi;
            for (qi = txtInfo.begin(); qi != txtInfo.end(); ++qi) {
                txt.insert(qi.key(), qi.value());
            }

            QVariantMap map;
            map.insert(QString("name"), item->name());
            map.insert(QString("ip"), item->ip().toString());
            map.insert(QString("port"), item->port());
            map.insert(QString("mdns"), m_discoverableServices[i]);
            map.insert(QString("txt"), txt);
            m_discoveredServices.insert(item->name(), map);

            emit serviceDiscovered(m_discoveredServices);
            emit discoveredServicesChanged();
        });
        m_zeroConfBrowser->startBrowser(m_discoverableServices[i]);
    }
}

void YioAPI::discoverNetworkServices(QString mdns) {
    // FIXME this is not multi threading safe! See discoverNetworkServices()
    m_discoveredServices.clear();

    // FIXME memory leak
    m_zeroConfBrowser = new QZeroConf;

    connect(m_zeroConfBrowser, &QZeroConf::serviceAdded, this, [=](QZeroConfService item) {
        QVariantMap txt;

        QMap<QByteArray, QByteArray> txtInfo = item->txt();

        QMap<QByteArray, QByteArray>::iterator i;
        for (i = txtInfo.begin(); i != txtInfo.end(); ++i) {
            txt.insert(i.key(), i.value());
        }

        QVariantMap map;
        map.insert(QString("name"), item->name());
        map.insert(QString("ip"), item->ip().toString());
        map.insert(QString("port"), item->port());
        map.insert(QString("mdns"), mdns);
        map.insert(QString("txt"), txt);
        m_discoveredServices.insert(item->name(), map);

        emit serviceDiscovered(m_discoveredServices);
        emit discoveredServicesChanged();
    });

    m_zeroConfBrowser->startBrowser(mdns);
}

QVariantList YioAPI::discoveredServices() {
    // FIXME this is not multi threading safe! See discoverNetworkServices()
    QVariantList list;

    QMap<QString, QVariantMap>::iterator i;
    for (i = m_discoveredServices.begin(); i != m_discoveredServices.end(); i++) {
        QVariantMap map = i.value();
        list.append(map);
    }

    return list;
}

void YioAPI::onNewConnection() {
    QWebSocket *socket = m_server->nextPendingConnection();

    connect(socket, &QWebSocket::textMessageReceived, this, &YioAPI::processMessage);
    connect(socket, &QWebSocket::disconnected, this, &YioAPI::onClientDisconnected);

    // send message to client after connected to authenticate
    QVariantMap map;
    map.insert("type", "auth_required");
    QJsonDocument doc     = QJsonDocument::fromVariant(map);
    QString       message = doc.toJson(QJsonDocument::JsonFormat::Compact);

    socket->sendTextMessage(message);
    m_clients.insert(socket, false);
}

void YioAPI::processMessage(QString message) {
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());

    if (client) {
        // qDebug(CLASS_LC) << message;

        // convert message to json
        QJsonParseError parseerror;
        QJsonDocument   doc = QJsonDocument::fromJson(message.toUtf8(), &parseerror);
        if (parseerror.error != QJsonParseError::NoError) {
            qCWarning(CLASS_LC) << "JSON error:" << parseerror.errorString();
            return;
        }

        QVariantMap map  = doc.toVariant().toMap();
        QString     type = map.value("type").toString();
        int         id;

        if (map.contains("id")) {
            id = map.value("id").toInt();
        }

        if (type == "auth" && !m_clients[client]) {
            /// Authentication
            apiAuth(client, map);
        } else if (m_clients[client]) {
            if (type == "button") {
                /// Button simulation through the api
                apiSystemButton(id, map);
            } else if (type == "reboot") {
                /// Reboot
                apiSystemReboot(client, id);
            } else if (type == "shutdown") {
                /// Shutdown
                apiSystemShutdown(client, id);
            } else if (type == "get_config") {
                /// Get config
                apiGetConfig(client, id);
            } else if (type == "set_config") {
                /// Set config
                apiSetConfig(client, id, map);
            } else if (type == "get_supported_integrations") {
                /// Get supported integrations
                apiIntegrationsGetSupported(client, id);
            } else if (type == "get_loaded_integrations") {
                /// Get loaded integrations
                apiIntegrationsGetLoaded(client, id);
            } else if (type == "get_integration_setup_data") {
                /// Get data required to setup an integration
                apiIntegrationGetData(client, id, map);
            } else if (type == "add_integration") {
                /// Add a new integration
                apiIntegrationAdd(client, id, map);
            } else if (type == "update_integration") {
                /// Update an integration
                apiIntegrationUpdate(client, id, map);
            } else if (type == "remove_integration") {
                /// Remove an integration
                apiIntegrationRemove(client, id, map);
            }

            else if (type == "getEntities") {
                /// Get all available entities of an integration
                apiGetEntities(client, id, map);
            } else if (type == "getIntegrations") {
                /// Get all loaded integrations
                //                apiGetIntegrations(client, id);
            }
        } else {
            QVariantMap response;
            qCWarning(CLASS_LC) << "Client not authenticated";
            response.insert("type", "auth_error");
            response.insert("message", "Please authenticate");
            QJsonDocument json = QJsonDocument::fromVariant(response);
            client->sendTextMessage(json.toJson(QJsonDocument::JsonFormat::Compact));
            client->disconnect();
        }
    }
}

void YioAPI::onClientDisconnected() {
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        m_clients.remove(client);
        client->deleteLater();
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// API CALLS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void YioAPI::apiSendResponse(QWebSocket *client, const int &id, const bool &success, QVariantMap &response) {
    response.insert("id", id);
    response.insert("success", success);
    response.insert("type", "result");

    QJsonDocument json = QJsonDocument::fromVariant(response);
    client->sendTextMessage(json.toJson(QJsonDocument::JsonFormat::Compact));
    qCDebug(CLASS_LC()) << "Response sent to client:" << client << "id:" << id << "response:" << response;
}

void YioAPI::apiAuth(QWebSocket *client, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Client authenticating:" << m_clients[client];

    QVariantMap response;

    if (map.contains("token")) {
        qCDebug(CLASS_LC) << "Has token";

        // QByteArray hash = QCryptographicHash::hash(map.value("token").toString().toLocal8Bit(),
        //                                            QCryptographicHash::Sha512);

        if (map.value("token").toString() == m_token) {
            qDebug(CLASS_LC) << "Token OK";
            response.insert("type", "auth_ok");
            QJsonDocument json = QJsonDocument::fromVariant(response);
            client->sendTextMessage(json.toJson(QJsonDocument::JsonFormat::Compact));

            m_clients[client] = true;

            qCDebug(CLASS_LC) << "Client connected:" << client;

        } else {
            qCWarning(CLASS_LC) << "Token NOT OK";
            response.insert("type", "auth_error");
            response.insert("message", "Invalid token");
            QJsonDocument json = QJsonDocument::fromVariant(response);
            client->sendTextMessage(json.toJson(QJsonDocument::JsonFormat::Compact));
            client->disconnect();
        }
    } else {
        qCWarning(CLASS_LC) << "No token";
        response.insert("type", "auth_error");
        response.insert("message", "Token needed");
        QJsonDocument json = QJsonDocument::fromVariant(response);
        client->sendTextMessage(json.toJson(QJsonDocument::JsonFormat::Compact));
        client->disconnect();
    }
}

void YioAPI::apiSystemReboot(QWebSocket *client, const int &id) {
    Q_UNUSED(id);
    qCDebug(CLASS_LC) << "Request for reboot" << client;
    Launcher launcher;
    launcher.launch("reboot");
}

void YioAPI::apiSystemShutdown(QWebSocket *client, const int &id) {
    Q_UNUSED(id);
    qCDebug(CLASS_LC) << "Request for shutdown" << client;
    StandbyControl::getInstance()->shutdown();
}

void YioAPI::apiIntegrationsGetSupported(QWebSocket *client, const int &id) {
    qCDebug(CLASS_LC) << "Request for get supported integrations" << client;

    QVariantMap response;
    QStringList supportedIntegrations = m_integrations->supportedIntegrations();
    if (supportedIntegrations.length() > 1) {
        response.insert("supported_integrations", supportedIntegrations);
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiIntegrationsGetLoaded(QWebSocket *client, const int &id) {
    qCDebug(CLASS_LC) << "Request for get supported integrations" << client;

    QVariantMap response;
    QStringList loadedIntegrations = m_integrations->listIds();
    QVariantMap config             = getConfig().value("integrations").toMap();

    QVariantMap integrations;
    // iterate through the loaded integrations
    for (int i = 0; i < loadedIntegrations.length(); i++) {
        if (config.contains(loadedIntegrations[i])) {
            integrations.insert(loadedIntegrations[i], config.value(loadedIntegrations[i]).toMap());
        }
    }
    if (integrations.count() > 0) {
        response.insert("loaded_integrations", integrations);
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiIntegrationGetData(QWebSocket *client, const int &id, const QVariantMap &map) {
    QString integration = map.value("integration").toString();
    qCDebug(CLASS_LC) << "Request for get integration" << integration << "setup data" << client;

    QVariantMap response;
    bool        success = false;

    // check if the integration type is valid
    QStringList supportedIntegrations = m_integrations->supportedIntegrations();
    if (supportedIntegrations.contains(integration)) {
        success = true;

        // get data from integration
        QVariantMap data = m_integrations->getPluginMetaData(integration).toVariantMap().value("setup_data").toMap();
        response.insert("data", data);
    } else {
        success = false;
        response.insert("message", "Unsupported integration");
    }

    apiSendResponse(client, id, success, response);
}

void YioAPI::apiIntegrationAdd(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for add integration" << client;

    QVariantMap response;
}

void YioAPI::apiIntegrationUpdate(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for update integration" << client;

    QVariantMap response;
}

void YioAPI::apiIntegrationRemove(QWebSocket *client, const int &id, const QVariantMap &map) {
    QString integrationId = map.value("integration_id").toString();
    qCDebug(CLASS_LC) << "Request for remove integration" << integrationId << client;

    QVariantMap response;

    if (removeIntegration(integrationId)) {
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiGetConfig(QWebSocket *client, const int &id) {
    qCDebug(CLASS_LC) << "Request for get config" << client;

    QVariantMap response;
    QVariantMap config = getConfig();

    if (!config.isEmpty()) {
        response.insert("config", config);
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiSetConfig(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for set config" << client;

    QVariantMap response;
    QVariantMap config = map.value("config").toMap();

    if (setConfig(config)) {
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiSystemButton(const int &id, const QVariantMap &map) {
    Q_UNUSED(id);
    QString buttonName   = map["name"].toString();
    QString buttonAction = map["action"].toString();
    qDebug(CLASS_LC) << "Button simulation:" << buttonName << "," << buttonAction;

    if (buttonAction == "pressed") {
        emit buttonPressed(buttonName);
    } else {
        emit buttonReleased(buttonName);
    }
}

void YioAPI::apiGetEntities(QWebSocket *client, const int &id, const QVariantMap &map) {
    QVariantMap response;

    if (map.contains("integrationId")) {
        QString integrationId = map["integrationId"].toString();
        qCDebug(CLASS_LC) << "Request for getEntities" << integrationId << client;

        QObject *object = m_integrations->get(integrationId);

        if (object) {
            IntegrationInterface *integrationInterface = qobject_cast<IntegrationInterface *>(object);
            if (integrationInterface) {
                QStringList entitiesList = integrationInterface->getAllAvailableEntities();

                if (entitiesList.length() > 1) {
                    response.insert("success", true);
                    response.insert("type", "entities");
                    response.insert("entities", entitiesList);
                } else {
                    response.insert("success", false);
                    response.insert("type", "entities");
                }
            }
        } else {
            response.insert("success", false);
            response.insert("type", "entities");
        }
    } else {
        response.insert("success", false);
        response.insert("type", "entities");
    }

    QJsonDocument json = QJsonDocument::fromVariant(response);
    client->sendTextMessage(json.toJson(QJsonDocument::JsonFormat::Compact));
}

// void YioAPI::apiGetIntegrations(QWebSocket *client, const int &id) {
//    QVariantMap response;

//    qCDebug(CLASS_LC) << "Request for getIntegrations" << client;
//    QStringList integrations = m_integrations->listIds();

//    if (integrations.length() > 1) {
//        response.insert("success", true);
//        response.insert("type", "integrations");
//        response.insert("integrations", integrations);
//    } else {
//        response.insert("success", false);
//        response.insert("type", "integrations");
//    }

//    QJsonDocument json = QJsonDocument::fromVariant(response);
//    client->sendTextMessage(json.toJson(QJsonDocument::JsonFormat::Compact));
//}
