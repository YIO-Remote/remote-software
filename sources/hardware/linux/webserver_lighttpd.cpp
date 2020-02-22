/******************************************************************************
 *
 * Copyright (C) 2019-2020 Markus Zehnder <business@markuszehnder.ch>
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

#include <QFile>
#include <QLoggingCategory>
#include <QtDebug>

#include "webserver_lighttpd.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "WebLighttpd");

WebServerLighttpd::WebServerLighttpd(SystemService *systemService, QObject *parent)
    : WebServerControl(parent),
      p_systemService(systemService),
      m_configFile(),
      m_wifiSetupConfig(),
      m_webConfiguratorConfig() {
}

bool WebServerLighttpd::startService() { return p_systemService->startService(SystemServiceName::WEBSERVER); }

bool WebServerLighttpd::stopService() { return p_systemService->stopService(SystemServiceName::WEBSERVER); }

bool WebServerLighttpd::restartService() { return p_systemService->restartService(SystemServiceName::WEBSERVER); }

bool WebServerLighttpd::reloadService() { return p_systemService->reloadService(SystemServiceName::WEBSERVER); }

bool WebServerLighttpd::startWifiSetupPortal() {
    if (!QFile::exists(m_wifiSetupConfig)) {
        qCCritical(CLASS_LC) << "Lighttpd wifi-setup configuration file does not exist:" << m_wifiSetupConfig;
        return false;
    }
    QFile::remove(m_configFile);
    if (!QFile::copy(m_wifiSetupConfig, m_configFile)) {
        qCCritical(CLASS_LC) << "Error replacing lighttpd configuration file " << m_configFile
                             << "with:" << m_wifiSetupConfig;
        return false;
    }
    return restartService();
}

bool WebServerLighttpd::startWebConfigurator() {
    if (!QFile::exists(m_webConfiguratorConfig)) {
        qCCritical(CLASS_LC) << "Lighttpd web-configurator configuration file does not exist:"
                             << m_webConfiguratorConfig;
        return false;
    }
    QFile::remove(m_configFile);
    if (!QFile::copy(m_webConfiguratorConfig, m_configFile)) {  // NOLINT false positive
        qCCritical(CLASS_LC) << "Error replacing lighttpd configuration file " << m_configFile
                             << "with:" << m_webConfiguratorConfig;
        return false;
    }
    return restartService();
}

QString WebServerLighttpd::configFile() const { return m_configFile; }

void WebServerLighttpd::setConfigFile(const QString &configFile) { m_configFile = configFile; }

QString WebServerLighttpd::wifiSetupConfig() const { return m_wifiSetupConfig; }

void WebServerLighttpd::setWifiSetupConfig(const QString &wifiSetupConfig) { m_wifiSetupConfig = wifiSetupConfig; }

QString WebServerLighttpd::webConfiguratorConfig() const { return m_webConfiguratorConfig; }

void WebServerLighttpd::setWebConfiguratorConfig(const QString &webConfiguratorConfig) {
    m_webConfiguratorConfig = webConfiguratorConfig;
}
