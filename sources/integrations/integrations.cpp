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

    // read the config
    QVariantMap c = config->read().value("integrations").toMap();

    int i = 0;

    // return array of integration objects
    QMap<QObject *, QVariant> ha;

    // let's load the plugins first
    for (QVariantMap::const_iterator iter = c.begin(); iter != c.end(); ++iter) {
        QObject* obj = l->loadPlugin(m_appPath, iter.key());

        // store the plugin objects
        m_plugins.insert(iter.key(), obj);

        // push the config to the integration
        QVariantMap map = iter.value().toMap();

        // create instances of the integration based on how many are defined in the config
        IntegrationInterface *interface = qobject_cast<IntegrationInterface *>(obj);
        if (interface) {
            ha = interface->create(map, entities, notifications, api, config);
        }
        i++;
    }

    // add the integrations to the integration database
    for (QMap<QObject *, QVariant>::const_iterator iter = ha.begin(); iter != ha.end(); ++iter) {
        add(iter.value().toMap(), iter.key());
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

QObject *Integrations::getByID(const QString& id)
{
    return m_integrations.value(id);
}

void Integrations::add(const QVariantMap& config, QObject *obj)
{   
    m_integrations.insert(config.value("id").toString(), obj);
    m_integrations_friendly_names.insert(config.value("id").toString(), config.value("friendly_name").toString());
    m_integrations_mdns.insert(config.value("id").toString(), config.value("mdns").toString());
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

QString Integrations::getMDNS(const QString &type)
{
    return m_integrations_mdns.value(type);
}

QStringList Integrations::getMDNSList()
{
    return  m_integrations_mdns.values();
}
