/******************************************************************************
 *
 * Copyright (C) 2019 Markus Zehnder <business@markuszehnder.ch>
 *
 * Third party work used:
 *
 * DigitalRooster - QT/QML internet radio, podcast player and alarmclock.
 * Copyright (C) 2018 Thomas Ruschival <thomas@ruschival.de>
 * Licensed under GPL 3.0 or later.
 *
 * wpaCute - A graphical wpa_supplicant front end.
 * Copyright (C) 2018 loh.tar@googlemail.com
 * Licensed under BSD license.
 *
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

#ifndef WIFIWPASUPPLICANT_H
#define WIFIWPASUPPLICANT_H

#include <QObject>
#include <QSocketNotifier>
#include <QString>
#include <QStringRef>
#include <QList>
#include <QProcess>

#include <memory>
#include <mutex>
#include <thread>

#include "wifi_control.h"
#include "wpa_ctrl.h"

/**
 * Function to register as callback for the wpa_supplicant control interface
 */
void wpa_msg_cb(char* buf, size_t len);


class WifiWpaSupplicant : public WifiControl
{
    Q_OBJECT
public:
    /**
     * Destructor must close wpa_cli
     */
    virtual ~WifiWpaSupplicant() override;

    virtual bool init() override;

    Q_INVOKABLE virtual bool reset() override;
    Q_INVOKABLE virtual bool clearConfiguredNetworks() override;
    Q_INVOKABLE virtual bool join(const QString &ssid, WifiNetwork::Security security, const QString &password) override;
    Q_INVOKABLE virtual void startNetworkScan() override;
    Q_INVOKABLE virtual bool startAccessPoint() override;

    virtual QString countryCode() override;
    virtual void setCountryCode(QString &countryCode) override;

    /**
      * TESTING ONLY! Proof of concept implementation for interactive authentication.
      * Callback method for signal authenticationRequest().
      */
    Q_INVOKABLE void authenticationResponse(const QString& type, int networkId, const QString& response);

    /**
      * Trigger Push Button Configuration (PBC) authentication with given network
      * @return false if authentication request couldn't be sent
      */
    bool wpsPushButtonConfigurationAuth(const WifiNetwork& network);

signals:

    /**
     * @brief authenticationRequest TESTING ONLY! Proof of concept implementation for interactive authentication request.
     * @param type Authorization type: "PASSWORD", "NEW_PASSWORD", "IDENTITY", "PASSPHRASE", "OTP"
     * @param networkId Network Id, must be used in authenticationResponse
     * @param text Human readable authorization information
     */
    void authenticationRequest(QString type, int networkId, QString text);

public slots:

    virtual void on() override;
    virtual void off() override;

protected slots:

    /**
     * Data on control channel available
     */
    void controlEvent(int fd);

private:
    explicit WifiWpaSupplicant(QObject *parent = nullptr);

    /**
     * @brief setNetworkParam Helper method to set a network parameter with SET_NETWORK
     * @param networkId Network identification
     * @param parm Parameter name
     * @param val Value to set for parameter
     * @param quote true = quote value
     * @return false if parameter could not be set
     */
    bool setNetworkParam(const QString& networkId, const QString& parm, const QString& val, bool quote = false);

    bool writeWepKey(const QString& networkId, const QString& value, int keyId);

    /**
     * Connect to wpa control interface socket
     * @param wpaSupplicantSocketPath Path to management socket e.g. /var/lib/wpa_supplicant/wlan0
     * @return false if operation failed
     */
    bool connectWpaControlSocket(const QString &wpaSupplicantSocketPath);

    /**
     * @brief parseStatus Parse a STATUS response message
     * @param buffer Response message
     * @return Result as WifiStatus object
     */
    WifiStatus parseStatus(const char* buffer);

    /**
     * @brief parseSignalStrength Parse a SIGNAL_POLL response message
     * @param buffer Response message
     * @return The parsed rssi value
     */
    int parseSignalStrength(const char* buffer);

    /**
     * @brief processCtrlReq TESTING ONLY! Proof of concept implementation for interactive authentication request.
     * @details Emits authenticationRequest to retrieve the required authentication from the client
     * @param req the authentication request message to process
     */
    void processCtrlReq(const QString& req);

    /**
     * @brief readScanResults Send read scan_results from wpa_ctrl
     */
    void readScanResults();

    /**
     * @brief addBSS Get detailed per-BSS scan results.
     * @details BSS command can be used to iterate through scan results one BSS at a time and to fetch
     *          all information from the found BSSes. This provides access to the same data that is
     *          available through SCAN_RESULTS but in a way that avoids problems with large number of
     *          scan results not fitting in the ctrl_iface messages.
     * @param networkId the network index, 0 based
     * @return false if no information could be retrieved
     */
    bool addBSS(int networkId);

    /**
     * @brief getSecurityFromFlags Parse security flags
     * @param flags security flags
     * @param networkId optional network identification to retrieve more information if required
     * @return Security enumeration
     */
    WifiNetwork::Security getSecurityFromFlags(const QString& flags, int networkId = -1);

    QList<WifiNetwork>& getConfiguredNetworks();

    /**
     * @brief controlRequest Issue a command to wpa_supplicant without returning the response message
     * @param cmd wpa_supplicant command
     * @return true on success
     */
    bool controlRequest(const QString& cmd);
    /**
     * @brief controlRequest Issue a command to wpa_supplicant and store the response in buf
     * @param cmd wpa_supplicant command
     * @param buf Response buffer
     * @param buflen Response buffer length
     * @return true on success
     */
    bool controlRequest(const QString& cmd, char* buf, const size_t buflen);

    /**
     * @brief parseEvent Interpret event string from wpa_socket monitor
     * @param msg Event string
     */
    void parseEvent(char* msg);

    /**
     * @brief checkConnection Issue a STATUS command to check the WiFi connection
     * @return true if the WiFi connection is established
     */
    bool checkConnection();

    virtual void timerEvent(QTimerEvent *event) override;

    /**
     * Handle for lower layer wpa_ctrl
     */
    struct wpa_ctrl* m_ctrl;

    /**
     * Mutex to protect concurrent access to wpa_ctrl and buffers m_reply or m_replySize
     */
    mutable std::mutex m_wpaMutex;

    /**
     * Notifier for watching asynchronous events from wpa_ctrl socket
     */
    std::unique_ptr<QSocketNotifier> m_ctrlNotifier;

    QString m_wifiOnScript;
    QString m_wifiOffScript;
    QProcess *m_scriptProcess;

    // Only allow WifiControl to create an instance
    friend WifiControl& WifiControl::instance();
};

#endif // WIFIWPASUPPLICANT_H