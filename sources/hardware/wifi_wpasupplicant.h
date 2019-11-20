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

#ifndef WIFIWPASUPPLICANT_H
#define WIFIWPASUPPLICANT_H

#include <QObject>
#include <QSocketNotifier>
#include <QString>
#include <QStringRef>
#include <QList>

#include <memory>
#include <mutex>
#include <thread>

#include "wifi_control.h"
#include "wpa_ctrl.h"

/**
 * Function to register as callback
 * @param buf
 * @param len
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

    bool init() override;

    void on() override;
    void off() override;

    Q_INVOKABLE void reset() override;
    Q_INVOKABLE void join(const QString &ssid, const QString &password) override;
    Q_INVOKABLE bool isConnected() override;
    Q_INVOKABLE QString getMacAddress() override;
    Q_INVOKABLE QString getCurrentSSID() override;
    Q_INVOKABLE int getCurrentSignalStrength() override;
    Q_INVOKABLE QString getCurrentIp() override;
    Q_INVOKABLE void startNetworkScan() override;

    /**
      * Trigger Push Button Configuration (PBC) authentication
      * with given network
      * @param network
      */
    void wps_pbc_auth(const WifiNetwork& network);

    /**
     * Connect to wpa control interface socket
     */
    void connectWpaControlSocket();

signals:

public slots:
    /**
     * data on control channel available
     */
    void controlEvent(int fd);

private:
    explicit WifiWpaSupplicant(QObject *parent = nullptr);

    /**
     * Tokenize a single line of scan results and interpret fields as WifiNetwork
     * @param line refernce to string in scan_results
     * @return WifiNetwork data object
     */
    WifiNetwork lineToNetwork(const QStringRef& line);

    /**
     * Tokenize a buffer returend by wpa_ctrl SCAN_RESULT into a vector of
     * WifiNetworks
     * @param buffer result
     * @param len buffer length
     */
    QList<WifiNetwork> parseScanresult(const char* buffer, size_t len);

    /**
     * Send read scan_results from wpa_ctrl
     */
    void readScanResults();

    /**
     * Issue a command, modifies reply and reply_size
     * @param cmd wpa_supplicant command
     */
    void requestWrapper(const QString& cmd);

    /**
     * Interpret event string from wpa_socket monitor
     * @param e_string event string
     */
    void parseEvent(const QString& e_string);

private:
    /**
     * Handle for lower layer wpa_ctrl
     */
    struct wpa_ctrl* ctrl;

    /**
     * Buffer to hold reply of command
     */
    char reply[2048] = {};
    size_t reply_size;

    /**
     * Mutex to protect concurrent access to wpa_ctrl and buffers
     * reply or reply_size
     */
    mutable std::mutex wpa_mtx;

    /**
     * Path to management socket e.g. /var/lib/wpa_supplicant/wlan0
     */
    QString wpa_supplicant_sock_path;

    /**
     * Notifier for watching asynchornous events from wpa_ctrl socket
     */
    std::unique_ptr<QSocketNotifier> ctrl_notifier;

    friend WifiControl& WifiControl::instance();
};

#endif // WIFIWPASUPPLICANT_H
