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

#include "wifi_control.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "WifiCtrl");

WifiControl::WifiControl(QObject* parent)
    : QObject(parent)
    , m_scanStatus(Idle)
    , m_maxScanResults(50)
    , m_scanInterval(10000)         // TODO make scan interval configurable
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

WifiControl::~WifiControl()
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    stopScanTimer();
}

bool WifiControl::validateAuthentication(WifiSecurity security, const QString &preSharedKey)
{
    switch (security) {
    case WifiSecurity::IEEE8021X :
    case WifiSecurity::WPA_EAP :
    case WifiSecurity::WPA2_EAP :
        qWarning(CLASS_LC) << "Authentication mode not supported:" << security;
        return false;
    default:
        break;
    }

    if (security == WifiSecurity::DEFAULT
            || security == WifiSecurity::WPA_PSK
            || security == WifiSecurity::WPA2_PSK) {
        if (preSharedKey.length() < 8 || preSharedKey.length() > 64) {
            qWarning(CLASS_LC) << "WPA Pre-Shared Key Error:"
                << "WPA-PSK requires a passphrase of 8 to 63 characters or 64 hex digit PSK";
            return false;
        }
    }

    return true;
}

/**
 * Default implementation
 */
bool WifiControl::isConnected()
{
    return m_connected;
}

void WifiControl::setConnected(bool connected)
{
    if (connected == m_connected) {
        return;
    }
    qCDebug(CLASS_LC) << "Connection status changed to:" << connected;
    m_connected = connected;
    // TODO emit signal on connection status change?
}

QString WifiControl::macAddress() const
{
    return m_wifiStatus.macAddress;
}

QString WifiControl::ssid() const
{
    return m_wifiStatus.name;
}

int WifiControl::signalStrength() const
{
    return m_wifiStatus.signalStrength;
}

QString WifiControl::ipAddress() const
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
    qCDebug(CLASS_LC) << Q_FUNC_INFO << stat;
    m_scanStatus = stat;
    scanStatusChanged(m_scanStatus);
}

QList<WifiNetwork>& WifiControl::scanResult()
{
    return m_scanResults;
}

QVariantList WifiControl::networkScanResult() const
{
    QVariantList list;
    for (WifiNetwork wifiNetwork : m_scanResults) {
        list.append(QVariant::fromValue(wifiNetwork));
    }
    return list;
}

int WifiControl::maxScanResults() const
{
    return m_maxScanResults;
}

void WifiControl::setMaxScanResults(int number)
{
    m_maxScanResults = number;
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



