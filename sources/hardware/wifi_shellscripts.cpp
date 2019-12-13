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
#include <QtDebug>
#include <QVector>

#include "../fileio.h"
#include "wifi_shellscripts.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "WifiShellScripts");


WifiShellScripts::WifiShellScripts(QObject *parent) :
    WifiControl(parent),
    m_scriptProcess(new QProcess(this))
{

}

bool WifiShellScripts::init()
{
    startSignalStrengthScanning();
    startWifiStatusScanning();
    startNetworkScan();
    return true;
}

void WifiShellScripts::on()
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    // TODO make configurable
    launch(m_scriptProcess, "systemctl start wpa_supplicant@wlan0.service");
    // TODO emit signal
    startScanTimer();
    setConnected(true);
}

void WifiShellScripts::off()
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;

    stopScanTimer();
    // TODO make configurable
    launch(m_scriptProcess, "systemctl stop wpa_supplicant@wlan0.service");
    setConnected(false);
}


bool WifiShellScripts::reset()
{
    // just make sure all timers are running
    return init();
}

bool WifiShellScripts::clearConfiguredNetworks()
{
    // TODO this also starts the AP! Only reset wpa_supplicant
    launch(m_scriptProcess, "/usr/bin/yio-remote/reset-wifi.sh");
    return true;
}

bool WifiShellScripts::join(const QString &ssid, WifiNetwork::Security authentication, const QString &password)
{
    if (!validateAuthentication(authentication, password)) {
        return false;
    }

    QStringList args;
    args.append(ssid);
    args.append(password);
    launch(m_scriptProcess, "/usr/bin/yio-remote/wifi_network_create.sh", args);
    return true;
}

bool WifiShellScripts::isConnected()
{
    FileIO fileIO;
    m_connected = ssid() == fileIO.read("/ssid").trimmed();
    return m_connected;
}

void WifiShellScripts::startNetworkScan()
{
    setScanStatus(Scanning);
    QString scanResult = launch(m_scriptProcess, "/usr/bin/yio-remote/wifi_network_list.sh");
    m_scanResults = parseScanresult(scanResult);

    setScanStatus(ScanOk);
    emit networksFound(m_scanResults);
}

bool WifiShellScripts::startAccessPoint()
{
    qCDebug(CLASS_LC) << "Resettin WiFi and starting access point...";

    launch(m_scriptProcess, "/usr/bin/yio-remote/reset-wifi.sh");
    return true;
}

QString WifiMock::countryCode() {
    return "";
}

void WifiMock::setCountryCode(QString &countryCode) {
    qCWarning("setCountryCode not implemented!")
}

WifiNetwork WifiShellScripts::lineToNetwork(int index, const QStringRef& line) {
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
    QVector<QStringRef> list = line.split(",");

    if (list.size() > 1) {
        auto name = list.at(1).toString();
        auto signal = list.at(0).toInt();
        qCDebug(CLASS_LC) << "network:" << name << signal;
        WifiNetwork nw { QVariant(index).toString(), name, "", signal };
        return nw;
    } else {
        throw std::runtime_error("parse error");
    }
}

QList<WifiNetwork> WifiShellScripts::parseScanresult(const QString& buffer) {
    auto lines = buffer.splitRef("\n");
    QList<WifiNetwork> cont;
    for (int i = 0; i < lines.length() - 1; i++) {
        try {
            cont.append(lineToNetwork(i, lines[i]));
        } catch (std::exception& e) {
            setScanStatus(ScanFailed);
            qCCritical(CLASS_LC) << e.what() << lines[i];
        }
    }
    return cont;
}

void WifiShellScripts::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    if (m_wifiStatusScanning) {
        QString ssid = launch(m_scriptProcess, "/usr/bin/yio-remote/wifi_ssid.sh");
        if (ssid != m_wifiStatus.name) {
            m_wifiStatus.name = ssid;
            emit networkNameChanged("dummy");
        }

        QString ipAddress = launch(m_scriptProcess, "/usr/bin/yio-remote/wifi_ip.sh");
        if (ipAddress != m_wifiStatus.ipAddress) {
            m_wifiStatus.ipAddress = ipAddress;
            emit ipAddressChanged(ipAddress);
        }

        QString macAddress = launch(m_scriptProcess, "cat /sys/class/net/wlan0/address");
        if (macAddress != m_wifiStatus.macAddress) {
            m_wifiStatus.macAddress = macAddress;
            emit macAddressChanged(macAddress);
        }
    }

    if (m_signalStrengthScanning) {
        int value = launch(m_scriptProcess, "/usr/bin/yio-remote/wifi_rssi.sh").toInt();
        if (value != m_wifiStatus.signalStrength) {
            m_wifiStatus.signalStrength = value;
            emit signalStrengthChanged(value);
        }
    }
}
