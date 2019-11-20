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

#include <QObject>
#include <QString>
#include <QList>

class WifiNetwork;

class WifiControl : public QObject
{
    Q_OBJECT;
    Q_PROPERTY(
        ScanStatus scanStatus READ getScanStatus NOTIFY scanStatusChanged)

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
    Q_INVOKABLE virtual QString getMacAddress() = 0;
    Q_INVOKABLE virtual QString getCurrentSSID() = 0;
    Q_INVOKABLE virtual int getCurrentSignalStrength() = 0;
    Q_INVOKABLE virtual QString getCurrentIp() = 0;
    Q_INVOKABLE virtual void startNetworkScan() = 0;

    ScanStatus getScanStatus() const;

    /**
     * Returns the last network scan result
     */
    const QList<WifiNetwork>& getNetworkScanResult();

signals:
    //void signalStrength(float rssi);

    /**
     * Scan was successful and result is available
     */
    void networksFound(const QList<WifiNetwork>& network);

    /**
     * Status changed
     */
    void scanStatusChanged(WifiControl::ScanStatus status);

public slots:

protected:
    WifiControl(QObject* parent = nullptr);

    /**
     * Sets scan status and emits scanStatusChanged signal
     */
    void setScanStatus(ScanStatus stat);

protected:

    /**
     * Current scan status
     */
    ScanStatus m_scanStatus;

    /**
     * Holds last scan result
     */
    QList<WifiNetwork> m_scanResults;

};

// Plain Old Data class (POD)
class WifiNetwork {
public:
    QString name;
    QString bssid;
    int signal_strength = 0;
    bool wps_available = false;
    bool connected = false;
//  Attention: <brace-enclosed initializer list> only works with C++14 and newer. Otherwise we need a constructor!
//    WifiNetwork(QString name, QString bssid) : name(name), bssid(bssid) {}
};

#endif // WIFICONTROL_H
