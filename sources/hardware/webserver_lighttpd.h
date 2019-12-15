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

#ifndef WEBSERVERLIGHTTPD_H
#define WEBSERVERLIGHTTPD_H

#include "webserver_control.h"
#include "systemservice.h"

/**
 * @brief A lighttpd implementation of the WebServerControl interface.
 */
class WebServerLighttpd : public WebServerControl
{
    Q_OBJECT
public:
    WebServerLighttpd(SystemService *systemService, QObject *parent = nullptr);

private:
    SystemService *p_systemService;

    // WebServerControl interface
public:
    Q_INVOKABLE virtual bool startService() override;
    Q_INVOKABLE virtual bool stopService() override;
    Q_INVOKABLE virtual bool restartService() override;
    Q_INVOKABLE virtual bool reloadService() override;
    Q_INVOKABLE virtual bool startWifiSetupPortal() override;
    Q_INVOKABLE virtual bool startWebConfigurator() override;
};

#endif // WEBSERVERLIGHTTPD_H
