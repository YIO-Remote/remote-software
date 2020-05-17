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

#include "launcher.h"
#include "standbycontrol.h"
#include "translation.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "api");

YioAPI *YioAPI::s_instance = nullptr;

YioAPI::YioAPI(QQmlApplicationEngine *engine) : m_engine(engine) {
    s_instance     = this;
    m_entities     = Entities::getInstance();
    m_integrations = Integrations::getInstance();
    m_config       = Config::getInstance();
}

YioAPI::~YioAPI() { s_instance = nullptr; }

void YioAPI::start() {
    m_server = new QWebSocketServer(QStringLiteral("YIO API"), QWebSocketServer::NonSecureMode, this);

    // start websocket server on port 946(YIO)
    if (m_server->listen(QHostAddress::Any, 946)) {
        connect(m_server, &QWebSocketServer::newConnection, this, &YioAPI::onNewConnection);
        connect(m_server, &QWebSocketServer::closed, this, &YioAPI::onClosed);

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

bool YioAPI::addEntity(QVariantMap entity) {
    // get the type of the new entity
    QString entityType = entity.value("type").toString();
    qCDebug(CLASS_LC) << "Adding entity type:" << entityType;

    // remove the key that is not needed
    entity.remove("type");

    // check if the type is supported
    if (!m_entities->supportedEntities().contains(entityType)) {
        return false;
    }

    qCDebug(CLASS_LC) << "Entity type is supported";

    // check the input if it's OK
    if (!entity.contains(Config::KEY_AREA) && !entity.contains(Config::KEY_ENTITY_ID) &&
        !entity.contains(Config::KEY_FRIENDLYNAME) && !entity.contains(Config::KEY_INTEGRATION) &&
        !entity.contains(Config::KEY_SUPPORTED_FEATURES) && !entity.contains(Config::KEY_TYPE)) {
        return false;
    }

    qCDebug(CLASS_LC) << "Input data is OK.";

    // check if entity alread loaded. If so, it exist in config.json and the database
    QObject *eObj = m_entities->get(entity.value("entity_id").toString());

    qCDebug(CLASS_LC) << "Entity object:" << eObj;
    if (eObj) {
        qCDebug(CLASS_LC) << "Entity is loaded.";
        return false;
    }

    qCDebug(CLASS_LC) << "Entity is not loaded.";

    // get the config
    QVariantMap  c            = getConfig();
    QVariantMap  entities     = c.value("entities").toMap();
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

            qCDebug(CLASS_LC) << "Add entity success: true";

            return true;
        } else {
            qCDebug(CLASS_LC) << "Add entity success: false";
            return false;
        }
    } else {
        qCDebug(CLASS_LC) << "Add entity success: false";
        return false;
    }
}

bool YioAPI::updatEntity(QVariantMap entity) {
    qCDebug(CLASS_LC) << "Update entity:" << entity.value("entity_id").toString();

    // remove entity
    if (!removeEntity(entity.value("entity_id").toString())) {
        return false;
    }

    // add new entity
    return addEntity(entity);
}

bool YioAPI::removeEntity(QString entityId) {
    qCDebug(CLASS_LC) << "Removing entity:" << entityId;

    QObject *        o = m_entities->get(entityId);
    EntityInterface *eIface;
    if (o) {
        eIface = qobject_cast<EntityInterface *>(o);
    } else {
        return false;
    }

    // remove entity from groups
    QVariantMap groups = m_config->getGroups();
    for (QVariantMap::const_iterator iter = groups.begin(); iter != groups.end(); ++iter) {
        QVariantMap  item          = iter.value().toMap();
        QVariantList groupEntities = item.value("entities").toList();
        for (int i = 0; i < groupEntities.length(); i++) {
            if (groupEntities[i].toString() == entityId) {
                groupEntities.removeAt(i);
                break;
            }
        }
        item.insert("entities", groupEntities);
        groups.insert(iter.key(), item);
    }
    m_config->setGroups(groups);

    // remove entity from favorites
    QVariantMap profiles = m_config->getProfiles();
    for (QVariantMap::const_iterator iter = profiles.begin(); iter != profiles.end(); ++iter) {
        QVariantMap  item            = iter.value().toMap();
        QVariantList profileEntities = item.value("favorites").toList();
        for (int i = 0; i < profileEntities.length(); i++) {
            if (profileEntities[i].toString() == entityId) {
                profileEntities.removeAt(i);
                break;
            }
        }
        item.insert("favorites", profileEntities);
        profiles.insert(iter.key(), item);
    }
    m_config->setProfiles(profiles);

    // remove from config
    // get the config
    QVariantMap  c            = getConfig();
    QVariantMap  entities     = c.value("entities").toMap();
    QVariantList entitiesType = entities.value(eIface->type()).toList();

    for (int i = 0; i < entitiesType.length(); i++) {
        if (entitiesType[i].toMap().value("entity_id").toString() == entityId) {
            entitiesType.removeAt(i);
            qCDebug(CLASS_LC) << "4";
            break;
        }
    }

    // put entities back to config
    entities.insert(eIface->type(), entitiesType);
    c.insert("entities", entities);

    delete eIface;

    // write the config back
    bool success = setConfig(c);
    if (success) {
        // if it is a media player and playing, remove from mini media player
        m_entities->removeMediaplayersPlaying(entityId, true);

        // remove from database
        m_entities->remove(entityId);
        return true;
    } else {
        return false;
    }
}

bool YioAPI::addIntegration(QVariantMap integration) {
    // get the type of the new integration
    QString integrationType = integration.value("type").toString();
    qCDebug(CLASS_LC) << "Adding integration type:" << integrationType;

    // remove the key that is not needed
    integration.remove("type");

    // check if the type is supported
    if (!m_integrations->supportedIntegrations().contains(integrationType)) {
        return false;
    }

    qCDebug(CLASS_LC) << "Integration type is supported";

    // check if the input is OK
    if (!integration.contains(Config::KEY_TYPE) && !integration.contains(Config::KEY_ID) &&
        !integration.contains(Config::KEY_FRIENDLYNAME) && !integration.contains(Config::OBJ_DATA)) {
        return false;
    }

    qCDebug(CLASS_LC) << "Input data is OK.";

    // check if the integration already exists
    QObject *iObj = m_integrations->get(integration.value("id").toString());
    if (iObj) {
        return false;
    }

    delete iObj;

    // get the config
    QVariantMap  c                = getConfig();
    QVariantMap  integrations     = c.value("integrations").toMap();
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
    QVariantMap  c                = getConfig();
    QVariantMap  integrations     = c.value("integrations").toMap();
    QVariantMap  integrationsType = integrations.value(integrationType).toMap();
    QVariantList integrationsData = integrationsType.value("data").toList();

    bool success = false;

    for (int i = 0; i < integrationsData.length(); i++) {
        if (integrationsData[i].toMap().value("id").toString() == integration.value("id").toString()) {
            QVariantMap ii = integrationsData[i].toMap();
            ii.insert("friendly_name", integration.value("friendly_name").toString());
            ii.insert("data", integration.value("data").toMap());
            integrationsData[i] = ii;
            success             = true;
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
    QObject *integration     = m_integrations->get(integrationId);
    QString  integrationType = m_integrations->getType(integrationId);

    // unload all entities connected to the integration
    QList<EntityInterface *> entities = m_entities->getByIntegration(integrationType);
    for (int i = 0; i < entities.length(); i++) {
        // remove entity from config and database
        if (entities[i]->integration() == integrationId) {
            if (!removeEntity(entities[i]->entity_id())) {
                return false;
            }
        }
    }

    // remove integration from database
    if (integration) {
        m_integrations->remove(integrationId);
    } else {
        return false;
    }

    // remove integration from config file
    QVariantMap  config                     = getConfig();
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
        qCDebug(CLASS_LC) << "Starting mdns discovery" << m_discoverableServices[i];
        discoverNetworkServices(m_discoverableServices[i]);
    }
}

void YioAPI::discoverNetworkServices(QString mdns) {
    m_zeroConfBrowser = new QZeroConf;
    QObject *context  = new QObject();
    m_prevIp          = "";

    connect(m_zeroConfBrowser, &QZeroConf::serviceAdded, context, [=](QZeroConfService item) {
        QVariantMap txt;

        QMap<QByteArray, QByteArray> txtInfo = item->txt();

        QMap<QByteArray, QByteArray>::iterator i;
        for (i = txtInfo.begin(); i != txtInfo.end(); ++i) {
            txt.insert(i.key(), i.value());
        }

        if (m_prevIp != item->ip().toString()) {
            qCDebug(CLASS_LC) << "Zeroconf found" << item;

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
        context->deleteLater();
        m_zeroConfBrowser->deleteLater();
    });

    m_zeroConfBrowser->startBrowser(mdns);
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

void YioAPI::onClosed() {}

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
    qCDebug(CLASS_LC) << "Client disconnected";
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        m_clients.remove(client);
        client->deleteLater();
        qCDebug(CLASS_LC) << "Client removed";
    }
}

void YioAPI::subscribeOnSignalEvent(const QString &event) {
    qCDebug(CLASS_LC) << "Sending message to all subscribed clients";

    for (int i = 0; i < m_subscribed_clients.length(); i++) {
        QVariantMap response;
        response.insert("event", event);
        QJsonDocument json = QJsonDocument::fromVariant(response);
        m_subscribed_clients[i]->sendTextMessage(json.toJson(QJsonDocument::JsonFormat::Compact));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// API CALLS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void YioAPI::apiSendResponse(QWebSocket *client, const int &id, const bool &success, QVariantMap response) {
    response.insert("id", id);
    response.insert("success", success);
    response.insert("type", "result");

    QJsonDocument json = QJsonDocument::fromVariant(response);

    if (m_clients.contains(client)) {
        qCDebug(CLASS_LC) << "Sending response to client" << client;
        client->sendTextMessage(json.toJson(QJsonDocument::JsonFormat::Compact));
    }
    //    qCDebug(CLASS_LC) << "Response sent to client:" << client << "id:" << id << "response:" << response;
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

void YioAPI::apiSystemSubscribeToEvents(QWebSocket *client, const int &id) {
    qCDebug(CLASS_LC) << "Request for subscribe to events" << client;
    QVariantMap response;

    if (m_subscribed_clients.contains(client)) {
        apiSendResponse(client, id, false, response);
        return;
    }

    // add client to the subscribed client list
    m_subscribed_clients.append(client);

    // connect to signals
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

    apiSendResponse(client, id, true, response);
}

void YioAPI::apiSystemUnsubscribeFromEvents(QWebSocket *client, const int &id) {
    qCDebug(CLASS_LC) << "Request for unsubscribe from events" << client;
    QVariantMap response;

    if (!m_subscribed_clients.contains(client)) {
        apiSendResponse(client, id, false, response);
        return;
    }

    QObject::disconnect(m_context);
    m_context->deleteLater();

    for (int i = 0; i < m_subscribed_clients.length(); i++) {
        if (m_subscribed_clients[i] == client) {
            m_subscribed_clients.removeAt(i);
            apiSendResponse(client, id, true, response);
            return;
        }
    }

    apiSendResponse(client, id, false, response);
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

void YioAPI::apiIntegrationsDiscover(QWebSocket *client, const int &id) {
    qCDebug(CLASS_LC) << "Request for discover integrations" << client;

    QTimer * timeOutTimer = new QTimer();
    QObject *context      = new QObject(this);

    QObject::connect(this, &YioAPI::serviceDiscovered, context, [=](QVariantMap services) {
        qCDebug(CLASS_LC) << "Service discovered";
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
        apiSendResponse(client, id, true, response);
    });

    discoverNetworkServices();

    // start a timeout timer if nothing is discovered
    timeOutTimer->setSingleShot(true);

    connect(timeOutTimer, &QTimer::timeout, this, [=]() {
        QVariantMap response;
        response.insert("message", "discovery_done");
        apiSendResponse(client, id, true, response);

        context->deleteLater();
        timeOutTimer->deleteLater();
    });
    timeOutTimer->start(10000);
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
    if (m_integrations->supportedIntegrations().contains(integration)) {
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

    if (addIntegration(map.value("config").toMap())) {
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiIntegrationUpdate(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for update integration" << client;

    QVariantMap response;

    if (updateIntegration(map.value("config").toMap())) {
        response.insert("message", "Restart the remote to update the integration.");
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
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

void YioAPI::apiEntitiesGetSupported(QWebSocket *client, const int &id) {
    qCDebug(CLASS_LC) << "Request for get supported entities" << client;

    QVariantMap response;
    QStringList supportedEntities = m_entities->supportedEntities();
    if (supportedEntities.length() > 1) {
        response.insert("supported_entities", supportedEntities);
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiEntitiesGetLoaded(QWebSocket *client, const int &id) {
    qCDebug(CLASS_LC) << "Request for get loaded entities" << client;

    QVariantMap response;
    QVariantMap loadedEntities = getConfig().value("entities").toMap();

    if (!loadedEntities.isEmpty()) {
        response.insert("loaded_entities", loadedEntities);
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiEntitiesGetAvailable(QWebSocket *client, const int &id) {
    qCDebug(CLASS_LC) << "Request for get all available entities" << client;

    QVariantMap      response;
    QVariantList     availableEntities;
    QList<QObject *> integrations = m_integrations->list();

    if (integrations.isEmpty()) {
        apiSendResponse(client, id, false, response);
        return;
    }

    for (int i = 0; i < integrations.length(); i++) {
        if (integrations[i]) {
            IntegrationInterface *ii = qobject_cast<IntegrationInterface *>(integrations[i]);
            if (ii) {
                availableEntities.append(ii->getAllAvailableEntities());
            }
        }
    }

    if (availableEntities.isEmpty()) {
        response.insert("available_entities", QVariantList());
        apiSendResponse(client, id, true, response);
        return;
    }

    response.insert("available_entities", availableEntities);
    apiSendResponse(client, id, true, response);
}

void YioAPI::apiEntitiesAdd(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for add entity" << client;

    QVariantMap response;

    if (addEntity(map.value("config").toMap())) {
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiEntitiesUpdate(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for update entity" << client;

    QVariantMap response;

    if (updatEntity(map.value("config").toMap())) {
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiEntitiesRemove(QWebSocket *client, const int &id, const QVariantMap &map) {
    QString entityId = map.value("entity_id").toString();
    qCDebug(CLASS_LC) << "Request for remove entity" << entityId << client;

    QVariantMap response;

    if (removeEntity(entityId)) {
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiProfilesGetAll(QWebSocket *client, const int &id) {
    qCDebug(CLASS_LC) << "Request for get all profiles" << client;

    QVariantMap response;
    QVariantMap profiles = m_config->getProfiles();

    if (!profiles.isEmpty()) {
        response.insert("profiles", profiles);
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiProfilesSet(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for set profile" << client;

    QVariantMap response;
    QString     newProfileId = map.value("profile").toString();

    if (!newProfileId.isEmpty()) {
        if (m_config->getProfileId() != newProfileId) {
            m_config->setProfileId(newProfileId);
        }
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiProfilesAdd(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for add profile" << client;

    QVariantMap response;
    QVariantMap profiles = m_config->getProfiles();

    QVariantMap newProfile = map.value("profile").toMap();
    if (!newProfile.isEmpty()) {
        for (QVariantMap::const_iterator iter = newProfile.begin(); iter != newProfile.end(); ++iter) {
            profiles.insert(iter.key(), iter.value().toMap());
        }

        m_config->setProfiles(profiles);

        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiProfilesUpdate(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for update profile" << client;

    QVariantMap response;
    if (!map.value("data").toMap().isEmpty()) {
        QVariantMap profiles = m_config->getProfiles();
        profiles.insert(map.value("uuid").toString(), map.value("data").toMap());
        m_config->setProfiles(profiles);
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiProfilesRemove(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for remove profile" << client;

    bool success = false;

    QVariantMap response;
    QVariantMap profiles = m_config->getProfiles();

    if (profiles.count() == 1) {
        success = false;
        response.insert("message", "There is only one profile. Cannot be removed.");
        apiSendResponse(client, id, false, response);
        return;
    }

    for (QVariantMap::const_iterator iter = profiles.begin(); iter != profiles.end(); ++iter) {
        if (iter.key() == map.value("profile_id").toString()) {
            profiles.remove(iter.key());
            success = true;
            break;
        }
    }

    if (success) {
        m_config->setProfileId(profiles.lastKey());
        m_config->setProfiles(profiles);
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiPagesGetAll(QWebSocket *client, const int &id) {
    qCDebug(CLASS_LC) << "Request for get all pages" << client;

    QVariantMap response;
    QVariantMap pages = m_config->getPages();
    if (!pages.isEmpty()) {
        response.insert("pages", pages);
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiPagesAdd(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for add a page" << client;

    QVariantMap response;
    QVariantMap pages   = m_config->getPages();
    QVariantMap newPage = map.value("page").toMap();

    if (!newPage.isEmpty()) {
        for (QVariantMap::const_iterator iter = newPage.begin(); iter != newPage.end(); ++iter) {
            pages.insert(iter.key(), iter.value().toMap());
            m_config->setPages(pages);
            apiSendResponse(client, id, true, response);
        }

    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiPagesUpdate(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for update page" << client;

    QVariantMap response;
    if (!map.value("data").toMap().isEmpty()) {
        QVariantMap pages = m_config->getPages();
        pages.insert(map.value("uuid").toString(), map.value("data").toMap());
        m_config->setPages(pages);
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiPagesRemove(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for remove page" << client;

    bool success = false;

    QVariantMap response;

    // remove the page from the profiles
    QVariantMap profiles = m_config->getProfiles();

    for (QVariantMap::const_iterator iter = profiles.begin(); iter != profiles.end(); ++iter) {
        QVariantMap profile = iter.value().toMap();
        QStringList pages   = profile.value("pages").toStringList();

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

    for (QVariantMap::const_iterator iter = pages.begin(); iter != pages.end(); ++iter) {
        if (iter.key() == map.value("page_id").toString()) {
            pages.remove(iter.key());
            m_config->setPages(pages);
            success = true;
            break;
        }
    }

    if (success) {
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiGroupsGetAll(QWebSocket *client, const int &id) {
    qCDebug(CLASS_LC) << "Request for get all groups" << client;

    QVariantMap response;
    QVariantMap groups = m_config->getGroups();
    if (!groups.isEmpty()) {
        response.insert("groups", groups);
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiGroupsAdd(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for add a group" << client;

    QVariantMap response;
    QVariantMap groups   = m_config->getGroups();
    QVariantMap newGroup = map.value("group").toMap();

    if (!newGroup.isEmpty()) {
        for (QVariantMap::const_iterator iter = newGroup.begin(); iter != newGroup.end(); ++iter) {
            groups.insert(iter.key(), iter.value().toMap());
            m_config->setGroups(groups);
            apiSendResponse(client, id, true, response);
        }

    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiGroupsUpdate(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for update group" << client;

    QVariantMap response;
    if (!map.value("data").toMap().isEmpty()) {
        QVariantMap groups = m_config->getGroups();
        groups.insert(map.value("uuid").toString(), map.value("data").toMap());
        m_config->setGroups(groups);
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiGroupsRemove(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for remove group" << client;

    bool success = false;

    QVariantMap response;
    QVariantMap groups = m_config->getGroups();

    for (QVariantMap::const_iterator iter = groups.begin(); iter != groups.end(); ++iter) {
        if (iter.key() == map.value("group_id").toString()) {
            groups.remove(iter.key());
            m_config->setGroups(groups);
            success = true;
            break;
        }
    }

    if (success) {
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiSettingsGetAllLanguages(QWebSocket *client, const int &id) {
    qCDebug(CLASS_LC) << "Request for get all languages" << client;

    QVariantMap  response;
    QVariantList languages = m_config->getLanguages();
    if (!languages.isEmpty()) {
        response.insert("languages", languages);
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiSettingsSetLanguage(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for set a language" << client;

    QVariantMap response;
    if (!map.value("language").toString().isEmpty()) {
        // need to check if the data is valid
        QVariantMap settings = m_config->getSettings();
        settings.insert("language", map.value("language").toString());
        m_config->setSettings(settings);
        TranslationHandler::getInstance()->selectLanguage(map.value("language").toString());
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiSettingsSetAutoBrightness(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for set auto brightness" << client;

    QVariantMap response;
    QVariantMap settings = m_config->getSettings();
    if (map.value("value").toBool() || !map.value("value").toBool()) {
        settings.insert("autobrightness", map.value("value").toBool());
        m_config->setSettings(settings);
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}

void YioAPI::apiSettingsSetDarkMode(QWebSocket *client, const int &id, const QVariantMap &map) {
    qCDebug(CLASS_LC) << "Request for set dark mode" << client;

    QVariantMap response;
    QVariantMap uiConfig = m_config->getUIConfig();
    if (map.value("value").toBool() || !map.value("value").toBool()) {
        uiConfig.insert("darkmode", map.value("value").toBool());
        m_config->setUIConfig(uiConfig);
        apiSendResponse(client, id, true, response);
    } else {
        apiSendResponse(client, id, false, response);
    }
}
