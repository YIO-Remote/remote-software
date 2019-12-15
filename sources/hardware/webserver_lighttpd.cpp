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

#include <QLoggingCategory>
#include <QtDebug>

#include "webserver_lighttpd.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "WebLighttpd");

WebServerLighttpd::WebServerLighttpd(SystemService *systemService, QObject *parent)
    : WebServerControl(parent)
    , p_systemService(systemService)
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}


bool WebServerLighttpd::startService()
{
    return p_systemService->startService(SystemServiceName::WEBSERVER);
}

bool WebServerLighttpd::stopService()
{
    return p_systemService->stopService(SystemServiceName::WEBSERVER);
}

bool WebServerLighttpd::restartService()
{
    return p_systemService->restartService(SystemServiceName::WEBSERVER);
}

bool WebServerLighttpd::reloadService()
{
    return p_systemService->reloadService(SystemServiceName::WEBSERVER);
}

bool WebServerLighttpd::startWifiSetupPortal()
{
    // TODO copy config
    // cp /etc/lighttpd/lighttpd-wifisetup.conf /etc/lighttpd/lighttpd.conf
    return restartService();
}

bool WebServerLighttpd::startWebConfigurator()
{
    // TODO copy config
    // cp /etc/lighttpd/lighttpd-config.conf /etc/lighttpd/lighttpd.conf
    return restartService();
}
