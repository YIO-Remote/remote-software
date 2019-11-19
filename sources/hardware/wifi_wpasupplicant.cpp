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

#include "wifi_wpasupplicant.h"

WiFiWpaSupplicant::WiFiWpaSupplicant(QObject *parent) : WifiControl(parent)
{

}

void WiFiWpaSupplicant::init()
{

}

void WiFiWpaSupplicant::on()
{

}

void WiFiWpaSupplicant::off()
{

}

void WiFiWpaSupplicant::reset()
{

}

void WiFiWpaSupplicant::join(const QString &ssid, const QString &password)
{

}

bool WiFiWpaSupplicant::isConnected()
{
    return false;
}

QString WiFiWpaSupplicant::getMacAddress()
{
    return "de:ad:be:ef:00:00";
}

QString WiFiWpaSupplicant::getCurrentSSID()
{
    return "Not Implemented";
}

int WiFiWpaSupplicant::getCurrentSignalStrength()
{
    return 0;
}

QString WiFiWpaSupplicant::getCurrentIp()
{
    return "127.0.0.1";
}

void WiFiWpaSupplicant::startNetworkScan()
{

}
