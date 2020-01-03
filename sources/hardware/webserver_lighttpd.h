/******************************************************************************
 *
 * Copyright (C) 2019 Markus Zehnder <business@markuszehnder.ch>
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

#include "systemservice.h"
#include "webserver_control.h"

/**
 * @brief A lighttpd implementation of the WebServerControl interface.
 */
class WebServerLighttpd : public WebServerControl {
    Q_OBJECT

 public:
    explicit WebServerLighttpd(SystemService *systemService, QObject *parent = nullptr);

 private:
    SystemService *p_systemService;

    // WebServerControl interface
 public:
    Q_INVOKABLE bool startService() override;
    Q_INVOKABLE bool stopService() override;
    Q_INVOKABLE bool restartService() override;
    Q_INVOKABLE bool reloadService() override;
    Q_INVOKABLE bool startWifiSetupPortal() override;
    Q_INVOKABLE bool startWebConfigurator() override;

    QString configFile() const;
    void    setConfigFile(const QString &configFile);

    QString wifiSetupConfig() const;
    void    setWifiSetupConfig(const QString &wifiSetupConfig);

    QString webConfiguratorConfig() const;
    void    setWebConfiguratorConfig(const QString &webConfiguratorConfig);

 private:
    QString m_configFile;
    QString m_wifiSetupConfig;
    QString m_webConfiguratorConfig;
};
