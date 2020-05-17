/******************************************************************************
 *
 * Copyright (C) 2018-2019 Marton Borzak <hello@martonborzak.com>
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
#include <QMap>
#include <QObject>
#include <QString>
#include <QThread>
#include <QTimer>
#include <QVariant>
#include <QtDebug>

#include "qbluetoothlocaldevice.h"

class BluetoothArea : public QObject {
    Q_OBJECT

 public:
    // the current area
    Q_PROPERTY(QString currentArea READ currentArea NOTIFY currentAreaChanged)
    Q_PROPERTY(int interval READ interval WRITE setInterval NOTIFY intervalChanged)

    QString currentArea() { return m_currentArea; }
    int     interval() { return m_timerInterval; }

    void setInterval(int interval) {
        m_timerInterval = interval;
        emit intervalChanged();
    }

    // initialise with the config from config.json
    Q_INVOKABLE void init(const QVariantMap &config);
    Q_INVOKABLE void turnOn();
    Q_INVOKABLE void turnOff();

    Q_INVOKABLE void startScan();
    Q_INVOKABLE void stopScan();

    Q_INVOKABLE void lookForDock();
    Q_INVOKABLE void sendInfoToDock(const QString &msg);

    explicit BluetoothArea(QObject *parent = nullptr);
    virtual ~BluetoothArea();

 signals:
    void currentAreaChanged();
    void intervalChanged();

    void startScanSignal();
    void stopScanSignal();
    void turnOnSignal();
    void turnOffSignal();

    void lookForDockSignal();
    void sendInfoToDockSignal(const QString &msg);

    void dockFound(const QString &address);
    void dockMessage(const QString &message);
    void dockPairingFinished();

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void deviceDiscovered(const QString &);
    void foundDock(const QString &address);
    void onPairingFinished();

 private:
    QString                m_currentArea;
    QMap<QString, QString> m_areas;  // "Living room", "xx:xx:xx:xx:xx:"
    int                    m_timerInterval = 5000;

    QThread m_thread;
};

class BluetoothThread : public QObject {
    Q_OBJECT

 public:
    bool running = false;

    BluetoothThread(QMap<QString, QString> areas, int interval);

 signals:
    void foundRoom(const QString &area);
    void foundDock(const QString &address);
    void pairingFinished();

 public slots:  // NOLINT open issue: https://github.com/cpplint/cpplint/pull/99
    void startScan();
    void stopScan();
    void turnOff();
    void turnOn();
    void onDeviceDiscovered(const QBluetoothDeviceInfo &device);
    void onDiscoveryFinished();

    void lookForDock();
    void receiveInfoForDock(const QString &msg);
    void onDockConnected();
    void onDockDisconnected();
    void onDockConnectionError(QBluetoothSocket::SocketError error);
    void onDockStateChanged(QBluetoothSocket::SocketState state);
    void onPairingFinished(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);

    void onTimeout();

 private:
    QString                m_currentArea;
    QMap<QString, QString> m_areas;  // "Living room", "xx:xx:xx:xx:xx:"
    QMap<QString, int>     m_rssi;   // "Living room", -89

    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    QBluetoothLocalDevice           m_localDevice;
    QString                         m_localDeviceName;
    QBluetoothAddress               m_localDeviceAddr;

    QTimer *m_timer = new QTimer(this);

    QBluetoothAddress m_dock_address;
    QBluetoothSocket *m_socket;
    QString           m_dock_message;
};
