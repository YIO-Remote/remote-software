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

#include <QLoggingCategory>
#include <QtDebug>

#include "wifi_mock.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "WifiMock");

WifiMock::WifiMock(QObject *parent) : WifiControl(parent)
  , m_countryCode("CH")
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

WifiMock::~WifiMock()
{
}

bool WifiMock::init()
{
    qCDebug(CLASS_LC) << "init";
    startSignalStrengthScanning();
    startWifiStatusScanning();
    startNetworkScan();
    return true;
}

void WifiMock::on()
{
    qCDebug(CLASS_LC) << "on";
}

void WifiMock::off()
{
    qCDebug(CLASS_LC) << "off";
}

bool WifiMock::reset()
{
    qCDebug(CLASS_LC) << "reset";
    // just make sure all timers are running
    return init();
}

bool WifiMock::clearConfiguredNetworks()
{
    qCDebug(CLASS_LC) << "clearConfiguredNetworks";
    return true;
}

bool WifiMock::join(const QString &ssid, const QString &password, WifiSecurity security)
{
    qCDebug(CLASS_LC) << "join " << ssid;

    if (!validateAuthentication(security, password)) {
        return false;
    }

    return true;
}

bool WifiMock::isConnected()
{
    qCDebug(CLASS_LC) << "isConnected";
    return rand() % 2 == 0;
}

QString WifiMock::macAddress() const
{
    qCDebug(CLASS_LC) << "macAddress";
    return "de:ad:be:ef:00:00";
}

QString WifiMock::ssid() const
{
    qCDebug(CLASS_LC) << "ssid";
    return "WiFi Mock";
}

int WifiMock::signalStrength() const
{
    qCDebug(CLASS_LC) << "signalStrength";

    return -70 + qrand() % 9;
}

QString WifiMock::ipAddress() const
{
    qCDebug(CLASS_LC) << "ipAddress";
    return "127.0.0.1";
}

void WifiMock::startNetworkScan()
{
    qCDebug(CLASS_LC) << "startNetworkScan";
    setScanStatus(Scanning);

    m_scanResults.clear();
    m_scanResults.append({ "0", "Mock Net", "34:31:c4:e1:d3:97", -77, WifiSecurity::WPA_PSK });
    m_scanResults.append({ "1", "Guest Network", "33:31:44:55:66:77", -66, WifiSecurity::NONE_OPEN });

    setScanStatus(ScanOk);

    emit networksFound(m_scanResults);
}

bool WifiMock::startAccessPoint()
{
    qCDebug(CLASS_LC) << "startAccessPoint";
    return false;
}

QString WifiMock::countryCode() {
    return m_countryCode;
}

void WifiMock::setCountryCode(QString &countryCode) {
    m_countryCode = countryCode;
}

void WifiMock::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    if (m_wifiStatusScanning) {
        emit ipAddressChanged("foo");
        emit macAddressChanged("bar");
        emit networkNameChanged("dummy");
    }

    if (m_signalStrengthScanning) {
        emit signalStrengthChanged(-42);
    }
}
