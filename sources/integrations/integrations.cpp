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

#include <QLoggingCategory>
#include <QPluginLoader>
#include <QtDebug>

#include "../config.h"
#include "../entities/entities.h"
#include "../launcher.h"
#include "../notifications.h"
#include "../yioapi.h"

IntegrationsInterface::~IntegrationsInterface() {}

Integrations* Integrations::s_instance = nullptr;

static Q_LOGGING_CATEGORY(CLASS_LC, "plugin");

Integrations::Integrations(const QString& pluginPath) : m_pluginPath(pluginPath) {
    s_instance = this;

    const QMetaObject& metaObject             = Integrations::staticMetaObject;
    int                index                  = metaObject.indexOfEnumerator("SupportedIntegrationTypes");
    QMetaEnum          metaEnumSupportedTypes = metaObject.enumerator(index);
    m_enumSupportedIntegrationTypes           = &metaEnumSupportedTypes;

    for (int i = 0; i < m_enumSupportedIntegrationTypes->keyCount(); i++) {
        QString name(m_enumSupportedIntegrationTypes->key(i));
        qCDebug(CLASS_LC()) << "Adding supported integration type:" << name.toLower();
        m_supportedIntegrations.append(name.toLower());
    }
}

QList<QObject*> Integrations::getAllPlugins() { return m_plugins.values(); }

QJsonObject Integrations::getPluginMetaData(const QString& pluginName) {
    Launcher* launcher   = new Launcher();
    QString   pluginPath = launcher->getPluginPath(m_pluginPath, pluginName);
    qCDebug(CLASS_LC()) << "Getting metadata for:" << pluginPath;
    QPluginLoader pluginLoader(pluginPath);
    return pluginLoader.metaData()["MetaData"].toObject();
}

// Integrations::~Integrations() { s_instance = nullptr; }

void Integrations::load() {
    Entities*      entities      = Entities::getInstance();
    Notifications* notifications = Notifications::getInstance();
    YioAPI*        api           = YioAPI::getInstance();
    Config*        config        = Config::getInstance();

    Launcher* l = new Launcher();

    // read the config
    QVariantMap c = config->getAllIntegrations();

    int integrationCount = 0;

    qCDebug(CLASS_LC) << "Plugin path:" << m_pluginPath;

    // let's load the plugins first
    for (QVariantMap::const_iterator iter = c.begin(); iter != c.end(); ++iter) {
        QObject* obj = l->loadPlugin(m_pluginPath, iter.key());

        // store the plugin objects
        m_plugins.insert(iter.key(), obj);

        // push the config to the integration
        QVariantMap map = iter.value().toMap();
        map.insert(Config::KEY_TYPE, iter.key());

        QString type = iter.key();

        // create instances of the integration based on how many are defined in the config
        PluginInterface* interface = qobject_cast<PluginInterface*>(obj);
        if (interface) {
            connect(interface, &PluginInterface::createDone, this, &Integrations::onCreateDone);

            interface->create(map, entities, notifications, api, config);
            integrationCount++;
        }
    }

    // load plugins that are not defined in config.json aka default plugins
    QStringList defaultIntegrations = {"dock"};

    for (int k = 0; k < defaultIntegrations.length(); k++) {
        QObject* obj = l->loadPlugin(m_pluginPath, defaultIntegrations[k]);

        // store the plugin objects
        m_plugins.insert(defaultIntegrations[k], obj);

        QString type = defaultIntegrations[k];

        // create instances of the integration, no config needed for built in integrations
        PluginInterface* interface = qobject_cast<PluginInterface*>(obj);
        if (interface) {
            connect(interface, &PluginInterface::createDone, this, &Integrations::onCreateDone);

            QVariantMap map;
            map.insert(Config::KEY_TYPE, defaultIntegrations[k]);

            interface->create(map, entities, notifications, api, config);
            integrationCount++;
        }
    }

    m_integrationsToLoad = integrationCount;

    if (m_integrationsToLoad == 0) {
        emit loadComplete();
    }
}

void Integrations::onCreateDone(QMap<QObject*, QVariant> map) {
    // add the integrations to the integration database
    for (QMap<QObject*, QVariant>::const_iterator iter = map.begin(); iter != map.end(); ++iter) {
        add(iter.value().toMap(), iter.key(), iter.value().toMap().value("type").toString());
    }
    m_integrationsLoaded++;

    if (m_integrationsLoaded == m_integrationsToLoad) {
        emit loadComplete();
    }
}

QList<QObject*> Integrations::list() { return m_integrations.values(); }

QStringList Integrations::listIds() {
    QStringList r_list;

    for (QMap<QString, QObject*>::const_iterator iter = m_integrations.begin(); iter != m_integrations.end(); ++iter) {
        r_list.append(iter.key());
    }

    return r_list;
}

QObject* Integrations::get(const QString& id) { return m_integrations.value(id); }

void Integrations::add(const QVariantMap& config, QObject* obj, const QString& type) {
    const QString id = config.value(Config::KEY_ID).toString();
    m_integrations.insert(id, obj);
    m_integrationsFriendlyNames.insert(id, config.value(Config::KEY_FRIENDLYNAME).toString());
    m_integrationsMdns.insert(id, config.value(Config::KEY_MDNS).toString());
    m_integrationsTypes.insert(id, type);
    emit listChanged();
}

void Integrations::remove(const QString& id) {
    m_integrations.remove(id);
    m_integrationsFriendlyNames.remove(id);
    emit listChanged();
}

QString Integrations::getFriendlyName(const QString& id) { return m_integrationsFriendlyNames.value(id); }

QString Integrations::getFriendlyName(QObject* obj) {
    QString name = m_integrations.key(obj);
    return m_integrationsFriendlyNames.value(name);
}

QString Integrations::getMDNS(const QString& id) { return m_integrationsMdns.value(id); }

QStringList Integrations::getMDNSList() { return m_integrationsMdns.values(); }

QString Integrations::getType(const QString& id) { return m_integrationsTypes.value(id); }
