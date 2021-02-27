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

#include <QHostInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

#include "launcher.h"
#include "logger.h"
#include "logging.h"
#include "standbycontrol.h"
#include "translation.h"

#define WEBSOCKET_CLIENT_DEBUG(message)                                \
    if (lcApi().isDebugEnabled()) {                                    \
        qCDebug(lcApi) << message << client->peerAddress().toString(); \
    }

YioAPI *YioAPI::getInstance() {
    static YioAPI api;
    return &api;
}

YioAPI::YioAPI() : m_port(946), m_server(nullptr), m_context(nullptr) {
    m_entities = Entities::getInstance();
    m_integrations = Integrations::getInstance();
    m_config = Config::getInstance();
}

YioAPI::~YioAPI() {
    if (m_server) {
        delete m_server;
    }
}

void YioAPI::start() {
    m_server = new QWebSocketServer(QStringLiteral("YIO API"), QWebSocketServer::NonSecureMode, this);

    // start websocket server
    if (m_server->listen(QHostAddress::Any, m_port)) {
        connect(m_server, &QWebSocketServer::newConnection, this, &YioAPI::onNewConnection);
        connect(m_server, &QWebSocketServer::closed, this, &YioAPI::onClosed);

        m_running = true;
        emit runningChanged();
    }

    m_hostname = QHostInfo::localHostName().split(QLatin1Char('.')).at(0);
    qCDebug(lcApi) << "YIO api published as ZeroConf service name:" << m_hostname;
    emit hostnameChanged();

    m_zeroConf.startServicePublish(m_hostname.toUtf8(), "_yio-remote._tcp", "local", m_port);

    qCDebug(lcApi) << "YIO api started on port" << m_port;
}

void YioAPI::stop() {
    m_server->close();
    m_clients.clear();
    m_running = false;
    m_zeroConf.stopServicePublish();
    emit runningChanged();
    qCDebug(lcApi) << "YIO api stopped";
}

void YioAPI::sendMessage(QString message) {
    if (m_logMsgPayload) {
        qCDebug(lcApi).noquote() << "MSG OUT:" << message;
    }

    QMap<QWebSocket *, bool>::iterator i;
    for (i = m_clients.begin(); i != m_clients.end(); i++) {
        if (i.value()) {
            QWebSocket *socket = i.key();
            socket->sendTextMessage(message);
        }
    }
}

QVariantMap YioAPI::getConfig() {
    return m_config->getConfig();
}

bool YioAPI::setConfig(QVariantMap config) {
    m_config->setConfig(config);
    if (!m_config->isValid()) {
        if (m_logMsgPayload) {
            QJsonDocument doc = QJsonDocument::fromVariant(config);
            QByteArray    json = doc.toJson(QJsonDocument::Compact);
            qCWarning(lcApi).noquote().nospace() << "Invalid configuration:\n"
                                                 << json << "\nValidation error: " << m_config->getError();
        } else {
            qCWarning(lcApi).noquote() << "Error setting invalid configuration:" << m_config->getError();
        }
        return false;
    }

    return m_config->writeConfig();
}

bool YioAPI::addEntity(QVariantMap entity) {
    // get the type of the new entity
    QString entityType = entity.value("type").toString();
    qCDebug(lcApi) << "Adding entity type:" << entityType;

    // remove the key that is not needed
    entity.remove("type");

    // check if the type is supported
    if (!m_entities->supportedEntities().contains(entityType)) {
        return false;
    }

    qCDebug(lcApi) << "Entity type is supported";

    // check the input if it's OK
    if (!entity.contains(Config::KEY_AREA) && !entity.contains(Config::KEY_ENTITY_ID) &&
        !entity.contains(Config::KEY_FRIENDLYNAME) && !entity.contains(Config::KEY_INTEGRATION) &&
        !entity.contains(Config::KEY_SUPPORTED_FEATURES) && !entity.contains(Config::KEY_TYPE)) {
        return false;
    }

    qCDebug(lcApi) << "Input data is OK.";

    // check if entity alread loaded. If so, it exist in config.json and the database
    QObject *eObj = m_entities->get(entity.value("entity_id").toString());

    qCDebug(lcApi) << "Entity object:" << eObj;
    if (eObj) {
        qCDebug(lcApi) << "Entity is loaded.";
        return false;
    }

    qCDebug(lcApi) << "Entity is not loaded.";

    // get the config
    QVariantMap  c = getConfig();
    QVariantMap  entities = c.value("entities").toMap();
    QVariantList entitiesType = entities.value(entityType).toList();

    // add the entity to the lsit
    entitiesType.append(entity);

    // put entities back to config
    entities.insert(entityType, entitiesType);
    c.insert("entities", entities);

    // write the config back
    bool success = setConfig(c);

    delete eObj;

    // if the config write is successful, load the entity to the database
    if (success) {
        // get the integration object
        QObject *obj = m_integrations->get(entity.value("integration").toString());
        if (obj) {
            IntegrationInterface *integration = qobject_cast<IntegrationInterface *>(obj);

            // add it to the entity registry
            m_entities->add(entityType, entity, integration);

            qCDebug(lcApi) << "Add entity success: true";

            return true;
        } else {
            qCDebug(lcApi) << "Add entity success: false";
            return false;
        }
    } else {
        qCDebug(lcApi) << "Add entity success: false";
        return false;
    }
}

bool YioAPI::updatEntity(QVariantMap entity) {
    qCDebug(lcApi) << "Update entity:" << entity.value("entity_id").toString();

    // remove entity
    if (!removeEntity(entity.value("entity_id").toString())) {
        return false;
    }

    // add new entity
    return addEntity(entity);
}

bool YioAPI::removeEntity(QString entityId) {
    qCDebug(lcApi) << "Removing entity:" << entityId;

    QObject *        o = m_entities->get(entityId);
    EntityInterface *eIface;
    if (o) {
        eIface = qobject_cast<EntityInterface *>(o);
        if (!eIface) {
            qCDebug(lcApi) << "Entity doesn't exist, probably already removed:" << entityId;
            return false;
        }
    }

    // remove entity from groups
    QVariantMap groups = m_config->getGroups();
    QVariantMap modifiedGroups;
    for (QVariantMap::const_iterator iter = groups.cbegin(); iter != groups.cend(); ++iter) {
        QVariantMap  item = iter.value().toMap();
        QVariantList groupEntities = item.value("entities").toList();
        for (int i = 0; i < groupEntities.length(); i++) {
            if (groupEntities[i].toString() == entityId) {
                qCDebug(lcApi) << "Removing entity" << entityId << "from group:" << iter.key();
                groupEntities.removeAt(i);
                item.insert("entities", groupEntities);
                modifiedGroups.insert(iter.key(), item);
                break;
            }
        }
    }
    for (QVariantMap::const_iterator iter = modifiedGroups.cbegin(); iter != modifiedGroups.cend(); ++iter) {
        groups.insert(iter.key(), iter.value());
    }
    m_config->setGroups(groups);

    // remove entity from favorites
    QVariantMap profiles = m_config->getProfiles();
    QVariantMap modifiedProfiles;
    for (QVariantMap::const_iterator iter = profiles.cbegin(); iter != profiles.cend(); ++iter) {
        QVariantMap  item = iter.value().toMap();
        QVariantList profileEntities = item.value("favorites").toList();
        for (int i = 0; i < profileEntities.length(); i++) {
            if (profileEntities[i].toString() == entityId) {
                qCDebug(lcApi) << "Removing entity" << entityId << "from favorites:" << iter.key();
                profileEntities.removeAt(i);
                item.insert("favorites", profileEntities);
                modifiedProfiles.insert(iter.key(), item);
                break;
            }
        }
    }
    for (QVariantMap::const_iterator iter = modifiedProfiles.cbegin(); iter != modifiedProfiles.cend(); ++iter) {
        profiles.insert(iter.key(), iter.value());
    }

    // remove from config
    // get the config
    eIface = qobject_cast<EntityInterface *>(o);
    QVariantMap  c = getConfig();
    QVariantMap  entities = c.value("entities").toMap();
    QVariantList entitiesType = entities.value(eIface->type()).toList();

    for (int i = 0; i < entitiesType.length(); i++) {
        if (entitiesType[i].toMap().value("entity_id").toString() == entityId) {
            qCDebug(lcApi) << "Removing entity definition:" << entityId;
            entitiesType.removeAt(i);
            break;
        }
    }

    // put entities back to config
    qCDebug(lcApi) << "put entities back to config without:" << entityId;
    entities.insert(eIface->type(), entitiesType);
    c.insert("entities", entities);

    delete eIface;

    // write the config back
    qCDebug(lcApi) << "Writing config without entity:" << entityId;
    bool success = setConfig(c);
    if (success) {
        // if it is a media player and playing, remove from mini media player
        m_entities->removeMediaplayersPlaying(entityId, true);

        // remove from database
        m_entities->remove(entityId);
        m_config->setProfiles(profiles);
        qCDebug(lcApi) << "Removing entity success:" << entityId;
        return true;
    } else {
        qCDebug(lcApi) << "Removing entity failure:" << entityId;
        return false;
    }
}

bool YioAPI::addIntegration(QVariantMap integration) {
    // get the type of the new integration
    QString integrationType = integration.value("type").toString();
    QString integrationId = integration.value("id").toString();

    qCDebug(lcApi) << "Adding integration type:" << integrationType;

    // remove the key that is not needed
    integration.remove("type");

    // check if the type is supported
    if (!m_integrations->supportedIntegrations().contains(integrationType)) {
        return false;
    }

    qCDebug(lcApi) << "Integration type is supported";

    // check if the input is OK
    if (!integration.contains(Config::KEY_TYPE) && !integration.contains(Config::KEY_ID) &&
        !integration.contains(Config::KEY_FRIENDLYNAME) && !integration.contains(Config::OBJ_DATA)) {
        return false;
    }

    qCDebug(lcApi) << "Input data is OK.";

    // check if the integration already exists
    QObject *iObj = m_integrations->get(integrationId);
    if (iObj) {
        IntegrationInterface *ii = qobject_cast<IntegrationInterface *>(iObj);
        if (ii) {
            if (ii->integrationId() == integrationId) {
                return false;
            }
        } else {
            return false;
        }
    }

    delete iObj;

    // get the config
    QVariantMap  c = getConfig();
    QVariantMap  integrations = c.value("integrations").toMap();
    QVariantMap  integrationsType = integrations.value(integrationType).toMap();
    QVariantList integrationsData = integrationsType.value("data").toList();

    // add the new integration to the list
    integrationsData.append(integration);

    // put integrations back to config
    integrationsType.insert("data", integrationsData);
    integrationsType.insert("mdns",
                            m_integrations->getPluginMetaData(integrationType).toVariantMap().value("mdns").toString());
    integrations.insert(integrationType, integrationsType);
    c.insert("integrations", integrations);

    // write the config back
    bool success = setConfig(c);

    if (success) {
        // load the integrations
        m_integrations->load();
        return true;
    } else {
        return false;
    }
}

bool YioAPI::updateIntegration(QVariantMap integration) {
    // get the integration of the new integration
    QString integrationType = integration.value("type").toString();

    // remove the key that is not needed
    integration.remove("type");

    // check if the type is supported
    if (!m_integrations->supportedIntegrations().contains(integrationType)) {
        return false;
    }

    // check if the input is OK
    if (!integration.contains(Config::KEY_TYPE) && !integration.contains(Config::KEY_ID) &&
        !integration.contains(Config::KEY_FRIENDLYNAME) && !integration.contains(Config::OBJ_DATA)) {
        return false;
    }

    // check if the integration already exists
    QObject *iObj = m_integrations->get(integration.value("id").toString());
    if (!iObj) {
        return false;
    }

    // delete iObj;

    // get the config
    QVariantMap  c = getConfig();
    QVariantMap  integrations = c.value("integrations").toMap();
    QVariantMap  integrationsType = integrations.value(integrationType).toMap();
    QVariantList integrationsData = integrationsType.value("data").toList();

    bool success = false;

    for (int i = 0; i < integrationsData.length(); i++) {
        if (integrationsData[i].toMap().value("id").toString() == integration.value("id").toString()) {
            QVariantMap ii = integrationsData[i].toMap();
            ii.insert("friendly_name", integration.value("friendly_name").toString());
            ii.insert("data", integration.value("data").toMap());
            integrationsData[i] = ii;
            success = true;
        }
    }

    if (!success) {
        return false;
    }

    // put integrations back to config
    integrationsType.insert("data", integrationsData);
    integrations.insert(integrationType, integrationsType);
    c.insert("integrations", integrations);

    // write the config back
    return setConfig(c);
}

bool YioAPI::removeIntegration(QString integrationId) {
    QString integrationType = m_integrations->getType(integrationId);

    // unload all entities connected to the integration
    QList<EntityInterface *> entities = m_entities->getByIntegration(integrationId);
    for (int i = 0; i < entities.length(); i++) {
        // remove entity from config and database
        if (entities[i]->integration() == integrationId) {
            if (!removeEntity(entities[i]->entity_id())) {
                return false;
            }
        }
    }

    // remove integration from database
    QObject *integration = m_integrations->get(integrationId);
    if (integration) {
        m_integrations->remove(integrationId);
    }

    // remove integration from config file
    QVariantMap  config = getConfig();
    QVariantMap  configIntegrations = config.value("integrations").toMap();
    QVariantMap  configIntegrationsType = configIntegrations.value(integrationType).toMap();
    QVariantList configIntegrationsTypeData = configIntegrationsType.value("data").toList();

    // iterate through the data and remove the integration config
    // FIXME doesn't seem to always remove the integration from the "integrations/TYPE/data/" section
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

    // write the config back
    return setConfig(config);
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

    // retrieve all supported mdns records from the integration plugins
    m_discoverableServices = Integrations::getInstance()->getMDNSList();

    for (int i = 0; i < m_discoverableServices.length(); i++) {
        if (m_discoverableServices[i] != "") {
            qCDebug(lcApi) << "Starting mdns discovery" << m_discoverableServices[i];
            discoverNetworkServices(m_discoverableServices[i]);
        }
    }
}

void YioAPI::discoverNetworkServices(QString mdns) {
    QZeroConf *zeroConfBrowser = new QZeroConf(this);
    QObject *  context = new QObject();
    m_prevIp = "";

    connect(zeroConfBrowser, &QZeroConf::serviceAdded, context, [=](QZeroConfService item) {
        QVariantMap txt;

        QMap<QByteArray, QByteArray> txtInfo = item->txt();

        QMap<QByteArray, QByteArray>::iterator i;
        for (i = txtInfo.begin(); i != txtInfo.end(); ++i) {
            txt.insert(i.key(), i.value());
        }

        if (m_prevIp != item->ip().toString()) {
            qCDebug(lcApi) << "Zeroconf found" << item;

            m_prevIp = item->ip().toString();

            QVariantMap map;
            map.insert(QString("name"), item->name());
            map.insert(QString("ip"), m_prevIp);
            map.insert(QString("port"), item->port());
            map.insert(QString("mdns"), mdns);
            map.insert(QString("txt"), txt);

            QVariantMap discoveredServices;
            discoveredServices.insert(item->name(), map);

            emit serviceDiscovered(discoveredServices);
        }
    });

    connect(zeroConfBrowser, &QZeroConf::serviceUpdated, context, [=](QZeroConfService item) {
        QVariantMap txt;

        QMap<QByteArray, QByteArray> txtInfo = item->txt();

        QMap<QByteArray, QByteArray>::iterator i;
        for (i = txtInfo.begin(); i != txtInfo.end(); ++i) {
            txt.insert(i.key(), i.value());
        }

        if (m_prevIp != item->ip().toString()) {
            qCDebug(lcApi) << "Zeroconf updated" << item;

            m_prevIp = item->ip().toString();

            QVariantMap map;
            map.insert(QString("name"), item->name());
            map.insert(QString("ip"), m_prevIp);
            map.insert(QString("port"), item->port());
            map.insert(QString("mdns"), mdns);
            map.insert(QString("txt"), txt);

            QVariantMap discoveredServices;
            discoveredServices.insert(item->name(), map);

            emit serviceDiscovered(discoveredServices);
        }
    });

    if (!zeroConfBrowser->browserExists()) {
        zeroConfBrowser->startBrowser(mdns);
        QTimer::singleShot(10000, [=]() {
            qCDebug(lcApi) << "Stopping mdns discovery after 10 seconds";
            if (context) {
                context->deleteLater();
            }
            qCDebug(lcApi) << "Context deleted";
            if (zeroConfBrowser->browserExists()) {
                zeroConfBrowser->stopBrowser();
            }
            qCDebug(lcApi) << "Zeroconf browser stopped";
            if (zeroConfBrowser) {
                zeroConfBrowser->deleteLater();
            }
            qCDebug(lcApi) << "Zeroconf browser deleted";
        });
    }
}

void YioAPI::onNewConnection() {
    QWebSocket *socket = m_server->nextPendingConnection();

    connect(socket, &QWebSocket::textMessageReceived, this, &YioAPI::processMessage);
    connect(socket, &QWebSocket::disconnected, this, &YioAPI::onClientDisconnected);

    // send message to client after connected to authenticate
    QVariantMap map;
    map.insert("type", "auth_required");
    QJsonDocument doc = QJsonDocument::fromVariant(map);
    QString       message = doc.toJson(QJsonDocument::JsonFormat::Compact);

    if (m_logMsgPayload) {
        qCDebug(lcApi).noquote() << "MSG OUT:" << message;
    }
    socket->sendTextMessage(message);
    m_clients.insert(socket, false);
}

void YioAPI::onClosed() {}

void YioAPI::processMessage(const QString &message) {
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());

    if (m_logMsgPayload) {
        qCDebug(lcApi).noquote() << "MSG IN:" << message;
    }

    if (client) {
        // convert message to json
        QJsonParseError parseerror;
        QJsonDocument   doc = QJsonDocument::fromJson(message.toUtf8(), &parseerror);
        if (parseerror.error != QJsonParseError::NoError) {
            qCWarning(lcApi) << "JSON error:" << parseerror.errorString();
            return;
        }

        QVariantMap map = doc.toVariant().toMap();
        QString     type = map.value("type").toString();

        if (type == "auth") {
            /// Authentication
            if (!m_clients[client]) {
                apiAuth(client, map);
            }
        } else if (m_clients[client]) {
            int id;

            if (map.contains("id")) {
                id = map.value("id").toInt();
            } else {
                qCWarning(lcApi) << "Missing id property in message:" << message;
                id = -1;
            }

            if (type == "button") {
                /// Button simulation through the api
                apiSystemButton(id, map);
            } else if (type == "reboot") {
                /// Reboot
                apiSystemReboot(client, id);
            } else if (type == "shutdown") {
                /// Shutdown
                apiSystemShutdown(client, id);
            } else if (type == "subscribe_events") {
                /// Subscribe to events
                apiSystemSubscribeToEvents(client, id);
            } else if (type == "unsubscribe_events") {
                /// Unsubscribe from events
                apiSystemUnsubscribeFromEvents(client, id);
            } else if (type == "get_config") {
                /// Get config
                apiGetConfig(client, id);
            } else if (type == "set_config") {
                /// Set config
                apiSetConfig(client, id, map);
            } else if (type == "discover_integrations") {
                /// Discover integrations
                apiIntegrationsDiscover(client, id);
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
            } else if (type == "get_supported_entities") {
                /// Get supported entities
                apiEntitiesGetSupported(client, id);
            } else if (type == "get_loaded_entities") {
                /// Get loaded entities
                apiEntitiesGetLoaded(client, id);
            } else if (type == "get_available_entities") {
                /// Get available entities from integrations
                apiEntitiesGetAvailable(client, id);
            } else if (type == "add_entity") {
                /// Get available entities from integrations
                apiEntitiesAdd(client, id, map);
            } else if (type == "update_entity") {
                /// Get available entities from integrations
                apiEntitiesUpdate(client, id, map);
            } else if (type == "remove_entity") {
                /// Get available entities from integrations
                apiEntitiesRemove(client, id, map);
            } else if (type == "get_all_profiles") {
                /// Get all profiles
                apiProfilesGetAll(client, id);
            } else if (type == "set_profile") {
                /// Set current profile
                apiProfilesSet(client, id, map);
            } else if (type == "add_profile") {
                /// Add new profile
                apiProfilesAdd(client, id, map);
            } else if (type == "update_profile") {
                /// Update a profile
                apiProfilesUpdate(client, id, map);
            } else if (type == "remove_profile") {
                /// Remove a profile
                apiProfilesRemove(client, id, map);
            } else if (type == "get_all_pages") {
                /// Get all pages
                apiPagesGetAll(client, id);
            } else if (type == "add_page") {
                /// Add a page
                apiPagesAdd(client, id, map);
            } else if (type == "update_page") {
                /// Update a page
                apiPagesUpdate(client, id, map);
            } else if (type == "remove_page") {
                /// Remove a page
                apiPagesRemove(client, id, map);
            } else if (type == "get_all_groups") {
                /// Get all groups
                apiGroupsGetAll(client, id);
            } else if (type == "add_group") {
                /// Add a group
                apiGroupsAdd(client, id, map);
            } else if (type == "update_group") {
                /// Update a group
                apiGroupsUpdate(client, id, map);
            } else if (type == "remove_group") {
                /// Remove a group
                apiGroupsRemove(client, id, map);
            } else if (type == "get_languages") {
                /// Get all languages
                apiSettingsGetAllLanguages(client, id);
            } else if (type == "set_language") {
                /// Set a languages
                apiSettingsSetLanguage(client, id, map);
            } else if (type == "set_auto_brightness") {
                /// Set auto brightness
                apiSettingsSetAutoBrightness(client, id, map);
            } else if (type == "set_dark_mode") {
                /// Set dark mode
                apiSettingsSetDarkMode(client, id, map);
            } else if (type == "log") {
                /// Logger control
                apiLoggerControl(client, id, map);
            }

        } else {
            QVariantMap response;
            qCWarning(lcApi) << "Client not authenticated";
            response.insert("type", "auth_error");
            response.insert("message", "Please authenticate");
            QJsonDocument json = QJsonDocument::fromVariant(response);
            QString       message = json.toJson(QJsonDocument::JsonFormat::Compact);
            if (m_logMsgPayload) {
                qCDebug(lcApi).noquote() << "MSG OUT:" << message;
            }
            client->sendTextMessage(message);
            client->disconnect();
        }
    }
}

void YioAPI::onClientDisconnected() {
    qCDebug(lcApi) << "Client disconnected";
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        client->close();
        WEBSOCKET_CLIENT_DEBUG("Client closed")
        m_clients.remove(client);
        client->deleteLater();
        qCDebug(lcApi) << "Client removed";
    }
}

void YioAPI::subscribeOnSignalEvent(const QString &event) {
    QVariantMap response;
    response.insert("event", event);
    QJsonDocument json = QJsonDocument::fromVariant(response);
    QString       message = json.toJson(QJsonDocument::JsonFormat::Compact);
    if (m_logMsgPayload) {
        qCDebug(lcApi).noquote() << "MSG OUT:" << message;
    } else {
        qCDebug(lcApi) << "Sending message to all subscribed clients";
    }

    for (int i = 0; i < m_subscribed_clients.length(); i++) {
        m_subscribed_clients[i]->sendTextMessage(message);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// API CALLS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void YioAPI::apiSendSuccessResponse(QWebSocket *client, int id) {
    QVariantMap response;
    apiSendResponse(client, id, true, &response);
}

void YioAPI::apiSendErrorResponse(QWebSocket *client, int id) {
    QVariantMap response;
    apiSendResponse(client, id, false, &response);
}

void YioAPI::apiSendResponse(QWebSocket *client, int id, bool success, QVariantMap *response) {
    Q_ASSERT(response);

    if (!(client && m_clients.contains(client) && client->isValid())) {
        return;
    }

    response->insert("id", id);
    response->insert("success", success);
    response->insert("type", "result");

    QJsonDocument json = QJsonDocument::fromVariant(*response);
    QString       message = json.toJson(QJsonDocument::JsonFormat::Compact);
    client->sendTextMessage(message);

    if (m_logMsgPayload) {
        qCDebug(lcApi).noquote().nospace() << "MSG OUT (" << client->peerAddress().toString() << "): " << message;
    } else {
        WEBSOCKET_CLIENT_DEBUG(QString("Sent %1 response to client").arg(success ? "success" : "error"))
    }
}

void YioAPI::apiAuth(QWebSocket *client, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG(QString("Client is authenticated: %1").arg(m_clients[client]));
    //    qCDebug(lcApi) << "Client" << client << "is authenticated:" << m_clients[client];

    QVariantMap response;

    if (map.contains("token")) {
        qCDebug(lcApi) << "Has token";

        // QByteArray hash = QCryptographicHash::hash(map.value("token").toString().toLocal8Bit(),
        //                                            QCryptographicHash::Sha512);

        if (map.value("token").toString() == m_token) {
            qCDebug(lcApi) << "Token OK";
            response.insert("type", "auth_ok");
            QJsonDocument json = QJsonDocument::fromVariant(response);
            client->sendTextMessage(json.toJson(QJsonDocument::JsonFormat::Compact));

            m_clients[client] = true;

            WEBSOCKET_CLIENT_DEBUG("Client connected:")

        } else {
            qCWarning(lcApi) << "Token NOT OK";
            response.insert("type", "auth_error");
            response.insert("message", "Invalid token");
            QJsonDocument json = QJsonDocument::fromVariant(response);
            client->sendTextMessage(json.toJson(QJsonDocument::JsonFormat::Compact));
            client->disconnect();
        }
    } else {
        qCWarning(lcApi) << "No token";
        response.insert("type", "auth_error");
        response.insert("message", "Token needed");
        QJsonDocument json = QJsonDocument::fromVariant(response);
        client->sendTextMessage(json.toJson(QJsonDocument::JsonFormat::Compact));
        client->disconnect();
    }
}

void YioAPI::apiSystemButton(int id, const QVariantMap &map) {
    Q_UNUSED(id);
    QString buttonName = map["name"].toString();
    QString buttonAction = map["action"].toString();
    qCDebug(lcApi) << "Button simulation:" << buttonName << "," << buttonAction;

    if (buttonAction == "pressed") {
        emit buttonPressed(buttonName);
    } else {
        emit buttonReleased(buttonName);
    }
}

void YioAPI::apiSystemReboot(QWebSocket *client, int id) {
    Q_UNUSED(id);

#if defined(Q_OS_LINUX)
#if defined(Q_PROCESSOR_ARM)
    WEBSOCKET_CLIENT_DEBUG("Request for reboot")
    Launcher launcher;
    launcher.launch("reboot");
    return;
#endif
#endif
    Q_UNUSED(client);
    qCInfo(lcApi) << "Ignoring reboot! Please restart app";
}

void YioAPI::apiSystemShutdown(QWebSocket *client, int id) {
    Q_UNUSED(id);
    WEBSOCKET_CLIENT_DEBUG("Request for shutdown")
    StandbyControl::getInstance()->shutdown();
}

void YioAPI::apiSystemSubscribeToEvents(QWebSocket *client, int id) {
    WEBSOCKET_CLIENT_DEBUG("Request for subscribe to events")

    if (m_subscribed_clients.contains(client)) {
        apiSendErrorResponse(client, id);
        return;
    }

    // add client to the subscribed client list
    m_subscribed_clients.append(client);

    // connect to signals. FIXME handle multiple clients and perform proper cleanup
    m_context = new QObject(this);

    QObject::connect(m_config, &Config::configChanged, m_context, [=]() { subscribeOnSignalEvent("config_changed"); });
    QObject::connect(m_config, &Config::settingsChanged, m_context,
                     [=]() { subscribeOnSignalEvent("settings_changed"); });
    QObject::connect(m_config, &Config::profileIdChanged, m_context,
                     [=]() { subscribeOnSignalEvent("profileId_changed"); });
    QObject::connect(m_config, &Config::profileFavoritesChanged, m_context,
                     [=]() { subscribeOnSignalEvent("profileFavorites_changed"); });
    QObject::connect(m_config, &Config::profilesChanged, m_context,
                     [=]() { subscribeOnSignalEvent("profiles_changed"); });
    QObject::connect(m_config, &Config::uiConfigChanged, m_context,
                     [=]() { subscribeOnSignalEvent("uiConfig_changed"); });
    QObject::connect(m_config, &Config::pagesChanged, m_context, [=]() { subscribeOnSignalEvent("pages_changed"); });
    QObject::connect(m_config, &Config::groupsChanged, m_context, [=]() { subscribeOnSignalEvent("groups_changed"); });

    apiSendSuccessResponse(client, id);
}

void YioAPI::apiSystemUnsubscribeFromEvents(QWebSocket *client, int id) {
    WEBSOCKET_CLIENT_DEBUG("Request for unsubscribe from events")

    if (!m_subscribed_clients.contains(client)) {
        apiSendErrorResponse(client, id);
        return;
    }

    // FIXME handle multiple clients and perform proper cleanup
    if (m_context) {
        QObject::disconnect(m_context);
        m_context->deleteLater();
    }

    for (int i = 0; i < m_subscribed_clients.length(); i++) {
        if (m_subscribed_clients[i] == client) {
            m_subscribed_clients.removeAt(i);
            apiSendSuccessResponse(client, id);
            return;
        }
    }

    apiSendErrorResponse(client, id);
}

void YioAPI::apiGetConfig(QWebSocket *client, int id) {
    WEBSOCKET_CLIENT_DEBUG("Request for get config")

    QVariantMap config = getConfig();

    if (!config.isEmpty()) {
        QVariantMap response;
        response.insert("config", config);
        apiSendResponse(client, id, true, &response);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiSetConfig(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for set config")

    QVariantMap config = map.value("config").toMap();

    if (setConfig(config)) {
        apiSendSuccessResponse(client, id);
    } else {
        QVariantMap response;
        response.insert("message", m_config->getError());
        apiSendResponse(client, id, false, &response);
    }
}

void YioAPI::apiIntegrationsDiscover(QWebSocket *client, int id) {
    WEBSOCKET_CLIENT_DEBUG("Request for discover integrations")

    QTimer * timeOutTimer = new QTimer();
    QObject *context = new QObject(this);

    QObject::connect(this, &YioAPI::serviceDiscovered, context, [=](QVariantMap services) {
        qCDebug(lcApi) << "Service discovered";
        QVariantMap response;
        QVariantMap map;

        // let's go through the returned list of discovered integrations
        QVariantMap::iterator i;
        for (i = services.begin(); i != services.end(); i++) {
            QString friendlyName = i.value().toMap().value("txt").toMap().value("FriendlyName").toString();
            if (friendlyName.isEmpty()) {
                friendlyName = i.value().toMap().value("name").toString();
            }
            map.insert("friendly_name", friendlyName);
            map.insert("ip", i.value().toMap().value("ip").toString());
            map.insert("type", m_integrations->getTypeByMdns(i.value().toMap().value("mdns").toString()));
        }

        response.insert("discovered_integration", map);
        apiSendResponse(client, id, true, &response);
    });

    discoverNetworkServices();

    // start a timeout timer if nothing is discovered
    timeOutTimer->setSingleShot(true);

    connect(timeOutTimer, &QTimer::timeout, this, [=]() {
        QVariantMap response;
        response.insert("message", "discovery_done");
        qCDebug(lcApi) << "API Discovery timeout, response created.";
        apiSendResponse(client, id, true, &response);

        if (context) {
            context->deleteLater();
        }
        timeOutTimer->deleteLater();
    });
    timeOutTimer->start(10000);
}

void YioAPI::apiIntegrationsGetSupported(QWebSocket *client, int id) {
    WEBSOCKET_CLIENT_DEBUG("Request for get supported integrations")

    QStringList supportedIntegrations = m_integrations->supportedIntegrations();
    if (supportedIntegrations.length() > 0) {
        QVariantMap response;
        response.insert("supported_integrations", supportedIntegrations);
        apiSendResponse(client, id, true, &response);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiIntegrationsGetLoaded(QWebSocket *client, int id) {
    WEBSOCKET_CLIENT_DEBUG("Request for get supported integrations")

    QStringList loadedIntegrations = m_integrations->listIds();
    QVariantMap config = getConfig().value("integrations").toMap();

    QVariantMap integrations;
    // iterate through the loaded integrations
    for (int i = 0; i < loadedIntegrations.length(); i++) {
        if (config.contains(loadedIntegrations[i])) {
            integrations.insert(loadedIntegrations[i], config.value(loadedIntegrations[i]).toMap());
        }
    }
    if (integrations.count() > 0) {
        QVariantMap response;
        response.insert("loaded_integrations", integrations);
        apiSendResponse(client, id, true, &response);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiIntegrationGetData(QWebSocket *client, int id, const QVariantMap &map) {
    QString integration = map.value("integration").toString();
    WEBSOCKET_CLIENT_DEBUG("Request for get integration" << integration << "setup data")

    QVariantMap response;
    bool        success = false;

    // check if the integration type is valid
    if (m_integrations->supportedIntegrations().contains(integration)) {
        success = true;

        // get data from integration
        QVariantMap data = m_integrations->getPluginMetaData(integration).toVariantMap().value("setup_data").toMap();
        response.insert("data", data);
    } else {
        success = false;
        response.insert("message", "Unsupported integration");
    }

    apiSendResponse(client, id, success, &response);
}

void YioAPI::apiIntegrationAdd(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for add integration")

    if (addIntegration(map.value("config").toMap())) {
        apiSendSuccessResponse(client, id);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiIntegrationUpdate(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for update integration")

    if (updateIntegration(map.value("config").toMap())) {
        QVariantMap response;
        response.insert("message", "Restart the remote to update the integration.");
        apiSendResponse(client, id, true, &response);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiIntegrationRemove(QWebSocket *client, int id, const QVariantMap &map) {
    QString integrationId = map.value("integration_id").toString();
    WEBSOCKET_CLIENT_DEBUG("Request for remove integration" << integrationId)

    if (removeIntegration(integrationId)) {
        apiSendSuccessResponse(client, id);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiEntitiesGetSupported(QWebSocket *client, int id) {
    WEBSOCKET_CLIENT_DEBUG("Request for get supported entities")

    QStringList supportedEntities = m_entities->supportedEntities();
    if (supportedEntities.length() > 0) {
        QVariantMap response;
        response.insert("supported_entities", supportedEntities);
        apiSendResponse(client, id, true, &response);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiEntitiesGetLoaded(QWebSocket *client, int id) {
    WEBSOCKET_CLIENT_DEBUG("Request for get loaded entities")

    QVariantMap loadedEntities = getConfig().value("entities").toMap();

    if (!loadedEntities.isEmpty()) {
        QVariantMap response;
        response.insert("loaded_entities", loadedEntities);
        apiSendResponse(client, id, true, &response);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiEntitiesGetAvailable(QWebSocket *client, int id) {
    WEBSOCKET_CLIENT_DEBUG("Request for get all available entities")

    QList<QObject *> integrations = m_integrations->list();
    if (integrations.isEmpty()) {
        apiSendErrorResponse(client, id);
        return;
    }

    QVariantList availableEntities;
    for (int i = 0; i < integrations.length(); i++) {
        if (integrations[i]) {
            IntegrationInterface *ii = qobject_cast<IntegrationInterface *>(integrations[i]);
            if (ii) {
                availableEntities.append(ii->getAllAvailableEntities());
            }
        }
    }

    QVariantMap response;
    if (availableEntities.isEmpty()) {
        response.insert("available_entities", QVariantList());
        apiSendResponse(client, id, true, &response);
        return;
    }

    response.insert("available_entities", availableEntities);
    apiSendResponse(client, id, true, &response);
}

void YioAPI::apiEntitiesAdd(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for add entity")

    if (addEntity(map.value("config").toMap())) {
        apiSendSuccessResponse(client, id);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiEntitiesUpdate(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for update entity")

    if (updatEntity(map.value("config").toMap())) {
        apiSendSuccessResponse(client, id);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiEntitiesRemove(QWebSocket *client, int id, const QVariantMap &map) {
    QString entityId = map.value("entity_id").toString();
    WEBSOCKET_CLIENT_DEBUG("Request for remove entity" << entityId)

    if (removeEntity(entityId)) {
        apiSendSuccessResponse(client, id);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiProfilesGetAll(QWebSocket *client, int id) {
    WEBSOCKET_CLIENT_DEBUG("Request for get all profiles")

    QVariantMap profiles = m_config->getProfiles();

    if (!profiles.isEmpty()) {
        QVariantMap response;
        response.insert("profiles", profiles);
        apiSendResponse(client, id, true, &response);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiProfilesSet(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for set profile")

    QString newProfileId = map.value("profile").toString();

    if (!newProfileId.isEmpty()) {
        if (m_config->getProfileId() != newProfileId) {
            m_config->setProfileId(newProfileId);
        }
        apiSendSuccessResponse(client, id);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiProfilesAdd(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for add profile")

    QVariantMap profiles = m_config->getProfiles();

    QVariantMap newProfile = map.value("profile").toMap();
    if (!newProfile.isEmpty()) {
        for (QVariantMap::const_iterator iter = newProfile.cbegin(); iter != newProfile.cend(); ++iter) {
            profiles.insert(iter.key(), iter.value().toMap());
        }

        m_config->setProfiles(profiles);

        apiSendSuccessResponse(client, id);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiProfilesUpdate(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for update profile")

    if (!map.value("data").toMap().isEmpty()) {
        QVariantMap profiles = m_config->getProfiles();
        profiles.insert(map.value("uuid").toString(), map.value("data").toMap());
        m_config->setProfiles(profiles);
        apiSendSuccessResponse(client, id);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiProfilesRemove(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for remove profile")

    bool        success = false;
    QVariantMap profiles = m_config->getProfiles();

    if (profiles.count() == 1) {
        QVariantMap response;
        response.insert("message", "There is only one profile. Cannot be removed.");
        apiSendResponse(client, id, false, &response);
        return;
    }

    for (QVariantMap::const_iterator iter = profiles.cbegin(); iter != profiles.cend(); ++iter) {
        if (iter.key() == map.value("profile_id").toString()) {
            profiles.remove(iter.key());
            success = true;
            break;
        }
    }

    if (success) {
        m_config->setProfileId(profiles.lastKey());
        m_config->setProfiles(profiles);
        apiSendSuccessResponse(client, id);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiPagesGetAll(QWebSocket *client, int id) {
    WEBSOCKET_CLIENT_DEBUG("Request for get all pages")

    QVariantMap pages = m_config->getPages();
    if (!pages.isEmpty()) {
        QVariantMap response;
        response.insert("pages", pages);
        apiSendResponse(client, id, true, &response);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiPagesAdd(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for add a page")

    QVariantMap pages = m_config->getPages();
    QVariantMap newPage = map.value("page").toMap();

    if (!newPage.isEmpty()) {
        for (QVariantMap::const_iterator iter = newPage.cbegin(); iter != newPage.cend(); ++iter) {
            pages.insert(iter.key(), iter.value().toMap());
            m_config->setPages(pages);
            apiSendSuccessResponse(client, id);
        }

    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiPagesUpdate(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for update page")

    if (!map.value("data").toMap().isEmpty()) {
        QVariantMap pages = m_config->getPages();
        pages.insert(map.value("uuid").toString(), map.value("data").toMap());
        m_config->setPages(pages);
        apiSendSuccessResponse(client, id);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiPagesRemove(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for remove page")

    bool success = false;

    // remove the page from the profiles
    QVariantMap profiles = m_config->getProfiles();

    for (QVariantMap::const_iterator iter = profiles.cbegin(); iter != profiles.cend(); ++iter) {
        QVariantMap profile = iter.value().toMap();
        QStringList pages = profile.value("pages").toStringList();

        for (int i = 0; i < pages.length(); i++) {
            if (pages[i] == map.value("page_id").toString()) {
                pages.removeAt(i);
                break;
            }
        }

        profile.insert("pages", pages);
        profiles.insert(iter.key(), profile);
    }

    m_config->setProfiles(profiles);

    // remove the page from the profiles list
    QVariantMap pages = m_config->getPages();

    for (QVariantMap::const_iterator iter = pages.cbegin(); iter != pages.cend(); ++iter) {
        if (iter.key() == map.value("page_id").toString()) {
            pages.remove(iter.key());
            m_config->setPages(pages);
            success = true;
            break;
        }
    }

    if (success) {
        apiSendSuccessResponse(client, id);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiGroupsGetAll(QWebSocket *client, int id) {
    WEBSOCKET_CLIENT_DEBUG("Request for get all groups")

    QVariantMap groups = m_config->getGroups();
    if (!groups.isEmpty()) {
        QVariantMap response;
        response.insert("groups", groups);
        apiSendResponse(client, id, true, &response);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiGroupsAdd(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for add a group")

    QVariantMap groups = m_config->getGroups();
    QVariantMap newGroup = map.value("group").toMap();

    if (!newGroup.isEmpty()) {
        for (QVariantMap::const_iterator iter = newGroup.cbegin(); iter != newGroup.cend(); ++iter) {
            groups.insert(iter.key(), iter.value().toMap());
            m_config->setGroups(groups);
            apiSendSuccessResponse(client, id);
        }

    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiGroupsUpdate(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for update group")

    if (!map.value("data").toMap().isEmpty()) {
        QVariantMap groups = m_config->getGroups();
        groups.insert(map.value("uuid").toString(), map.value("data").toMap());
        m_config->setGroups(groups);
        apiSendSuccessResponse(client, id);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiGroupsRemove(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for remove group")

    bool success = false;

    QVariantMap groups = m_config->getGroups();

    for (QVariantMap::const_iterator iter = groups.cbegin(); iter != groups.cend(); ++iter) {
        if (iter.key() == map.value("group_id").toString()) {
            groups.remove(iter.key());
            m_config->setGroups(groups);
            success = true;
            break;
        }
    }

    if (success) {
        apiSendSuccessResponse(client, id);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiSettingsGetAllLanguages(QWebSocket *client, int id) {
    WEBSOCKET_CLIENT_DEBUG("Request for get all languages")

    QVariantList languages = m_config->getLanguages();
    if (!languages.isEmpty()) {
        QVariantMap response;
        response.insert("languages", languages);
        apiSendResponse(client, id, true, &response);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiSettingsSetLanguage(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for set a language")

    if (!map.value("language").toString().isEmpty()) {
        // need to check if the data is valid
        QVariantMap settings = m_config->getSettings();
        settings.insert("language", map.value("language").toString());
        m_config->setSettings(settings);
        TranslationHandler::getInstance()->selectLanguage(map.value("language").toString());
        apiSendSuccessResponse(client, id);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiSettingsSetAutoBrightness(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for set auto brightness")

    QVariantMap settings = m_config->getSettings();
    if (map.value("value").toBool() || !map.value("value").toBool()) {
        settings.insert("autobrightness", map.value("value").toBool());
        m_config->setSettings(settings);
        apiSendSuccessResponse(client, id);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiSettingsSetDarkMode(QWebSocket *client, int id, const QVariantMap &map) {
    WEBSOCKET_CLIENT_DEBUG("Request for set dark mode")

    QVariantMap uiConfig = m_config->getUIConfig();
    if (map.value("value").toBool() || !map.value("value").toBool()) {
        uiConfig.insert("darkmode", map.value("value").toBool());
        m_config->setUIConfig(uiConfig);
        apiSendSuccessResponse(client, id);
    } else {
        apiSendErrorResponse(client, id);
    }
}

void YioAPI::apiLoggerControl(QWebSocket *client, int id, const QVariantMap &map) {
    // Handle log
    Logger *logger = Logger::instance();
    QString logAction = map["action"].toString();
    QString logTarget = map["target"].toString();

    qCDebug(lcApi) << "YioAPI LOGGER : " << logAction;

    if (logAction == "start") {
        // enable logger target, default is queue
        if (logTarget == "file") {
            logger->setFileEnabled(true);
        } else if (logTarget == "console") {
            logger->setDefaultHandlerEnabled(true);
        } else {
            logger->setQueueEnabled(true);
        }
        apiSendSuccessResponse(client, id);
    } else if (logAction == "stop") {
        // disable logger queue target, default is queue
        if (logTarget == "file") {
            logger->setFileEnabled(false);
        } else if (logTarget == "console") {
            logger->setDefaultHandlerEnabled(false);
        } else {
            logger->setQueueEnabled(false);
        }
        apiSendSuccessResponse(client, id);
    } else if (logAction == "show_source") {
        // show source position in log line
        logger->setShowSourcePos(true);
        apiSendSuccessResponse(client, id);
    } else if (logAction == "hide_source") {
        // hide source position in log line
        logger->setShowSourcePos(false);
        apiSendSuccessResponse(client, id);
    } else if (logAction == "purge") {
        // purge log files
        int hours = 5 * 24;
        if (map.contains("days")) {
            hours = map["days"].toInt() * 24;
        } else if (map.contains("hours")) {
            hours = map["hours"].toInt();
        }
        logger->purgeFiles(hours);
        apiSendSuccessResponse(client, id);
    } else if (logAction == "clear_log_level") {
        logger->clearLogRules();
        apiSendSuccessResponse(client, id);
    } else if (logAction == "set_log_level") {
        // set log level
        QString level = QStringLiteral("DEBUG");
        QString category;
        if (map.contains("level")) {
            level = map["level"].toString();
        }
        if (map.contains("category")) {
            category = map["category"].toString();
            logger->addLogRule({category + QLatin1Char('=') + level.toLower()});
        } else {
            logger->setLogLevel(level);
        }
        apiSendSuccessResponse(client, id);
    } else if (logAction == "get_messages") {
        // get log messages
        int         count = 50;
        QtMsgType   level = QtDebugMsg;
        QStringList categories;
        if (map.contains("count")) {
            count = map["count"].toInt();
        }
        if (map.contains("level")) {
            level = logger->toMsgType(map["level"].toString());
        }
        if (map.contains("categories")) {
            categories = map["categories"].toStringList();
        }
        QJsonArray  messages = logger->getQueuedMessages(count, level, categories);
        QJsonObject jsonObj;
        jsonObj.insert("id", id);
        jsonObj.insert("success", true);
        jsonObj.insert("type", "result");
        jsonObj.insert("messages", messages);
        QJsonDocument json = QJsonDocument(jsonObj);
        if (client && m_clients.contains(client) && client->isValid()) {
            client->sendTextMessage(json.toJson(QJsonDocument::JsonFormat::Compact));
        }
    } else if (logAction == "get_info") {
        // get log info
        QJsonObject info = logger->getInformation();
        QJsonObject jsonObj;
        jsonObj.insert("id", id);
        jsonObj.insert("success", true);
        jsonObj.insert("type", "result");
        jsonObj.insert("info", info);
        QJsonDocument json = QJsonDocument(jsonObj);
        if (client && m_clients.contains(client) && client->isValid()) {
            client->sendTextMessage(json.toJson(QJsonDocument::JsonFormat::Compact));
        }
    } else {
        qCWarning(lcApi) << "YioAPI Bad LOGGER Action : " << logAction;
    }
}
