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

#pragma once

/**
 * Common hardware configuration keys.
 * Platform specific keys should be defined either in the specific factory or in a dedicated header file.
 */

#define HW_CFG_PATH_I2C_DEV       "i2c/device"
#define HW_CFG_PATH_I2C_ID        "i2c/id"

#define HW_CFG_PATH_GPIO_PIN      "gpio/pin"
#define HW_CFG_PATH_INTR_GPIO_PIN "interrupt/gpio/pin"

#define HW_CFG_DISPLAY_CONTROL    "display/control"
#define HW_CFG_BTN_INTR_HANDLER   "buttonInterruptHandler"
#define HW_CFG_BATTERY_CHARGER    "batteryCharger"
#define HW_CFG_BATTERY_FUEL_GAUGE "batteryFuelGauge"
#define HW_CFG_HAPTIC_MOTOR       "hapticMotor"
#define HW_CFG_GESTURE            "gesture"
#define HW_CFG_LIGHT              "light"
#define HW_CFG_PROXIMITY          "proximity"

#define HW_CFG_SYSTEMSERVICE      "systemservice"

#define HW_CFG_SYSTEMD            "systemd"
#define HW_CFG_SYSTEMD_SERVICES   "services"

#define HW_CFG_SYSTEMD_SUDO       "sudo"
#define HW_DEF_SYSTEMD_SUDO       false

#define HW_CFG_SYSTEMD_TIMEOUT    "timeout"
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
#define HW_CFG_SERVICE_ZEROCONF   "zeroconf"
#define HW_DEF_SERVICE_ZEROCONF   "avahi-daemon"
#define HW_CFG_SERVICE_NETWORKING "networking"
#define HW_DEF_SERVICE_NETWORKING "systemd-networkd"

#define HW_CFG_WIFI                "wifi"

#define HW_CFG_WIFI_SCAN_RESULTS   "maxScanResults"
#define HW_DEF_WIFI_SCAN_RESULTS   50
#define HW_CFG_WIFI_JOIN_RETRY     "joinRetryCount"
#define HW_DEF_WIFI_JOIN_RETRY     5
#define HW_CFG_WIFI_JOIN_DELAY     "joinRetryDelay"
#define HW_DEF_WIFI_JOIN_DELAY     3000
#define HW_CFG_WIFI_POLL_INTERVAL  "pollInterval"
#define HW_DEF_WIFI_POLL_INTERVAL  10000
#define HW_CFG_WIFI_USE_SH         "useShellScript"
#define HW_DEF_WIFI_USE_SH         false

#define HW_CFG_WIFI_INTERFACE      "interface"

#define HW_CFG_WIFI_IF_WPA_SUPP    "wpa_supplicant"

#define HW_CFG_WIFI_WPA_SOCKET     "socketPath"
#define HW_DEF_WIFI_WPA_SOCKET     "/var/run/wpa_supplicant/wlan0"
#define HW_CFG_WIFI_RM_BEFORE_JOIN "removeNetworksBeforeJoin"
#define HW_DEF_WIFI_RM_BEFORE_JOIN false

#define HW_CFG_WIFI_IF_SHELLSCRIPT "shellScript"

#define HW_CFG_WIFI_SH_SUDO        "sudo"
#define HW_DEF_WIFI_SH_SUDO        false
#define HW_CFG_WIFI_SH_TIMEOUT     "timeout"
#define HW_DEF_WIFI_SH_TIMEOUT     30000
#define HW_CFG_WIFI_SH_CLEAR_NET   "clearNetworks"
#define HW_DEF_WIFI_SH_CLEAR_NET   ""
#define HW_CFG_WIFI_SH_CONNECT     "connect"
#define HW_DEF_WIFI_SH_CONNECT     "/opt/yio/scripts/wifi_network_create.sh"
#define HW_CFG_WIFI_SH_LIST        "listNetworks"
#define HW_DEF_WIFI_SH_LIST        "/opt/yio/scripts/wifi_network_list.sh"
#define HW_CFG_WIFI_SH_START_AP    "startAP"
#define HW_DEF_WIFI_SH_START_AP    "/opt/yio/scripts/reset-wifi.sh"
#define HW_CFG_WIFI_SH_GET_SSID    "getSSID"
#define HW_DEF_WIFI_SH_GET_SSID    "/opt/yio/scripts/wifi_ssid.sh"
#define HW_CFG_WIFI_SH_GET_IP      "getIP"
#define HW_DEF_WIFI_SH_GET_IP      "/opt/yio/scripts/wifi_ip.sh"
#define HW_CFG_WIFI_SH_GET_MAC     "getMAC"
#define HW_DEF_WIFI_SH_GET_MAC     "cat /sys/class/net/wlan0/address"
#define HW_CFG_WIFI_SH_GET_RSSI    "getRSSI"
#define HW_DEF_WIFI_SH_GET_RSSI    "/opt/yio/scripts/wifi_rssi.sh"


#define HW_CFG_ACCESS_POINT        "accessPoint"


#define HW_CFG_WEBSERVER           "webserver"
#define HW_CFG_LIGHTTPD            "lighttpd"

#define HW_CFG_LIGHTTPD_CFG_FILE   "configFile"
#define HW_DEF_LIGHTTPD_CFG_FILE   "/etc/lighttpd/lighttpd.conf"
#define HW_CFG_LIGHTTPD_WIFI_CFG   "wifiSetupConfig"
#define HW_DEF_LIGHTTPD_WIFI_CFG   "/etc/lighttpd/lighttpd-wifisetup.conf"
#define HW_CFG_LIGHTTPD_WEB_CFG    "webConfiguratorConfig"
#define HW_DEF_LIGHTTPD_WEB_CFG    "/etc/lighttpd/lighttpd-config.conf"

#define HW_CFG_SYSINFO             "sysinfo"
#define HW_CFG_SYSINFO_SYSFS_TEMP  "sysfsTemp"
#define HW_DEF_SYSINFO_SYSFS_TEMP  "/sys/class/thermal/thermal_zone0/temp"
