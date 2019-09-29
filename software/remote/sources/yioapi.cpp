#include <QtDebug>

#include "yioapi.h"
#include "fileio.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkInterface>

YioAPI* YioAPI::s_instance = nullptr;

YioAPI::YioAPI(QQmlApplicationEngine *engine) :
    m_engine(engine)
{
    s_instance = this;
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

}

void YioAPI::stop()
{
    m_server->close();
    m_clients.clear();
    m_running = false;
    m_qzero_conf.stopServicePublish();
    emit runningChanged();
}

QVariantMap YioAPI::getConfig()
{
    return Config::getInstance()->read();
}

void YioAPI::addEntityToConfig(QVariantMap entity)
{
    // get the config
    QVariantMap c = getConfig();

    // check what is the type of the new entity
    QString entityType = entity["tpye"].toString();

    //find the entities key and insert the new entity
    foreach (QVariant value, c)
    {
        if (value["type"].toString() == entityType) {

        }
    }


    // writeh the config back
    Config::getInstance()->readWrite(c);
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
        qDebug() << message;

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
             qDebug() << m_clients[client];

            if (map.contains("token")) {
                qDebug() << "Has token";
                if (map.value("token").toString() == m_token) {
                    qDebug() << "Token OK";
                    r_map.insert("type", "auth_ok");
                    QJsonDocument r_doc = QJsonDocument::fromVariant(r_map);
                    QString r_message = r_doc.toJson(QJsonDocument::JsonFormat::Compact);

                    client->sendTextMessage(r_message);

                    m_clients[client] = true;

                    qDebug() << m_clients[client];

                } else {
                    qDebug() << "Token NOT OK";
                    r_map.insert("type", "auth_error");
                    r_map.insert("message", "Invalid token");
                    QJsonDocument r_doc = QJsonDocument::fromVariant(r_map);
                    QString r_message = r_doc.toJson(QJsonDocument::JsonFormat::Compact);

                    client->sendTextMessage(r_message);
                }
            } else {
                qDebug() << "No token";
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

        if (type != "auth" && m_clients[client]) {
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
