#ifndef BLUETOOTHAREA_H
#define BLUETOOTHAREA_H

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMap>
#include <QTimer>
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
    Q_PROPERTY  (int     interval       READ interval       WRITE setInterval       NOTIFY intervalChanged)

    QString currentArea ()              { return m_currentArea; }
    int     interval ()                 { return m_timerInterval; }

    void setInterval(int interval)
    {
        m_timerInterval = interval;
        emit intervalChanged();
    }

    // initialise with the config from config.json
    Q_INVOKABLE void init               (const QVariantMap& config);
    Q_INVOKABLE void turnOff            ();

    Q_INVOKABLE void startScan();
    Q_INVOKABLE void stopScan();


    explicit BluetoothArea(QObject *parent = nullptr);
    virtual ~BluetoothArea() {}


signals:
    void currentAreaChanged();
    void intervalChanged();

public slots:
    void deviceDiscovered(const QBluetoothDeviceInfo &device);
    void discoveryFinished();

    void onRestartTimeout();


private:
    QString                             m_currentArea;
    QMap<QString, QString>              m_areas; // "Living room", "xx:xx:xx:xx:xx:"
    QMap<QString, int>                  m_rssi;  // "Living room", -89

    QBluetoothDeviceDiscoveryAgent*     m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    QBluetoothLocalDevice               m_localDevice;
    QString                             m_localDeviceName;
    QBluetoothAddress                   m_localDeviceAddr;

    QTimer                              m_restartTimer;
    int                                 m_timerInterval = 10000;
    bool                                running = false;

};

#endif // BLUETOOTHAREA_H
