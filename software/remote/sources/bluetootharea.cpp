#include "bluetootharea.h"
#include "notifications.h"
#include "math.h"

BluetoothArea::BluetoothArea(QObject *parent) : QObject(parent)
{
    m_restartTimer.setSingleShot(true);
    m_restartTimer.setInterval(m_timerInterval);
    m_restartTimer.stop();
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

        connect(m_discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this, SLOT(deviceDiscovered(QBluetoothDeviceInfo)));
        connect(m_discoveryAgent, SIGNAL(finished()), this, SLOT(discoveryFinished()));

        connect(&m_restartTimer, SIGNAL(timeout()), this, SLOT(onRestartTimeout()));
    } else {
        Notifications::getInstance()->add(true, tr("Bluetooth device was not found."));
    }
}

void BluetoothArea::turnOff()
{
    // power off
    m_localDevice.setHostMode(QBluetoothLocalDevice::HostPoweredOff);
}

void BluetoothArea::startScan()
{   
    if (!running) {

        running = true;

        qDebug() << "Turn on and start bluetooth scan";

        // turn bluetooth on if it was off
        if (m_localDevice.hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
            m_localDevice.powerOn();

            // read the name
            m_localDeviceName = m_localDevice.name();

            // read the address
            m_localDeviceAddr = m_localDevice.address();
        }

        m_discoveryAgent->start();
    }
}

void BluetoothArea::stopScan()
{
    if (running) {

        running = false;

        qDebug() << "Stop bluetooth scan and poweroff";

        // stop scan
        m_discoveryAgent->stop();
    }
}

void BluetoothArea::deviceDiscovered(const QBluetoothDeviceInfo &device)
{
    if (m_areas.contains(device.address().toString()) && device.rssi() != 0) {

        int rssi = int(device.rssi())*-1;
        m_rssi.insert(m_areas.value(device.address().toString()), rssi);

        //qDebug() << "Found" << m_areas.value(device.address().toString()) << rssi;

        // this mapp will be sorted by default
        QMap<int, QString> sortedMap;

        for (QMap<QString, int>::iterator i = m_rssi.begin(); i != m_rssi.end(); ++i) {
            sortedMap.insert(i.value(), i.key());
        }

        // qmap ordered by key, so now we get the first key to get the closest beacon
        m_currentArea = sortedMap.value(sortedMap.firstKey());
        emit currentAreaChanged();

        //qDebug() << "Current area" << m_currentArea;
    }
}

void BluetoothArea::discoveryFinished()
{
    // restart scan after delay
    m_restartTimer.start();
}

void BluetoothArea::onRestartTimeout()
{
    qDebug() << "Restarting discovery";
    m_discoveryAgent->start();
}
