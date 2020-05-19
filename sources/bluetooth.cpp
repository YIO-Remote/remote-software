#include "bluetooth.h"

#include <QLoggingCategory>

#include "notifications.h"

static Q_LOGGING_CATEGORY(CLASS_LC, "bluetooth");

BluetoothControl::BluetoothControl(QObject *parent) : QObject(parent) {
    qCritical(CLASS_LC) << "Bluetooth starting";

    // if there is a bluetooth device, let's set up some things
    if (m_localDevice.isValid()) {
        qCDebug(CLASS_LC) << "Bluetooth init OK";

    } else {
        qCritical(CLASS_LC) << "Bluetooth device was not found.";
        Notifications::getInstance()->add(true, tr("Bluetooth device was not found."));
    }
}

void BluetoothControl::turnOn() {
    if (m_localDevice.hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
        m_localDevice.powerOn();
        qCDebug(CLASS_LC) << "Bluetooth on";
    }
}

void BluetoothControl::turnOff() {
    m_localDevice.setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    qCDebug(CLASS_LC) << "Bluetooth off";
}

void BluetoothControl::lookForDocks() {
    turnOn();

    qCDebug(CLASS_LC) << "Creating bluetooth discovery agent";
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);

    QObject::connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this,
                     &BluetoothControl::onDeviceDiscovered);
    QObject::connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this,
                     &BluetoothControl::onDiscoveryFinished);
    m_discoveryAgent->start();
    qCDebug(CLASS_LC) << "Bluetooth discovery started.";
}

void BluetoothControl::onDeviceDiscovered(const QBluetoothDeviceInfo &device) {
    // if dock is found
    if (device.name().contains("YIO-Dock")) {
        // stop the discovery
        m_discoveryAgent->stop();

        m_dockAddress = device.address();
        if (device.serviceUuids().length() != 0) {
            m_dockServiceUuid = device.serviceUuids()[0];
        } else {
            m_dockServiceUuid = QBluetoothUuid(QStringLiteral("00001101-0000-1000-8000-00805f9b34fb"));
        }
        QString name = device.name();

        emit dockFound(name);
        qCDebug(CLASS_LC) << "YIO Dock found" << name;
    }
}

void BluetoothControl::onDiscoveryFinished() {
    qCDebug(CLASS_LC) << "Bluetooth discovery finished.";
    if (m_discoveryAgent) {
        m_discoveryAgent->deleteLater();
    }
}

void BluetoothControl::sendCredentialsToDock(const QString &msg) {
    m_dockMessage = msg;
    m_dockSocket  = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this);

    QObject::connect(m_dockSocket, &QBluetoothSocket::connected, this, &BluetoothControl::onDockConnected);

    m_dockSocket->connectToService(m_dockAddress, m_dockServiceUuid);
    qCDebug(CLASS_LC) << "Connect to bluetooth service" << m_dockAddress << m_dockServiceUuid;
}

void BluetoothControl::onDockConnected() {
    qCDebug(CLASS_LC) << "Open bluetooth socket";
    // open bluetooth socket
    m_dockSocket->open(QIODevice::WriteOnly);

    // format the message
    QByteArray text = m_dockMessage.toUtf8() + '\n';
    qCDebug(CLASS_LC) << "Sending message to dock:" << text;
    m_dockSocket->write(text);
    qCDebug(CLASS_LC) << "Message sent to dock.";
    emit dockMessageSent();

    // close and delete the socket
    m_dockSocket->close();
    m_dockSocket->deleteLater();

    // power off the bluetooth
    turnOff();
    m_discoveryAgent->deleteLater();
}
