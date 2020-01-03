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

#include "wifi_security.h"
#include "wifi_signal.h"

// unfortunatley we can't use the minimalistic C++ 14 POD classes for QML interaction :-(

/**
 * @brief Immutable value class for WiFi network information.
 */
class WifiNetwork {
    Q_GADGET
    Q_PROPERTY(QString id READ id CONSTANT)
    Q_PROPERTY(QString name READ name CONSTANT)
    Q_PROPERTY(QString bssid READ bssid CONSTANT)
    Q_PROPERTY(int rssi READ rssi CONSTANT)
    Q_PROPERTY(SignalStrength signalStrength READ signalStrength CONSTANT)
    Q_PROPERTY(bool encrypted READ isEncrypted CONSTANT)
    Q_PROPERTY(WifiSecurity security READ security CONSTANT)
    Q_PROPERTY(bool wpsAvailable READ isWpsAvailable CONSTANT)
    Q_PROPERTY(bool connected READ isConnected CONSTANT)

 public:
    WifiNetwork() {}
    WifiNetwork(QString id, QString name, QString bssid, int rssi, WifiSecurity security = WifiSecurity::WPA_PSK,
                bool wpsAvailable = false, bool connected = false)
        : m_id(id),
          m_name(name),
          m_bssid(bssid),
          m_rssi(rssi),
          m_security(security),
          m_wpsAvailable(wpsAvailable),
          m_connected(connected) {}

    /**
     * @brief id Network identification
     */
    QString id() const { return m_id; }

    /**
     * @brief name Service set ID (SSID) of the network.
     */
    QString name() const { return m_name; }

    /**
     * @brief bssid Network basic service set identifier (BSSID).
     */
    QString bssid() const { return m_bssid; }

    /**
     * @brief rssi Signal strength in dBm. This might not be true for all wpa_supplicant drivers.
     */
    int rssi() const { return m_rssi; }

    /**
     * @brief isEncrypted Convenience method for the client if the network is open or needs authentication.
     */
    bool isEncrypted() const { return m_security != WifiSecurity::NONE_OPEN; }  // what about NoneWep?

    WifiSecurity security() const { return m_security; }

    bool isWpsAvailable() const { return m_wpsAvailable; }

    bool isConnected() const { return m_connected; }

    SignalStrength signalStrength() const { return SignalStrengthEnum::fromRssi(m_rssi); }

 private:
    QString      m_id;
    QString      m_name;
    QString      m_bssid;
    int          m_rssi = -100;
    WifiSecurity m_security = WifiSecurity::WPA_PSK;
    bool         m_wpsAvailable = false;
    bool         m_connected = false;
};
