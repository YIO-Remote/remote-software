/******************************************************************************
 *
 * Copyright (C) 2020 Marton Borzak <hello@martonborzak.com>
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

#include <QBluetoothAddress>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothDeviceInfo>
#include <QBluetoothSocket>
#include <QObject>

#include "qbluetoothlocaldevice.h"

class BluetoothControl : public QObject {
    Q_OBJECT

 public:
    explicit BluetoothControl(QObject *parent = nullptr);
    virtual ~BluetoothControl() {}

    void turnOn();
    void turnOff();

 signals:
    void dockFound(QString name);
    void dockMessageSent();

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void lookForDocks();

 private slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void onDeviceDiscovered(const QBluetoothDeviceInfo &device);
    void onDiscoveryFinished();
    void sendCredentialsToDock(const QString &msg);
    void onPairingDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);
    void onDockConnected();
    void onSocketError(QBluetoothSocket::SocketError error);

 private:
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent;
    QBluetoothLocalDevice           m_localDevice;

    QBluetoothAddress m_dockAddress;
    QBluetoothUuid    m_dockServiceUuid;
    QBluetoothSocket *m_dockSocket;
    QString           m_dockMessage;
};
