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

#ifndef WIFIMOCK_H
#define WIFIMOCK_H

#include <QObject>

#include "wifi_control.h"

class WifiMock : public WifiControl
{
    Q_OBJECT
public:
    explicit WifiMock(QObject *parent = nullptr);
    virtual ~WifiMock() override;

    bool init() override;

    Q_INVOKABLE bool reset() override;
    Q_INVOKABLE bool join(const QString &ssid, WifiNetwork::Security security, const QString &password) override;
    Q_INVOKABLE bool isConnected() override;
    Q_INVOKABLE void startNetworkScan() override;

    QString macAddress() const override;
    QString ssid() const override;
    int signalStrength() const override;
    QString ipAddress() const override;

public slots:
    void on() override;
    void off() override;

private:
    void timerEvent(QTimerEvent *event) override;

};

#endif // WIFIMOCK_H
