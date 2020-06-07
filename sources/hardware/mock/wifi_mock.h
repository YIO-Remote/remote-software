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

#pragma once

#include <QObject>

#include "../wifi_control.h"

/**
 * @brief Mock implementation for the WifiControl interface for unsupported platforms.
 */
class WifiMock : public WifiControl {
    Q_OBJECT

 public:
    explicit WifiMock(QObject *parent = nullptr);
    ~WifiMock() override;

    bool init() override;

    Q_INVOKABLE bool reset() override;
    Q_INVOKABLE bool clearConfiguredNetworks() override;
    Q_INVOKABLE bool join(const QString &ssid, const QString &password,
                          WifiSecurity::Enum security = WifiSecurity::DEFAULT) override;
    Q_INVOKABLE bool isConnected() override;
    Q_INVOKABLE void startNetworkScan() override;
    Q_INVOKABLE bool startAccessPoint() override;

    QString    countryCode() override;
    void       setCountryCode(const QString &countryCode) override;
    WifiStatus wifiStatus() const override;

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void on() override;
    void off() override;

 private:
    void timerEvent(QTimerEvent *event) override;

    QString m_countryCode;
};
