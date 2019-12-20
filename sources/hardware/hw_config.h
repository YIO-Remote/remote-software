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

#ifndef HW_CONFIG_H
#define HW_CONFIG_H


#define HW_CFG_SYSTEMD            "systemd"
#define HW_CFG_SYSTEMD_SERVICES   "services"

#define HW_CFG_SYSTEMD_SUDO       "sudo"
#define HW_DEF_SYSTEMD_SUDO       false

#define HW_CFG_SYSTEMD_TIMEOUT    "timeoutMs"
#define HW_DEF_SYSTEMD_TIMEOUT    30000

#define HW_CFG_SERVICE_WIFI       "wifi"
#define HW_DEF_SERVICE_WIFI       "wpa_supplicant@wlan0.service"
#define HW_CFG_SERVICE_DNS        "dns"
#define HW_DEF_SERVICE_DNS        "systemd-resolved.service"
#define HW_CFG_SERVICE_WEBSERVER  "webserver"
#define HW_DEF_SERVICE_WEBSERVER  "lighttpd.service"
#define HW_CFG_SERVICE_NTP        "ntp"
#define HW_DEF_SERVICE_NTP        "systemd-timesyncd.service"
#define HW_CFG_SERVICE_DHCP       "dhcp"
#define HW_DEF_SERVICE_DHCP       "dhcpcd.service"
#define HW_CFG_SERVICE_SHUTDOWN   "shutdown"
#define HW_DEF_SERVICE_SHUTDOWN   "shutdown.service"
#define HW_CFG_SERVICE_YIO_UPDATE "yio-update"
#define HW_DEF_SERVICE_YIO_UPDATE "update.service"
#define HW_CFG_SERVICE_ZEROCONF   "zeroconf"
#define HW_DEF_SERVICE_ZEROCONF   "avahi-daemon"
#define HW_CFG_SERVICE_NETWORKING "networking"
#define HW_DEF_SERVICE_NETWORKING "systemd-networkd"

#define HW_CFG_WIFI                "wifi"

#define HW_CFG_WIFI_SCAN_RESULTS   "maxScanResults"
#define HW_DEF_WIFI_SCAN_RESULTS   50
#define HW_CFG_WIFI_POLL_INTERVAL  "pollIntervalMs"
#define HW_DEF_WIFI_POLL_INTERVAL  10000
#define HW_CFG_WIFI_WPA_SOCKET     "wpaSupplicantSocketPath"
#define HW_DEF_WIFI_WPA_SOCKET     "/var/run/wpa_supplicant/wlan0"
#define HW_CFG_WIFI_RM_BEFORE_JOIN "removeNetworksBeforeJoin"
#define HW_DEF_WIFI_RM_BEFORE_JOIN false
#define HW_CFG_WIFI_JOIN_RETRY     "joinRetryCount"
#define HW_DEF_WIFI_JOIN_RETRY     5
#define HW_CFG_WIFI_JOIN_DELAY     "joinRetryDelayMs"
#define HW_DEF_WIFI_JOIN_DELAY     3000
#define HW_CFG_WIFI_SCRIPT_TIMEOUT "scriptTimeoutMs"
#define HW_DEF_WIFI_SCRIPT_TIMEOUT 30000

#define HW_CFG_WEBSERVER           "webserver"
#define HW_CFG_LIGHTTPD            "lighttpd"

#define HW_CFG_LIGHTTPD_CFG_FILE   "configFile"
#define HW_DEF_LIGHTTPD_CFG_FILE   "/etc/lighttpd/lighttpd.conf"
#define HW_CFG_LIGHTTPD_WIFI_CFG   "wifiSetupConfig"
#define HW_DEF_LIGHTTPD_WIFI_CFG   "/etc/lighttpd/lighttpd-wifisetup.conf"
#define HW_CFG_LIGHTTPD_WEB_CFG    "webConfiguratorConfig"
#define HW_DEF_LIGHTTPD_WEB_CFG    "/etc/lighttpd/lighttpd-config.conf"

#endif // HW_CONFIG_H
