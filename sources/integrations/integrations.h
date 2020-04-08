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

#include "integrationsinterface.h"
#include "yio-interface/integrationinterface.h"
#include "yio-interface/plugininterface.h"

class Integrations : public QObject, IntegrationsInterface {
    Q_OBJECT
    Q_INTERFACES(IntegrationsInterface)

 public:
    enum States {
        CONNECTED    = IntegrationInterface::CONNECTED,
        CONNECTING   = IntegrationInterface::CONNECTING,
        DISCONNECTED = IntegrationInterface::DISCONNECTED
    };
    Q_ENUM(States)

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
    Q_INVOKABLE void add(const QVariantMap& config, QObject* obj, const QString& type) override;

    // remove an integraiton
    Q_INVOKABLE void remove(const QString& id);

    // get friendly name
    Q_INVOKABLE QString getFriendlyName(const QString& id);
    Q_INVOKABLE QString getFriendlyName(QObject* obj);

    // get mdns _tcp name
    Q_INVOKABLE QString     getMDNS(const QString& id);
    Q_INVOKABLE QStringList getMDNSList();

    // get the type of integration by id
    Q_INVOKABLE QString getType(const QString& id);

    // get a list of supported integrations
    QStringList supportedIntegrations() { return m_supported_integrations; }

    explicit Integrations(const QString& pluginPath);

    // get all plugins
    QList<QObject*> getAllPlugins();

    static Integrations* getInstance() { return s_instance; }

 signals:
    void listChanged();
    void loadComplete();

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void onCreateDone(QMap<QObject*, QVariant> map);

 private:
    /// ADD NEW INTEGRATION TYPE HERE
    QStringList m_supported_integrations = {"dock",    "homeassistant", "homey", "spotify",
                                            "openhab", "openweather",   "roon"};

    QMap<QString, QObject*> m_plugins;
    QMap<QString, QObject*> m_integrations;
    QMap<QString, QString>  m_integrations_friendly_names;
    QMap<QString, QString>  m_integrations_mdns;
    QMap<QString, QString>  m_integrations_types;
    QString                 m_pluginPath;
    int                     m_integrationsToLoad = 0;
    int                     m_integrationsLoaded = 0;

    static Integrations* s_instance;
};
