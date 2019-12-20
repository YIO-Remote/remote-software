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

#include "hw_config.h"
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

    QMap<QString, QVariant> systemdCfg = config.value(HW_CFG_SYSTEMD).toMap();
    QMap<QString, QVariant> serviceCfg = config.value(HW_CFG_SYSTEMD_SERVICES).toMap();

    // KISS: sufficient for now, custom logic possible with config interface when needed.
    QMap<SystemServiceName, QString> serviceNameMap;
    serviceNameMap.insert(SystemServiceName::WIFI, serviceCfg.value(HW_CFG_SERVICE_WIFI, HW_DEF_SERVICE_WIFI).toString());
    serviceNameMap.insert(SystemServiceName::NAME_RESOLUTION, serviceCfg.value(HW_CFG_SERVICE_DNS, HW_DEF_SERVICE_DNS).toString());
    serviceNameMap.insert(SystemServiceName::WEBSERVER, serviceCfg.value(HW_CFG_SERVICE_WEBSERVER, HW_DEF_SERVICE_WEBSERVER).toString());
    serviceNameMap.insert(SystemServiceName::NTP, serviceCfg.value(HW_CFG_SERVICE_NTP, HW_DEF_SERVICE_NTP).toString());
    serviceNameMap.insert(SystemServiceName::DHCP, serviceCfg.value(HW_CFG_SERVICE_DHCP, HW_DEF_SERVICE_DHCP).toString());
    serviceNameMap.insert(SystemServiceName::SHUTDOWN, serviceCfg.value(HW_CFG_SERVICE_SHUTDOWN, HW_DEF_SERVICE_SHUTDOWN).toString());
    serviceNameMap.insert(SystemServiceName::YIO_UPDATE, serviceCfg.value(HW_CFG_SERVICE_YIO_UPDATE, HW_DEF_SERVICE_YIO_UPDATE).toString());
    serviceNameMap.insert(SystemServiceName::ZEROCONF, serviceCfg.value(HW_CFG_SERVICE_ZEROCONF, HW_DEF_SERVICE_ZEROCONF).toString());
    serviceNameMap.insert(SystemServiceName::NETWORKING, serviceCfg.value(HW_CFG_SERVICE_NETWORKING, HW_DEF_SERVICE_NETWORKING).toString());

    Systemd *systemd = new Systemd(serviceNameMap, this);
    systemd->setUseSudo(systemdCfg.value(HW_CFG_SYSTEMD_SUDO, HW_DEF_SYSTEMD_SUDO).toBool());
    systemd->setSystemctlTimeout(systemdCfg.value(HW_CFG_SYSTEMD_TIMEOUT, HW_DEF_SYSTEMD_TIMEOUT).toInt());

    p_systemService = systemd;

    QMap<QString, QVariant> webCfg = config.value(HW_CFG_WEBSERVER).toMap().value(HW_CFG_LIGHTTPD).toMap();
    WebServerLighttpd *lighttpd = new WebServerLighttpd(p_systemService, this);
    lighttpd->setConfigFile(webCfg.value(HW_CFG_LIGHTTPD_CFG_FILE, HW_DEF_LIGHTTPD_CFG_FILE).toString());
    lighttpd->setWifiSetupConfig(webCfg.value(HW_CFG_LIGHTTPD_WIFI_CFG, HW_DEF_LIGHTTPD_WIFI_CFG).toString());
    lighttpd->setWebConfiguratorConfig(webCfg.value(HW_CFG_LIGHTTPD_WEB_CFG, HW_DEF_LIGHTTPD_WEB_CFG).toString());
    p_webServerControl = lighttpd;

    QMap<QString, QVariant> wifiCfg = config.value(HW_CFG_WIFI).toMap();
#if defined (CONFIG_WPA_SUPPLICANT)
    WifiWpaSupplicant *wps = new WifiWpaSupplicant(p_webServerControl, p_systemService, this);
    wps->setWpaSupplicantSocketPath(wifiCfg.value(HW_CFG_WIFI_WPA_SOCKET, HW_DEF_WIFI_WPA_SOCKET).toString());
    wps->setRemoveNetworksBeforeJoin(wifiCfg.value(HW_CFG_WIFI_RM_BEFORE_JOIN, HW_DEF_WIFI_RM_BEFORE_JOIN).toBool());
    wps->setNetworkJoinRetryCount(wifiCfg.value(HW_CFG_WIFI_JOIN_RETRY, HW_DEF_WIFI_JOIN_RETRY).toInt());
    wps->setNetworkJoinRetryDelayMs(wifiCfg.value(HW_CFG_WIFI_JOIN_DELAY, HW_DEF_WIFI_JOIN_DELAY).toInt());
    p_wifiControl = wps;
#else
    WifiShellScripts *wss = new WifiShellScripts(p_systemService, this);
    wss->setScriptTimeout(wifiCfg.value(HW_CFG_WIFI_SCRIPT_TIMEOUT, HW_DEF_WIFI_SCRIPT_TIMEOUT).toInt());
    p_wifiControl = wss;
#endif
    p_wifiControl->setMaxScanResults(wifiCfg.value(HW_CFG_WIFI_SCAN_RESULTS, HW_DEF_WIFI_SCAN_RESULTS).toInt());
    p_wifiControl->setPollInterval(wifiCfg.value(HW_CFG_WIFI_POLL_INTERVAL, HW_DEF_WIFI_POLL_INTERVAL).toInt());
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
