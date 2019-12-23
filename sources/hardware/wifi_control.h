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

/**
 * @brief Abstract WiFi control interface for client interactions with Wifi networks.
 */
class WifiControl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString countryCode            READ countryCode       WRITE setCountryCode)
    Q_PROPERTY(WifiStatus wifiStatus          READ wifiStatus        NOTIFY wifiStatusChanged)
    Q_PROPERTY(ScanStatus scanStatus          READ scanStatus        NOTIFY scanStatusChanged)
    Q_PROPERTY(QVariantList networkScanResult READ networkScanResult NOTIFY networksFound)

public:
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

    /**
     * @brief init Initializes the driver.
     * @details Intended to be a one-time initialization after driver creation.
     * @return true if initialization succeeded
     */
    virtual bool init() = 0;

    /**
     * @brief reset Resets and re-initializes the driver. An active WiFi connection might be disconnected and reassociated.
     * @return true if reassociation succeeded
     */
    Q_INVOKABLE virtual bool reset() = 0;

    /**
     * @brief Resets all WiFi connection settings.
     * @details An active connection is disconnected, all known networks are removed and the empty network configuration is saved.
     */
    Q_INVOKABLE virtual bool clearConfiguredNetworks() = 0;

    /**
     * @brief Joins the WiFi network with the given ssid. The network will be added to the known networks.
     */
    Q_INVOKABLE virtual bool join(const QString &ssid, const QString &password, WifiSecurity security = WifiSecurity::DEFAULT) = 0;

    /**
     * @brief Checks if the WiFi connection is established
     */
    Q_INVOKABLE virtual bool isConnected();

    /**
     * @brief Starts a new scan for available WiFi networks.
     *        Emits scanStatusChanged and finally networksFound if successful.
     */
    Q_INVOKABLE virtual void startNetworkScan() = 0;

    /**
     * @brief Starts an access point for the WiFi connection wizard.
     */
    Q_INVOKABLE virtual bool startAccessPoint() = 0;

    /**
     * @brief countryCode Returns the configured country code in which the device is operating
     * @return ISO/IEC alpha2 country code
     */
    virtual QString countryCode() = 0;

    /**
     * @brief setCountryCode Sets the country code in which the device is operating
     * @param countryCode ISO/IEC alpha2 country code
     */
    virtual void setCountryCode(QString &countryCode) = 0;

    virtual WifiStatus wifiStatus() const;
    virtual ScanStatus scanStatus() const;
    QList<WifiNetwork>& scanResult();

    /**
     * @brief setMaxScanResults Limits the number of Wifi network scan results
     * @param number Maximum number of results
     */
    void setMaxScanResults(int number);
    int maxScanResults() const;

    /**
     * @brief setPollInterval Set polling interval for wifi status requests
     * @param pollIntervalMs interval in milliseconds
     */
    void setPollInterval(int pollIntervalMs);
    int pollInterval() const;

    int getNetworkJoinRetryCount() const;
    void setNetworkJoinRetryCount(int count);

    int getNetworkJoinRetryDelay() const;
    void setNetworkJoinRetryDelay(int msDelay);

signals:
    /**
     * @brief wifiStatusChanged Notifies that the client status was updated
     * @param wifiStatus Client WiFi status information
     */
    void wifiStatusChanged(WifiStatus wifiStatus);

    /**
     * @brief Notifies that the signal strength of the connected network changed
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
     * @brief Enable WiFi device
     */
    virtual void on() = 0;

    /**
     * @brief Disable WiFi device
     */
    virtual void off() = 0;

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
    // abstract base class
    WifiControl(QObject* parent = nullptr);

    /**
     * @brief validateAuthentication Validates the authentication mode and pre shared key
     */
    bool validateAuthentication(WifiSecurity security, const QString &preSharedKey);

    /**
     * @brief setConnected Set Wifi connection status
     * @param connected true = connection to network established
     */
    virtual void setConnected(bool connected);

    /**
     * @brief Sets scan status and emits scanStatusChanged signal
     */
    void setScanStatus(ScanStatus stat);

    void startScanTimer();
    void stopScanTimer();

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
    bool m_connected;

    /**
     * @brief Returns a QML compatible representation of the WifiNetwork list
     * @return a QVariantList copy of the WifiNetwork list
     */
    QVariantList networkScanResult() const;

    int m_maxScanResults;
    int m_pollInterval;
    int m_timerId;
    int m_networkJoinRetryCount;
    int m_networkJoinRetryDelay;

};

#endif // WIFICONTROL_H
