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

#include "hw_factory_linux.h"

#include <QLoggingCategory>
#include <QtDebug>

#include "../../configutil.h"
#include "../hw_config.h"
#include "../systemservice_name.h"
#include "systemd.h"
#include "webserver_lighttpd.h"
#include "wifi_shellscripts.h"

#if defined(CONFIG_WPA_SUPPLICANT)
#include "wifi_wpasupplicant.h"
#endif

static Q_LOGGING_CATEGORY(CLASS_LC, "hw.factory.linux");

HardwareFactoryLinux::HardwareFactoryLinux(const QVariantMap &config, const QString &profile, QObject *parent)
    : HardwareFactoryDefault(parent) {
    Q_UNUSED(config)
    // profile might come in handy later on...
    Q_UNUSED(profile)
}

bool HardwareFactoryLinux::buildDevices(const QVariantMap &config) {
    // real Linux devices
    QVariantMap deviceCfg = ConfigUtil::getValue(config, HW_CFG_SYSTEMSERVICE).toMap();
    p_systemService = ConfigUtil::isEnabled(deviceCfg) ? buildSystemService(deviceCfg) : dummySystemService();

    deviceCfg = ConfigUtil::getValue(config, HW_CFG_WEBSERVER).toMap();
    p_webServerControl = ConfigUtil::isEnabled(deviceCfg) ? buildWebServerControl(deviceCfg) : dummyWebServerControl();

    deviceCfg = ConfigUtil::getValue(config, HW_CFG_WIFI).toMap();
    p_wifiControl = ConfigUtil::isEnabled(deviceCfg) ? buildWifiControl(deviceCfg) : dummyWifiControl();

    // dummy devices
    p_interruptHandler = dummyInterruptHandler();
    p_displayControl = dummyDisplayControl();
    p_batteryCharger = dummyBatteryCharger();
    p_batteryFuelGauge = dummyBatteryFuelGauge();
    p_hapticMotor = dummyHapticMotor();
    p_gestureSensor = dummyGestureSensor();
    p_lightSensor = dummyLightSensor();
    p_proximitySensor = dummyProximitySensor();

    return true;
}

// -- System services - RPi uses systemd
SystemService *HardwareFactoryLinux::buildSystemService(const QVariantMap &config) {
    QVariantMap systemdCfg = ConfigUtil::getValue(config, HW_CFG_SYSTEMD).toMap();
    QVariantMap serviceCfg = systemdCfg.value(HW_CFG_SYSTEMD_SERVICES).toMap();

    QMap<SystemServiceName, QString> serviceNameMap;
    serviceNameMap.insert(SystemServiceName::WIFI,
                          serviceCfg.value(HW_CFG_SERVICE_WIFI, HW_DEF_SERVICE_WIFI).toString());
    serviceNameMap.insert(SystemServiceName::NAME_RESOLUTION,
                          serviceCfg.value(HW_CFG_SERVICE_DNS, HW_DEF_SERVICE_DNS).toString());
    serviceNameMap.insert(SystemServiceName::WEBSERVER,
                          serviceCfg.value(HW_CFG_SERVICE_WEBSERVER, HW_DEF_SERVICE_WEBSERVER).toString());
    serviceNameMap.insert(SystemServiceName::NTP, serviceCfg.value(HW_CFG_SERVICE_NTP, HW_DEF_SERVICE_NTP).toString());
    serviceNameMap.insert(SystemServiceName::DHCP,
                          serviceCfg.value(HW_CFG_SERVICE_DHCP, HW_DEF_SERVICE_DHCP).toString());
    serviceNameMap.insert(SystemServiceName::ZEROCONF,
                          serviceCfg.value(HW_CFG_SERVICE_ZEROCONF, HW_DEF_SERVICE_ZEROCONF).toString());
    serviceNameMap.insert(SystemServiceName::NETWORKING,
                          serviceCfg.value(HW_CFG_SERVICE_NETWORKING, HW_DEF_SERVICE_NETWORKING).toString());

    Systemd *systemd = new Systemd(serviceNameMap, this);
    systemd->setUseSudo(systemdCfg.value(HW_CFG_SYSTEMD_SUDO, HW_DEF_SYSTEMD_SUDO).toBool());
    systemd->setSystemctlTimeout(systemdCfg.value(HW_CFG_SYSTEMD_TIMEOUT, HW_DEF_SYSTEMD_TIMEOUT).toInt());

    return systemd;
}

// -- Web Server control - RPi uses httpd
WebServerControl *HardwareFactoryLinux::buildWebServerControl(const QVariantMap &config) {
    QVariantMap        webCfg = ConfigUtil::getValue(config, HW_CFG_LIGHTTPD).toMap();
    WebServerLighttpd *lighttpd = new WebServerLighttpd(getSystemService(), this);
    lighttpd->setConfigFile(webCfg.value(HW_CFG_LIGHTTPD_CFG_FILE, HW_DEF_LIGHTTPD_CFG_FILE).toString());
    lighttpd->setWifiSetupConfig(webCfg.value(HW_CFG_LIGHTTPD_WIFI_CFG, HW_DEF_LIGHTTPD_WIFI_CFG).toString());
    lighttpd->setWebConfiguratorConfig(webCfg.value(HW_CFG_LIGHTTPD_WEB_CFG, HW_DEF_LIGHTTPD_WEB_CFG).toString());
    return lighttpd;
}

// -- WiFi control - RPi uses wpa_supplicant control interface and as fallback the old shell scripts
WifiControl *HardwareFactoryLinux::buildWifiControl(const QVariantMap &wifiCfg) {
    WifiControl *wifiControl = nullptr;
    // determine which interface driver to use
    bool useShellScript = wifiCfg.value(HW_CFG_WIFI_USE_SH, HW_DEF_WIFI_USE_SH).toBool();
    bool wpaSupplicantAvailable = false;

#if defined(CONFIG_WPA_SUPPLICANT)
    wpaSupplicantAvailable = true;
    if (!useShellScript) {
        qCDebug(CLASS_LC()) << "Using wpa_supplicant as WiFi control interface";
        WifiWpaSupplicant *wps = new WifiWpaSupplicant(getWebServerControl(), getSystemService(), this);

        QVariantMap wpaCfg = wifiCfg.value(HW_CFG_WIFI_INTERFACE).toMap().value(HW_CFG_WIFI_IF_WPA_SUPP).toMap();
        wps->setWpaSupplicantSocketPath(wpaCfg.value(HW_CFG_WIFI_WPA_SOCKET, HW_DEF_WIFI_WPA_SOCKET).toString());
        wps->setRemoveNetworksBeforeJoin(wpaCfg.value(HW_CFG_WIFI_RM_BEFORE_JOIN, HW_DEF_WIFI_RM_BEFORE_JOIN).toBool());

        wifiControl = wps;
    }
#endif
    if (useShellScript || !wpaSupplicantAvailable) {
        qCDebug(CLASS_LC()) << "Using shell scripts to control WiFi";
        WifiShellScripts *wss = new WifiShellScripts(getSystemService(), this);

        QVariantMap shCfg = wifiCfg.value(HW_CFG_WIFI_INTERFACE).toMap().value(HW_CFG_WIFI_IF_SHELLSCRIPT).toMap();
        wss->setUseSudo(shCfg.value(HW_CFG_WIFI_SH_SUDO, HW_DEF_WIFI_SH_SUDO).toBool());
        wss->setScriptTimeout(shCfg.value(HW_CFG_WIFI_SH_TIMEOUT, HW_DEF_WIFI_SH_TIMEOUT).toInt());
        wss->setScriptGetIp(shCfg.value(HW_CFG_WIFI_SH_GET_IP, HW_DEF_WIFI_SH_GET_IP).toString());
        wss->setScriptGetRssi(shCfg.value(HW_CFG_WIFI_SH_GET_RSSI, HW_DEF_WIFI_SH_GET_RSSI).toString());
        wss->setScriptGetSsid(shCfg.value(HW_CFG_WIFI_SH_GET_SSID, HW_DEF_WIFI_SH_GET_SSID).toString());
        wss->setScriptGetMacAddress(shCfg.value(HW_CFG_WIFI_SH_GET_MAC, HW_DEF_WIFI_SH_GET_MAC).toString());
        wss->setScriptConnectWifi(shCfg.value(HW_CFG_WIFI_SH_CONNECT, HW_DEF_WIFI_SH_CONNECT).toString());
        wss->setScriptListNetworks(shCfg.value(HW_CFG_WIFI_SH_LIST, HW_DEF_WIFI_SH_LIST).toString());
        wss->setScriptClearNetworks(shCfg.value(HW_CFG_WIFI_SH_CLEAR_NET, HW_DEF_WIFI_SH_CLEAR_NET).toString());
        wss->setScriptStartAP(shCfg.value(HW_CFG_WIFI_SH_START_AP, HW_DEF_WIFI_SH_START_AP).toString());

        wifiControl = wss;
    }

    Q_ASSERT(wifiControl);

    wifiControl->setMaxScanResults(wifiCfg.value(HW_CFG_WIFI_SCAN_RESULTS, HW_DEF_WIFI_SCAN_RESULTS).toInt());
    wifiControl->setPollInterval(wifiCfg.value(HW_CFG_WIFI_POLL_INTERVAL, HW_DEF_WIFI_POLL_INTERVAL).toInt());
    wifiControl->setNetworkJoinRetryCount(wifiCfg.value(HW_CFG_WIFI_JOIN_RETRY, HW_DEF_WIFI_JOIN_RETRY).toInt());
    wifiControl->setNetworkJoinRetryDelay(wifiCfg.value(HW_CFG_WIFI_JOIN_DELAY, HW_DEF_WIFI_JOIN_DELAY).toInt());

    return wifiControl;
}
