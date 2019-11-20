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

#include <QtDebug>

#include "../fileio.h"
#include "wifi_shellscripts.h"


WifiShellScripts::WifiShellScripts(QObject *parent) :
    WifiControl(parent),
    m_process(new QProcess(this))
{

}

bool WifiShellScripts::init()
{
    return true;
}

void WifiShellScripts::on()
{
    launch("systemctl start wpa_supplicant@wlan0.service");
}

void WifiShellScripts::off()
{
    launch("systemctl stop wpa_supplicant@wlan0.service");
}

void WifiShellScripts::join(const QString &ssid, const QString &password)
{
    QStringList args;
    args.append(ssid);
    args.append(password);
    launch("/usr/bin/yio-remote/wifi_network_create.sh", args);
}

void WifiShellScripts::reset()
{
    launch("/usr/bin/yio-remote/reset-wifi.sh");
}

bool WifiShellScripts::isConnected()
{
    FileIO fileIO;
    return getCurrentSSID() == fileIO.read("/ssid").trimmed();
}

QString WifiShellScripts::getMacAddress()
{
    return launch("cat /sys/class/net/wlan0/address");
}

QString WifiShellScripts::getCurrentSSID()
{
    return launch("/usr/bin/yio-remote/wifi_ssid.sh");
}

int WifiShellScripts::getCurrentSignalStrength()
{
    return launch("/usr/bin/yio-remote/wifi_rssi.sh").toInt();
}

QString WifiShellScripts::getCurrentIp()
{
    return launch("/usr/bin/yio-remote/wifi_ip.sh");
}

void WifiShellScripts::startNetworkScan()
{
    QString scanResult = launch("/usr/bin/yio-remote/wifi_network_list.sh");
}

QString WifiShellScripts::launch(const QString &command)
{
    QStringList arguments;
    return launch(command, arguments);
}

QString WifiShellScripts::launch(const QString &command, const QStringList &arguments)
{
    // FIXME synchronize launcher
    m_process->start(command, arguments);
    m_process->waitForFinished(-1);
    QByteArray bytes = m_process->readAllStandardOutput();
    QString output = QString::fromLocal8Bit(bytes);
    return output;
}
