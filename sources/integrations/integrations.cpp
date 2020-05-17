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

    const QMetaObject& metaObject             = IntegrationsSupported::staticMetaObject;
    int                index                  = metaObject.indexOfEnumerator("SupportedIntegrationTypes");
    QMetaEnum          metaEnumSupportedTypes = metaObject.enumerator(index);
    m_enumSupportedIntegrationTypes           = &metaEnumSupportedTypes;

    for (int i = 0; i < m_enumSupportedIntegrationTypes->keyCount(); i++) {
        QString name(m_enumSupportedIntegrationTypes->key(i));
        qCDebug(CLASS_LC()) << "Adding supported integration type:" << name.toLower();
        m_supportedIntegrations.append(name.toLower());
    }
}

QObject* Integrations::loadPlugin(const QString& type) {
    Launcher* l   = new Launcher();
    QObject*  obj = l->loadPlugin(m_pluginPath, type);

    // store the plugin objects
    m_plugins.insert(type, obj);

    // cleanup
    l->deleteLater();

    return obj;
}

QObject* Integrations::getPlugin(const QString& type) { return m_plugins.value(type); }

QList<QObject*> Integrations::getAllPlugins() { return m_plugins.values(); }

bool Integrations::isPluginLoaded(const QString& type) {
    if (m_plugins.contains(type)) {
        return true;
    } else {
        return false;
    }
}

void Integrations::createInstance(QObject* pluginObj, QVariantMap map) {
    Entities*      entities      = Entities::getInstance();
    Notifications* notifications = Notifications::getInstance();
    YioAPI*        api           = YioAPI::getInstance();
    Config*        config        = Config::getInstance();

    PluginInterface* interface = qobject_cast<PluginInterface*>(pluginObj);
    if (interface) {
        connect(interface, &PluginInterface::createDone, this, &Integrations::onCreateDone);

        interface->create(map, entities, notifications, api, config);
    }
}

QJsonObject Integrations::getPluginMetaData(const QString& pluginName) {
    Launcher* launcher   = new Launcher();
    QString   pluginPath = launcher->getPluginPath(m_pluginPath, pluginName);
    qCDebug(CLASS_LC()) << "Getting metadata for:" << pluginPath;
    QPluginLoader pluginLoader(pluginPath);
    return pluginLoader.metaData()["MetaData"].toObject();
}

// Integrations::~Integrations() { s_instance = nullptr; }

void Integrations::load() {
    Config* config = Config::getInstance();

    // read the config
    QVariantMap c = config->getAllIntegrations();

    qCDebug(CLASS_LC) << "Plugin path:" << m_pluginPath;

    // let's load the plugins
    for (QVariantMap::const_iterator iter = c.begin(); iter != c.end(); ++iter) {
        QObject* obj;
        if (!isPluginLoaded(iter.key())) {
            // load the plugin
            obj = loadPlugin(iter.key());
        } else {
            obj = getPlugin(iter.key());
        }

        // push the config to the integration
        QVariantMap map = iter.value().toMap();
        map.insert(Config::KEY_TYPE, iter.key());

        // create instance of the integration
        m_integrationsToLoad++;
        createInstance(obj, map);
    }

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

    qCDebug(CLASS_LC) << "Integrations loaded:" << m_integrationsLoaded << "from:" << m_integrationsToLoad;

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
    qCDebug(CLASS_LC()) << "Adding integration:" << type;
    const QString id = config.value(Config::KEY_ID).toString();
    m_integrations.insert(id, obj);
    m_integrationsFriendlyNames.insert(id, config.value(Config::KEY_FRIENDLYNAME).toString());
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

QStringList Integrations::getMDNSList() {
    QStringList mdnsList;
    for (int i = 0; i < m_supportedIntegrations.length(); i++) {
        mdnsList.append(getPluginMetaData(m_supportedIntegrations[i]).toVariantMap().value("mdns").toString());
    }

    return mdnsList;
}

QString Integrations::getType(const QString& id) { return m_integrationsTypes.value(id); }

QString Integrations::getTypeByMdns(const QString& mdns) {
    for (int i = 0; i < m_supportedIntegrations.length(); i++) {
        if (getPluginMetaData(m_supportedIntegrations[i]).toVariantMap().value("mdns").toString() == mdns) {
            return m_supportedIntegrations[i];
        }
    }

    return "";
}
