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
#include <QVariant>
#include <QList>
#include <QVariantList>
#include <QProcess>

#include "wifi_status.h"
#include "wifi_network.h"


class WifiControl : public QObject
{
    Q_OBJECT
    // TODO do we need individual properties or would WifiStatus class be better?
    Q_PROPERTY(QString ssid                   READ ssid              NOTIFY networkNameChanged)
    Q_PROPERTY(QString macAddress             READ macAddress        NOTIFY macAddressChanged)
    Q_PROPERTY(QString ipAddress              READ ipAddress         NOTIFY ipAddressChanged)
    Q_PROPERTY(int signalStrength             READ signalStrength    NOTIFY signalStrengthChanged)
    Q_PROPERTY(ScanStatus scanStatus          READ scanStatus        NOTIFY scanStatusChanged)
    Q_PROPERTY(QVariantList networkScanResult READ networkScanResult NOTIFY networksFound)

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

    /**
     * Resets WiFi connection settings.
     * Active connection is disconnected and removed.
     * After disconnection an access point is started for the WiFi connection wizard.
     */
    Q_INVOKABLE virtual void reset() = 0;

    /**
     * Join the WiFi network with the given ssid
     */
    Q_INVOKABLE virtual void join(const QString &ssid, const QString &password) = 0;

    /**
     * Checks if the WiFi connection is established
     */
    Q_INVOKABLE virtual bool isConnected();

    /**
     * Starts a new scan for available WiFi networks.
     * Emits scanStatusChanged and finally networksFound if successful.
     */
    Q_INVOKABLE virtual void startNetworkScan() = 0;

    virtual QString macAddress() const;
    virtual QString ssid() const;
    virtual int signalStrength() const;
    virtual QString ipAddress() const;
    virtual ScanStatus scanStatus() const;
    QList<WifiNetwork>& scanResult();

signals:
    // TODO do we need individual signals or simply use wifiStatusChanged?
    void networkNameChanged(QString ssid);
    void macAddressChanged(QString macAddress);
    void ipAddressChanged(QString ipAddress);

    /**
     * @brief Notifies that that the signal strength of the connected network changed
     * @param rssi received signal strength indicator
     */
    void signalStrengthChanged(int rssi);

    /**
     * @brief Network scan status changed
     * @param status the new scan status
     */
    void scanStatusChanged(WifiControl::ScanStatus status);

    /**
     * @brief Network scan was successful and result is available
     * @param network List of found WiFi networks
     */
    void networksFound(const QList<WifiNetwork>& network); // TODO shouldn't we return a QVariantList for QML?

public slots:
    /**
     * Enable WiFi device
     */
    virtual void on();

    /**
     * Disable WiFi device
     */
    virtual void off();

    /**
     * @brief Starts observing the WiFi signal strength of the connected network. Emits signalStrengthChanged for every change.
     */
    void startSignalStrengthScanning();
    void stopSignalStrengthScanning();

    /**
     * @brief Starts observing the WiFi status if the IP address, MAC address or SSID changes. Emits networkNameChanged, macAddressChanged, ipAddressChanged
     */
    void startWifiStatusScanning();
    void stopWifiStatusScanning();

protected:
    WifiControl(QObject* parent = nullptr);

    /**
     * Sets scan status and emits scanStatusChanged signal
     */
    void setScanStatus(ScanStatus stat);

    void startScanTimer();
    void stopScanTimer();

    QString launch(QProcess *process, const QString &command);
    QString launch(QProcess *process, const QString &command, const QStringList &arguments);

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

    bool m_connected;
    bool m_signalStrengthScanning;
    bool m_wifiStatusScanning;

private:
    /**
     * @brief Returns a QML compatible representation of the WifiNetwork list
     * @return a QVariantList copy of the WifiNetwork list
     */
    QVariantList networkScanResult() const;

    QProcess *m_process;

    int  m_scanInterval;
    int  m_timerId;

};

#endif // WIFICONTROL_H
