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

#include <assert.h>

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

HardwareFactoryRPi0::HardwareFactoryRPi0(const QVariantMap &config, QObject *parent) : HardwareFactory(parent)
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    // TODO define configuration structure
    QMap<QString, QVariant> systemdCfg = config.value("systemd").toMap();

    // KISS: sufficient for now, custom logic possible with config interface when needed.
    QMap<SystemServiceName, QString> serviceNameMap;
    serviceNameMap.insert(SystemServiceName::WIFI, systemdCfg.value("wifi", "wpa_supplicant@wlan0.service").toString());
    serviceNameMap.insert(SystemServiceName::NAME_RESOLUTION, systemdCfg.value("name-resolution", "systemd-resolved.service").toString());
    serviceNameMap.insert(SystemServiceName::WEBSERVER, systemdCfg.value("webserver", "lighttpd.service").toString());
    serviceNameMap.insert(SystemServiceName::NTP, systemdCfg.value("ntp", "systemd-timesyncd.service").toString());
    serviceNameMap.insert(SystemServiceName::DHCP, systemdCfg.value("dhcp", "dhcpcd.service").toString());
    serviceNameMap.insert(SystemServiceName::SHUTDOWN, systemdCfg.value("shutdown", "shutdown.service").toString());
    serviceNameMap.insert(SystemServiceName::YIO_UPDATE, systemdCfg.value("yio-update", "update.service").toString());
    serviceNameMap.insert(SystemServiceName::ZEROCONF, systemdCfg.value("zeroconf", "avahi-daemon").toString());
    serviceNameMap.insert(SystemServiceName::NETWORKING, systemdCfg.value("networking", "systemd-networkd").toString());

    Systemd *systemd = new Systemd(serviceNameMap, this);
    // TODO define configuration key
    systemd->setUseSudo(config.value("sudo", false).toBool());
    p_systemService = systemd;

    p_webServerControl = new WebServerLighttpd(p_systemService);

    QMap<QString, QVariant> wifiCfg = config.value("wifi").toMap();
#if defined (CONFIG_WPA_SUPPLICANT)
    WifiWpaSupplicant *wps = new WifiWpaSupplicant(p_webServerControl, p_systemService, this);
    wps->setWpaSupplicantSocketPath(wifiCfg.value("wpaSupplicantSocketPath", "/var/run/wpa_supplicant/wlan0").toString());
    wps->setRemoveNetworksBeforeJoin(wifiCfg.value("removeNetworksBeforeJoin", false).toBool());
    wps->setNetworkJoinRetryCount(wifiCfg.value("networkJoinRetryCount", 5).toInt());
    wps->setNetworkJoinRetryDelayMs(wifiCfg.value("networkJoinRetryDelayMs", 3000).toInt());
    p_wifiControl = wps;
#else
    WifiShellScripts *wss = new WifiShellScripts(p_systemService, this);
    wss->setScriptTimeout(wifiCfg.value("scriptTimeoutMs", 30000).toInt());
    p_wifiControl = wss;
#endif
}

WifiControl *HardwareFactoryRPi0::getWifiControl()
{
    assert(p_wifiControl);

    return p_wifiControl;
}

SystemService *HardwareFactoryRPi0::getSystemService()
{
    assert(p_systemService);

    return p_systemService;
}


WebServerControl *HardwareFactoryRPi0::getWebServerControl()
{
    assert(p_webServerControl);

    return p_webServerControl;
}
