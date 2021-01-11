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

BluetoothControl::BluetoothControl(QObject *parent)
    : QObject(parent), m_discoveryAgent(nullptr), m_dockSocket(nullptr) {
    qCInfo(lcBluetooth) << "Bluetooth starting";

    // if there is a Bluetooth device, let's set up some things
    // FIXME move out of constructor
    if (m_localDevice.isValid()) {
        qCDebug(lcBluetooth) << "Bluetooth init OK";

        QObject::connect(&m_localDevice, &QBluetoothLocalDevice::pairingDisplayConfirmation, this,
                         &BluetoothControl::onPairingDisplayConfirmation);
        QObject::connect(&m_localDevice, &QBluetoothLocalDevice::pairingFinished, this,
                         &BluetoothControl::onPairingDone);
        QObject::connect(&m_localDevice, &QBluetoothLocalDevice::error, this, &BluetoothControl::onPairingError);
    } else {
        qCCritical(lcBluetooth) << "Bluetooth device was not found.";
        Notifications::getInstance()->add(true, tr("Bluetooth device was not found."));
    }
}

BluetoothControl::~BluetoothControl() {
    close();
}

bool BluetoothControl::isEnabled() {
    return (m_localDevice.hostMode() != QBluetoothLocalDevice::HostPoweredOff);
}

void BluetoothControl::setEnabled(bool enabled) {
    if (enabled) {
        if (m_localDevice.hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
            m_localDevice.powerOn();
            qCInfo(lcBluetooth) << "Bluetooth on";
        }
    } else {
        m_localDevice.setHostMode(QBluetoothLocalDevice::HostPoweredOff);
        qCInfo(lcBluetooth) << "Bluetooth off";
    }

    emit bluetoothStatusChanged();
}

void BluetoothControl::lookForDocks() {
    setEnabled(true);

    qCDebug(lcBluetooth) << "Creating Bluetooth discovery agent";
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    QObject::connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this,
                     &BluetoothControl::onDeviceDiscovered);
    QObject::connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this,
                     &BluetoothControl::onDiscoveryFinished);

    qCInfo(lcBluetooth) << "Starting Bluetooth discovery...";
    m_discoveryAgent->start();
}

void BluetoothControl::onDeviceDiscovered(const QBluetoothDeviceInfo &device) {
    qCDebug(lcBluetooth()) << "Discovered:" << device.address() << device.name();
    // if dock is found
    // FIXME support multiple discovered docks
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
        emit    dockFound(name);

        // TODO(zehnm) split pairing & socket connection from discovery: UI should initiate pairing & command sending
        qCInfo(lcBluetooth) << "YIO Dock found. Request pairing with" << name;

        m_localDevice.requestPairing(m_dockAddress, QBluetoothLocalDevice::Paired);
    }
}

void BluetoothControl::onDiscoveryFinished() {
    if (m_discoveryAgent->error() != QBluetoothDeviceDiscoveryAgent::NoError) {
        qCWarning(lcBluetooth) << "Bluetooth discovery finished with error:" << m_discoveryAgent->error()
                               << m_discoveryAgent->errorString();
        dockSetupError();
    } else {
        QMap<QString, QString> devices;
        for (const QBluetoothDeviceInfo &device : m_discoveryAgent->discoveredDevices()) {
            devices.insert(device.address().toString(), device.name());
        }
        qCInfo(lcBluetooth) << "Bluetooth discovery finished." << devices;
    }

    m_discoveryAgent->deleteLater();
    m_discoveryAgent = nullptr;
}

void BluetoothControl::sendCredentialsToDock(const QString &msg) {
    qCDebug(lcBluetooth) << "Got the wifi credentials.";
    m_dockMessage = msg;
}

void BluetoothControl::onPairingDisplayConfirmation(const QBluetoothAddress &address, QString pin) {
    qCInfo(lcBluetooth()) << "Confirming pairing request" << address << pin;
    m_localDevice.pairingConfirmation(true);
}

void BluetoothControl::onPairingDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing) {
    if (address == m_dockAddress && pairing == QBluetoothLocalDevice::Paired) {
        qCInfo(lcBluetooth) << "Pairing done with" << address;
        emit dockPaired(address.toString());
        m_dockSocket = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);

        QObject::connect(m_dockSocket, &QBluetoothSocket::connected, this, &BluetoothControl::onDockConnected);
        QObject::connect(m_dockSocket, QOverload<QBluetoothSocket::SocketError>::of(&QBluetoothSocket::error), this,
                         &BluetoothControl::onDockSocketError);

        m_dockSocket->connectToService(m_dockAddress, m_dockServiceUuid);
        qCDebug(lcBluetooth) << "Connecting to Bluetooth service" << m_dockAddress << "with uuid" << m_dockServiceUuid;
    }
}

void BluetoothControl::onPairingError(QBluetoothLocalDevice::Error error) {
    Q_UNUSED(error)
    qCWarning(lcBluetooth) << "Pairing failed with" << m_dockAddress;
    dockSetupError();
}

void BluetoothControl::onDockConnected() {
    qCDebug(lcBluetooth) << "Opening Bluetooth socket to dock";
    emit dockConnected(m_dockAddress.toString(), m_dockServiceUuid.toString());

    if (!m_dockSocket->open(QIODevice::WriteOnly)) {
        qCCritical(lcBluetooth()) << "Could not open Bluetooth socket to dock";
        dockSetupError();
        return;
    }

    // send credential message
    QByteArray text = m_dockMessage.toUtf8() + '\n';
    qCDebug(lcBluetooth) << "Sending message to dock:" << text;
    if (m_dockSocket->write(text) == -1) {
        qCCritical(lcBluetooth()) << "Could not send message to dock";
        dockSetupError();
        return;
    }

    qCInfo(lcBluetooth) << "Wifi setup message sent to dock.";

    // TODO(zehnm) why this delay? For the dock to connect to wifi?
    QTimer::singleShot(6000, [=]() {
        close();
        setEnabled(false);

        emit dockMessageSent();
    });
}

void BluetoothControl::onDockSocketError(QBluetoothSocket::SocketError error) {
    qCCritical(lcBluetooth()) << "Dock socket error:" << error << m_dockSocket->errorString();
    dockSetupError();
}

void BluetoothControl::close() {
    if (m_dockSocket) {
        m_dockSocket->close();
        m_dockSocket->deleteLater();
        m_dockSocket = nullptr;
    }
    if (m_discoveryAgent) {
        m_discoveryAgent->deleteLater();
        m_discoveryAgent = nullptr;
    }
}

void BluetoothControl::dockSetupError() {
    close();
    setEnabled(false);

    // TODO(zehnm) restart Bluetooth service?

    emit dockSetupFailed();
}
