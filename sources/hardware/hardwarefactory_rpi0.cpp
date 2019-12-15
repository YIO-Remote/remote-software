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

#include "hardwarefactory_rpi0.h"
#include "systemservice_name.h"
#include "systemd.h"
#include "webserver_lighttpd.h"

#if defined (CONFIG_WPA_SUPPLICANT)
    #include "wifi_wpasupplicant.h"
#else
    #include "wifi_shellscripts.h"
#endif

static Q_LOGGING_CATEGORY(CLASS_LC, "HwRpi0");

HardwareFactoryRPi0::HardwareFactoryRPi0(ConfigInterface *config, QObject *parent) : HardwareFactory(parent)
{
    Q_UNUSED(config)

    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    // KISS: sufficient for now, custom logic possible with config interface when needed.

    QMap<SystemServiceName, QString> serviceNameMap;
    serviceNameMap.insert(SystemServiceName::WIFI, "wpa_supplicant@wlan0.service");
    serviceNameMap.insert(SystemServiceName::NAME_RESOLUTION, "systemd-resolved.service");
    serviceNameMap.insert(SystemServiceName::WEBSERVER, "lighttpd.service");
    serviceNameMap.insert(SystemServiceName::NTP, "systemd-timesyncd.service");
    serviceNameMap.insert(SystemServiceName::DHCP, "dhcpcd.service");
    serviceNameMap.insert(SystemServiceName::SHUTDOWN, "shutdown.service");
    serviceNameMap.insert(SystemServiceName::YIO_UPDATE, "update.service");
    serviceNameMap.insert(SystemServiceName::NETWORKING, "lighttpd.service");
    serviceNameMap.insert(SystemServiceName::ZEROCONF, "avahi-daemon");
    serviceNameMap.insert(SystemServiceName::NETWORKING, "systemd-networkd");

    p_systemService = new Systemd(serviceNameMap, this);

    p_webServerControl = new WebServerLighttpd(p_systemService);

#if defined (CONFIG_WPA_SUPPLICANT)
    p_wifiControl = new WifiWpaSupplicant(p_webServerControl, p_systemService, this);
#else
    p_wifiControl = new WifiShellScripts(this);
#endif
}

WifiControl *HardwareFactoryRPi0::getWifiControl()
{
    return p_wifiControl;
}

SystemService *HardwareFactoryRPi0::getSystemService()
{
    return p_systemService;
}


WebServerControl *HardwareFactoryRPi0::getWebServerControl()
{
    return p_webServerControl;
}
