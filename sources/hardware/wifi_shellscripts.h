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

#ifndef WIFISHELLSCRIPTS_H
#define WIFISHELLSCRIPTS_H

#include <QObject>
#include <QProcess>

#include "wifi_control.h"

/**
 * @brief Deprecated WifiControl implementation using the legacy shell scripts
 */
class WifiShellScripts : public WifiControl
{
    Q_OBJECT
public:
    explicit WifiShellScripts(QObject *parent = nullptr);

    virtual bool init() override;

    Q_INVOKABLE virtual bool reset() override;
    Q_INVOKABLE virtual bool clearConfiguredNetworks() override;
    Q_INVOKABLE virtual bool join(const QString &ssid, const QString &password, WifiSecurity security = WifiSecurity::DEFAULT) override;
    Q_INVOKABLE virtual bool isConnected() override;
    Q_INVOKABLE virtual void startNetworkScan() override;
    Q_INVOKABLE virtual bool startAccessPoint() override;

    virtual QString countryCode() override;
    virtual void setCountryCode(QString &countryCode) override;

public slots:

    virtual void on() override;
    virtual void off() override;

private:
    virtual void timerEvent(QTimerEvent *event) override;

    /**
     * Tokenize a single line of scan results and interpret fields as WifiNetwork
     * @param index line number
     * @param line reference to string in scan results
     * @return WifiNetwork data object
     */
    WifiNetwork lineToNetwork(int index, const QStringRef& line);

    /**
     * Tokenize a buffer returned by the shell script into a list of WifiNetworks
     * @param buffer result
     */
    QList<WifiNetwork> parseScanresult(const QString& buffer);

    QProcess *m_scriptProcess;

};

#endif // WIFISHELLSCRIPTS_H
