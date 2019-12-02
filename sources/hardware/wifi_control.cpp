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

static Q_LOGGING_CATEGORY(CLASS_LC, "WifiCtrl");

/*!
    \class WifiControl

    \brief Abstract WiFi control interface

    \details wpa_supplicant integration code is based on https://github.com/truschival/DigitalRoosterGui
*/
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

bool WifiControl::validateAuthentication(WifiNetwork::Security security, const QString &preSharedKey)
{
    switch (security) {
    case WifiNetwork::Security::IEEE8021X :
    case WifiNetwork::Security::WPA_EAP :
    case WifiNetwork::Security::WPA2_EAP :
        qWarning(CLASS_LC) << "Authentication mode not supported:" << security;
        return false;
    default:
        break;
    }

    if (security == WifiNetwork::Security::WPA_PSK
            || security == WifiNetwork::Security::WPA2_PSK) {
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

QString WifiControl::launch(QProcess *process, const QString &command)
{
    QStringList arguments;
    return launch(process, command, arguments);
}

QString WifiControl::launch(QProcess *process, const QString &command, const QStringList &arguments)
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO << command;

    // TODO should launcher be synchronized? i.e. guarded by a mutex? --> shell script launching needs rework anyways and should soon be deprecated for most tasks.
    process->start(command, arguments);
    process->waitForFinished(-1);  // FIXME use timeout. Infinite is never a good idea.
    QByteArray bytes = process->readAllStandardOutput();
    QString output = QString::fromLocal8Bit(bytes);
    return output;
}


