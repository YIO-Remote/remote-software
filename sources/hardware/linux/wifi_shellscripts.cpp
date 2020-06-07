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

#include <QLoggingCategory>
#include <QVector>
#include <QtDebug>

#include "../../fileio.h"
#include "../hw_config.h"
#include "wifi_shellscripts.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "WifiShellScripts");

WifiShellScripts::WifiShellScripts(SystemService *systemService, QObject *parent)
    : WifiControl(parent),
      p_systemService(systemService),
      m_scriptTimeout(HW_DEF_WIFI_SH_TIMEOUT),
      m_useSudo(HW_DEF_WIFI_SH_SUDO),
      m_scriptClearNetworks(HW_DEF_WIFI_SH_CLEAR_NET),
      m_scriptConnectWifi(HW_DEF_WIFI_SH_CONNECT),
      m_scriptListNetworks(HW_DEF_WIFI_SH_LIST),
      m_scriptStartAP(HW_DEF_WIFI_SH_START_AP),
      m_scriptGetSsid(HW_DEF_WIFI_SH_GET_SSID),
      m_scriptGetIp(HW_DEF_WIFI_SH_GET_IP),
      m_scriptGetMac(HW_DEF_WIFI_SH_GET_MAC),
      m_scriptGetRssi(HW_DEF_WIFI_SH_GET_RSSI) {}

bool WifiShellScripts::init() {
    startSignalStrengthScanning();
    startWifiStatusScanning();
    startNetworkScan();
    return true;
}

void WifiShellScripts::on() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    p_systemService->startService(SystemServiceName::WIFI);
    startScanTimer();
    setConnected(true);
}

void WifiShellScripts::off() {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    stopScanTimer();
    p_systemService->stopService(SystemServiceName::WIFI);
    setConnected(false);
}

bool WifiShellScripts::reset() {
    // just make sure all timers are running
    return init();
}

bool WifiShellScripts::clearConfiguredNetworks() {
    launch(m_scriptClearNetworks);
    return true;
}

bool WifiShellScripts::join(const QString &ssid, const QString &password, WifiSecurity::Enum security) {
    if (!validateAuthentication(security, password)) {
        return false;
    }

    QStringList args;
    args.append(ssid);
    args.append(password);
    launch(m_scriptConnectWifi, args);
    return true;
}

bool WifiShellScripts::isConnected() {
    FileIO fileIO;
    return wifiStatus().name() == fileIO.read("/ssid").trimmed();
}

void WifiShellScripts::startNetworkScan() {
    setScanStatus(Scanning);
    QString scanResult = launch(m_scriptListNetworks);
    m_scanResults = parseScanresult(scanResult);

    setScanStatus(ScanOk);
    emit networksFound(m_scanResults);
}

bool WifiShellScripts::startAccessPoint() {
    qCDebug(CLASS_LC) << "Resetting WiFi and starting access point...";

    launch(m_scriptStartAP);
    return true;
}

QString WifiShellScripts::countryCode() { return ""; }

void WifiShellScripts::setCountryCode(const QString &countryCode) {
    qCWarning(CLASS_LC) << "setCountryCode not implemented! Requested:" << countryCode;
}

WifiNetwork WifiShellScripts::lineToNetwork(int index, const QStringRef &line) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QVector<QStringRef> list = line.split(",");

    if (list.size() > 1) {
        auto name = list.at(1).toString();
        auto signal = list.at(0).toInt();
        qCDebug(CLASS_LC) << "network:" << name << signal;
        WifiNetwork nw{QVariant(index).toString(), name, "", signal};
        return nw;
    } else {
        throw std::runtime_error("parse error");
    }
}

QList<WifiNetwork> WifiShellScripts::parseScanresult(const QString &buffer) {
    auto               lines = buffer.splitRef("\n");
    QList<WifiNetwork> cont;
    for (int i = 0; i < lines.length() - 1; i++) {
        try {
            cont.append(lineToNetwork(i, lines[i]));
        } catch (std::exception &e) {
            setScanStatus(ScanFailed);
            qCCritical(CLASS_LC) << e.what() << lines[i];
        }
    }
    return cont;
}

void WifiShellScripts::timerEvent(QTimerEvent *event) {
    Q_UNUSED(event)

    if (!(m_wifiStatusScanning || m_signalStrengthScanning)) {
        return;
    }

    int rssi = launch(m_scriptGetRssi).toInt();

    if (m_wifiStatusScanning) {
        QString ssid = launch(m_scriptGetSsid);
        QString ipAddress = launch(m_scriptGetIp);
        QString macAddress = launch(m_scriptGetMac);

        m_wifiStatus = WifiStatus(ssid, "", ipAddress, macAddress, rssi);
        emit wifiStatusChanged(m_wifiStatus);
    }

    if (m_signalStrengthScanning) {
        if (rssi != m_wifiStatus.rssi()) {
            m_wifiStatus.setRssi(rssi);
            emit signalStrengthChanged(rssi);
        }
    }
}

QString WifiShellScripts::launch(const QString &command) {
    QStringList arguments;
    return launch(command, arguments);
}

QString WifiShellScripts::launch(const QString &command, const QStringList &arguments) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO << command;

    if (command.isNull() || command.isEmpty()) {
        return "";
    }

    QProcess process;
    process.start(command, arguments);
    if (!process.waitForFinished(m_scriptTimeout)) {
        qCWarning(CLASS_LC) << "Failed to execute" << command << ":"
                            << "stdout:" << QString::fromLocal8Bit(process.readAllStandardOutput())
                            << "errout:" << QString::fromLocal8Bit(process.readAllStandardError());
        return "";
    }
    QByteArray bytes = process.readAllStandardOutput();
    QString    output = QString::fromLocal8Bit(bytes);
    return output;
}
