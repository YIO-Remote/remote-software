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

#include <QtDebug>

#include "yioapi.h"
#include "fileio.h"
#include "entities/entities.h"
#include "logger.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkInterface>
#include <QTimer>

YioAPI* YioAPI::s_instance = nullptr;

YioAPI::YioAPI(QQmlApplicationEngine *engine) :
    m_log("yioapi"),
    m_engine(engine)
{
    s_instance = this;
    Logger::getInstance()->defineLogCategory(m_log.categoryName(), QtMsgType::QtDebugMsg, &m_log);
}

YioAPI::~YioAPI()
{
    s_instance = nullptr;
}

void YioAPI::start()
{
    m_server = new QWebSocketServer(QStringLiteral("YIO API"), QWebSocketServer::NonSecureMode, this);

    // start websocket server on port 946(YIO)
    if (m_server->listen(QHostAddress::Any, 946)) {
        connect(m_server, &QWebSocketServer::newConnection, this, &YioAPI::onNewConnection);
        connect(m_server, &QWebSocketServer::closed, this, &YioAPI::closed);
        m_running = true;
        emit runningChanged();
    }

    FileIO fileIO;
    m_hostname = fileIO.read("/apssid").trimmed();

    emit hostnameChanged();

    m_qzero_conf.startServicePublish(m_hostname.toUtf8(), "_yio-remote._tcp", "local", 946);

    qCDebug(m_log) << "YIO api started";
}

void YioAPI::stop()
{
    m_server->close();
    m_clients.clear();
    m_running = false;
    m_qzero_conf.stopServicePublish();
    emit runningChanged();
}

void YioAPI::sendMessage(QString message)
{
    QMap<QWebSocket *, bool>::iterator i;
    for (i = m_clients.begin(); i != m_clients.end(); i++)
    {
        if (i.value()) {
            QWebSocket *socket = i.key();
            socket->sendTextMessage(message);
        }
    }
}

QVariantMap YioAPI::getConfig()
{
    return Config::getInstance()->config();
}

void YioAPI::setConfig(QVariantMap config)
{
    Config::getInstance()->setConfig(config);
    Config::getInstance()->writeConfig();
}

bool YioAPI::addEntityToConfig(QVariantMap entity)
{
    // check the input if it's OK
    if (!entity.contains("area") && !entity.contains("entity_id") && !entity.contains("friendly_name") && !entity.contains("integration") && !entity.contains("supported_features") && !entity.contains("type"))
    {
        return false;
    }

    // get the config
    QVariantMap c = getConfig();
    QVariantList e = c.value("entities").toJsonArray().toVariantList();

    // check what is the type of the new entity
    QString entityType = entity.value("type").toString();


    //find the entities key and insert the new entity
    for (int i=0; i<e.length(); i++) {
        if (e[i].toMap().value("type").toString() == entityType) {

            // get the data key array
            QVariantMap r = e[i].toMap();
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

    // writeh the config back
    Config::getInstance()->setConfig(c);
    Config::getInstance()->writeConfig();

    return true;
}

void YioAPI::discoverNetworkServices()
{
    m_discoveredServices.clear();

    m_discoverableServices = Integrations::getInstance()->getMDNSList();

    for (int i=0; i<m_discoverableServices.length(); i++) {
        m_qzero_conf_browser = new QZeroConf;
        connect(m_qzero_conf_browser, &QZeroConf::serviceAdded, this, [=](QZeroConfService item) {
            QVariantMap txt;

            QMap<QByteArray, QByteArray> txtInfo = item->txt();

            QMap<QByteArray, QByteArray>::iterator qi;
            for (qi = txtInfo.begin(); qi != txtInfo.end(); ++qi)
            {
                txt.insert(qi.key(), qi.value());
            }

            QVariantMap map;
            map.insert(QString("name"),item->name());
            map.insert(QString("ip"),item->ip().toString());
            map.insert(QString("port"), item->port());
            map.insert(QString("mdns"), m_discoverableServices[i]);
            map.insert(QString("txt"), txt);
            m_discoveredServices.insert(item->name(), map);

            emit serviceDiscovered(m_discoveredServices);
            emit discoveredServicesChanged();
        });
        m_qzero_conf_browser->startBrowser(m_discoverableServices[i]);
    }
}

void YioAPI::discoverNetworkServices(QString mdns)
{
    m_discoveredServices.clear();

    m_qzero_conf_browser = new QZeroConf;

    connect(m_qzero_conf_browser, &QZeroConf::serviceAdded, this, [=](QZeroConfService item) {
        QVariantMap txt;

        QMap<QByteArray, QByteArray> txtInfo = item->txt();

        QMap<QByteArray, QByteArray>::iterator i;
        for (i = txtInfo.begin(); i != txtInfo.end(); ++i)
        {
            txt.insert(i.key(), i.value());
        }

        QVariantMap map;
        map.insert(QString("name"),item->name());
        map.insert(QString("ip"),item->ip().toString());
        map.insert(QString("port"), item->port());
        map.insert(QString("mdns"), mdns);
        map.insert(QString("txt"), txt);
        m_discoveredServices.insert(item->name(), map);

        emit serviceDiscovered(m_discoveredServices);
        emit discoveredServicesChanged();
    });

    m_qzero_conf_browser->startBrowser(mdns);
}

QVariantList YioAPI::discoveredServices()
{
    QVariantList list;

    QMap<QString, QVariantMap>::iterator i;
    for (i = m_discoveredServices.begin(); i != m_discoveredServices.end(); i++)
    {
        QVariantMap map = i.value();
        list.append(map);
    }

    return list;
}

void YioAPI::onNewConnection()
{
    QWebSocket *socket = m_server->nextPendingConnection();

    connect(socket, &QWebSocket::textMessageReceived, this, &YioAPI::processMessage);
    connect(socket, &QWebSocket::disconnected, this, &YioAPI::onClientDisconnected);

    // send message to client after connected to authenticate
    QVariantMap map;
    map.insert("type", "auth_required");
    QJsonDocument doc = QJsonDocument::fromVariant(map);
    QString message = doc.toJson(QJsonDocument::JsonFormat::Compact);

    socket->sendTextMessage(message);

    m_clients.insert(socket, false);
}

void YioAPI::processMessage(QString message)
{
    QVariantMap r_map;

    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
//        qDebug() << message;

        // convert message to json
        QJsonParseError parseerror;
        QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseerror);
        if (parseerror.error != QJsonParseError::NoError) {
            qDebug() << "JSON error : " << parseerror.errorString();
            return;
        }
        QVariantMap map = doc.toVariant().toMap();

        QString type = map.value("type").toString();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // AUTHENTICATION
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (type == "auth") {
            qCDebug(m_log) << m_clients[client];

            if (map.contains("token")) {
                qCDebug(m_log) << "Has token";

//                QByteArray hash = QCryptographicHash::hash(map.value("token").toString().toLocal8Bit(), QCryptographicHash::Sha512);

                if (map.value("token").toString() == m_token) {
                    qDebug() << "Token OK";
                    r_map.insert("type", "auth_ok");
                    QJsonDocument r_doc = QJsonDocument::fromVariant(r_map);
                    QString r_message = r_doc.toJson(QJsonDocument::JsonFormat::Compact);

                    client->sendTextMessage(r_message);

                    m_clients[client] = true;

                    qCDebug(m_log) << m_clients[client];

                } else {
                    qCDebug(m_log) << "Token NOT OK";
                    r_map.insert("type", "auth_error");
                    r_map.insert("message", "Invalid token");
                    QJsonDocument r_doc = QJsonDocument::fromVariant(r_map);
                    QString r_message = r_doc.toJson(QJsonDocument::JsonFormat::Compact);

                    client->sendTextMessage(r_message);
                }
            } else {
                qCDebug(m_log) << "No token";
                r_map.insert("type", "auth_error");
                r_map.insert("message", "Token needed");
                QJsonDocument r_doc = QJsonDocument::fromVariant(r_map);
                QString r_message = r_doc.toJson(QJsonDocument::JsonFormat::Compact);

                client->sendTextMessage(r_message);
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // EMIT MESSAGES OF AUTHENTICATED CLIENTS
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        if (type == "getconfig" && m_clients[client]){
            qCDebug(m_log) << "REQUEST FOR GETCONFIG";

            QVariantMap response;

            QVariantMap config = getConfig();
            response.insert("config", config);
            response.insert("type", "config");

            QJsonDocument json = QJsonDocument::fromVariant(response);
            client->sendTextMessage(json.toJson());
        } else if (type == "setconfig" && m_clients[client]) {
            qCDebug(m_log) << "REQUEST FOR SETCONFIG";

            QVariantMap config = map.value("config").toMap();
            setConfig(config);
        } else if (type == "button" && m_clients[client]) {
            // Handle buttons
            QString buttonName = map["name"].toString();
            QString buttonAction = map["action"].toString();
            qDebug() << "BUTTON SIMULATION : " << buttonName << " : " << buttonAction;
            if (buttonAction == "pressed")
                emit buttonPressed(buttonName);
            else
                emit buttonReleased(buttonName);
        } else if (type == "log" && m_clients[client]) {
            // Handle log
            Logger* logger = Logger::getInstance();
            QString logAction = map["action"].toString();
            QString logTarget = map["target"].toString();
            qCDebug(m_log) << "LOGGER : " << logAction;
            if (logAction == "start") {
                // enable logger target, default is queue
                if (logTarget == "file")
                    logger->setFileEnabled(true);
                else if (logTarget == "debug")
                    logger->setDebugEnabled(true);
                else
                    logger->setQueueEnabled(true);
            }
            else if (logAction == "stop") {
                // edisable logger queue target, default is queue
                if (logTarget == "file")
                    logger->setFileEnabled(false);
                else if (logTarget == "debug")
                    logger->setDebugEnabled(false);
                else
                    logger->setQueueEnabled(false);
            }
            else if (logAction == "showsource") {
                logger->setShowSourcePos(true);
            }
            else if (logAction == "hidesource") {
                logger->setShowSourcePos(false);
            }
            else if (logAction == "purge") {
                int hours = 24;
                if (map.contains("hours"))
                    hours = map["hours"].toInt();
                logger->purgeFiles(hours);
            }
            else if (logAction == "setloglevel") {
                // set log level
                int level = QtMsgType::QtDebugMsg;
                QString category;
                if (map.contains("level"))
                    level = logger->toMsgType(map["level"].toString());
                if (map.contains("category")) {
                    category = map["category"].toString();
                    logger->setCategoryLogLevel(category, level);
                }
                else
                    logger->setLogLevel(level);
            }
            else if (logAction == "getmessages") {
                // get log messages
                int count = 50;
                int level = QtMsgType::QtDebugMsg;
                QStringList categories;
                if (map.contains("count"))
                    count = map["count"].toInt();
                if (map.contains("level"))
                    level = logger->toMsgType(map["level"].toString());
                if (map.contains("categories"))
                    categories = map["categories"].toStringList();
                QJsonArray messages = logger->getQueuedMessages(count, level, categories);
                QJsonObject jsonObj;
                jsonObj.insert("type", "log");
                jsonObj.insert("messages", messages);
                QJsonDocument json = QJsonDocument(jsonObj);
                client->sendTextMessage(json.toJson());
            }
            else if (logAction == "getinfo") {
                // get log info
                QJsonObject info = logger->getInformation();
                QJsonObject jsonObj;
                jsonObj.insert("type", "log");
                jsonObj.insert("info", info);
                QJsonDocument json = QJsonDocument(jsonObj);
                client->sendTextMessage(json.toJson());
            }
        } else {
            emit messageReceived(map);
        }
    }
}

void YioAPI::onClientDisconnected()
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        m_clients.remove(client);
        client->deleteLater();
    }
}
