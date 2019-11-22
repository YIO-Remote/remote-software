/******************************************************************************
 *
 * Copyright (C) 2019 Markus Zehnder <business@markuszehnder.ch>
 * Copyright (C) 2018 Thomas Ruschival <thomas@ruschival.de>
 *                    wpa_supplicant integration code based on
 *                    https://github.com/truschival/DigitalRoosterGui
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

#include "wifi_control.h"

#if defined (CONFIG_WPA_SUPPLICANT)
    #include "wifi_wpasupplicant.h"
#elif defined (Q_OS_LINUX) && defined (USE_WPA_SUPPLICANT)
    #include "wifi_wpasupplicant.h"
#elif defined (Q_OS_LINUX)
    #include "wifi_shellscripts.h"
#else
    #include "wifi_mock.h"
#endif

static Q_LOGGING_CATEGORY(CLASS_LC, "WifiControl");

/*!
    \class WifiControl

    \brief Abstract WiFi control interface

    \details wpa_supplicant integration code is based on https://github.com/truschival/DigitalRoosterGui
*/
WifiControl::WifiControl(QObject* parent)
    : QObject(parent)
    , m_scanStatus(Idle)
    , m_scanInterval(10000)         // TODO make scan interval configurable
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

WifiControl::~WifiControl()
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    stopScanTimer();
}

WifiControl& WifiControl::instance()
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    // TODO think about a redesign: device factory?
    #if defined (CONFIG_WPA_SUPPLICANT)
        static WifiWpaSupplicant singleton;
    #elif defined (Q_OS_LINUX) && defined (USE_WPA_SUPPLICANT)
        static WifiWpaSupplicant singleton;
    #elif defined (Q_OS_LINUX)
        static WifiShellScripts singleton;
    #else
        static WifiMock singleton;
    #endif

    return singleton;
}

QString WifiControl::macAddress()
{
    return m_wifiStatus.macAddress;
}

QString WifiControl::ssid()
{
    return m_wifiStatus.name;
}

int WifiControl::signalStrength()
{
    return m_wifiStatus.signalStrength;
}

QString WifiControl::ipAddress()
{
    return m_wifiStatus.ipAddress;
}

WifiControl::ScanStatus WifiControl::scanStatus() const
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return m_scanStatus;
}

void WifiControl::setScanStatus(ScanStatus stat)
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    m_scanStatus = stat;
    scanStatusChanged(m_scanStatus);
}

const QList<WifiNetwork>& WifiControl::networkScanResult()
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    return m_scanResults;
}

void WifiControl::startSignalStrengthScanning()
{
    m_signalStrengthScanning = true;
    startScanTimer();
}

void WifiControl::stopSignalStrengthScanning()
{
    m_signalStrengthScanning = false;
    if (!m_wifiStatusScanning) {
        stopScanTimer();
    }
}

void WifiControl::startWifiStatusScanning()
{
    m_wifiStatusScanning = true;
    startScanTimer();
}

void WifiControl::stopWifiStatusScanning()
{
    m_wifiStatusScanning = false;
    if (!m_signalStrengthScanning) {
        stopScanTimer();
    }
}

void WifiControl::startScanTimer()
{
    if (m_timerId == 0) {
        m_timerId = startTimer(m_scanInterval);
    }
}

void WifiControl::stopScanTimer()
{
    if (m_timerId > 0) {
        killTimer(m_timerId);
        m_timerId = 0;
    }
}

