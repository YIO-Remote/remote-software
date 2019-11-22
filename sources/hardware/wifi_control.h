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

#ifndef WIFICONTROL_H
#define WIFICONTROL_H

#include <QDebug>
#include <QObject>
#include <QString>
#include <QList>

#include "wifi_status.h"

class WifiNetwork;

class WifiControl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ssid READ ssid NOTIFY networkNameChanged)
    Q_PROPERTY(QString macAddress READ macAddress NOTIFY macAddressChanged)
    Q_PROPERTY(QString ipAddress READ ipAddress NOTIFY ipAddressChanged)
    Q_PROPERTY(int signalStrength READ signalStrength NOTIFY signalStrengthChanged)
    Q_PROPERTY(ScanStatus scanStatus READ scanStatus NOTIFY scanStatusChanged)

public:
    static WifiControl& instance();
    virtual ~WifiControl();

    /**
     * Component state during scanning
     */
    enum ScanStatus {
        Idle,       //!< Never started a scan
        Scanning,   //!< currently scanning
        ScanFailed, //!< scan failed
        ScanOk      //!< Scan o.k. probably with results
    };
    Q_ENUM(ScanStatus)

    virtual bool init() = 0;

    virtual void on() = 0;
    virtual void off() = 0;

    Q_INVOKABLE virtual void reset() = 0;
    Q_INVOKABLE virtual void join(const QString &ssid, const QString &password) = 0;
    Q_INVOKABLE virtual bool isConnected() = 0;
    Q_INVOKABLE virtual void startNetworkScan() = 0;

    virtual QString macAddress();
    virtual QString ssid();
    virtual int signalStrength();
    virtual QString ipAddress();
    virtual ScanStatus scanStatus() const;

    /**
     * Returns the last network scan result
     */
    Q_INVOKABLE const QList<WifiNetwork>& networkScanResult();

signals:

    void networkNameChanged(QString ssid);
    void macAddressChanged(QString macAddress);
    void ipAddressChanged(QString ipAddress);

    /**
     * @brief Notifies that that the signal strength of the connected network changed
     * @param rssi received signal strength indicator
     */
    void signalStrengthChanged(int rssi);

    /**
     * Network scan was successful and result is available
     */
    void networksFound(const QList<WifiNetwork>& network);

    /**
     * Network scan status changed
     */
    void scanStatusChanged(WifiControl::ScanStatus status);

public slots:

    void startSignalStrengthScanning();
    void stopSignalStrengthScanning();
    void startWifiStatusScanning();
    void stopWifiStatusScanning();

protected:
    WifiControl(QObject* parent = nullptr);

    /**
     * Sets scan status and emits scanStatusChanged signal
     */
    void setScanStatus(ScanStatus stat);

protected:

    /**
     * @brief Current wifi connection status
     */
    WifiStatus m_wifiStatus;

    /**
     * Current scan status
     */
    ScanStatus m_scanStatus;

    /**
     * Holds last scan result
     */
    QList<WifiNetwork> m_scanResults;

    bool m_signalStrengthScanning;
    bool m_wifiStatusScanning;

private:
    int  m_scanInterval;
    int  m_timerId;

    void startScanTimer();
    void stopScanTimer();

};

// Plain Old Data class (POD). Requires C++14 or newer!
class WifiNetwork {
public:
    QString name;
    QString bssid;
    int signalStrength = -100;
    bool wpsAvailable = false;
    bool connected = false;
};

#endif // WIFICONTROL_H
