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
{
    qCDebug(CLASS_LC) << Q_FUNC_INFO;
}

void WifiMock::init()
{
    qCDebug(CLASS_LC) << "init";
}

void WifiMock::on()
{
    qCDebug(CLASS_LC) << "on";
}

void WifiMock::off()
{
    qCDebug(CLASS_LC) << "off";
}

void WifiMock::reset()
{
    qCDebug(CLASS_LC) << "reset";
}

void WifiMock::join(const QString &ssid, const QString &password)
{
    qCDebug(CLASS_LC) << "join " << ssid;
}

bool WifiMock::isConnected()
{
    qCDebug(CLASS_LC) << "isConnected";
    return rand() % 2 == 0;
}

QString WifiMock::getMacAddress()
{
    qCDebug(CLASS_LC) << "getMacAddress";
    return "de:ad:be:ef:00:00";
}

QString WifiMock::getCurrentSSID()
{
    qCDebug(CLASS_LC) << "getCurrentSSID";
    return "WiFi Mock";
}

int WifiMock::getCurrentSignalStrength()
{
    qCDebug(CLASS_LC) << "getCurrentSignalStrength";

    return -70 + qrand() % 9;
}

QString WifiMock::getCurrentIp()
{
    qCDebug(CLASS_LC) << "getCurrentIp";
    return "127.0.0.1";
}

void WifiMock::startNetworkScan()
{
    qCDebug(CLASS_LC) << "startNetworkScan";
    setScanStatus(Scanning);

    m_scanResults.clear();

    WifiNetwork nw { "Mock Net", "34:31:c4:e1:d3:97" }; //, -77, false, false };
    m_scanResults.append(nw);

    setScanStatus(ScanOk);

    emit networksFound(m_scanResults);
}
