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

/**
 * @brief Mock implementation for the WifiControl interface for unsupported platforms.
 */
class WifiMock : public WifiControl
{
    Q_OBJECT
public:
    explicit WifiMock(QObject *parent = nullptr);
    virtual ~WifiMock() override;

    virtual bool init() override;

    Q_INVOKABLE virtual bool reset() override;
    Q_INVOKABLE virtual bool clearConfiguredNetworks() override;
    Q_INVOKABLE virtual bool join(const QString &ssid, const QString &password, WifiSecurity security = WifiSecurity::DEFAULT) override;
    Q_INVOKABLE virtual bool isConnected() override;
    Q_INVOKABLE virtual void startNetworkScan() override;
    Q_INVOKABLE virtual bool startAccessPoint() override;

    virtual QString countryCode() override;
    virtual void setCountryCode(QString &countryCode) override;
    virtual QString macAddress() const override;
    virtual QString ssid() const override;
    virtual int signalStrength() const override;
    virtual QString ipAddress() const override;

public slots:
    virtual void on() override;
    virtual void off() override;

private:
    virtual void timerEvent(QTimerEvent *event) override;

    QString m_countryCode;
};

#endif // WIFIMOCK_H
