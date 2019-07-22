#include "bluetootharea.h"

BluetoothArea::BluetoothArea(QObject *parent) : QObject(parent)
{
}

void BluetoothArea::init(const QVariantMap &config)
{
    // init code
    if (config.value("settings").toMap().value("bluetootharea").toBool() && m_localDevice.isValid())  {

        // load the areas
        QVariantList areas = config.value("areas").toList();

        for (int i=0; i<areas.length(); i++) {
            QVariantMap m = areas[i].toMap();

            m_areas.insert(m["bluetooth"].toString(), m["area"].toString());
        }

        // turn bluetooth on
        m_localDevice.powerOn();

        // read the name
        m_localDeviceName = m_localDevice.name();

        // read the address
        m_localDeviceAddr = m_localDevice.address();

        startScan();
    }
}

void BluetoothArea::startScan()
{
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(m_discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));

    m_discoveryAgent->start();
}

void BluetoothArea::stopScan()
{
    m_discoveryAgent->stop();
}

void BluetoothArea::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
//    qDebug() << "Found" << device.name() << device.address() << device.rssi();

//    qDebug() << "Found" << m_areas.value(device.address().toString());

    if (m_areas.contains(device.address().toString())) {
        qDebug() << "Found a device in" << m_areas.value(device.address().toString());
    }
}
