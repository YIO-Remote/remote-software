#include <QtDebug>
#include <QJsonDocument>
#include <QJsonArray>

#include "openhab.h"
#include "../../entities/entity.h"

OpenHAB::OpenHAB()
{
    m_polling_timer.setSingleShot(false);
    m_polling_timer.setInterval(1000);
    m_polling_timer.stop();

    QObject::connect(&m_polling_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

void OpenHAB::initialize(int integrationId, const QVariantMap& config, QObject* entities, QObject* notifications)
{
    setIntegrationId(integrationId);
    for (QVariantMap::const_iterator iter = config.begin(); iter != config.end(); ++iter) {
        if (iter.key() == "data") {
            QVariantMap map = iter.value().toMap();
            m_ip = map.value("ip").toString();
            if (map.value("polling_interval").toBool()) {
                m_polling_interval = map.value("polling_interval").toInt();
            }
        }
        else if (iter.key() == "type")
            setType(iter.value().toString());
        else if (iter.key() == "friendly_name")
            setFriendlyName(iter.value().toString());
    }
    m_entities = qobject_cast<EntitiesInterface *>(entities);
    m_notifications = qobject_cast<NotificationsInterface *>(notifications);
}

void OpenHAB::connect()
{
    setState(CONNECTING);
    // check if the api can be accessed
    getRequest(QString(""));
}

void OpenHAB::disconnect()
{
    m_polling_timer.stop();
    setState(DISCONNECTED);
}

void OpenHAB::sendCommand(const QString& type, const QString& entity_id, const QString& command, const QVariant& param)
{
    if (type == "light") {
    }
    if (type == "blind") {
    }
}

void OpenHAB::updateEntity(const QString& entity_id, const QVariantMap& attr)
{
    Entity* entity = (Entity*)m_entities->get(entity_id);
    if (entity) {
        if (entity->type() == "light") {
            updateLight(entity, attr);
        }
    }
}

void OpenHAB::updateLight(Entity* entity, const QVariantMap& attr)
{
    QVariantMap attributes;

    m_entities->update(entity->entity_id(), attributes);
}

void OpenHAB::getRequest(const QString &url)
{
    QNetworkAccessManager manager;
    QNetworkRequest request;

    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processResponse(QNetworkReply*)));
//    QObject::connect(&manager, SIGNAL(finished(QNetworkReply*)), &manager, SLOT(deleteLater()));

    QUrl fullUrl = QUrl(QString("http://" + m_ip + "/rest" + url));

    request.setRawHeader("Content-Type", "application/json");
    request.setUrl(fullUrl);

    manager.get(request);
}

void OpenHAB::onTimeout()
{
    // get a list of items
    getRequest("/items");
}

void OpenHAB::processResponse(QNetworkReply* reply)
{
    if (reply->error()) {
        qDebug() << reply->errorString();
        m_notifications->add(true,"Cannot connect to openHAB.", "Reconnect", "function() { integrations.openhab.obj.connect(); }");
    }

    QByteArray response = reply->readAll();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
    QVariantMap map = jsonDoc.toVariant().toMap();

    // check if api is up
    if (map.value("version").toString() != "0") {
        setState(CONNECTED);
        qDebug() << "OpenHAB connected.";

        // start polling
        m_polling_timer.start();
    }

    reply->deleteLater();

    // process the list of items
//    if (map.count() > 0) {
//        // we have items
//        qDebug() << "Found" << map.count() << "items";
//    } else {
//        qDebug() << "Nothing found";
//    }
}
