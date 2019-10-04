#include "integrations.h"
#include "../config.h"
#include "../launcher.h"
#include "../entities/entities.h"
#include "../notifications.h"
#include "../yioapi.h"

#include <QJsonArray>
#include <QtDebug>
#include <QQuickItem>

Integrations* Integrations::s_instance = NULL;

Integrations::Integrations(QQmlApplicationEngine *engine, const QString& appPath) :
    m_engine(engine),
    m_appPath(appPath)
{
    s_instance = this;
}

Integrations::~Integrations()
{
    s_instance = NULL;
}

bool Integrations::load()
{
    Entities* entities = Entities::getInstance();
    Notifications* notifications = Notifications::getInstance();
    YioAPI* api = YioAPI::getInstance();
    Config* config = Config::getInstance();

    Launcher* l = new Launcher();

    QVariantMap c = config->read();
    QVariantList integrations = c.value("integration").toJsonArray().toVariantList();

    int i = 0;

    for (i=0; i<integrations.length(); i++)
    {
        if (integrations[i].toMap().contains("plugin")) {
            QObject* obj = l->loadIntegration(m_appPath, integrations[i].toMap().value("plugin").toString(), i, integrations[i].toMap(), entities, notifications, api, config);

            // add the integration to the list
            add(integrations[i].toMap().value("type").toString(), integrations[i].toMap().value("friendly_name").toString(), obj);
        }
    }

    if (i != 0) {
        return true;
    } else {
        return false;
    }
}

QList<QObject *> Integrations::list()
{
    return m_integrations.values();
}

QObject *Integrations::getByType(const QString& type)
{
    return m_integrations.value(type);
}

void Integrations::add(const QString& type, const QString& friendlyName, QObject *obj)
{
    m_integrations.insert(type, obj);
    m_integrations_friendly_names.insert(type, friendlyName);
    emit listChanged();
}

void Integrations::remove(const QString& type)
{
    m_integrations.remove(type);
    m_integrations_friendly_names.remove(type);
    emit listChanged();
}

QString Integrations::getFriendlyName(const QString& type)
{
    return m_integrations_friendly_names.value(type);
}

QString Integrations::getFriendlyName(QObject *obj)
{
    QString name = m_integrations.key(obj);
    return m_integrations_friendly_names.value(name);
}
