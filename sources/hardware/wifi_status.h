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

#pragma once

#include <QObject>
#include <QString>

#include "wifi_signal.h"

// unfortunatley we can't use the minimalistic C++ 14 POD classes for QML interaction :-(

/**
 * @brief Current WiFi client status information.
 */
class WifiStatus {
    Q_GADGET
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString bssid READ bssid CONSTANT)
    Q_PROPERTY(QString ipAddress READ ipAddress CONSTANT)
    Q_PROPERTY(QString macAddress READ macAddress CONSTANT)
    Q_PROPERTY(int rssi READ rssi CONSTANT)
    Q_PROPERTY(SignalStrength signalStrength READ signalStrength CONSTANT)
    Q_PROPERTY(bool connected READ isConnected CONSTANT)

 public:
    WifiStatus(QString name = "", QString bssid = "", QString ipAddress = "", QString macAddress = "", int rssi = -100,
               bool connected = false)
        : m_name(name),
          m_bssid(bssid),
          m_ipAddress(ipAddress),
          m_macAddress(macAddress),
          m_rssi(rssi),
          m_connected(connected) {}

    /**
     * @brief name Service set ID (SSID) of the network.
     */
    QString name() const { return m_name; }

    /**
     * @brief bssid Network basic service set identifier (BSSID).
     */
    QString bssid() const { return m_bssid; }

    /**
     * @brief ipAddress Client IP address
     */
    QString ipAddress() const { return m_ipAddress; }

    /**
     * @brief macAddress Client MAC address
     */
    QString macAddress() const { return m_macAddress; }

    /**
     * @brief rssi Signal strength in dBm. This might not be true for all wpa_supplicant drivers.
     */
    int  rssi() const { return m_rssi; }
    void setRssi(int rssi) { m_rssi = rssi; }

    /**
     * @brief signalStrength Signal strength classification based on rssi.
     */
    SignalStrength signalStrength() const { return SignalStrengthEnum::fromRssi(m_rssi); }

    /**
     * @brief isConnected Returns the access point connection status.
     */
    bool isConnected() const { return m_connected; }

 private:
    QString m_name;
    QString m_bssid;
    QString m_ipAddress;
    QString m_macAddress;
    int     m_rssi;
    bool    m_connected;
};
