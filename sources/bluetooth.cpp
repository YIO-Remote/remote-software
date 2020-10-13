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

#include "bluetooth.h"

#include <QTimer>

#include "logging.h"
#include "notifications.h"

BluetoothControl::BluetoothControl(QObject *parent) : QObject(parent) {
    qCInfo(lcBluetooth) << "Bluetooth starting";

    // if there is a bluetooth device, let's set up some things
    if (m_localDevice.isValid()) {
        qCDebug(lcBluetooth) << "Bluetooth init OK";

    } else {
        qCCritical(lcBluetooth) << "Bluetooth device was not found.";
        Notifications::getInstance()->add(true, tr("Bluetooth device was not found."));
    }
}

void BluetoothControl::turnOn() {
    if (m_localDevice.hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
        m_localDevice.powerOn();
        qCDebug(lcBluetooth) << "Bluetooth on";
    }
}

void BluetoothControl::turnOff() {
    m_localDevice.setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    qCDebug(lcBluetooth) << "Bluetooth off";
}

void BluetoothControl::lookForDocks() {
    turnOn();

    qCDebug(lcBluetooth) << "Creating bluetooth discovery agent";
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    QObject::connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this,
                     &BluetoothControl::onDeviceDiscovered);
    QObject::connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this,
                     &BluetoothControl::onDiscoveryFinished);
    qCDebug(lcBluetooth) << "Starting Bluetooth discovery...";
    m_discoveryAgent->start();
}

void BluetoothControl::onDeviceDiscovered(const QBluetoothDeviceInfo &device) {
    // if dock is found
    if (device.name().contains("YIO-Dock")) {
        // stop the discovery
        m_discoveryAgent->stop();

        m_dockAddress = device.address();
        if (device.serviceUuids().length() >= 1) {
            m_dockServiceUuid = device.serviceUuids()[0];
        } else {
            m_dockServiceUuid = QBluetoothUuid(QStringLiteral("00001101-0000-1000-8000-00805f9b34fb"));
        }
        QString name = device.name();

        emit dockFound(name);
        qCDebug(lcBluetooth) << "YIO Dock found" << name;

        QObject::connect(&m_localDevice, &QBluetoothLocalDevice::pairingFinished, this,
                         &BluetoothControl::onPairingDone);
        qCDebug(lcBluetooth) << "Pairing requested.";
        m_localDevice.requestPairing(m_dockAddress, QBluetoothLocalDevice::Paired);
    }
}

void BluetoothControl::onDiscoveryFinished() {
    qCDebug(lcBluetooth) << "Bluetooth discovery finished.";
    if (m_discoveryAgent) {
        m_discoveryAgent->deleteLater();
    }
}

void BluetoothControl::sendCredentialsToDock(const QString &msg) {
    qCDebug(lcBluetooth) << "Got the wifi credentials.";
    m_dockMessage = msg;
}

void BluetoothControl::onDockConnected() {
    qCDebug(lcBluetooth) << "Open bluetooth socket";
    // open bluetooth socket
    m_dockSocket->open(QIODevice::WriteOnly);

    // format the message
    QByteArray text = m_dockMessage.toUtf8() + '\n';
    qCDebug(lcBluetooth) << "Sending message to dock:" << text;
    m_dockSocket->write(text);
    qCDebug(lcBluetooth) << "Message sent to dock.";

    QTimer::singleShot(6000, [=]() {
        // close and delete the socket
        m_dockSocket->close();
        m_dockSocket->deleteLater();

        // power off the bluetooth
        turnOff();
        m_discoveryAgent->deleteLater();
        emit dockMessageSent();
    });
}

void BluetoothControl::onPairingDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing) {
    if (address == m_dockAddress && pairing == QBluetoothLocalDevice::Paired) {
        qCDebug(lcBluetooth) << "Pairing done.";
        m_dockSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);

        QObject::connect(m_dockSocket, &QBluetoothSocket::connected, this, &BluetoothControl::onDockConnected);

        m_dockSocket->connectToService(m_dockAddress, m_dockServiceUuid);
        qCDebug(lcBluetooth) << "Connecting to bluetooth service" << m_dockAddress << m_dockServiceUuid;
    }
}
