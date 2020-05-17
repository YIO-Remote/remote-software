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

#pragma once

#include <QMap>
#include <QObject>

#include "integrations_supported.h"
#include "integrationsinterface.h"
#include "yio-interface/integrationinterface.h"
#include "yio-interface/plugininterface.h"

class Integrations : public QObject, IntegrationsInterface {
    Q_OBJECT
    Q_INTERFACES(IntegrationsInterface)

 public:
    // list of all integrations
    Q_PROPERTY(QList<QObject*> list READ list NOTIFY listChanged)

    // load all integrations from config file
    Q_INVOKABLE void load();

    // get all integrations
    QList<QObject*> list();

    // get all integration ids
    QStringList listIds();

    // get an integration object by id
    Q_INVOKABLE QObject* get(const QString& id);

    // add an integration
    void add(const QVariantMap& config, QObject* obj, const QString& type) override;

    // remove an integraiton
    void remove(const QString& id);

    // get friendly name
    Q_INVOKABLE QString getFriendlyName(const QString& id);
    Q_INVOKABLE QString getFriendlyName(QObject* obj);

    // get mdns _tcp name
    Q_INVOKABLE QString     getMDNS(const QString& id);
    Q_INVOKABLE QStringList getMDNSList();

    // get the type of integration by id
    Q_INVOKABLE QString getType(const QString& id);
    QString             getTypeByMdns(const QString& mdns);

    // get a list of supported integrations
    QStringList supportedIntegrations() { return m_supportedIntegrations; }

    explicit Integrations(const QString& pluginPath);

    // get all plugins
    QObject*        loadPlugin(const QString& type);
    QObject*        getPlugin(const QString& type);
    QList<QObject*> getAllPlugins();
    bool            isPluginLoaded(const QString& type);

    // create integration instance
    void createInstance(QObject* pluginObj, QVariantMap map);

    // get plugin metadata
    QJsonObject getPluginMetaData(const QString& pluginName);

    static Integrations* getInstance() { return s_instance; }

 signals:
    void listChanged();
    void loadComplete();

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void onCreateDone(QMap<QObject*, QVariant> map);

 private:
    QStringList m_supportedIntegrations;

    QMap<QString, QObject*> m_plugins;
    QMap<QString, QObject*> m_integrations;
    QMap<QString, QString>  m_integrationsFriendlyNames;
    QMap<QString, QString>  m_integrationsMdns;
    QMap<QString, QString>  m_integrationsTypes;
    QString                 m_pluginPath;
    int                     m_integrationsToLoad = 0;
    int                     m_integrationsLoaded = 0;

    static Integrations* s_instance;

 protected:
    QMetaEnum* m_enumSupportedIntegrationTypes;
};
