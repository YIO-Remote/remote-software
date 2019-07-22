#ifndef BLUETOOTHAREA_H
#define BLUETOOTHAREA_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMap>
#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QBluetoothDeviceDiscoveryAgent>
#include "qbluetoothlocaldevice.h"

#include <QtDebug>


class BluetoothArea : public QObject
{
    Q_OBJECT

public:
    // the current area
    Q_PROPERTY  (QString currentArea    READ currentArea    NOTIFY currentAreaChanged CONSTANT)

    QString currentArea ()              { return m_currentArea; }


    // initialise with the config from config.json
    Q_INVOKABLE void init               (const QVariantMap& config);

    Q_INVOKABLE void startScan();
    Q_INVOKABLE void stopScan();


    explicit BluetoothArea(QObject *parent = nullptr);
    virtual ~BluetoothArea() {}


signals:
    void currentAreaChanged();

public slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &device);


private:
    QString                             m_currentArea;
    QMap<QString, QString>              m_areas;
    QMap<QString, int>                  m_rssi;

    QBluetoothDeviceDiscoveryAgent*     m_discoveryAgent;
    QBluetoothLocalDevice               m_localDevice;
    QString                             m_localDeviceName;
    QBluetoothAddress                   m_localDeviceAddr;

};

#endif // BLUETOOTHAREA_H
