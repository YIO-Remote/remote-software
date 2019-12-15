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

#include "integrations.h"
#include "../config.h"
#include "../launcher.h"
#include "../entities/entities.h"
#include "../notifications.h"
#include "../yioapi.h"

#include <QJsonArray>
#include <QQuickItem>
#include <QTimer>

Integrations* Integrations::s_instance = nullptr;

Integrations::Integrations(QQmlApplicationEngine *engine, const QString& appPath) :
    m_engine(engine),
    m_appPath(appPath)
{
    s_instance = this;
}

Integrations::~Integrations()
{
    s_instance = nullptr;
}

void Integrations::load()
{
    Entities* entities = Entities::getInstance();
    Notifications* notifications = Notifications::getInstance();
    YioAPI* api = YioAPI::getInstance();
    Config* config = Config::getInstance();

    Launcher* l = new Launcher();

    // read the config
    QVariantMap c = config->getIntegrations(); //config->config().value("integrations").toMap();

    int i = 0;

    // let's load the plugins first
    for (QVariantMap::const_iterator iter = c.begin(); iter != c.end(); ++iter) {
        QObject* obj = l->loadPlugin(m_appPath, iter.key());

        // store the plugin objects
        m_plugins.insert(iter.key(), obj);

        // push the config to the integration
        QVariantMap map = iter.value().toMap();
        map.insert("type", iter.key());

        QString type = iter.key();

        // create instances of the integration based on how many are defined in the config
        IntegrationInterface *interface = qobject_cast<IntegrationInterface *>(obj);
        if (interface) {
            connect(interface, &IntegrationInterface::createDone, this, &Integrations::onCreateDone);

            interface->create(map, entities, notifications, api, config);
            i++;
        }
    }

    // load plugins that are not defined in config.json aka default plugins
    QStringList defaultIntegrations = {"dock"};

    for (int k = 0; k<defaultIntegrations.length(); k++)
    {
        QObject* obj = l->loadPlugin(m_appPath, defaultIntegrations[k]);

        // store the plugin objects
        m_plugins.insert(defaultIntegrations[k], obj);

        QString type = defaultIntegrations[k];

        // create instances of the integration, no config needed for built in integrations
        IntegrationInterface *interface = qobject_cast<IntegrationInterface *>(obj);
        if (interface) {
            connect(interface, &IntegrationInterface::createDone, this, &Integrations::onCreateDone);

            QVariantMap map;
            map.insert("type", defaultIntegrations[k]);

            interface->create(map, entities, notifications, api, config);
            i++;
        }
    }

    m_integrationsToLoad = i;

    if (m_integrationsToLoad == 0)
        emit loadComplete();
}

void Integrations::onCreateDone(QMap<QObject *, QVariant> map)
{
    // add the integrations to the integration database
    for (QMap<QObject *, QVariant>::const_iterator iter = map.begin(); iter != map.end(); ++iter) {
        add(iter.value().toMap(), iter.key(), iter.value().toMap().value("type").toString());
    }
    m_integrationsLoaded++;

    if (m_integrationsLoaded == m_integrationsToLoad) {
        emit loadComplete();
    }
}

QList<QObject *> Integrations::list()
{
    return m_integrations.values();
}

QObject *Integrations::get(const QString& id)
{
    return m_integrations.value(id);
}

void Integrations::add(const QVariantMap& config, QObject *obj, const QString& type)
{   
    m_integrations.insert(config.value("id").toString(), obj);
    m_integrations_friendly_names.insert(config.value("id").toString(), config.value("friendly_name").toString());
    m_integrations_mdns.insert(config.value("id").toString(), config.value("mdns").toString());
    m_integrations_types.insert(config.value("id").toString(), type);
    emit listChanged();
}

void Integrations::remove(const QString& id)
{
    m_integrations.remove(id);
    m_integrations_friendly_names.remove(id);
    emit listChanged();
}

QString Integrations::getFriendlyName(const QString& id)
{
    return m_integrations_friendly_names.value(id);
}

QString Integrations::getFriendlyName(QObject *obj)
{
    QString name = m_integrations.key(obj);
    return m_integrations_friendly_names.value(name);
}

QString Integrations::getMDNS(const QString &id)
{
    return m_integrations_mdns.value(id);
}

QStringList Integrations::getMDNSList()
{
    return  m_integrations_mdns.values();
}

QString Integrations::getType(const QString &id)
{
    return m_integrations_types.value(id);
}
