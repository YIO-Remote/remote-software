/******************************************************************************
 *
 * Copyright (C) 2019 Markus Zehnder <business@markuszehnder.ch>
 * Copyright (C) 2018 Marton Borzak <hello@yio-remote.com>
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
#include <QProcess>

#include "../systemservice.h"
#include "../wifi_control.h"

/**
 * @brief Deprecated WifiControl implementation using the legacy shell scripts
 */
class WifiShellScripts : public WifiControl {
    Q_OBJECT

 public:
    explicit WifiShellScripts(SystemService *systemService, QObject *parent = nullptr);

    bool init() override;

    Q_INVOKABLE bool reset() override;
    Q_INVOKABLE bool clearConfiguredNetworks() override;
    Q_INVOKABLE bool join(const QString &ssid, const QString &password,
                                  WifiSecurity security = WifiSecurity::DEFAULT) override;
    Q_INVOKABLE bool isConnected() override;
    Q_INVOKABLE void startNetworkScan() override;
    Q_INVOKABLE  bool startAccessPoint() override;

    QString countryCode() override;
    void    setCountryCode(const QString &countryCode) override;

    void setUseSudo(bool useSudo) { m_useSudo = useSudo; }

    void setScriptGetRssi(const QString &script) { m_scriptGetRssi = script; }

    void setScriptGetMacAddress(const QString &script) { m_scriptGetMac = script; }

    void setScriptGetIp(const QString &script) { m_scriptGetIp = script; }

    void setScriptGetSsid(const QString &script) { m_scriptGetSsid = script; }

    void setScriptStartAP(const QString &script) { m_scriptStartAP = script; }

    void setScriptListNetworks(const QString &script) { m_scriptListNetworks = script; }

    void setScriptConnectWifi(const QString &script) { m_scriptConnectWifi = script; }

    void setScriptClearNetworks(const QString &script) { m_scriptClearNetworks = script; }

    void setScriptTimeout(int scriptTimeoutMs) { m_scriptTimeout = scriptTimeoutMs; }

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99

    void on() override;
    void off() override;

 private:
    void timerEvent(QTimerEvent *event) override;

    /**
     * Tokenize a single line of scan results and interpret fields as WifiNetwork
     * @param index line number
     * @param line reference to string in scan results
     * @return WifiNetwork data object
     */
    WifiNetwork lineToNetwork(int index, const QStringRef &line);

    /**
     * Tokenize a buffer returned by the shell script into a list of WifiNetworks
     * @param buffer result
     */
    QList<WifiNetwork> parseScanresult(const QString &buffer);

    QString launch(const QString &command);
    QString launch(const QString &command, const QStringList &arguments);

    SystemService *p_systemService;

    // configuration parameters
    int     m_scriptTimeout;
    bool    m_useSudo;
    QString m_scriptClearNetworks;
    QString m_scriptConnectWifi;
    QString m_scriptListNetworks;
    QString m_scriptStartAP;
    QString m_scriptGetSsid;
    QString m_scriptGetIp;
    QString m_scriptGetMac;
    QString m_scriptGetRssi;
};
