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
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY bluetoothStatusChanged)

 public:
    explicit BluetoothControl(QObject *parent = nullptr);
    ~BluetoothControl();

    bool isEnabled();
    void setEnabled(bool enabled);

 signals:
    void bluetoothStatusChanged();

    void dockFound(const QString &name);
    void dockPaired(const QString &address);
    void dockConnected(const QString &address, const QString &serviceUuid);

    void dockMessageSent();
    void dockSetupFailed();

 public slots:
    void lookForDocks();
    void sendCredentialsToDock(const QString &msg);

 private slots:
    void onDeviceDiscovered(const QBluetoothDeviceInfo &device);
    void onDiscoveryFinished();
    void onPairingDisplayConfirmation(const QBluetoothAddress &address, QString pin);
    void onPairingDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);
    void onPairingError(QBluetoothLocalDevice::Error error);
    void onDockConnected();
    void onDockSocketError(QBluetoothSocket::SocketError error);

 private:
    void close();
    void dockSetupError();

 private:
    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent;
    QBluetoothLocalDevice           m_localDevice;

    // FIXME support multiple devices
    QBluetoothAddress m_dockAddress;
    QBluetoothUuid    m_dockServiceUuid;
    QBluetoothSocket *m_dockSocket;
    QString           m_dockMessage;
};
