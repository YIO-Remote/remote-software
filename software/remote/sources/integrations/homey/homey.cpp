#include <QtDebug>
#include <QJsonDocument>
#include <QJsonArray>

#include "homey.h"
#include "../../entities/entity.h"
#include "../../entities/entities.h"
#include "math.h"

void Homey::initialize(int integrationId, const QVariantMap &config, QObject *entities, QObject *notifications)
{
    setIntegrationId(integrationId);

    for (QVariantMap::const_iterator iter = config.begin(); iter != config.end(); ++iter)
    {
        if (iter.key() == "type")
            setType(iter.value().toString());
        else if (iter.key() == "friendly_name")
            setFriendlyName(iter.value().toString());
    }

    // crate a new instance and pass on variables
    HomeyThread *HAThread = new HomeyThread(config, entities, notifications);

    // move to thread
    HAThread->moveToThread(&m_thread);

    // connect signals and slots
    QObject::connect(&m_thread, &QThread::finished, HAThread, &QObject::deleteLater);

    QObject::connect(this, &Homey::connectSignal, HAThread, &HomeyThread::connect);
    QObject::connect(this, &Homey::disconnectSignal, HAThread, &HomeyThread::disconnect);
    QObject::connect(this, &Homey::sendCommandSignal, HAThread, &HomeyThread::sendCommand);

    QObject::connect(HAThread, &HomeyThread::stateChanged, this, &Homey::stateHandler);

    m_thread.start();
}

void Homey::connect()
{
    emit connectSignal();
}

void Homey::disconnect()
{
    emit disconnectSignal();
}

void Homey::sendCommand(const QString &type, const QString &entity_id, const QString &command, const QVariant &param)
{
    emit sendCommandSignal(type, entity_id, command, param);
}

void Homey::stateHandler(int state)
{
    if (state == 0)
    {
        setState(CONNECTED);
    }
    else if (state == 1)
    {
        setState(CONNECTING);
    }
    else if (state == 2)
    {
        setState(DISCONNECTED);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//// Homey THREAD CLASS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HomeyThread::HomeyThread(const QVariantMap &config, QObject *entities, QObject *notifications)
{
    for (QVariantMap::const_iterator iter = config.begin(); iter != config.end(); ++iter)
    {
        if (iter.key() == "data")
        {
            QVariantMap map = iter.value().toMap();
            m_ip = map.value("ip").toString();
            m_token = map.value("token").toString();
        }
    }
    m_entities = qobject_cast<EntitiesInterface *>(entities);
    m_notifications = qobject_cast<NotificationsInterface *>(notifications);

    m_webSocketId = 4;

    m_websocketReconnect = new QTimer(this);

    m_websocketReconnect->setSingleShot(true);
    m_websocketReconnect->setInterval(2000);
    m_websocketReconnect->stop();

    m_socket = new QWebSocket;
    m_socket->setParent(this);

    QObject::connect(m_socket, SIGNAL(textMessageReceived(const QString &)), this, SLOT(onTextMessageReceived(const QString &)));
    QObject::connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
    QObject::connect(m_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));

    QObject::connect(m_websocketReconnect, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

void HomeyThread::onTextMessageReceived(const QString &message)
{
    QJsonParseError parseerror;
    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8(), &parseerror);
    if (parseerror.error != QJsonParseError::NoError)
    {
        qDebug() << "JSON error : " << parseerror.errorString();
        return;
    }
    QVariantMap map = doc.toVariant().toMap();

    QString m = map.value("error").toString();
    if (m.length() > 0)
    {
        qDebug() << "error : " << m;
    }

    QString type = map.value("type").toString();
    //    int id = map.value("id").toInt();

    if (type == "connected")
    {
        setState(0);
    }

    // handle get config request from homey app
    if (type == "command" && map.value("command").toString() == "get_config")
    {
        // get loaded homey entities
        QList<QObject *> es = m_entities->getByIntegration("homey");

        // create return map object
        QVariantMap returnData;

        // set type
        returnData.insert("type", "sendConfig");

        // create list to store entity ids
        QStringList list;

        // interate throug the list and get the entity ids

        foreach (QObject *value, es)
        {
            list.append(value->property("entity_id").toString());
            qDebug() << value->property("entity_id").toString();
        }
        qDebug() << "LIST" << list;
        // insert list to data key in response
        returnData.insert("devices", list);

        // convert map to json
        QJsonDocument doc = QJsonDocument::fromVariant(returnData);
        QString message = doc.toJson(QJsonDocument::JsonFormat::Compact);

        // send message
        m_socket->sendTextMessage(message);
    }

    // handle fetch states from homey app
    if (type == "sendStates")
    {
        QVariantMap data = map.value("data").toMap();
        updateEntity(data.value("entity_id").toString(), data);
    }

    if (type == "event")
    {
        QVariantMap data = map.value("data").toMap();
        updateEntity(data.value("entity_id").toString(), data);
    }
}

void HomeyThread::onStateChanged(QAbstractSocket::SocketState state)
{
    if (state == QAbstractSocket::UnconnectedState && !m_userDisconnect)
    {
        setState(2);
        m_websocketReconnect->start();
    }
}

void HomeyThread::onError(QAbstractSocket::SocketError error)
{
    qDebug() << error;
    m_socket->close();
    setState(2);
    m_websocketReconnect->start();
}

void HomeyThread::onTimeout()
{
    if (m_tries == 3)
    {
        m_websocketReconnect->stop();

        m_notifications->add(true, tr("Cannot connect to Homey."), tr("Reconnect"), "homey");
        disconnect();
        m_tries = 0;
    }
    else
    {
        m_webSocketId = 4;
        if (m_state != 1)
        {
            setState(1);
        }
        QString url = QString("ws://").append(m_ip);
        m_socket->open(QUrl(url));

        m_tries++;
    }
}

void HomeyThread::webSocketSendCommand(QVariantMap data)
{
    QJsonDocument doc = QJsonDocument::fromVariant(data);
    QString message = doc.toJson(QJsonDocument::JsonFormat::Compact);
    m_socket->sendTextMessage(message);
}

int HomeyThread::convertBrightnessToPercentage(float value)
{
    return int(round(value * 100));
}

void HomeyThread::updateEntity(const QString &entity_id, const QVariantMap &attr)
{
    Entity *entity = (Entity *)m_entities->get(entity_id);
    if (entity)
    {
        if (entity->type() == "light")
        {
            updateLight(entity, attr);
        }
        if (entity->type() == "blind")
        {
            updateBlind(entity, attr);
        }
        if (entity->type() == "media_player")
        {
            updateMediaPlayer(entity, attr);
        }
    }
}

void HomeyThread::updateLight(Entity *entity, const QVariantMap &attr)
{
    QVariantMap attributes;

    //onoff to state.
    if (attr.contains("onoff"))
    {
        attributes.insert("state", attr.value("onoff"));
        printf("Setting state");
    }

    // brightness
    if (entity->supported_features().indexOf("BRIGHTNESS") > -1)
    {
        if (attr.contains("dim"))
        {
            attributes.insert("brightness", convertBrightnessToPercentage(attr.value("dim").toFloat()));
            printf("Setting brightness");
        }
    }

    // color
    if (entity->supported_features().indexOf("COLOR") > -1)
    {
        QVariant color = attr.value("attributes").toMap().value("rgb_color");
        QVariantList cl(color.toList());
        char buffer[10];
        sprintf(buffer, "#%02X%02X%02X", cl.value(0).toInt(), cl.value(1).toInt(), cl.value(2).toInt());
        attributes.insert("color", buffer);
    }

    m_entities->update(entity->entity_id(), attributes);
}

void HomeyThread::updateBlind(Entity *entity, const QVariantMap &attr)
{
    //    QVariantMap attributes;

    //    // state
    //    if (attr.value("state").toString() == "open") {
    //        attributes.insert("state", true);
    //    } else {
    //        attributes.insert("state", false);
    //    }

    //    // position
    //    if (entity->supported_features().indexOf("POSITION") > -1) {
    //        attributes.insert("position", attr.value("attributes").toMap().value("current_position").toInt());
    //    }

    //    m_entities->update(entity->entity_id(), attributes);
}

void HomeyThread::updateMediaPlayer(Entity *entity, const QVariantMap &attr)
{
    //    QVariantMap attributes;

    //    //state
    //    if (attr.value("state").toString() == "off") {
    //        attributes.insert("state", 0);
    //    } else if (attr.value("state").toString() == "on") {
    //        attributes.insert("state", 1);
    //    } else if (attr.value("state").toString() == "idle") {
    //        attributes.insert("state", 2);
    //    } else if (attr.value("state").toString() == "playing") {
    //        attributes.insert("state", 3);
    //    } else {
    //        attributes.insert("state", 0);
    //    }

    //    // source
    //    if (entity->supported_features().indexOf("SOURCE") > -1 && attr.value("attributes").toMap().contains("source")) {
    //        attributes.insert("source", attr.value("attributes").toMap().value("source").toString());
    //    }

    //    // volume
    //    if (entity->supported_features().indexOf("VOLUME") > -1 && attr.value("attributes").toMap().contains("volume_level")) {
    //        attributes.insert("volume", attr.value("attributes").toMap().value("volume_level").toDouble());
    //    }

    //    // media type
    //    if (entity->supported_features().indexOf("MEDIA_TYPE") > -1 && attr.value("attributes").toMap().contains("media_content_type")) {
    //        attributes.insert("mediaType", attr.value("attributes").toMap().value("media_content_type").toString());
    //    }

    //    // media image
    //    if (entity->supported_features().indexOf("MEDIA_IMAGE") > -1 && attr.value("attributes").toMap().contains("entity_picture")) {
    //        QString url = attr.value("attributes").toMap().value("entity_picture").toString();
    //        QString fullUrl = QString("http://").append(m_ip).append(url);
    //        attributes.insert("mediaImage", fullUrl);
    //    }

    //    // media title
    //    if (entity->supported_features().indexOf("MEDIA_TITLE") > -1 && attr.value("attributes").toMap().contains("media_title")) {
    //        attributes.insert("mediaTitle", attr.value("attributes").toMap().value("media_title").toString());
    //    }

    //    // media artist
    //    if (entity->supported_features().indexOf("MEDIA_ARTIST") > -1 && attr.value("attributes").toMap().contains("media_artist")) {
    //        attributes.insert("mediaArtist", attr.value("attributes").toMap().value("media_artist").toString());
    //    }

    //    m_entities->update(entity->entity_id(), attributes);
}

void HomeyThread::setState(int state)
{
    m_state = state;
    emit stateChanged(state);
}

void HomeyThread::connect()
{
    m_userDisconnect = false;

    setState(1);

    // reset the reconnnect trial variable
    m_tries = 0;

    // turn on the websocket connection
    QString url = QString("ws://").append(m_ip);
    m_socket->open(QUrl(url));
}

void HomeyThread::disconnect()
{
    m_userDisconnect = true;

    // turn of the reconnect try
    m_websocketReconnect->stop();

    // turn off the socket
    m_socket->close();

    setState(2);
}

void HomeyThread::sendCommand(const QString &type, const QString &entity_id, const QString &command, const QVariant &param)
{
    QVariantMap map;
    //example
    //{"command":"onoff","deviceId":"78f3ab16-c622-4bd7-aebf-3ca981e41375","type":"command","value":true}
    map.insert("type", "command");

    map.insert("deviceId", QVariant(entity_id));
    if (type == "light")
    {
        if (command == "TOGGLE")
        {
            map.insert("command", QVariant("toggle"));
            map.insert("value", true);
            webSocketSendCommand(map);
        }
        else if (command == "ON")
        {
            map.insert("command", QVariant("onoff"));
            map.insert("value", true);
            webSocketSendCommand(map);
        }
        else if (command == "OFF")
        {
            map.insert("command", QVariant("onoff"));
            map.insert("value", false);
            webSocketSendCommand(map);
        }
        else if (command == "BRIGHTNESS")
        {
            map.insert("command", "dim");
            float value = param.toFloat() / 100;
            map.insert("value", value);
            webSocketSendCommand(map);
        }
        else if (command == "COLOR")
        {
            QColor color = param.value<QColor>();
            //QVariantMap data;
            QVariantList list;
            list.append(color.red());
            list.append(color.green());
            list.append(color.blue());
            map.insert("command", "color");
            map.insert("value", list);
            webSocketSendCommand(map);
            //webSocketSendCommand(type, "turn_on", entity_id, &data);
        }
    }
    if (type == "blind")
    {
        if (command == "OPEN")
        {
            map.insert("command", "windowcoverings_closed");
            map.insert("value", "false");
            webSocketSendCommand(map);
        }
        else if (command == "CLOSE")
        {
            map.insert("command", "windowcoverings_closed");
            map.insert("value", "true");
            webSocketSendCommand(map);
        }
        else if (command == "STOP")
        {
            map.insert("command", "windowcoverings_tilt_set");
            map.insert("value", 0);
            webSocketSendCommand(map);
        }
        else if (command == "POSITION")
        {
            map.insert("command", "windowcoverings_set");
            map.insert("value", param);
            webSocketSendCommand(map);
        }
    }
    if (type == "media_player")
    {
        if (command == "VOLUME_SET")
        {
            map.insert("command", "volume_set");
            map.insert("value", param);
            webSocketSendCommand(map);
        }
        else if (command == "PLAY")
        {
            map.insert("command", "speaker_playing");
            map.insert("value", true);
            webSocketSendCommand(map);
        }
        else if (command == "STOP")
        {
            map.insert("command", "speaker_playing");
            map.insert("value", false);
            webSocketSendCommand(map);
        }
        else if (command == "PREVIOUS")
        {
            map.insert("command", "speaker_prev");
            map.insert("value", true);
            webSocketSendCommand(map);
        }
        else if (command == "NEXT")
        {
            map.insert("command", "speaker_next");
            map.insert("value", true);
            webSocketSendCommand(map);
        }
        else if (command == "TURNON")
        {
            map.insert("command", QVariant("onoff"));
            map.insert("value", true);
            webSocketSendCommand(map);
        }
        else if (command == "TURNOFF")
        {
            map.insert("command", QVariant("onoff"));
            map.insert("value", false);
            webSocketSendCommand(map);
        }
    }
}
