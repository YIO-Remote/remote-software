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

#ifndef WIFINETWORK_H
#define WIFINETWORK_H

#include <QObject>
#include <QString>

// unfortunatley we can't use the minimalistic C++ 14 POD classes for QML interaction :-(

/**
 * @brief Immutable value class for WiFi network information
 */
class WifiNetwork {
    Q_GADGET
    Q_PROPERTY (QString  name           READ name           CONSTANT)
    Q_PROPERTY (QString  bssid          READ bssid          CONSTANT)
    Q_PROPERTY (int      signalStrength READ signalStrength CONSTANT)
    Q_PROPERTY (bool     wpsAvailable   READ isWpsAvailable CONSTANT)
    Q_PROPERTY (bool     connected      READ isConnected    CONSTANT)

public:
    WifiNetwork() {}
    WifiNetwork(QString name, QString bssid, int signalStrength, bool wpsAvailable = false, bool connected = false)
        : m_name(name)
        , m_bssid(bssid)
        , m_signalStrength(signalStrength)
        , m_wpsAvailable(wpsAvailable)
        , m_connected(connected) {}

    QString name () const { return m_name; }
    QString bssid () const { return m_bssid; }
    int     signalStrength () const { return m_signalStrength; }
    bool    isWpsAvailable() const { return m_wpsAvailable; }
    bool    isConnected() const { return m_connected; }

private:
    QString m_name;
    QString m_bssid;
    int m_signalStrength = -100;
    bool m_wpsAvailable = false;
    bool m_connected = false;
};

//Q_DECLARE_METATYPE(WifiNetwork) // doesn't seem required?


#endif // WIFINETWORK_H
